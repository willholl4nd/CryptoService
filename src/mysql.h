#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class MysqlService {
    public:
        //FIELDS
        MYSQL *connection = mysql_init(NULL);

        //METHODS

        /**
         * Selects all of the prices from table (tablename)
         * and treats the prices as values from column (columnname)
         * @params
         * tablename: the name of the table that we're selecting from
         * columnname: the name of the column that the prices are stored in
         * @return the vector containing all of the prices for that day
         */
        cryptoPrices selectPricesFromTable(char *tablename, char *columnname);

        /**
         * Insert new data into the table
         * @params
         * tablename: the name of the table we are inserting into
         * columnname: the name of the column that stores the prices info
         * price: the price to insert
         */
        void insertData(char *tablename, char *columnname, double price); 

        /**
         * Deletes data from the database tables that is older than 
         * 30 days old.
         * @params
         * tablename: the table to delete the old data from
         */
        void deleteDataFromTables(char *tablename);

        /**
         * The constructor for the class that initializes the connection to the 
         * database
         * @params
         * pj: the project settings 
         */
        MysqlService(project_json pj);

        /**
         * Closes the connection upon destructing the class variable
         */
        ~MysqlService();
};
