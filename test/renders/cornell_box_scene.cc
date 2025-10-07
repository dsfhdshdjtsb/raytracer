#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <iomanip>
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
    back_wall->set_transform(Translation(0, 0, 11) * Rotation_x(M_PI/2));
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
    
    //diagonal plasticky cube to put teapot on top of
    std::shared_ptr<Shape> cube = std::make_shared<Cube>();
    cube->set_transform(
        Translation(0, 0, 8) * 
        Rotation_y(M_PI/4) * 
        Scaling(1.5, 2, 1.5)
    );
    cube->material.set_color(Color(0.1, 0.15, 0.4));
    cube->material.diffuse = 0.7;
    cube->material.ambient = 0.1;
    cube->material.specular = 0.3;
    cube->material.shininess = 200;
    cube->material.reflective = 0.5;
    cube->material.refractive_index = 1.5;
    cube->material.transparency = 0.0;

    Parser parser;
    std::shared_ptr<Group> teapot = parser.parse("../renders/assets/teapot3.obj"); 
    teapot->set_transform( Translation(0, 2, 8) * Rotation_y(0) * Scaling(0.04, 0.04, 0.04));


    Material teapot_material;
    teapot_material.set_color(Color(0.373, 0.404, 0.550));
    teapot_material.diffuse = 0.2;
    teapot_material.ambient = 0.0;
    teapot_material.specular = 1.0;
    teapot_material.reflective = 0.7;
    teapot_material.transparency = 0.7;
    teapot_material.refractive_index = 1.5; 
    teapot_material.shininess = 200;

    for (auto& shape : teapot->shapes) {
        shape->set_material(teapot_material);
    }
    
    // std::shared_ptr<Shape> tri1 = std::make_shared<SmoothTriangle>(
    //     Point(-1,0.5,1), Point(0,1,0), Point(0,0,0),
    //     Vector(-1,0,0),   // Left vertex: normal pointing left
    //     Vector(0,1,0),    // Top vertex: normal pointing up  
    //     Vector(1,0,0)     // Bottom vertex: normal pointing right
    // );
    // std::shared_ptr<Shape> tri2 = std::make_shared<SmoothTriangle>(
    //     Point(1,0.5,1), Point(0,1,0), Point(0,0,0),
    //     Vector(1,0,0),    // Right vertex: normal pointing right
    //     Vector(0,1,0),    // Top vertex: normal pointing up
    //     Vector(-1,0,0)    // Bottom vertex: normal pointing left  
    // );
    // std::shared_ptr<Group> tris = std::make_shared<Group>();
    // tris->add_child(tri1);
    // tris->add_child(tri2);
    // tris->set_transform(Translation(0,3,8) * Scaling(2,2,2));
    // Material tri_material;
    // tri_material.set_color(Color(0.9, 0.9, 0.1));
    // tri_material.diffuse = 0.7;
    // tri_material.ambient = 0.4;
    // tri_material.specular = 0.3;
    // tri_material.shininess = 200;
    // tri_material.reflective = 0.0;
    // tri1->set_material(tri_material);
    // tri2->set_material(tri_material);


    // std::shared_ptr<Shape> tri3 = std::make_shared<Triangle>(
    //     Point(-1,0.5,1), Point(0,1,0), Point(0,0,0)
    // );
    // std::shared_ptr<Shape> tri4 = std::make_shared<Triangle>(
    //     Point(1,0.5,1), Point(0,1,0), Point(0,0,0)
    // );
    // std::shared_ptr<Group> tris2 = std::make_shared<Group>();
    // tris2->add_child(tri3);
    // tris2->add_child(tri4);
    // tris2->set_transform(Translation(0,0,8) * Scaling(2,2,2));
    // tri3->set_material(tri_material);
    // tri4->set_material(tri_material);

    w.objects = {floor, ceiling, back_wall, left_wall, right_wall, cube, teapot };

    w.light = PointLight(Color(1.0, 0.95, 0.85), Point(0, 8.0, 0));
    
    w.camera = Camera(600, 600, M_PI / 3);
    w.camera.transform = ViewTransform(Point(0, 5, -3), Point(0, 5, 5), Vector(0, 1, 0));
    
    Canvas image = w.render();
    image.canvasToPPM();

    
    return 0;
}