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
    
    std::shared_ptr<Group> obj_model = parser.parse_string(simple_obj);
    
    obj_model->set_transform(Translation(0, 1, 0) * Scaling(0.5, 0.5, 0.5) * Rotation_y(1 / 3.0 * M_PI));
     
    Material model_material;
    model_material.set_color(Color(0.8, 0.2, 0.9));
    model_material.diffuse = 0.7;
    model_material.specular = 0.3;
    
    for (auto& shape : obj_model->shapes) {
        shape->set_material(model_material);
    }
    
    w.objects = {floor, obj_model};
    
    w.light = PointLight(Color(1, 1, 1), Point(-10, 10, -10));
    
    w.camera = Camera(400, 200, M_PI / 3);
    w.camera.transform = ViewTransform(Point(0, 1.5, -5), Point(0, 1, 0), Vector(0, 1, 0));
    
    Canvas image = w.render();
    image.canvasToPPM();
    
    return 0;
}
