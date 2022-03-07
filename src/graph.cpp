#ifndef GRAPH_H
#define GRAPH_H
#include "graph.h"
#include <cmath>
#endif


GraphService::GraphService(int width, int height, char *title) : gr(0, width, height){
    gr.SubPlot(1,1,0);
    gr.Title(title);
    gr.SetPlotFactor(1.75);
    gr.Box("", false);
    this->title = title;
};


GraphService::~GraphService() {
    gr.Finish();
};


void GraphService::constructGraph(cryptoInfo c) {
    size_t size = c.prices.size(); //The number of data points
    //Stores the x and y information in two arrays
    double y[size];
    double x[size];    
    for(size_t i = 0; i < size; i++) {
        y[i] = c.prices.at(i);
        x[i] = i;
    }

    //Creates usable data for the graph class
    mglData yy(y, size);
    mglData xx(x, size);

    gr.SetRanges(xx, yy);
    gr.Plot(xx, yy, "r"); //Plots the data

    double diffX = size;
    double diffY = c.max - c.min;
    double stepX = diffX / 14;
    double stepY = diffY / 10;

    gr.SetTicks('y', stepY, 0, NAN, "");
    gr.SetTicks('x', stepX, 0, NAN, "");
    gr.Axis();

    gr.Label('x', "Time", 0, "value .25");
    gr.Label('y', "Price", 0, "value .3");

    //Create the file name
    char temp[strlen(title)+5];
    strcpy(temp, title);
    strcat(temp, ".png");

    //Writes the graph to a png
    gr.WritePNG(temp);
    gr.Finish();
};
