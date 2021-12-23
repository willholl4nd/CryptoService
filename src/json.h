#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class JsonService {

    public:
        //FIELDS
        char *filename; //This filename has the mysql database information

        //METHODS

        /**
         * Uses the json library to parse the json files from the 
         * curl response to grab the price information from the two
         * crypto currencies: BTC and ETH.
         */
        double grabPrice(char *filename);

        /**
         * Getter for project_json
         */
        project_json getProject_json();

        /**
         * Kinda does the thing
         */
        JsonService(char *filename);

        /**
         * Kinda destroys the thing
         */
        ~JsonService();
    private:
        //FIELDS
        project_json pj;

        //METHODS

        /**
         * Does a proper copy from one character pointer to the next and adds 
         * and null-terminating character to the end
         */
        void strzcpy(char *dest, const char* src, size_t len);

        /**
         * Moves the current curl files that hold the jsons
         * and moves them to the backups folder for viewing later
         * to see what parsing error there was.
         */
        void moveJSONFile(char *filename);

        /**
         * Grabs the mysql database information ie. login, password, port, and apikey
         */
        project_json projectSettings(char *filename);
};
