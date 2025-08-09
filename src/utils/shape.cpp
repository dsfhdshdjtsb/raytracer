#include "shape.h"
#include "ray.h"
#include "pattern.h"
#include "math.h"
#include <math.h>
#include <list>
#include <algorithm>

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

Material::Material(std::shared_ptr<Pattern> pattern, double a, double d, double sp, double sh, double rf, double tr, double re_i) : ambient(a), diffuse(d), specular(sp), shininess(sh), reflective(rf), transparency(tr), refractive_index(re_i) {
    this->pattern = pattern;
}

Material::Material(const Tuple& c, double a, double d, double sp, double sh, double rf, double tr, double re_i) : ambient(a), diffuse(d), specular(sp), shininess(sh), reflective(rf), transparency(tr), refractive_index(re_i) {
    pattern = std::make_shared<Solid>(c);
}

Material::Material() : ambient(0.1), diffuse(0.9), specular(0.9), shininess(200), transparency(0), refractive_index(1), reflective(0) {
    pattern = std::make_shared<Solid>(Color(1,1,1));
}

Shape::Shape() : material(Material()), transform(IDENTITY_MATRIX) {}

Intersection::Intersection(double t, std::shared_ptr<Shape> c): t(t), object(c) {}

Intersection::Intersection(const Intersection& other) : t(other.t), object(other.object) {}

Computations Intersection::prepare_computations(const Ray& r, const Intersections& xs) const {
    double n1, n2;
    n1 = 1;
    n2 = 1;
    std::list<std::shared_ptr<Shape>> containers;
    std::shared_ptr<Shape> hit;
    hit = this->object;
    for(const Intersection& x : xs) {
        if(x.object == hit && abs(x.t - this->t) < EPSILON)  {
            if(containers.empty()) {
                n1 = 1;
            } else {
                n1 = containers.back()->material.refractive_index;
            }
        }
        auto iter = std::find(containers.begin(), containers.end(), x.object);
        if(iter != containers.end()) {
            containers.erase(iter);
            
        } else {
            containers.insert(containers.end(), x.object);
        }

        if(x.object == hit && abs(x.t - this->t) < EPSILON) {
            if(containers.empty()) {
                n2 = 1;
            } else {
                n2 = containers.back()->material.refractive_index;
            }
            break;
        }
    }
    Tuple point = r.position(this->t);
    Tuple eyev = -r.direction;
    Tuple normalv = this->object->normal_at(point);
    Tuple reflectv = r.direction.reflect(normalv);
    bool inside = false;
    if(normalv.dot(eyev) < 0) {
        inside = true;
        normalv = -normalv;
    } 
    Tuple op = point + normalv * (EPSILON); 
    Tuple up = point - normalv * EPSILON;
    Computations res(this->t, this->object, point, op, eyev, normalv, reflectv, up, inside, n1, n2);
    return res;
}

Computations::Computations(double t, std::shared_ptr<Shape> object, Tuple point, Tuple over_point, Tuple eyev, Tuple normalv, Tuple reflectv, Tuple under_point, bool inside, double n1, double n2) : t(t), object(object), point(point), over_point(over_point), eyev(eyev), normalv(normalv), reflectv(reflectv), under_point(under_point), inside(inside), n1(n1), n2(n2) {}

double Computations::schlick() const {
    double cos_1 = eyev.dot(normalv) ;
    if( n1 > n2) {
        double n = n1 / n2;
        double sin2_t = n * n * (1.0 - cos_1 * cos_1);
        if ( sin2_t > 1.0) {
            return 1.0;
        }
        double cos_t = sqrt(1.0 -sin2_t);
        cos_1 = cos_t;
    }
    double r0 = ((n1 - n2) / (n1 + n2)) * ((n1 - n2) / (n1 + n2));
    //TODO: FIX THIS EXPONENT;
    return r0 + (1 - r0) * (1 - cos_1)* (1 - cos_1)* (1 - cos_1)* (1 - cos_1)* (1 - cos_1);
}
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

