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
        JsonService js;
        EmailService em;
        MysqlService mq;
        CurlService cs;

        //METHODS
        void calculateInfo(cryptoInfo *c);
        double findAvg(cryptoPrices p);

        /**
         * The constructor for the Service class
         * filename is the project.json file 
         * that holds all of the information about the mysql database 
         * and tables that we will be using.
         */
        Service(char *filename, char *email);

};

Service::Service(char *filename, char *email) : js(filename), mq(js.getProject_json()), em(email){
    time_t now = time(0);
    struct tm *timeinfo = localtime(&now);
    project_json pj = js.getProject_json();

    double currentPrice[pj.tableCount];
    char resp[] = "resp.json";
    for(size_t i = 0; i < pj.tableCount; i++) {
        int size = strlen(resp) + strlen(pj.tables[i]) + 1;
        char filename[size];
        strcat(filename, pj.symbols[i]);
        strcat(filename, resp);

        char URL[256];
        sprintf(URL, "https://api.lunarcrush.com/v2?data=assets&key=%s&symbol=%s", pj.apiKey, pj.symbols[i]);

        cs.runCurlRequest(URL, filename);
        currentPrice[i] = js.grabPrice(filename);
        memset(filename, 0, size);
    }

    if(timeinfo->tm_hour == 0 && timeinfo->tm_min == 0) {
        //TODO grab all of the data from the database (done)
        //and delete the database (done)
        //and construct the graph (done)
        //and make the email to send out (done)
        cryptoInfo info[pj.tableCount];
        for(size_t i = 0; i < pj.tableCount; i++) {
            info[i].prices = mq.selectPricesFromTable(pj.tables[i], pj.columns[i]);
            info[i].prices.push_back(currentPrice[i]);
            calculateInfo(&info[i]);

            mq.deleteDataFromTables(pj.tables[i]);
            GraphService gs(2000, 1000, pj.symbols[i]);
            gs.constructGraph(info[i].prices);
        }
        em.constructEmail(info, pj);
    } else {
        //Insert the new data into the database
        for(size_t i = 0; i < pj.tableCount; i++) {
            mq.insertData(pj.tables[i], pj.columns[i], currentPrice[i]);
        }
    }
};

/**
 * Calculates all of the necessary information for the email
 */
void Service::calculateInfo(cryptoInfo *c) {
    cryptoPrices p = c->prices;
    double min = p.at(0), max = p.at(0);
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


/**
 * Calculates the average of the entire dataset 
 */
double Service::findAvg(cryptoPrices p) {
    if (p.empty())
        return 0;

    size_t count = p.size();
    return std::reduce(p.begin(), p.end()) / count;
};


int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "ERROR: Failed to find email address to email\n"
                "Please supplement an email through program arguments\n");
        return 1;
    }
    char *emailAddress = argv[1];
    Service s((char*)"project.json", emailAddress);
    
    return 0;
}
