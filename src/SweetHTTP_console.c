#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "SweetHTTP.h"
void SweetHTTP_console(volatile bool *closing,struct HTTP_server_envolvirment *envolviment)
{
    wprintf(L"Press Ctrl+C to stop or type exit\n");
    int bufferSize = 512;
	char* consoleBuffer = calloc(1, bufferSize);
	if (consoleBuffer == NULL)
	{
		perror("Erro ao alocar memória");
		return;
	}
    printf("SweetHTTP Console\nVersion "SWEETHTTP_VERSION_H"-"SWEETHTTP_VERSION_HASH"-"SWEETHTTP_VERSION_TAG"\nWelcome to HTTP console!\n_> ");
    while (!(*closing))
    {
        SweetThread_sleep(100); // Pausa por 0.1 segundo
        // Ler a entrada padrão
        int rollUp = 1;
        fgets(consoleBuffer, bufferSize, stdin);
        while (true)
        {
            int end = (bufferSize * rollUp) - 2; // Posição para verificar se o buffer está cheio
            if (consoleBuffer[end] != '\0' && consoleBuffer[end] != '\n')
            {
                char *tempBuffer = realloc(consoleBuffer, bufferSize * ++rollUp);
                if (tempBuffer == NULL)
                {
                    perror("Erro ao realocar memória");
                    free(consoleBuffer);
                    return; // Sai do loop e encerra o programa
                }
                consoleBuffer = tempBuffer;
                // Continua a leitura a partir do final do buffer existente
                fgets(consoleBuffer + (bufferSize * (rollUp - 1)), bufferSize, stdin);
                continue;
            }
            break;
        }

        // Processa o conteúdo do buffer
        if (consoleBuffer[0] != '\0')
        {
            // Console
            if (strcmp("exit\n", consoleBuffer) == 0)
            {
                *closing = 1;
                break;
            }
            else if (strcmp("list connections\n", consoleBuffer) == 0)
            {
                printf("Connections : [%lld]\n", envolviment->context->connectionsAlive);
                for (struct SweetSocket_peer_clients *actual = envolviment->context->clients; actual != NULL; actual = actual->next)
                {
                    SweetSocket_resolvePeer(actual);
                    printf("[%lld] -> ADDR = %s ,PORT = %d\n", actual->id, actual->client->addr, actual->client->port);
                }
            }
            else if (strcmp("list servers\n", consoleBuffer) == 0)
            {
                printf("Servers : [%lld]\n", envolviment->context->connections.size);
                for (struct SweetSocket_peer_connects *actual = envolviment->context->connections.base; actual != NULL; actual = actual->next)
                {
                    printf("[%lld] -> ADDR = %s ,PORT = %d, TYPE = %s\n", actual->id, actual->socket.addr, actual->socket.port, actual->socket.type == AF_INET ? "IPv4" : actual->socket.type == AF_INET6 ? "IPv6"
                                                                                                                                                                                                         : "UNKNOW");
                }
            }
            else if (strcmp("show config\n", consoleBuffer) == 0)
            {
                wprintf(L"Root: '%s'\n"
                       "Allow Listing Directory: %d\n"
                       "Max block send: %d\n"
                       "Max connections: %d\n",
                       envolviment->server.root,
                       envolviment->server.allowDirectoryListing,
                       envolviment->server.partialMaxFileBlock,
                       envolviment->server.maxClientConnections);
            }
            else if (strcmp("help\n", consoleBuffer) == 0)
            {
                printf("Help\n'list connections'\tSee clients table\n'list servers'\t\tSee servers table\n'show config'\t\tShowing settings uses now\n'exit'\t\t\tClose server\n");
            }
            else if (strcmp("version\n", consoleBuffer) == 0)
            {
                printf("Version Number: " SWEETHTTP_VERSION_H "\nGit upstream: " SWEETHTTP_VERSION_HASH "_" SWEETHTTP_VERSION_TAG "\n");
            }
            else
            {
                printf("Comand unknow, use help to viewer comands\n");
            }
            printf("_> ");
            // Limpeza e reset do buffer
            if (rollUp > 1)
            {
                free(consoleBuffer);
                consoleBuffer = calloc(bufferSize, sizeof(char));
                if (consoleBuffer == NULL)
                {
                    perror("Erro ao alocar memória");
                    return; // Sai do loop e encerra o programa
                }
            }
            else
            {
                memset(consoleBuffer, 0, bufferSize);
            }
        }
    }
}