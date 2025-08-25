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

Tuple World::shade_hit(const Computations& comp, int remaining) const {
    bool shadow = is_shadowed(comp.over_point);

    Tuple surface = comp.object->material.lighting(this->light, comp.object, comp.over_point, comp.eyev, comp.normalv, shadow);
    Tuple reflect = reflected_color(comp, remaining);
    Tuple refract = refracted_color(comp, remaining);
    remaining--;
    Material material = comp.object->material;

    if(material.reflective > 0 && material.transparency > 0) {
        double reflectance = comp.schlick();
        return surface + reflect * reflectance + refract * (1 - reflectance);
    }
    return surface + reflect + refract;
}

bool World::is_shadowed(Tuple point) const {
    Tuple dir = (light.position - point).normalize();
    double mag = (light.position - point).magnitude();

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
Tuple World::reflected_color(const Computations& comp, int remaining) const {
    if(comp.object->material.reflective == 0 || remaining <= 0) {
        return Color(0,0,0);
    }

    Ray reflectr = Ray(comp.over_point, comp.reflectv);
    Tuple c = color_at(reflectr, remaining);

    return c * comp.object->material.reflective;
}

Tuple World::refracted_color(const Computations& comp, int remaining) const {
    //todo with wifi: refractance total internal reflectionn understanding
    if(comp.object->material.transparency == 0 || remaining <= 0) {
        return Color(0,0,0);
    }
    double ratio = comp.n1 / comp.n2;
    double cos_i = comp.eyev.dot(comp.normalv);
    double sin2_t = ratio * ratio * (1 - cos_i * cos_i);
    if(sin2_t > 1) return Color(0,0,0);

    double cos_t = sqrt(1.0 - sin2_t);

    Tuple direction = comp.normalv * (ratio * cos_i - cos_t) - comp.eyev * ratio;

    Ray refract_ray = Ray(comp.under_point, direction);

    Tuple color = color_at(refract_ray, 5) * comp.object->material.transparency;
    return color;
}

Tuple World::color_at(const Ray& r, int remaining) const {
    Intersections rs = intersect_world(r);
    if(!rs.has_hit()) { //might have to deal with negatives?
        return Color(0,0,0);
    }
    Intersection hit = rs.hit(); 
    Computations comps = hit.prepare_computations(r, rs);
    return shade_hit(comps, remaining);
}

Canvas World::render() const {
    Canvas canvas(camera.hsize, camera.vsize);

    for(int i = 0; i < camera.hsize; i++) {
        for(int j = 0; j < camera.vsize; j++) {
            Ray r = camera.ray_for_pixel(i, j);
            Tuple color = this->color_at(r, 5); //5 IS THE TOTAL NUMBER OF REFLECTIONS ALLOWED !!!
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
    Material m(Color(0.8, 1, 0.6), 0.1, 0.7, 0.2, 200, 0, 0, 1);
    s1.set_material(m);

    Sphere s2;
    s2.set_transform(Scaling(0.5, 0.5, 0.5));

    w.objects.push_back(std::make_shared<Sphere>(s2));
    w.objects.push_back(std::make_shared<Sphere>(s1));

    return w;
}