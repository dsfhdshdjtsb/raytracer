#include "world.h"
#include "../utils/math.h"
#include "../utils/ray.h"
#include "../utils/shape.h"

World::World() : light(Color(1,1,1), Point(-10,10, -10)) {

    Sphere s1;
    Material m(Color(0.8, 1, 0.6), 0.1, 0.7, 0.2, 200);
    s1.set_material(m);

    Sphere s2;
    s2.set_transform(Scaling(0.5, 0.5, 0.5));

    objects.push_back(std::make_shared<Sphere>(s1));
    objects.push_back(std::make_shared<Sphere>(s2));
}

Intersections World::intersect_world(Ray& r) const {
    Intersections result;
    for(std::shared_ptr<Shape> shape : objects) {
        
        const Intersections& is = r.intersect(shape);
        std::cout << is.size() <<std::endl;
        for(const Intersection& i : is)  {
            result.insert(i);
        }
    }
    return result;
}

