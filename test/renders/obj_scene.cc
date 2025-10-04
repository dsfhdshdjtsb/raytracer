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
    
    std::shared_ptr<Shape> floor = std::make_shared<Plane>();
    floor->material.set_color(Color(1, 0.9, 0.9));
    floor->material.specular = 0;
    
    Parser parser;

    std::string simple_obj = 
        "# Simple tetrahedron\n"
        "v 0.0 1.0 0.0\n"      
        "v -1.0 -1.0 -1.0\n"  
        "v 1.0 -1.0 -1.0\n"
        "v 0.0 -1.0 1.0\n"
        "f 1 2 3\n"            
        "f 1 3 4\n"            
        "f 1 4 2\n"
        "f 2 4 3\n";           
    
    std::shared_ptr<Group> teapot = parser.parse("../renders/assets/teapot-low.obj");

    teapot->set_transform( Rotation_x(-M_PI/2) * Translation(0,1,0) * Scaling(0.1,0.1,0.1) );

    Material model_material;
    model_material.set_color(Color(0.6, 0.6, 0.65));
    model_material.diffuse = 0.12;
    model_material.ambient = 0.1;
    model_material.specular = 0.3;
    model_material.reflective = 0.7;
    model_material.shininess = 100;

    
    for (auto& shape : teapot->shapes) { //bad but fast
        shape->set_material(model_material);
    }
    std::cout << "done setting mat" << std::endl;
    
    w.objects = {floor, teapot};
    
    w.light = PointLight(Color(1, 1, 1), Point(-10, 10, -10));
    
    w.camera = Camera(40, 20, M_PI / 3);
    w.camera.transform = ViewTransform(Point(0, 1.5, -5), Point(0, 1, 0), Vector(0, 1, 0));
    
    Canvas image = w.render();
    image.canvasToPPM();
    
    return 0;
}
