#ifndef JSON_H 
#define JSON_H
#include "json.h"
#endif

project_json JsonService::getProject_json() {
    return this->pj;
}

JsonService::JsonService(char *filename) {
    this->filename = filename;
    this->pj = projectSettings(filename);
};

void JsonService::print_project_json(project_json pj) {
    printf("API_key = %s\n", pj.apiKey);
    printf("Hostame = %s\n", pj.hostname);
    printf("Database = %s\n", pj.database);
    printf("Username = %s\n", pj.username);
    printf("Password = %s\n", pj.password);
    printf("Table count = %ld\n", pj.tableCount);
    for(int i = 0; i < pj.tableCount; i++) 
        printf("Symbol: %s, Table: %s, columns: %s\n", pj.symbols[i], pj.tables[i], pj.columns[i]);
    printf("Email length = %ld\n", pj.emaillength);
    for(int i = 0; i < pj.emaillength; i++) 
        printf("Emails: %s\n", pj.emails[i]);
}

JsonService::~JsonService() {
    //Frees all of the memory for the project_json
    free(this->pj.apiKey);
    free(this->pj.hostname);
    free(this->pj.database);
    free(this->pj.username);
    free(this->pj.password);
    for(int i = 0; i < pj.tableCount; i++) {
        free(this->pj.symbols[i]);
        free(this->pj.tables[i]);
        free(this->pj.columns[i]);
    }
    free(this->pj.symbols);
    free(this->pj.tables);
    free(this->pj.columns);
    if(pj.emaillength > 0) {
        for(int i = 0; i < pj.emaillength; i++) {
            free(this->pj.emails[i]);
        }
        free(this->pj.emails);
    }
};

void JsonService::moveJSONFile(char *filename) {
    char actFilename[512];
    sprintf(actFilename, "./%s", filename);

    //Grabs the current time the program is 
    //running where there was an error
    time_t now = time(0);
    struct tm *tm = localtime(&now);
    char newPath[512];
    int month, day, year, hour, min;

    month = tm->tm_mon;
    day = tm->tm_mday;
    year = tm->tm_year+1900;
    hour = tm->tm_hour;
    min = tm->tm_min;

    //Creates a new filename for the file that has an error in it
    sprintf(newPath, "./backups/%d\\%d\\%d-%d:%02d-%s", month, day, year, hour, min, filename);
    //Renames the file, which in turn moves it to the backups folder
    int res = rename(actFilename, newPath);
    if(res != 0) {
        fprintf(stderr, "ERROR: Failed to rename file %s to %s\n", filename, newPath);
        fprintf(stderr, "ERROR NUMBER: %d\n", errno);
    }
};

double JsonService::grabPrice(char *filename) {
    //The first json_object from the file
    char actFilename[256];
    sprintf(actFilename, "./%s", filename);
    json_object *jso = json_object_from_file(actFilename);
    if(jso == NULL) {
        //Print error and save file to backup folder
        fprintf(stderr, "ERROR: Failed to open json_object from file %s\n", filename);
        moveJSONFile(filename);
        json_object_put(jso);
        exit(1);
    }

    //Get data object
    json_object *data = NULL;
    int res = json_object_object_get_ex(jso, "data", &data);
    if(res == 0) {
        //Print error and save file to backup folder
        fprintf(stderr, "ERROR: Data obj is NULL\n");
        moveJSONFile(filename);
        json_object_put(jso);
        exit(1);
    }

    //Get index 0 from array inside data
    json_object *arr = json_object_array_get_idx(data, 0);
    if(arr == NULL) {
        //Print error and save file to backup folder
        fprintf(stderr, "ERROR: Arr obj is NULL\n");
        moveJSONFile(filename);
        json_object_put(jso);
        exit(1);
    }

    //Grabs the price of the cryptocurrency
    json_object *price = NULL;
    res = json_object_object_get_ex(arr, "price", &price);
    if(res == 0) {
        //Print error and save file to backup folder
        fprintf(stderr, "ERROR: Price obs is NULL\n");
        moveJSONFile(filename);
        json_object_put(jso);
        exit(1);
    }
    //The price from the json file
    double p = json_object_get_double(price);

    //Free the initial json_object 
    json_object_put(jso);
    return p;
};

void JsonService::strzcpy(char *dest, const char* src, size_t len) {
    strncpy(dest, src, len);
    dest[len] = '\0';
};

