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
void moveJSONFile(char *fileName) {
        char actFilename[512];
        sprintf(actFilename, "./%s", fileName);
        time_t now = time(0);
        struct tm *tm = localtime(&now);
        char newPath[512];
        int month, day, year, hour, min;
        month = tm->tm_mon;
        day = tm->tm_mday;
        year = tm->tm_year+1900;
        hour = tm->tm_hour;
        min = tm->tm_min;

        sprintf(newPath, "./backups/%d\\%d\\%d-%d:%02d-%s", month, day, year, hour, min, fileName);
        int res = rename(actFilename, newPath);
        if(res != 0) {
                fprintf(stderr, "ERROR: Failed to rename file %s to %s\n", fileName, newPath);
                fprintf(stderr, "ERROR NUMBER: %d\n", errno);
        }
}

/**
 * Uses the json library to parse the json files from the 
 * curl response to grab the price information from the two
 * crypto currencies: BTC and ETH.
 */
double grabPrice(char *fileName) {
        char actFilename[256];
        sprintf(actFilename, "./%s", fileName);
        json_object *jso = json_object_from_file(actFilename);
        if(jso == NULL) {
                //Print error and save file to backup folder
                fprintf(stderr, "ERROR: Failed to open json_object from file %s\n", fileName);
                moveJSONFile(fileName);
                json_object_put(jso);
                exit(1);
        }

        //Get data object
        json_object *data = NULL;
        int res = json_object_object_get_ex(jso, "data", &data);
        if(res == 0) {
                //Print error and save file to backup folder
                fprintf(stderr, "ERROR: Data obj is NULL\n");
                moveJSONFile(fileName);
                json_object_put(jso);
                exit(1);
        }

        //Get index 0 from array inside data
        json_object *arr = json_object_array_get_idx(data, 0);
        if(arr == NULL) {
                //Print error and save file to backup folder
                fprintf(stderr, "ERROR: Arr obj is NULL\n");
                moveJSONFile(fileName);
                json_object_put(jso);
                exit(1);
        }

        //Grabs the price of the cryptocurrency
        json_object *price = NULL;
        res = json_object_object_get_ex(arr, "price", &price);
        if(res == 0) {
                //Print error and save file to backup folder
                fprintf(stderr, "ERROR: Price obs is NULL\n");
                moveJSONFile(fileName);
                json_object_put(jso);
                exit(1);
        }
        double p = json_object_get_double(price);

        json_object_put(jso);
        return p;
}

/**
 * This is the function that is called every time the curl_easy_perform()
 * is called. It ensures that the data is being saved despite the size of 
 * the data that is sent back in response from our call to the API.
 */
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t realsize = size * nmemb;

        struct curl_data *d = (struct curl_data *) userp;

        char *temp = realloc(d->data, d->size+realsize+1);

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

/**
 * Handles the curl requests of each cryptocurrency and saves to their respective file.
 */
