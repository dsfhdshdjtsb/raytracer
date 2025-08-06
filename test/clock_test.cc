#include "../src/screen/canvas.h"
#include "../src/utils/math.h"
#define _USE_MATH_DEFINES
#include <math.h>

int main() {
    Canvas c(500, 500);

    Tuple origin = Point(250, 250, 0);

    Tuple p = Point(100, 0, 0);
    Tuple red = Color(1, 0, 0);

    
    for(double i = 0; i < 2 * M_PI; i += (M_PI / 6)) {
        Tuple cur = Rotation_z(i) * p + origin; 
        c[floor(cur.x)][floor(cur.y)] = red;
    }
    c.canvasToPPM();
}


