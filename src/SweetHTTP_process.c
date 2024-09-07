#include <stdio.h>
#include <wchar.h>
#include "SweetHTTP_plugin.h"
#include "SweetHTTP_config.h"
#include "SweetHTTP_process.h"
#include "SweetHTTP_utils.h"
#include "SweetHTTP_file.h"
static bool HTTP_transferData(struct HTTP_request *request, struct SweetSocket_global_context *context, struct SweetSocket_peer_clients *thisClient)
{
	bool status = true;
	char *options = NULL;
	char *type = NULL;
	uint16_t command = 200;

	// Abrir o arquivo
	HTTP_file hFile = HTTP_fileOpen(request->filePath);
	if (hFile == HTTP_FILE_INVALID)
	{
		HTTP_sendErrorResponse(404, L"<h1>404 Not Found</h1><p>File not found.</p>", context, thisClient->id);
		goto HTTP_transferDataExitNoFree;
	}

	// Obter o tamanho do arquivo
	int64_t fileSize = HTTP_fileSize(hFile);
	if (fileSize == HTTP_FILE_INVALID_SIZE)
	{
		HTTP_fileClose(hFile);
		HTTP_sendErrorResponse(500, L"<h1>500 Internal Server Error</h1><p>Cannot get file size.</p>", context, thisClient->id);
		goto HTTP_transferDataExitNoFree;
	}

	// Verificar se é um intervalo de bytes (parcial)
	if (request->startRange != -1 || request->endRange != -1)
	{
		const char *format = "Accept-Ranges: bytes\r\nContent-Range: bytes %lld-%lld/%lld\r\n";
		options = (char *)malloc(100);
		if (!options)
		{
			HTTP_fileClose(hFile);
			HTTP_sendErrorResponse(500, L"<h1>500 Internal Server Error</h1><p>Cannot allocate memory for headers.</p>", context, thisClient->id);
			goto HTTP_transferDataExitNoFree;
		}

		request->endRange = (request->endRange == -1) ? fileSize - 1 : request->endRange;
		snprintf(options, 100, format, request->startRange, request->endRange, fileSize);
		fileSize = request->endRange - request->startRange + 1;
		if (HTTP_fileSize(hFile) != fileSize)
			command = 206;

		if (!HTTP_fileSeek(hFile, request->startRange))
		{
			HTTP_fileClose(hFile);
			HTTP_sendErrorResponse(500, L"<h1>500 Internal Server Error</h1><p>Cannot seek file.</p>", context, thisClient->id);
			goto HTTP_transferDataExitNoFree;
		}
	}

	// Enviar cabeçalho
	type = HTTP_getMimeType(request->filePath, &request->envolvirment->mimeTypes);
	HTTP_sendHeaderResponse(type == NULL ? "application/octet-stream" : type, command, fileSize, options, context, thisClient->id);
	if (type != NULL)
		free(type);
	if (options != NULL)
		free(options);

	// Enviar o arquivo
	int64_t sent = (fileSize > request->envolvirment->server.partialMaxFileBlock) ? request->envolvirment->server.partialMaxFileBlock : fileSize;
	char *fileData = (char *)malloc(sent);
	if (fileData == NULL)
	{
		CloseHandle(hFile);
		HTTP_sendErrorResponse(500, L"<h1>500 Internal Server Error</h1><p>Cannot allocate memory for file data.</p>", context, thisClient->id);
		goto HTTP_transferDataExitNoFree;
	}

	for (int64_t totalSent = 0; totalSent < fileSize;)
	{
		int64_t toRead = (fileSize - totalSent > request->envolvirment->server.partialMaxFileBlock) ? request->envolvirment->server.partialMaxFileBlock : (size_t)(fileSize - totalSent);
		int32_t readed = 0;
		if (HTTP_fileRead(hFile, toRead, &readed, fileData))
		{
			if (readed > 0)
			{
				if (!SweetSocket_sendData(fileData, readed, context, thisClient->id))
				{
					status = false;
					goto HTTP_transferDataExit;
				}
				totalSent += readed;
				continue;
			}
		}
		break; // Se não conseguiu ler, sair do loop
	}
HTTP_transferDataExit:
	free(fileData);
HTTP_transferDataExitNoFree:
	HTTP_fileClose(hFile);
	return status;
}

static void HTTP_sendDirectoryListing(struct HTTP_request *request, struct SweetSocket_global_context *context, struct SweetSocket_peer_clients *thisClient)
{
	if (!request->envolvirment->server.allowDirectoryListing)
	{
		HTTP_sendErrorResponse(403, L"<h1>403 Forbidden</h1><p>Directory listing not allowed.</p>", context, thisClient->id);
		return;
	}

	wchar_t *html = NULL;
	size_t htmlSize = HTTP_createHtmlDirectoryList(request->filePath, request->virtualPath, &html);
	if (htmlSize == 0)
	{
		HTTP_sendErrorResponse(404, L"<h1>404 Not Found</h1><p>Directory not found.</p>", context, thisClient->id);
		return;
	}

	HTTP_sendHeaderResponse("text/html; charset=UTF-16", 200, htmlSize, NULL, context, thisClient->id);
	SweetSocket_sendData((const char *)html, htmlSize, context, thisClient->id);
	free(html);
}

