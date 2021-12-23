#ifndef CURL_H
#define CURL_H
#include "curl.h"
#endif

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;

    struct curl_data *d = (struct curl_data*) userp;

    char *temp = (char*)realloc(d->data, d->size+realsize+1);

    if(temp == NULL) {
        fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback\n");
        free(d->data);
        exit(1);
    }

    d->data = temp;

    memcpy(&(d->data[d->size]), contents, realsize);
    d->size += realsize;
    d->data[d->size] = 0;

    return realsize;
}

void CurlService::runCurlRequest(char *URL, char *fileName) {
    CURL *c = curl_easy_init();
    if (c != NULL) {
        CURLcode res;
        curl_easy_setopt(c, CURLOPT_URL, URL);

        char actFilename[256];
        sprintf(actFilename, "./%s", fileName);
        FILE *f = fopen(actFilename, "w");
        if(f == NULL) {
            fprintf(stderr, "ERROR: Failed to open file %s\n", fileName);
            exit(1);
        }

        struct curl_data cd;
        cd.data = (char*)calloc(0, sizeof(cd.data));
        cd.size = 0;

        curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &cd);


        res = curl_easy_perform(c);
        if(res != 0) {
            fprintf(stderr, "ERROR: Failed to perform curl request\n");
            fclose(f);
            remove(fileName);
            free(cd.data);
            exit(1);
        }

        fwrite(cd.data, 1, cd.size, f);
        fclose(f);
        free(cd.data);
        curl_easy_cleanup(c);
    }
}

