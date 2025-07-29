#ifndef CANVAS_INCLUDE
#define CANVAS_INCLUDE

#include <vector>
#include "../utils/math.h"
#include <fstream>

struct Canvas {
    int height, width;
    std::vector<std::vector<Tuple>> buffer;
    
    Canvas(int width, int height);

    void writePixel(int x, int y, const Tuple& color);
    void canvasToPPM();
};

#endif