static struct HTTP_request HTTP_requestConstruct(char *data, uint64_t size, struct HTTP_server_envolvirment *envolvirment)
{
	struct HTTP_request request = {0};
	HTTP_splitRequest(data, size, &request.header, &request.data, &request.dataSize);
	request.verb = HTTP_getVerb(data);
	request.host = HTTP_getHost(data);
	request.userAgent = HTTP_getUserAgent(data, size);
	HTTP_parsingUrl(data, envolvirment->server.root, &request.filePath, &request.virtualPath, &request.getContent);
	request.envolvirment = envolvirment;
	request.startRange = -1;
	request.endRange = -1;
	request.useRange = HTTP_getRangeValues(data, &request.startRange, &request.endRange);
	return request;
}

static void HTTP_requestDestroy(struct HTTP_request *request)
{
	free(request->filePath);
	free(request->verb);
	free(request->userAgent);
	free(request->host);
}

enum SweetSocket_sweet_callback_status HTTP_processClientRequest(char *data, uint64_t size, struct SweetSocket_global_context *ctx, struct SweetSocket_peer_clients *thisClient, void *parms)
{
	enum SweetSocket_sweet_callback_status status = SWEET_SOCKET_CALLBACK_OK;

	struct HTTP_server_envolvirment *envolvirment = (struct HTTP_server_envolvirment *)parms;

	struct HTTP_request request = HTTP_requestConstruct(data, size, envolvirment);

	HTTP_logClientRequest(&request, thisClient);

	// Processar plugins
	// Essa parte não está completamente implementanda ou definida
	for (struct HTTP_object *plugin = envolvirment->plugins.base; plugin != NULL; plugin = plugin->next)
	{
		struct HTTP_plugin_metadata *metadata = (struct HTTP_plugin_metadata *)plugin->object;
		if (!metadata->isKeepLoaded)
			metadata->entryPoint();

		char *pluginContent = NULL;
		char *pluginAdditionalHeader = NULL;
		char *typeResponse = NULL;
		uint64_t pluginResponseSize = 0;
		uint16_t pluginResponseCode = 0;
		if (metadata->responsePoint((void *)&request, &pluginContent, &pluginResponseSize, &pluginResponseCode, &typeResponse, &pluginAdditionalHeader))
		{
			HTTP_sendHeaderResponse(typeResponse, pluginResponseCode, pluginResponseSize, pluginAdditionalHeader, ctx, thisClient->id);
			SweetSocket_sendData(pluginContent, pluginResponseSize, ctx, thisClient->id);
			free(pluginContent);
			free(pluginAdditionalHeader);
			free(typeResponse);
			goto HTTPexitNoDestroy;
		}
		if (!metadata->isKeepLoaded)
			metadata->shutdownPoint();
	}

	// Verificar se é um diretório
	if (HTTP_fileIsDirectory(request.filePath))
	{
		size_t len = wcslen(request.filePath);
		size_t lenV = wcslen(request.virtualPath);
		if (request.filePath[len - 1] != L'/')
		{
			len += 1;
			request.filePath = realloc(request.filePath, (len + 1) * sizeof(wchar_t));
			if (request.filePath == NULL)
			{
				HTTP_sendErrorResponse(500, L"<h1>500 Internal Server Error</h1><p>Memory allocation failed.</p>", ctx, thisClient->id);
				goto HTTPexit;
			}
			request.filePath[len - 1] = L'/';
			request.filePath[len] = L'\0';
			lenV += 1;
		}

		// Tentar arquivo padrão
		wchar_t *defaultPage = HTTP_findDefaultFile(envolvirment->server.defaultPages, request.filePath);
		if (defaultPage != NULL)
		{
			free(request.filePath);
			request.filePath = defaultPage;
			if (!HTTP_transferData(&request, ctx, thisClient))
				status = SWEET_SOCKET_CALLBACK_ERROR;
			goto HTTPexit;
		}

		// Listar diretório
		if (envolvirment->server.allowDirectoryListing)
		{
			request.virtualPath = request.filePath + (len - lenV);
			HTTP_sendDirectoryListing(&request, ctx, thisClient);
			goto HTTPexit;
		}
	}
	else if (HTTP_fileIsFile(request.filePath))
	{
		// Verificar se é um arquivo
		if (!HTTP_transferData(&request, ctx, thisClient))
			status = SWEET_SOCKET_CALLBACK_ERROR;
		goto HTTPexit;
	}

	// Arquivo não encontrado
	HTTP_sendErrorResponse(404, L"<h1>404 Not Found</h1><p>File not found.</p>", ctx, thisClient->id);

HTTPexit:
	HTTP_requestDestroy(&request);
HTTPexitNoDestroy:
	if (status == SWEET_SOCKET_CALLBACK_OK && !HTTP_isKeepAlive(data))
		status = SWEET_SOCKET_CALLBACK_CLOSE;
	return status;
}