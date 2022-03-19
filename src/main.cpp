#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

#ifndef CURL_H
#define CURL_H
#include "curl.h"
#endif

#ifndef EMAIL_H
#define EMAIL_H
#include "email.h"
#endif

#ifndef GRAPH_H
#define GRAPH_H
#include "graph.h"
#endif 

#ifndef JSON_H
#define JSON_H
#include "json.h"
#endif 

#ifndef MYSQL_H
#define MYSQL_H
#include "mysql.h"
#endif 


/**
 * This class will handle all of the interactions between the different
 * services required to make this program work
 */
class Service {

    public:
        //FIELDS
        EmailService em;
        MysqlService mq;
        CurlService cs;

        //METHODS

        /**
         * Calculates the average, high, low, start and end prices of 
         * the current cryptocurrency
         * @params
         * c: the entire days worth of data to be worked on 
         */
        void calculateInfo(cryptoInfo *c);

        /**
         * Finds the average of the entire data set using vector math
         * @params 
         * p: the prices for the day 
         * @return the average for the day
         */
        double findAvg(cryptoPrices p);

        /**
         * The constructor for the Service class
         * filename is the project.json file 
         * that holds all of the information about the mysql database 
         * and tables that we will be using.
         * @params
         * pj: the parsed project.json in object form
         * email: the email supplied from the commandline
         */
        Service(JsonService *js, project_json pj, char **email, int numOfEmails);

};

Service::Service(JsonService *js, project_json pj, char **email, int numOfEmails) : mq(pj), em(email, numOfEmails){
    //Grabs the current time
    time_t now = time(0);
    struct tm *timeinfo = localtime(&now);
    
    //Creates an array that will store all of the current prices for this minute
    double currentPrice[pj.tableCount];
    char resp[] = "resp.json";
    //Grabs all of the current prices and stores it
    for(size_t i = 0; i < pj.tableCount; i++) {
        int size = strlen(resp) + strlen(pj.tables[i]) + 20;
        char filename[size];
        sprintf(filename, "%s%s", pj.symbols[i], resp);
        //strcat(filename, pj.symbols[i]);
        //strcat(filename, resp);

        char URL[256];
        sprintf(URL, "https://api.lunarcrush.com/v2?data=assets&key=%s&symbol=%s", pj.apiKey, pj.symbols[i]);

        cs.runCurlRequest(URL, filename);
        currentPrice[i] = js->grabPrice(filename);
        memset(filename, 0, size); //Reset the memory
    }

    //Checks if it is midnight
    if(timeinfo->tm_hour == pj.hour && timeinfo->tm_min == pj.minute) {
        cryptoInfo info[pj.tableCount]; //A table for all of the cryptocurrency info
        for(size_t i = 0; i < pj.tableCount; i++) {
            mq.insertData(pj.tables[i], pj.columns[i], currentPrice[i]);
            info[i].prices = mq.selectPricesFromTable(pj.tables[i], pj.columns[i]);
            calculateInfo(&info[i]);

            mq.deleteDataFromTables(pj.tables[i]);
            GraphService gs(2000, 1250, pj.symbols[i]);
            gs.constructGraph(info[i]);
        }
        em.constructEmail(info, pj);
    } else {
        //Insert the new data into the database
        for(size_t i = 0; i < pj.tableCount; i++) {
            mq.insertData(pj.tables[i], pj.columns[i], currentPrice[i]);
        }
    }
};

void Service::calculateInfo(cryptoInfo *c) {
    cryptoPrices p = c->prices;
    double min = p.at(0), max = p.at(0);
    //Finds the min and max of the entire dataset
    for(size_t i = 0; i < p.size(); i++) {
        if (p.at(i) < min) {
            min = p.at(i);
        } else if(p.at(i) > max) {
            max = p.at(i);
        }
    }
    c->avg = findAvg(p);
    c->max = max;
    c->min = min;
    c->start = p.front();
    c->end = p.back();
};

double Service::findAvg(cryptoPrices p) {
    if (p.empty())
        return 0;

    size_t count = p.size();
    return std::reduce(p.begin(), p.end()) / count;
};


int main(int argc, char **argv) {
    JsonService js((char*)"project.json");
    project_json pj = js.getProject_json();
    if(argc < 2 && pj.emaillength < 1) {
        fprintf(stderr, "ERROR: Failed to find email address(es) to email\n"
                "Please supply email(s) through program arguments or the project.json file\n");
        return 1;
    }

    int numOfEmails = argc - 1;
    char *emails[numOfEmails];
    for(int i = 0; i < argc-1; i++) emails[i] = argv[i+1];

    if(pj.emaillength > 0) {
        for(int i = 0; i < pj.emaillength; i++) {
            emails[i+numOfEmails] = pj.emails[i];
        }
        numOfEmails += pj.emaillength;
    }
    Service s(&js, pj, emails, numOfEmails);
    
    return 0;
}
