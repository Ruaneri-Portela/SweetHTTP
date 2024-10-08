#ifndef HTTP_PROCESS
#define HTTP_PROCESS
#include <stdbool.h>
#include <stdint.h>
#include "SweetHTTP_config.h"
struct HTTP_request
{
	char* header;
	char* data;
	uint64_t dataSize;
	wchar_t* filePath;
	wchar_t* virtualPath;
	wchar_t* getContent;
	char* verb;
	char* userAgent;
	char* host;

	bool useRange;
	int64_t startRange, endRange;
	struct HTTP_server_envolvirment* envolvirment;
};
#endif