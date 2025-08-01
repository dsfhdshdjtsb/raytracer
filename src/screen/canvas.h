#ifndef CANVAS_INCLUDE
#define CANVAS_INCLUDE

#include <vector>
#include "../utils/math.h"
#include <fstream>

struct Canvas {
    int height, width;
    std::vector<std::vector<Tuple>> buffer;
    
    Canvas(int width, int height);

    std::vector<Tuple>& operator[](int i);
    const std::vector<Tuple>& operator[](int i ) const;

    void canvasToPPM();
};

#endif