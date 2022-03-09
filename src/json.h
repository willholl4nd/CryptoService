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
         * curl response to grab the price information.
         * @params 
         * filename: the file where the price information is stored from the 
         * curl request
         * @return the price that was in the file
         */
        double grabPrice(char *filename);

        /**
         * Getter for project_json
         * @return the project_json struct
         */
        project_json getProject_json();

        void print_project_json(project_json pj);

        /**
         *  The constructor that stores the filename 
         *  and runs the function that grabs the project 
         *  settings
         *  @params
         *  filename: the name of the file where the project settings are stored
         */
        JsonService(char *filename);

        /**
         * Frees all of the memory allocated by the project_json struct 
         */
        ~JsonService();
    private:
        //FIELDS
        project_json pj;

        //METHODS

        /**
         * Does a proper copy from one character pointer to the next and adds 
         * and null-terminating character to the end
         * @params
         * dest: the pointer to where we are copying data to
         * str: the pointer where we are copying from
         * len: the number of bytes that we are copying
         */
        void strzcpy(char *dest, const char* src, size_t len);

        /**
         * Moves the current curl files that hold the jsons
         * and moves them to the backups folder for viewing later
         * to see what parsing error there was.
         * @params
         * filename: the file of the curl request data
         */
        void moveJSONFile(char *filename);

        /**
         * Grabs the mysql database information ie. login, password, port, and apikey
         * @params: the file that stores the information of the project
         * @return the parsed information in a struct 
         */
        project_json projectSettings(char *filename);
};
