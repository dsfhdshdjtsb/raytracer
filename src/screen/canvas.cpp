#include <vector>
#include "canvas.h"
#include "../utils/math.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <algorithm>

Canvas::Canvas(int width, int height) {
    this->height = height;
    this->width = width;
    buffer = std::vector<std::vector<Tuple>>(height, std::vector<Tuple>(width));
}

void Canvas::writePixel(int x, int y, const Tuple& color) {
    buffer[y][x] = color;
}

int digits(int d) {
    if(d >= 100 ) {
        return 3;
    } else if(d >= 10) {
        return 2; 
    } else {
        return 1;
    }
}

void Canvas::canvasToPPM() {
    std::ofstream output("out.ppm");
    output << "P3\n" << width << " " << height << "\n" << "255\n";
    for(auto& vec : buffer) {
        int curLen = 0;
        for(Tuple& color : vec) {

            int red = std::clamp((int) (color.x * 255 + 0.5), 0, 255);
            int green = std::clamp((int) (color.y * 255 + 0.5), 0, 255);
            int blue = std::clamp((int) (color.z * 255 + 0.5), 0, 255);
            
            int len = digits(red) + 1 + digits(green) + 1 + digits(blue) + 1;
            if(curLen + len > 70) {
                output << "\n";
                curLen = 0;
            }
            curLen += len;
            output << red << " " << green << " " << blue << " ";
        }
        output << "\n";
    }
    output.close();
}
