#include "../src/screen/canvas.h"
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"
#include "../src/utils/light.h"

int HEIGHT = 500;
int WIDTH = 500;

int main() {
    int half_h = HEIGHT / 2;
    int half_w = WIDTH / 2; 
    Canvas c(HEIGHT, WIDTH);

    Tuple origin = Point(half_h, half_w, -120);
    Tuple color = Color(1,0,0);

    Material mat;
    mat.color = Color(1,1,0.2);
    //mat.shininess = 100;
    PointLight light(Color(1,1,1), Point(-100, 200, -200));

    Sphere sphere;
    sphere.set_transform(Translation(half_h, half_w, 0) * Scaling(100, 100, 100));

    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            Tuple direction = (Point(i, j, 0) - origin).normalize();
            Ray r(origin, direction);

            Tuple eyev = -r.direction;
            Intersections is = r.intersect(sphere);
            if(is.size() == 0) continue;

            Tuple point = r.position(is.hit().t);
            Tuple normalv = sphere.normal_at(point);

            Tuple res = mat.lighting(light, point, eyev, normalv);

            c[i][j] = res;
        }
    }
    
    c.canvasToPPM();
}