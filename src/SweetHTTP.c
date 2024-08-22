#include "SweetHTTP.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

// Variável global para indicar quando o servidor deve ser fechado
static bool g_closing = 0;
wchar_t *configFile = L"http.conf";

void HTTP_processClientRequest(char *data, uint64_t size, struct SweetSocket_global_context *ctx, struct SweetSocket_peer_clients *thisClient, void *parms);

static void HTTP_handleSigint(int sig)
{
    (void)sig;
    g_closing = true;
}

struct HTTP_upper_server_hosts
{
    struct SweetSocket_global_context *context;
    struct HTTP_server_config *server;
};

struct HTTP_upper_server_ports
{
    struct HTTP_upper_server_hosts *up;
    wchar_t *host;
};

static enum HTTP_linked_list_actions HTTP_ports(struct HTTP_object *actual, void *parms, uint64_t count)
{
    (void)count;
    struct HTTP_upper_server_ports *up = (struct HTTP_upper_server_ports *)parms;
    struct SweetSocket_global_context *context = up->up->context;
    uint16_t *port = (uint16_t *)actual->object;
    uint8_t type = 0;

    // Converter o endereço do host de wide string para string
    size_t len = wcslen(up->host) + 1;
    char *host = (char *)malloc(len);
    if (!host)
    {
        perror("Failed to allocate memory for host");
        return ARRAY_STOP; // Interrompe a execução em caso de erro
    }
    wcstombs(host, up->host, len);

    // Determinar o tipo de socket com base no endereço do host
    if (strchr(host, ':') != NULL)
    {
        type = AF_INET6;
    }
    else if (strchr(host, '.') != NULL)
    {
        type = AF_INET;
    }

    // Criar e adicionar a conexão ao contexto
    SweetSocket_pushNewConnection(&context->connections, SweetSocket_createPeer(context, type, host, *port));
    free(host);
    return ARRAY_CONTINUE;
}

static enum HTTP_linked_list_actions HTTP_hosts(struct HTTP_object *actual, void *parms, uint64_t count)
{
    (void)count;
    struct HTTP_upper_server_hosts *up = (struct HTTP_upper_server_hosts *)parms;
    struct HTTP_upper_server_ports upPort = {up, (wchar_t *)actual->object};

    HTTP_arrayForEach(&up->server->ports, HTTP_ports, &upPort);
    return ARRAY_CONTINUE;
}

static void HTTP_deamonize()
{
    wchar_t *commandLine = GetCommandLineW();
    for (wchar_t *p = commandLine; *p; p++)
    {
        if (*p == L'-' && *(p + 1) == L'd')
        {
            *(p + 1) = L'#';
            break;
        }
    }
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    // Cria o processo
    if (!CreateProcessW(
            NULL,             // Módulo a ser executado
            commandLine,      // Linha de comando
            NULL,             // Atributos de segurança do processo
            NULL,             // Atributos de segurança da thread
            FALSE,            // Herança de handles
            DETACHED_PROCESS, // Flag para criar um processo detached
            NULL,             // Bloco de ambiente
            NULL,             // Diretório de trabalho
            &si,              // Informações de inicialização
            &pi               // Informações do processo
            ))
    {
        fprintf(stderr, "Failed to create process: %lu\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    // Fechar handles do processo e thread do processo filho
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Terminar o processo pai
    exit(EXIT_SUCCESS);
}

static void HTTP_arguments(int argc, char *argv[])
{
    bool daemonize = false;
    for (int i = 1; i < argc; i++)
    {
        int len = strlen(argv[i]);
        if (len == 2)
        {
            switch (argv[i][1])
            {
            case 'c':
                if (i + 1 < argc)
                {
                    configFile = (wchar_t *)malloc(strlen((argv[i + 1]) + 1) * sizeof(wchar_t));
                    if (!configFile)
                    {
                        perror("Failed to allocate memory for config file");
                        exit(1);
                    }
                    mbstowcs(configFile, argv[i + 1], strlen(argv[i + 1]) + 1);
                    i++;
                }
                break;
            case 'd':
                daemonize = true;
                break;
            case 'h':
                printf(
                    "SweetHTTP - A sweet HTTP server\n"
                    "Usage: SweetHTTP [options]\n"
                    "Options:\n"
                    "-c <config file>   Set the configuration file\n"
                    "-d                 Daemonize the server\n"
                    "-h                 Display this help message\n"
                    "-v                 Display the version of the server\n\n"
                    "by default server try to load http.conf and save log as http.log\n");
                exit(0);
            case 'v':
                printf("SweetHTTP version %s\n", "NULL");
                exit(0);
            default:
                break;
            }
            continue;
        }
        printf("Invalid usage\n");
        exit(1);
    }
    if (daemonize)
        HTTP_deamonize();
}

int main(int argc, char *argv[])
{
    // Verificação de argumentos
    if (argc > 1)
        HTTP_arguments(argc, argv);
    // Inicialização do ambiente do servidor
    struct HTTP_server_envolvirment envolviment = {0};
    envolviment.server = HTTP_loadConfig(configFile);
    envolviment.context = SweetSocket_initGlobalContext(PEER_SERVER);
    envolviment.context->useHeader = false;

    // Carregamento de MIME types e plugins
    HTTP_loadMimeTypes(&envolviment);
    HTTP_loadPlugins(&envolviment);

    // Configuração de hosts e portas
    struct HTTP_upper_server_hosts up = {envolviment.context, &envolviment.server};
    HTTP_arrayForEach(&envolviment.server.hosts, HTTP_hosts, &up);

    // Início do servidor
    if (!SweetSocket_serverStartListening(envolviment.context, APPLY_ALL))
    {
        SweetSocket_closeGlobalContext(&envolviment.context);
        perror("Failed to start listening");
        return 1;
    }

    // Aceitação de conexões e processamento de requisições
    SweetSocket_serverStartAccepting(envolviment.context, APPLY_ALL, NULL, &HTTP_processClientRequest, &envolviment, NULL, ONLY_RECIVE);
    signal(SIGINT, HTTP_handleSigint);
    wprintf(L"Press Ctrl+C to stop\n");

    // Loop principal do servidor
    while (!g_closing)
    {
        SweetThread_sleep(1000); // Pausa por 1 segundo
    }

    // Fechamento do servidor
    SweetSocket_closeGlobalContext(&envolviment.context);
    return 0;
}
