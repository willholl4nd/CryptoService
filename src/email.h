#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class EmailService {
    public:
        //FIELDS
        char *email; 

        //METHODS
        EmailService(char *email);
        void constructEmail(cryptoInfo *c, project_json pj);
};
