#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include "../../src/utils/light.h"
#include "../../src/scene/world.h"
#include "../../src/utils/math.h"
#include "../../src/utils/ray.h"
#include "../../src/utils/shape.h"
#include "../../src/scene/camera.h"
#include "../../src/screen/canvas.h"


int main() {
    World w;
    std::shared_ptr<Shape> floor, left_wall, right_wall, middle, right, left;

    floor = std::make_shared<Plane>();
    left_wall = std::make_shared<Plane>();
    right_wall = std::make_shared<Plane>();
    middle = std::make_shared<Sphere>();
    right = std::make_shared<Sphere>();
    left = std::make_shared<Sphere>();

    w.objects = {floor, middle, right, left};

    std::shared_ptr<Pattern> pat = std::make_shared<Checkers>(Color(0.9,0.3,0.5), Color(0,0.6,0.8));
    std::shared_ptr<Pattern> pat2 = std::make_shared<Gradient>(Color(0,0.8,0.7), Color(0.6,0.0,0.6));
    pat2->transform = Scaling(2,2,2) * Translation(0.5,0,0);

    floor->material = Material();
    //floor->material.set_color(Color(1,0.9,0.9));
    floor->material.pattern = pat;
    floor->material.specular = 0;
    floor->material.reflective = 1;

    left_wall->transform = Translation(0, 0, 5) * Rotation_y(- M_PI / 4) * Rotation_x(M_PI / 2);
    left_wall->material = floor->material;

    right_wall->transform = Translation(0,0,5) * Rotation_y(M_PI / 4) * Rotation_x(M_PI / 2);
    right_wall->material = floor->material;

    middle->transform = Translation(-0.5, 1, 0.5);
    middle->material = Material();
    //middle->material.set_color(Color(0.1, 1, 0.5));
    middle->material.pattern = pat2;
    middle->material.diffuse = 0.7;
    middle->material.specular = 0.3;

    right->transform = Translation(1.5, 0.5, -0.5) * Scaling(.5, 0.5,.5);
    right->material = Material();
    right->material.set_color(Color(0.5, 1, 0.1));
    right->material.diffuse = 0.7;
    right->material.specular = 0.3;

    left->transform = Translation(-1.5, 0.33, -0.75) * Scaling(0.33, 0.33, 0.33);
    left->material = Material();
    left->material.set_color(Color(1, 0.8, 0.1));
    left->material.diffuse = 0.7;
    left->material.specular = .3;

    w.light = PointLight(Color(1,1,1), Point(-10, 10, -10));

    w.camera = Camera(300, 150, M_PI / 3);
    w.camera.transform = ViewTransform(Point(0, 1.5, -5),  Point(0,1,0), Vector(0,1,0)) * Translation(0,0, 3);

    Canvas image = w.render();
    image.canvasToPPM();
}