void runCurlRequest(char *URL, char *fileName) {
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
                cd.data = (char *) calloc(0, sizeof(cd.data));
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

/**
 * Inserts the prices of ETH and BTC into their respective database table.
 */
void insert(MYSQL *conn, double ETHPrice, double BTCPrice) {
        char *query1 = calloc(64, sizeof(char));
        sprintf(query1, "insert into ETH_prices values (0,%f);", ETHPrice);
        int res1 = mysql_real_query(conn, query1, strlen(query1));
        //Checking for success
        if(res1 != 0) {
                fprintf(stderr, "ERROR: Failed to execute query %s\n", query1);
                free(query1);
                exit(1);
        }

        char *query2 = calloc(64, sizeof(char));
        sprintf(query2, "insert into BTC_prices values (0,%f);", BTCPrice);
        int res2 = mysql_real_query(conn, query2, strlen(query2));
        //Checking for success
        if(res2 != 0) {
                fprintf(stderr, "ERROR: Failed to execute query %s\n", query2);
                free(query1);
                free(query2);
                exit(1);
        }
        free(query1);
        free(query2);
}

/**
 * The function that is used to grab min, max, and avg of the 
 * database tables for ETH and BTC.
 */
double DBfetch(MYSQL *conn, char *query) {
        double ret;

        int code = mysql_real_query(conn, query, strlen(query));
        if(code != 0) {
                fprintf(stderr, "ERROR: Failed to execute query %s\n", query);
                exit(1);
        }
        MYSQL_RES *res = mysql_use_result(conn);
        MYSQL_ROW data = mysql_fetch_row(res);
        ret = atof(data[0]);
        mysql_free_result(res);

        return ret;
}

/**
 * Inserts data into the database tables and requests the 
 * information for the daily email.
 */
struct priceAvg insertAndHLA(MYSQL *conn, double ETHPrice, double BTCPrice) {
        char *query1 = calloc(64, sizeof(char));
        sprintf(query1, "insert into ETH_prices values (0,%f);", ETHPrice);
        int res1 = mysql_real_query(conn, query1, strlen(query1));
        //Checking for success
        if(res1 != 0) {
                fprintf(stderr, "ERROR: Failed to execute query %s\n", query1);
                free(query1);
                exit(1);
        }

        char *query2 = calloc(64, sizeof(char));
        sprintf(query2, "insert into BTC_prices values (0, %f);", BTCPrice);
        int res2 = mysql_real_query(conn, query2, strlen(query2));
        //Checking for success
        if(res2 != 0) {
                fprintf(stderr, "ERROR: Failed to execute query %s\n", query2);
                free(query1);
                free(query2);
                exit(1);
        }

        free(query1);
        free(query2);

        double ETHAvg = DBfetch(conn, "select avg(price) from ETH_prices;");
        double BTCAvg = DBfetch(conn, "select avg(price) from BTC_prices;");
        double ETHHigh = DBfetch(conn, "select max(price) from ETH_prices;");
        double BTCHigh = DBfetch(conn, "select max(price) from BTC_prices;");
        double ETHLow = DBfetch(conn, "select min(price) from ETH_prices;");
        double BTCLow = DBfetch(conn, "select min(price) from BTC_prices;");
        double ETHEnd = DBfetch(conn, "select price from ETH_prices order by id desc limit 1;");
        double BTCEnd = DBfetch(conn, "select price from BTC_prices order by id desc limit 1;");

        struct priceAvg ret = {ETHAvg, ETHHigh, ETHLow, ETHEnd, BTCAvg, BTCHigh, BTCLow, BTCEnd};
        return ret;
}

/**
 * Deletes all of the data from the database tables at the 
 * end of the day to get fresh data for the next day.
 */
void deleteDataFromTables(MYSQL *conn) {
        char *q1 = calloc(64, sizeof(char));
        sprintf(q1, "delete from ETH_prices");
        int res1 = mysql_real_query(conn, q1, strlen(q1));
        //Checking for success
        if(res1 != 0) {
                fprintf(stderr, "ERROR: Failed to perform data deletion\n");
                free(q1);
                exit(1);
        }
        free(q1);

        char *q2 = calloc(64, sizeof(char));
        sprintf(q2, "delete from BTC_prices");
        int res2 = mysql_real_query(conn, q2, strlen(q2));
        //Checking for success
        if(res2 != 0) {
                fprintf(stderr, "ERROR: Failed to perform data deletion\n");
                free(q1);
                free(q2);
                exit(1);
        }
        free(q2);

        char *q3 = calloc(128, sizeof(char));
        sprintf(q3, "alter table ETH_prices auto_increment = 0");
        int res3 = mysql_real_query(conn, q3, strlen(q3));
        //Checking for success
        if(res3 != 0) {
                fprintf(stderr, "ERROR: Failed to reset auto_increment\n");
                free(q1);
                free(q2);
                free(q3);
                exit(1);
        }
        free(q3);

        char *q4 = calloc(128, sizeof(char));
        sprintf(q4, "alter table BTC_prices auto_increment = 0");
        int res4 = mysql_real_query(conn, q4, strlen(q4));
        //Checking for success
        if(res4 != 0) {
                fprintf(stderr, "ERROR: Failed to reset auto_increment\n");
                free(q1);
                free(q2);
                free(q3);
                free(q4);
                exit(1);
        }
        free(q4);
}

void APIKeyGrab(char *filename, char *output) {
        FILE *f = fopen(filename, "r");
        if(f == NULL) {
                fprintf(stderr, "ERROR: Failed to retrieve the API key\n");
                fclose(f);
                exit(1);
        }

        char *ret = calloc(128, sizeof(char));
        fgets(ret, 128, f);
        fclose(f);

        //Grabs the API key past the '=' sign
        char *del = "\n";
        ret = strtok(ret, del);
        del = "=";
        strtok(ret, del);
        strcpy(output, strtok(NULL, del));
        free(ret);
}

void constructEmail(struct priceAvg avgs, char *emailAddress) {
        char emailPath[256];
        sprintf(emailPath, "./email.txt");
        char to[64];
        sprintf(to, "%s", emailAddress);
        char subject[] = "Daily Report: CryptoService";
        char body[512];
        FILE *f = fopen(emailPath, "w");
        if(f == NULL){
                fprintf(stderr, "ERROR: Failed to open email.txt file\n");
                fclose(f);
                exit(1);
        }

        sprintf(body, "---ETH---\nHigh: %.2f\nLow: %.2f\nAverage(24h): %.2f\nEnd price: %.2f\n\n"
                        "---BTC---\nHigh: %.2f\nLow: %.2f\nAverage(24h): %.2f\nEnd price: %.2f", 
                        avgs.ETHHigh, avgs.ETHLow, avgs.ETHAvg, avgs.ETHEnd, avgs.BTCHigh, 
                        avgs.BTCLow, avgs.BTCAvg, avgs.BTCEnd);
        char email[1024];
        sprintf(email, "To: %s\nSubject: %s\n%s", to, subject, body);
        fprintf(f, "%s", email);
        fclose(f);

        char cmd[512];
        sprintf(cmd, "sendmail -vt < %s;", emailPath);
        system(cmd);
}

void strzcpy(char *dest, const char* src, size_t len) {
        strncpy(dest, src, len);
        dest[len] = '\0';
}

struct project_json projectSettings(char *fileName) {

        json_object *jso = json_object_from_file(fileName);
        if(jso == NULL) {
                //Print error and save file to backup folder
                fprintf(stderr, "ERROR: Failed to open json_object from file %s\n", fileName);
                json_object_put(jso);
                exit(1);
        }

        json_object *apikey = NULL;
        json_object_object_get_ex(jso, "APIKey", &apikey);
        if(apikey == NULL) {
                fprintf(stderr, "ERROR: apikey obj is NULL\n");
                json_object_put(jso);
                exit(1);
        }
        const char *tempKey = json_object_get_string(apikey);
        
        json_object *database = NULL;
        json_object_object_get_ex(jso, "Database", &database);
        if(database == NULL) {
                fprintf(stderr, "ERROR: database obj is NULL\n");
                json_object_put(jso);
                exit(1);
        }
        const char *tempDatabase = json_object_get_string(database);

        json_object *username = NULL;
        json_object_object_get_ex(jso, "Username", &username);
        if(username == NULL) {
                fprintf(stderr, "ERROR: username obj is NULL\n");
                json_object_put(jso);
                exit(1);
        }
        const char *tempUsername = json_object_get_string(username);

        json_object *password = NULL;
        json_object_object_get_ex(jso, "Password", &password);
        if(password == NULL) {
                fprintf(stderr, "ERROR: password obj is NULL\n");
                json_object_put(jso);
                exit(1);
        }
        const char *tempPassword = json_object_get_string(password);

        json_object *port = NULL;
        json_object_object_get_ex(jso, "Port", &port);
        if(port == NULL) {
                fprintf(stderr, "ERROR: port obj is NULL\n");
                json_object_put(jso);
                exit(1);
        }

        int tempPort = json_object_get_int(port);

        struct project_json ret = {.port = tempPort};
        
        ret.apiKey = calloc(strlen(tempKey)+1, sizeof(char));
        ret.database = calloc(strlen(tempDatabase)+1, sizeof(char));
        ret.username = calloc(strlen(tempUsername)+1, sizeof(char));
        ret.password = calloc(strlen(tempPassword)+1, sizeof(char));

        strzcpy(ret.apiKey, tempKey, strlen(tempKey));
        strzcpy(ret.database, tempDatabase, strlen(tempDatabase));
        strzcpy(ret.username, tempUsername, strlen(tempUsername));
        strzcpy(ret.password, tempPassword, strlen(tempPassword));

        json_object_put(jso);
        return ret;
}

int main(int argc, char **argv) {
        if(argc < 2) {
                fprintf(stderr, "ERROR: Failed to find email address to email\n"
                        "Please supplement an email through program arguments\n");
                return 1;
        }
        char *emailAddress = argv[1];

        char *BTCFileName = "BTCresp.json";
        char *ETHFileName = "ETHresp.json";
        char *projectSettingFileName = "project.json";

        struct project_json pj = projectSettings(projectSettingFileName);
        char BTCURL[256];
        sprintf(BTCURL, "https://api.lunarcrush.com/v2?data=assets&key=%s&symbol=BTC", pj.apiKey);
        char ETHURL[256];
        sprintf(ETHURL, "https://api.lunarcrush.com/v2?data=assets&key=%s&symbol=ETH", pj.apiKey);

        runCurlRequest(BTCURL, BTCFileName);
        runCurlRequest(ETHURL, ETHFileName);

        double BTCPrice = grabPrice(BTCFileName);
        double ETHPrice = grabPrice(ETHFileName);

        MYSQL *connection = mysql_init(NULL);
        if(mysql_real_connect(connection, NULL, pj.username, pj.password, 
                                pj.database, pj.port, NULL, 0) == NULL) {
                fprintf(stderr, "ERROR: Failed to connect to mariadb server\n");
                exit(1);
        }

        time_t now = time(0);
        struct tm *timeinfo = localtime(&now);
        if(timeinfo->tm_hour == 0 && timeinfo->tm_min == 0) {
                struct priceAvg avgs = insertAndHLA(connection, ETHPrice, BTCPrice);
                deleteDataFromTables(connection);

                constructEmail(avgs, emailAddress);
                
        } else {
                insert(connection, ETHPrice, BTCPrice);
        }

        //Final steps to clean up memory
        mysql_close(connection);
        free(pj.apiKey);
        free(pj.database);
        free(pj.username);
        free(pj.password);

        return 0;
}
