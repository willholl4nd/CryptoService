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
         */
        cryptoPrices selectPricesFromTable(char *tablename, char *columnname);

        /**
         * Insert new data into the table
         */
        void insertData(char *tablename, char *columnname, double price); 

        /**
         * Deletes data from the database tables that is older than 
         * 30 days old.
         */
        void deleteDataFromTables(char *tablename);

        MysqlService(project_json pj);
        ~MysqlService();
};
