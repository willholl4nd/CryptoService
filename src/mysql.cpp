#ifndef MYSQL_H 
#define MYSQL_H 
#include "mysql.h"
#endif

MysqlService::MysqlService(project_json pj) {
    //Some type of error occurred
    if(mysql_real_connect(this->connection, pj.hostname, pj.username,
                pj.password, pj.database, pj.port, NULL, 0) == NULL) {
        fprintf(stderr, "ERROR: Failed to connect to mariadb server\n");
        exit(1);
    }
};

MysqlService::~MysqlService() {
    mysql_close(connection);
};

void MysqlService::deleteDataFromTables(char *tablename) {
    //Stores the query for the delete statement
    char *q1 = (char*)calloc(64, sizeof(char));
    sprintf(q1, "delete from %s where time_created < NOW() - INTERVAL 30 DAY;", tablename);
    int res1 = mysql_real_query(connection, q1, strlen(q1));
    //Checking for success
    if(res1 != 0) {
        fprintf(stderr, "ERROR: Failed to perform data deletion\n");
        free(q1);
        delete this;
        exit(1);
    }
    free(q1);
};

void MysqlService::insertData(char *tablename, char *columnname, double price) {
    //Stores the query for the insert statement
    char *query = (char*)calloc(256, sizeof(char));
    sprintf(query, "insert into %s(%s) values (%f);", tablename, columnname, price);
    int result = mysql_real_query(connection, query, strlen(query));
    //Checks for success
    if(result != 0) {       
        fprintf(stderr, "ERROR: Failed to execute query %s\n", query);
        free(query);
        delete this;
        exit(1);
    }
};

cryptoPrices MysqlService::selectPricesFromTable(char *tablename, char *columnname = (char*)"price") {
    //Stores the query for the select statement
    char *query = (char*)calloc(256, sizeof(char));
    sprintf(query, "select %s from %s where time_created > NOW() - INTERVAL 1 DAY;", columnname, tablename);
    int res = mysql_real_query(connection, query, strlen(query));
    //Checks for success
    if (res != 0) {
        fprintf(stderr, "ERROR: Failed to execute query %s\n", query);
        free(query);
        delete this;
        exit(1);
    }

    MYSQL_RES *result = mysql_use_result(connection);
    cryptoPrices c;
    char **row;
    //Loops through the result set and stores the price data 
    while((row = mysql_fetch_row(result)) != NULL) {
        double val = atof(row[0]);
        c.push_back(val);
    }

    c.shrink_to_fit();
    mysql_free_result(result);

    return c;
};

