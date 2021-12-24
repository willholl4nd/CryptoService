#ifndef GRAPH_H
#define GRAPH_H
#include "graph.h"
#endif


GraphService::GraphService(int width, int height, char *title) : gr(0, width, height){
    gr.NewFrame();
    gr.Title(title);
    gr.Box();
    gr.Label('x', "Minutes", 0);
    gr.Label('y', "Prices", 0);
    gr.Axis();
    this->title = title;
};


GraphService::~GraphService() {
    gr.Finish();
};


void GraphService::constructGraph(cryptoPrices c) {
    size_t size = c.size(); //The number of data points
    //Stores the x and y information in two arrays
    double y[size];
    double x[size];    
    for(size_t i = 0; i < size; i++) {
        y[i] = c.at(i);
        x[i] = i;
    }

    //Creates usable data for the graph class
    mglData yy(y, size);
    mglData xx(x, size);

    gr.SetRanges(xx, yy);
    gr.Plot(xx, yy, "r"); //Plots the data

    //Create the file name
    char temp[strlen(title)+5];
    strcpy(temp, title);
    strcat(temp, ".png");

    //Writes the graph to a png
    gr.WritePNG(temp);
    gr.Finish();
};