project_json JsonService::projectSettings(char *filename) {
    //Create the original json object from the file
    json_object *jso = json_object_from_file(filename);
    if(jso == NULL) {
        //Print error and save file to backup folder
        fprintf(stderr, "ERROR: Failed to open json_object from file %s\n", filename);
        json_object_put(jso);
        delete this;
        exit(1);
    }

    //Grabs the value of the apikey for the api
    json_object *apikey = NULL;
    json_object_object_get_ex(jso, "APIKey", &apikey);
    if(apikey == NULL) {
        fprintf(stderr, "ERROR: apikey obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    const char *tempKey = json_object_get_string(apikey);

    //Grabs the value of the hostname
    json_object *hostname = NULL;
    json_object_object_get_ex(jso, "Hostname", &hostname);
    if(hostname == NULL) {
        fprintf(stderr, "ERROR: hostname obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    const char *tempHostname = json_object_get_string(hostname);

    //Grabs the value of the name for the database
    json_object *database = NULL;
    json_object_object_get_ex(jso, "Database", &database);
    if(database == NULL) {
        fprintf(stderr, "ERROR: database obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    const char *tempDatabase = json_object_get_string(database);

    //Grabs all of the values from the "Symbols" array in the json
    json_object *symbols = NULL;
    json_object_object_get_ex(jso, "Symbols", &symbols);
    if(symbols == NULL) {
        fprintf(stderr, "ERROR: Symbols obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    size_t symbollength = json_object_array_length(symbols);
    char **tempsymbols = (char**)malloc(sizeof(char*) * symbollength);
    for(size_t i = 0; i < symbollength; i++) {
        json_object *temp = json_object_array_get_idx(symbols, i);
        int tempsize = json_object_get_string_len(temp);
        tempsymbols[i] = (char*)calloc(tempsize+1, sizeof(char));
        strzcpy(tempsymbols[i], json_object_get_string(temp), tempsize);
    }

    //Grabs all of the values from the "Tables" array in the json
    json_object *tables = NULL;
    json_object_object_get_ex(jso, "Tables", &tables);
    if(tables == NULL) {
        fprintf(stderr, "ERROR: Tables obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    size_t tablelength = json_object_array_length(tables);
    char **temptables = (char**)malloc(sizeof(char*) * tablelength);
    for(size_t i = 0; i < tablelength; i++) {
        json_object *temp = json_object_array_get_idx(tables, i);
        int tempsize = json_object_get_string_len(temp);
        temptables[i] = (char*)calloc(tempsize+1, sizeof(char));
        strzcpy(temptables[i], json_object_get_string(temp), tempsize);
    }

    //Grabs all of the values from the "Columns" array in the json
    json_object *columns = NULL;
    json_object_object_get_ex(jso, "Columns", &columns);
    if(columns == NULL) {
        fprintf(stderr, "ERROR: Columns obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    size_t columnlength = json_object_array_length(columns);
    char **tempcolumns = (char**)malloc(sizeof(char*) * columnlength);
    for(size_t i = 0; i < columnlength; i++) {
        json_object *temp = json_object_array_get_idx(columns, i);
        int tempsize = json_object_get_string_len(temp);
        tempcolumns[i] = (char*)calloc(tempsize+1, sizeof(char));
        strzcpy(tempcolumns[i], json_object_get_string(temp), tempsize);
    }

    //Grabs the value of the username for the database
    json_object *username = NULL;
    json_object_object_get_ex(jso, "Username", &username);
    if(username == NULL) {
        fprintf(stderr, "ERROR: username obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    const char *tempUsername = json_object_get_string(username);

    //Grabs the value of the password for the database
    json_object *password = NULL;
    json_object_object_get_ex(jso, "Password", &password);
    if(password == NULL) {
        fprintf(stderr, "ERROR: password obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }
    const char *tempPassword = json_object_get_string(password);

    //Grabs the value of the port for the database
    json_object *port = NULL;
    json_object_object_get_ex(jso, "Port", &port);
    if(port == NULL) {
        fprintf(stderr, "ERROR: port obj is NULL\n");
        json_object_put(jso);
        delete this;
        exit(1);
    }

    int tempPort = json_object_get_int(port);

    //Grabs the value of the hour for the email
    json_object *hour = NULL;
    json_object_object_get_ex(jso, "Hour", &hour);
    int tempHour = 0; //By default, at the start of a new day
    if(hour != NULL) {
        tempHour = json_object_get_int(hour);
        if(tempHour > 23 || tempHour < 0) {
            fprintf(stderr, "ERROR: Invalid hour value in project json\nFalling back to default\n");
            tempHour = 0;
        }
    }

    //Grabs the value of the minute for the email
    json_object *minute = NULL;
    json_object_object_get_ex(jso, "Minute", &minute);
    int tempMinute = 0; //By default, at the start of an hour
    if(minute != NULL) {
        tempMinute = json_object_get_int(minute);
        if(tempMinute > 59 || tempMinute < 0) {
            fprintf(stderr, "ERROR: Invalid minute value in project json\nFalling back to default\n");
            tempMinute = 0;
        }
    }
    

    //Grabs all of the values from the "Emails" array in the json
    json_object *emails = NULL;
    json_object_object_get_ex(jso, "Emails", &emails);
    size_t emaillength = 0;
    char **tempemails = NULL;
    if(emails != NULL) {
        emaillength = json_object_array_length(emails);
        tempemails = (char**)malloc(sizeof(char*) * emaillength);
        for(size_t i = 0; i < emaillength; i++) {
            json_object *temp = json_object_array_get_idx(emails, i);
            int tempsize = json_object_get_string_len(temp);
            tempemails[i] = (char*)calloc(tempsize+1, sizeof(char));
            strzcpy(tempemails[i], json_object_get_string(temp), tempsize);
        }
    }

    //Allocates and stores all of the information
    project_json ret = {.symbols = tempsymbols, .tables = temptables, 
        .columns = tempcolumns, .port = tempPort, .hour = tempHour,
        .minute = tempMinute, .emails = tempemails, .emaillength = emaillength, 
        .tableCount = tablelength};

    ret.apiKey = (char*)calloc(strlen(tempKey)+1, sizeof(char));
    ret.hostname = (char*)calloc(strlen(tempHostname)+1, sizeof(char));
    ret.database = (char*)calloc(strlen(tempDatabase)+1, sizeof(char));
    ret.username = (char*)calloc(strlen(tempUsername)+1, sizeof(char));
    ret.password = (char*)calloc(strlen(tempPassword)+1, sizeof(char));

    //Makes sure all of the characters are being stored properly
    strzcpy(ret.apiKey, tempKey, strlen(tempKey));
    strzcpy(ret.hostname, tempHostname, strlen(tempHostname));
    strzcpy(ret.database, tempDatabase, strlen(tempDatabase));
    strzcpy(ret.username, tempUsername, strlen(tempUsername));
    strzcpy(ret.password, tempPassword, strlen(tempPassword));

    //Frees the json_object to stop memory leaks 
    json_object_put(jso);
    return ret;
};







