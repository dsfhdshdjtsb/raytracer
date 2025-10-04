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
#include "../../src/file/parser.h"

int main() {
    World w;
    
    // Cornell Box - Floor (white)
    std::shared_ptr<Shape> floor = std::make_shared<Plane>();
    floor->material.set_color(Color(1, 1, 1));
    floor->material.diffuse = 0.8;
    floor->material.ambient = 0.1;
    floor->material.specular = 0;
    
    // Cornell Box - Ceiling (white)
    std::shared_ptr<Shape> ceiling = std::make_shared<Plane>();
    ceiling->set_transform(Translation(0, 10, 0) * Rotation_x(M_PI));
    ceiling->material.set_color(Color(1, 1, 1));
    ceiling->material.diffuse = 0.8;
    ceiling->material.ambient = 0.1;
    ceiling->material.specular = 0;
    
    // Cornell Box - Back wall (white)
    std::shared_ptr<Shape> back_wall = std::make_shared<Plane>();
    back_wall->set_transform(Translation(0, 0, 10) * Rotation_x(M_PI/2));
    back_wall->material.set_color(Color(1, 1, 1));
    back_wall->material.diffuse = 0.8;
    back_wall->material.ambient = 0.1;
    back_wall->material.specular = 0;
    
    // Cornell Box - Left wall (red)
    std::shared_ptr<Shape> left_wall = std::make_shared<Plane>();
    left_wall->set_transform(Translation(-5, 0, 0) * Rotation_z(M_PI/2));
    left_wall->material.set_color(Color(1, 0, 0));
    left_wall->material.diffuse = 0.8;
    left_wall->material.ambient = 0.1;
    left_wall->material.specular = 0;
    
    // Cornell Box - Right wall (green)  
    std::shared_ptr<Shape> right_wall = std::make_shared<Plane>();
    right_wall->set_transform(Translation(5, 0, 0) * Rotation_z(-M_PI/2));
    right_wall->material.set_color(Color(0, 1, 0));
    right_wall->material.diffuse = 0.8;
    right_wall->material.ambient = 0.1;
    right_wall->material.specular = 0;
    
    Parser parser;
    std::shared_ptr<Group> teapot = parser.parse("../renders/assets/teapot2.obj");
    
    // Position teapot in center of Cornell box
    teapot->set_transform(Translation(0, 1, 7) * Scaling(0.7, 0.7, 0.7));
     
    Material teapot_material;
    teapot_material.set_color(Color(0.8, 0.8, 0.9));
    teapot_material.diffuse = 0.6;
    teapot_material.ambient = 0.1;
    teapot_material.specular = 0.4;
    teapot_material.reflective = 0.2;
    teapot_material.shininess = 50;
    
    for (auto& shape : teapot->shapes) {
        shape->set_material(teapot_material);
    }
    
    w.objects = {floor, ceiling, back_wall, left_wall, right_wall, teapot};
    
    // Light source on the roof, in front of the teapot
    w.light = PointLight(Color(1.0, 0.95, 0.85), Point(0, 8.0, 0));
    
    // Camera positioned to view horizontally into the Cornell box
    w.camera = Camera(400, 400, M_PI / 3);
    w.camera.transform = ViewTransform(Point(0, 5, -3), Point(0, 5, 5), Vector(0, 1, 0));
    
    Canvas image = w.render();
    image.canvasToPPM();
    
    return 0;
}