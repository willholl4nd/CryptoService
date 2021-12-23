#include <curl/curl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <json-c/json_util.h>
#include <json-c/json_object.h>
#include <json-c/json_util.h>
#include <json-c/arraylist.h>
#include <mariadb/mysql.h>

/**
 * Holds the pricing information for ETH and BTC when the email 
 * is being prepared.
 */
struct priceAvg {
        double ETHAvg;
        double ETHHigh;
        double ETHLow;
        double ETHEnd;
        double BTCAvg;
        double BTCHigh;
        double BTCLow;
        double BTCEnd;
};

/**
 * This is the struct that will hold all of the curl
 * request data, along with the size of the request.
 */
struct curl_data {
        char *data;
        size_t size;
};

/**
 * This struct contains the contents of the project.json
 */
struct project_json {
        char *apiKey;
        char *database;
        char *username;
        char *password;
        int port;
};


/**
 * Moves the current curl files that hold the jsons
 * and moves them to the backups folder for viewing later
 * to see what parsing error there was.
 */
void moveJSONFile(char *fileName);


/**
 * Uses the json library to parse the json files from the 
 * curl response to grab the price information from the two
 * crypto currencies: BTC and ETH.
 */
double grabPrice(char *fileName);


/**
 * This is the function that is called every time the curl_easy_perform()
 * is called. It ensures that the data is being saved despite the size of 
 * the data that is sent back in response from our call to the API.
 */
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);


/**
 * Handles the curl requests of each cryptocurrency and saves to their respective file.
 */
void runCurlRequest(char *URL, char *fileName);


/**
 * Inserts the prices of ETH and BTC into their respective database table.
 */
void insert(MYSQL *conn, double ETHPrice, double BTCPrice);


/**
 * The function that is used to grab min, max, and avg of the 
 * database tables for ETH and BTC.
 */
double DBfetch(MYSQL *conn, char *query);


/**
 * Inserts data into the database tables and requests the 
 * information for the daily email.
 */
struct priceAvg insertAndHLA(MYSQL *conn, double ETHPrice, double BTCPrice);


/**
 * Deletes all of the data from the database tables at the 
 * end of the day to get fresh data for the next day.
 */
void deleteDataFromTables(MYSQL *conn);


/**
 * Create the email the entirety of the email and the command to send the 
 * email, and then executes that email command
 */
void constructEmail(struct priceAvg avgs, char *emailAddress);


/**
 * Does a proper copy from one character pointer to the next and adds 
 * and null-terminating character to the end
 */
void strzcpy(char *dest, const char* src, size_t len);


/**
 * Grabs the mysql database information ie. login, password, port, and apikey
 */
struct project_json projectSettings(char *fileName);


/**
 * Handles the mysql connection logistics of what to do when
 */
void mysqlStuff(MYSQL *connection, struct project_json pj, double ETHPrice, double BTCPrice, char *email);


/**
 * Frees the memory of the objects used by the MYSQL and project_json objects
 */
void freeMemory(MYSQL *conn, struct project_json pj);

