#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class CurlService {

    public:
        void runCurlRequest(char *url, char *filename);
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
