#ifndef HTTP_CONSOLE
#define HTTP_CONSOLE
#include <stdbool.h>
#include "SweetHTTP_config.h"
void SweetHTTP_console(volatile bool *closing,struct HTTP_server_envolvirment *envolviment);
#endif