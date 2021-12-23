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
    size_t size = c.size();
    double y[size];
    double x[size];    
    for(size_t i = 0; i < size; i++) {
        y[i] = c.at(i);
        x[i] = i;
    }

    mglData yy(y, size);
    mglData xx(x, size);

    gr.SetRanges(xx, yy);
    gr.Plot(xx, yy, "r");

    char temp[strlen(title)+5];
    strcpy(temp, title);
    strcat(temp, ".png");
    gr.WritePNG(temp);
    gr.Finish();
};
