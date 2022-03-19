#include <curl/curl.h>
#include <mariadb/mysql.h>
#include <json-c/json_object.h>
#include <json-c/json_util.h>
#include <json-c/arraylist.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <mgl2/mgl.h>
#include <mgl2/base.h>
#include <mgl2/data.h>

#include <numeric>
#include <vector>

/**
 * Will hold all of the data from that day and perform the operations 
 * that the user would like to happen on that data. Vector is essential
 * an arraylist from java
 */
typedef std::vector<double> cryptoPrices;


/**
 * Holds the calculate stats on the cryptocurrency of interest along 
 * with the dataset
 */
typedef struct {
    cryptoPrices prices;
    double max, min, avg, start, end;
} cryptoInfo;


/**
 * This struct contains the contents of the project.json
 */
typedef struct {
    char *apiKey; //Stores the key for the API
    char *hostname; //The hostname/ip
    char *database; //The name of the database
    char **symbols; //The symbol of each of the cryptocurrency
    char **tables; //The name of each of the table
    char **columns; //The name of each column within their respective table
    char *username; //The username to the mysql database
    char *password; //The password to the mysql database 
    int port; //The port on which the database is running
    int hour; //The hour which the email is sent out
    int minute; //The minute of the hour which the email is sent out
    char **emails; //All email addresses
    std::size_t emaillength; //Number of email addresses
    std::size_t tableCount; //The number of tables we are operating on
} project_json;


/**
 * This is the struct that will hold all of the curl
 * request data, along with the size of the request.
 */
struct curl_data {
    char *data;
    std::size_t size;
};
