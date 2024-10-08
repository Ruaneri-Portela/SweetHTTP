#ifndef HTTP_PLUGIN
#define HTTP_PLUGIN
#include <SweetSocket.h>
#include <stdbool.h>
#include <stdint.h>
#include "SweetHTTP_process.h"

enum HTTP_plugin_type
{
	PLUGIN_TYPE_ALL,
	PLUGIN_TYPE_REQUEST,
	PLUGIN_TYPE_RESPONSE,
};

struct HTTP_plugin_metadata
{
	bool isKeepLoaded;
	enum HTTP_plugin_type type;
	const char* name;
	void (*entryPoint)();
	void (*shutdownPoint)();
	bool (*requestPoint)(struct HTTP_request* request);
	bool (*responsePoint)(struct HTTP_request* request, char** responseContent, uint64_t* responseSize, uint16_t* responseCode, char** responseType, char** adictionalHeader);
	void (*setModule)(void* module);
	void* (*getModule)();
};

const struct HTTP_plugin_metadata* HTTP_loadPlugin(const wchar_t* pluginPath);

void HTTP_pluginUnload(const struct HTTP_plugin_metadata* plugin);
#endif