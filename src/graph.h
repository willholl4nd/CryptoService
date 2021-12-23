#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class GraphService {
    public:
        //FIELDS

        //METHODS
        GraphService(int width, int height, char *title);
        ~GraphService();
        void constructGraph(cryptoPrices c);

    private: 
        //FIELDS
        mglGraph gr;
        char *title;

        //METHODS
};
