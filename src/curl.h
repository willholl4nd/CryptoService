#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class CurlService {

    public:
        //FIELDS
        //METHODS

        /**
         * This function is responsible for running the curl request
         * @params
         * url: The api url that will be used to get data
         * filename: the name of the file that will be written to with 
         * the data from the return of the curl request
         */
        void runCurlRequest(char *url, char *filename);
};

/**
 * This function is outlined by the libcurl api and handles all of the data
 * that the curl request returns. 
 * @params
 * contents: the contents of the curl request that is returned
 * size: the size of the contents 
 * nmemb: the number of members in the contents 
 * userp: the struct that provided for storing information
 *
 * @return the size of the bytes of current contents pointer
 */
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
