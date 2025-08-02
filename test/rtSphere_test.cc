#include "../src/screen/canvas.h"
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"

int HEIGHT = 500;
int WIDTH = 500;

int main() {
    int half_h = HEIGHT / 2;
    int half_w = WIDTH / 2; 
    Canvas c(HEIGHT, WIDTH);

    Tuple origin = Point(half_h, half_w, -10);
    Tuple color = Color(1,0,0);

    Sphere sphere;
    sphere.set_transform(Translation(half_h, half_w, 0) * Scaling(80, 100,1) * Shearing(10, 10, 5, 5, 14, 14));

    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            std::cout << i << " " << j << std::endl;
            Tuple direction = Point(i, j, 0) - origin;
            Ray r(origin, direction);

            std::vector<Intersection> vec = r.intersect(sphere);
            if(vec.size() == 0) continue;
            c[i][j] = color;
        }
    }
    
    c.canvasToPPM();
}