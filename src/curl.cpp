#ifndef CURL_H
#define CURL_H
#include "curl.h"
#endif

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;

    struct curl_data *d = (struct curl_data*) userp;

    char *temp = (char*)realloc(d->data, d->size+realsize+1);

    //Checks that the realloc call succeeded
    if(temp == NULL) {
        fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback\n");
        free(d->data);
        exit(1);
    }

    d->data = temp;

    //Stores all of the data inside the userp struct
    memcpy(&(d->data[d->size]), contents, realsize);
    d->size += realsize;
    d->data[d->size] = 0;

    return realsize;
}

void CurlService::runCurlRequest(char *URL, char *fileName) {
    //Initialize the curl handler
    CURL *c = curl_easy_init();
    if (c != NULL) {
        CURLcode res;
        curl_easy_setopt(c, CURLOPT_URL, URL);

        //Open the file where we are going to store the request contents
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

        //Setup the writeback function and run the curl request
        curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &cd);


        res = curl_easy_perform(c);
        //Check if the curl request failed
        if(res != 0) {
            fprintf(stderr, "ERROR: Failed to perform curl request\n");
            fclose(f);
            remove(fileName);
            free(cd.data);
            exit(1);
        }

        //Clean up allocated data and close the file
        fwrite(cd.data, 1, cd.size, f);
        fclose(f);
        free(cd.data);
        curl_easy_cleanup(c);
    }
}