Intersection Intersections::hit() const {
    return *pos.begin();
}
bool Intersections::has_hit() const{
    return !pos.empty();
}

bool IntersectionComparator::operator()(const Intersection& a, const Intersection& b) const {
    return a.t < b.t;
}

bool Shape::operator==(const Shape& other) const { return false;};
std::vector<double> Shape::intersect(const Ray& r) const {return {};}
Tuple Shape::normal_at(const Tuple& point) const { return Tuple(0,0,0,0);}

void Shape::set_material(const Material& mat) {
    material = mat;
}
Tuple Shape::color_at(const Tuple& point) const {
    Tuple transformed = material.pattern->transform.inverse() * transform.inverse() * point;
    return material.pattern->color_at(transformed);
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


std::vector<double> Sphere::intersect(const Ray& ray) const {
    Ray transformed = ray.transform(transform.inverse());
    Tuple transformed_origin = transformed.origin;
    Tuple transformed_direction = transformed.direction;

    Tuple sphere_to_ray = transformed_origin - center;

    double a = transformed_direction.dot(transformed_direction);
    double b = 2 * transformed_direction.dot(sphere_to_ray);
    double c = sphere_to_ray.dot(sphere_to_ray) - r * r;

    double discriminant = b * b - 4 * a * c;
    if(discriminant < 0) {
        return {};
    }

    double t1 = ( -b - sqrt(discriminant)) / (2 * a);
    double t2 = ( -b + sqrt(discriminant)) / (2 * a);

    return {t1, t2};
}

Tuple Sphere::normal_at(const Tuple& point) const {
    Tuple object_point = transform.inverse() * point;
    
    Tuple object_normal = object_point - center; //center is always (0,0,0) I think
    Tuple normal = transform.inverse().T() * object_normal;
    normal.w = 0; //avoid weirdness if transform matrix has translation
    return normal.normalize();
}
std::shared_ptr<Shape> GlassSphere() {
    std::shared_ptr<Shape> glass =  std::make_shared<Sphere>();
    glass->material.transparency = 1.0;
    glass->material.refractive_index = 1.5;
    return glass;
} 
Plane::Plane() {
    transform = IDENTITY_MATRIX;
}

std::vector<double> Plane::intersect(const Ray& r) const {
    Ray transformed = r.transform(transform.inverse());

    if(abs(transformed.direction.y) - 0 < EPSILON) {
        return {};
    }

    double t = - transformed.origin.y / transformed.direction.y;
    return {t};
}

Tuple Plane::normal_at(const Tuple& point) const {
    Tuple normal = transform.inverse().T() * Vector(0,1,0);
    normal.w = 0;
    return normal.normalize();
}


Tuple Material::lighting(PointLight light, std::shared_ptr<Shape> object, Tuple point, Tuple eyev, Tuple normalv, bool in_shadow) const {
    
    Tuple effective_color = object->color_at(point) * light.intensity;

    Tuple diffuse_color, specular_color, ambient_color;
    ambient_color = effective_color * ambient;
    if(in_shadow) {
        return ambient_color;
    }

    Tuple lightv = (light.position - point).normalize();
    double light_dot_normal = (lightv.dot(normalv));
    if (light_dot_normal < 0) {
        diffuse_color = Color(0,0,0);
        specular_color = Color(0,0,0);
    } else {
        diffuse_color = effective_color * diffuse * light_dot_normal;
         
        Tuple reflectv = (-lightv).reflect(normalv);
        double reflect_dot_eye = reflectv.dot(eyev);
        if(reflect_dot_eye <= 0) {
            specular_color = Color(0,0,0);
        } else {
            double factor = pow(reflect_dot_eye, shininess);
            specular_color = light.intensity * specular * factor;

        }
    }
    return ambient_color + diffuse_color + specular_color;

}
void Material::set_color(Tuple color) {
    pattern = std::make_shared<Solid>(color);
}