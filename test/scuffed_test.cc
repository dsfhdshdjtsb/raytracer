#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include "../src/utils/light.h"
#include "../src/scene/world.h"
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"
#include "../src/scene/camera.h"
#include "../src/screen/canvas.h"
#include "../src/utils/pattern.h"

int main() { 
    World w = DefaultWorld();
    
    std::shared_ptr<Shape> plane = std::make_shared<Plane>();

    plane->transform = Translation(0,-1,0);
    plane->material.transparency = 0.5;
    plane->material.refractive_index= 1.5;
    w.objects.push_back(plane);

    std::shared_ptr<Shape> ball = std::make_shared<Sphere>();
    ball->material.set_color(Color(1,0,0));
    ball->material.ambient = 0.5;
    ball->transform = Translation(0, -3.5, -0.5);

    w.objects.push_back(ball);

    Ray r = Ray(Point(0,0,-3), Vector(0, -1, 1).normalize());
    Intersections xs = {Intersection(sqrt(2),plane )};
    Computations comps = xs.hit().prepare_computations(r, xs);

    Tuple color = w.shade_hit(comps, 5);

    std::cout << color << std::endl; 
    //EXPECT_EQ(Color(0.93642, 0.68642, 0.68642), color);
}