#include "shape.h"
#include "ray.h"
#include "math.h"
#include <math.h>

Intersections::Iterator::Iterator(SetIter pos_it, SetIter pos_end, SetIter neg_it, SetIter neg_end)
    : pos_it(pos_it), pos_end(pos_end), neg_it(neg_it), neg_end(neg_end) {}

Intersection Intersections::Iterator::operator*() const {
    return pos_it != pos_end ? *pos_it : *neg_it;
}

Intersections::Iterator& Intersections::Iterator::operator++() {
    if(pos_it != pos_end) {
        ++pos_it;
    } else {
        ++neg_it;
    }
    return *this;
}

bool Intersections::Iterator::operator!=(const Iterator& other) const {
    return pos_it != other.pos_it || neg_it != other.neg_it;
}

Intersections::Iterator Intersections::begin() const {
    return Iterator(pos.begin(), pos.end(), neg.begin(), neg.end());
}

Intersections::Iterator Intersections::end() const {
    return Iterator(pos.end(), pos.end(), neg.end(), neg.end());
}


Material::Material(Tuple c, float a, float d, float sp, float sh) : color(c), ambient(a), diffuse(d), specular(sp), shininess(sh) {}

Material::Material() : color(Color(1,1,1)), ambient(0.1), diffuse(0.9), specular(0.9), shininess(200) {}

Shape::Shape() : material(Material()), transform(IDENTITY_MATRIX) {}

Intersection::Intersection(float t, std::shared_ptr<Shape> c): t(t), object(c) {}

Intersection::Intersection(const Intersection& other) : t(other.t), object(other.object) {}

Computations Intersection::prepare_computations(const Ray& r) const {
    Tuple point = r.position(this->t);
    Tuple eyev = -r.direction;
    Tuple normalv = this->object->normal_at(point);
    bool inside = false;
    if(normalv.dot(eyev) < 0) {
        inside = true;
        normalv = -normalv;
    } 
    Tuple op = point + normalv * (EPSILON + 0.001); //this is bad
    Computations res(this->t, this->object, point, op, eyev, normalv, inside);
    return res;
}

Computations::Computations(float t, std::shared_ptr<Shape> object, Tuple point, Tuple over_point, Tuple eyev, Tuple normalv, bool inside) : t(t), object(object), point(point), over_point(over_point), eyev(eyev), normalv(normalv), inside(inside) {}

bool Intersection::operator==(const Intersection& other) const {
    if(t - other.t >= EPSILON) return false;
    return *object == *other.object;
}

Intersections::Intersections(std::initializer_list<Intersection> list) {
    for(Intersection i : list) {
        if(i.t >= 0) {
            pos.insert(i);
        } else {
            neg.insert(i);
        }
    }
}
Intersections::Intersections(std::vector<Intersection> list) {
    for(Intersection i : list) {
        if(i.t >= 0) {
            pos.insert(i);
        } else {
            neg.insert(i);
        }
    }
}

Intersections::Intersections() {}

void Intersections::insert(const Intersection& is) {
    if(is.t >= 0) pos.insert(is);
    else neg.insert(is);
}

int Intersections::size() const {
    return pos.size() + neg.size();
}

Intersection Intersections::hit() {
    return *pos.begin();
}
bool Intersections::has_hit() const{
    return !pos.empty();
}

bool IntersectionComparator::operator()(const Intersection& a, const Intersection& b) const {
    return a.t < b.t;
}

bool Shape::operator==(const Shape& other) const { return false;};
std::vector<float> Shape::intersect(const Ray& r) const {return {};}
Tuple Shape::normal_at(const Tuple& point) const { return Tuple(0,0,0,0);}

void Shape::set_material(Material mat) {
    material = mat;
}

void Shape::set_transform(const Matrix& t) {
    transform = transform * t;
}

Sphere::Sphere() : r(1), center(Point(0,0,0)) {
    transform = IDENTITY_MATRIX;
}

bool Sphere::operator==(const Shape& other) const {
    const Sphere* s = dynamic_cast<const Sphere*>(&other);
    if (!s) return false;
    return std::abs(r - s->r) < EPSILON && center == s->center;
}


std::vector<float> Sphere::intersect(const Ray& ray) const {
    Ray transformed = ray.transform(transform.inverse());
    Tuple transformed_origin = transformed.origin;
    Tuple transformed_direction = transformed.direction;

    Tuple sphere_to_ray = transformed_origin - center;

    float a = transformed_direction.dot(transformed_direction);
    float b = 2 * transformed_direction.dot(sphere_to_ray);
    float c = sphere_to_ray.dot(sphere_to_ray) - r * r;

    float discriminant = b * b - 4 * a * c;
    if(discriminant < 0) {
        return {};
    }

    float t1 = ( -b - sqrt(discriminant)) / (2 * a);
    float t2 = ( -b + sqrt(discriminant)) / (2 * a);

    return {t1, t2};
}

Tuple Sphere::normal_at(const Tuple& point) const {
    Tuple object_point = transform.inverse() * point;
    
    Tuple object_normal = object_point - center; //center is always (0,0,0) I think
    Tuple normal = transform.inverse().T() * object_normal;
    normal.w = 0; //avoid weirdness if transform matrix has translation
    return normal.normalize();
}
    
Plane::Plane() {
    transform = IDENTITY_MATRIX;
}

std::vector<float> Plane::intersect(const Ray& r) const {
    Ray transformed = r.transform(transform.inverse());

    if(abs(transformed.direction.y) - 0 < EPSILON) {
        return {};
    }

    float t = - transformed.origin.y / transformed.direction.y;
    return {t};
}

Tuple Plane::normal_at(const Tuple& point) const {
    Tuple normal = transform.inverse().T() * Vector(0,1,0);
    normal.w = 0;
    return normal.normalize();
}


Tuple Material::lighting(PointLight light, Tuple point, Tuple eyev, Tuple normalv, bool in_shadow) const {
    
    Tuple effective_color = color * light.intensity;

    Tuple diffuse_color, specular_color, ambient_color;
    ambient_color = effective_color * ambient;
    if(in_shadow) {
        return ambient_color;
    }

    Tuple lightv = (light.position - point).normalize();
    float light_dot_normal = (lightv.dot(normalv));
    if (light_dot_normal < 0) {
        diffuse_color = Color(0,0,0);
        specular_color = Color(0,0,0);
    } else {
        diffuse_color = effective_color * diffuse * light_dot_normal;
         
        Tuple reflectv = (-lightv).reflect(normalv);
        float reflect_dot_eye = reflectv.dot(eyev);
        if(reflect_dot_eye <= 0) {
            specular_color = Color(0,0,0);
        } else {
            float factor = pow(reflect_dot_eye, shininess);
            specular_color = light.intensity * specular * factor;

        }
    }
    return ambient_color + diffuse_color + specular_color;

}