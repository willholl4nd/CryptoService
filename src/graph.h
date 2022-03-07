#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"
#endif

class GraphService {
    public:
        //FIELDS

        //METHODS
        
        /**
         * The constructor for the graph class that does some basic setup for 
         * each graph png
         * @params
         * width: the width of each graph picture
         * height: the height of each graph picture
         * title: the title of the graph 
         */
        GraphService(int width, int height, char *title);

        /**
         * The destructor for the graph class that finishes the graph 
         */
        ~GraphService();

        /**
         * This method constructs the graph from the data provided
         * @params
         * c: contains the data from the cryptocurrency
         */
        void constructGraph(cryptoInfo c);

    private: 
        //FIELDS
        mglGraph gr;
        char *title;

        //METHODS
};
