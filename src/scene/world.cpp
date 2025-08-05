#include "world.h"
#include <vector>
#include <memory>
#include "../utils/math.h"
#include "../utils/ray.h"
#include "../utils/shape.h"
#include "../utils/light.h"
#include "../screen/canvas.h"
#include "../scene/camera.h"

World::World() {}

Tuple World::shade_hit(const Computations& comp) const {
    bool shadow = is_shadowed(comp.over_point);
    return comp.object->material.lighting(this->light, comp.object, comp.over_point, comp.eyev, comp.normalv, shadow);
}

bool World::is_shadowed(Tuple point) const {
    Tuple dir = (light.position - point).normalize();
    float mag = (light.position - point).magnitude();

    Ray r(point, dir);
    Intersections rs = intersect_world(r);
    if(rs.has_hit()) {
        Intersection hit = rs.hit();
        if(hit.t < mag) {
            return true;
        } 
    } 
    return false;
}

Tuple World::color_at(const Ray& r) const {
    Intersections rs = intersect_world(r);
    if(!rs.has_hit()) { //might have to deal with negatives?
        return Color(0,0,0);
    }
    Intersection hit = rs.hit(); 
    Computations comps = hit.prepare_computations(r);
    return shade_hit(comps);
}

Canvas World::render() const {
    Canvas canvas(camera.hsize, camera.vsize);

    for(int i = 0; i < camera.hsize; i++) {
        for(int j = 0; j < camera.vsize; j++) {
            Ray r = camera.ray_for_pixel(i, j);
            Tuple color = this->color_at(r);
            canvas[j][i] = color;
        }
    }
    return canvas;
}

Intersections World::intersect_world(const Ray& r) const {
    Intersections result;
    for(std::shared_ptr<Shape> shape : objects) {
        
        const Intersections& is = r.intersect(shape);
        for(const Intersection& i : is)  {
            result.insert(i);
        }
    }
    return result;
}

World DefaultWorld() {
    World w;
    w.light = PointLight(Color(1,1,1), Point(-10, 10, -10));

    Sphere s1;
    Material m(Color(0.8, 1, 0.6), 0.1, 0.7, 0.2, 200);
    s1.set_material(m);

    Sphere s2;
    s2.set_transform(Scaling(0.5, 0.5, 0.5));

    w.objects.push_back(std::make_shared<Sphere>(s2));
    w.objects.push_back(std::make_shared<Sphere>(s1));

    return w;
}