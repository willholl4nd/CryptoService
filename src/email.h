#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class EmailService {
    public:
        //FIELDS
        char **email; 
        int numOfEmails;

        //METHODS

        /**
         * The constructor for this class just stores the email in the class variable
         * @params
         * email: the email address we are sending to
         */
        EmailService(char **email, int numOfEmails);

        /**
         * Constructs the email, attaches the graph pictures, and sends the email
         * @params
         * c: the array that stores all of the information on all of the 
         * cryptocurrencies we are tracking
         * pj: the project settings 
         */
        void constructEmail(cryptoInfo c[], project_json pj);
};
