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





Shape::Shape() : material(Material()), transform(IDENTITY_MATRIX) {}
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




bool Cube::operator==(const Shape& other) const {
    const Cube* cube = dynamic_cast<const Cube*>(&other);
    if (!cube) return false;
    return origin == cube->origin && transform == cube->transform;
}

std::vector<double> Cube::intersect(const Ray& ray) const {
    Ray transformed = ray.transform(transform.inverse());
    Tuple transformed_origin = transformed.origin;
    Tuple transformed_direction = transformed.direction;

    std::vector<double> x,y,z;
    x = check_axis(transformed_origin.x, transformed_direction.x);
    y = check_axis(transformed_origin.y, transformed_direction.y);
    z = check_axis(transformed_origin.z, transformed_direction.z);

    double tmin = std::max(x[0], std::max(y[0], z[0]));
    double tmax = std::min(x[1], std::min(y[1], z[1]));

    if(tmin > tmax) return {};
    return {tmin, tmax}; 
}

Tuple Cube::normal_at(const Tuple& point) const {
    Tuple object_point = transform.inverse() * point;
    double maxc = std::max(abs(object_point.x), std::max(abs(object_point.y), abs(object_point.z)));

    Tuple object_normal;
    if(abs(maxc - abs(object_point.x)) < EPSILON) {
        object_normal = Vector(object_point.x, 0, 0);
    }
    else if(abs(maxc - abs(object_point.y)) < EPSILON) {
        object_normal = Vector(0, object_point.y, 0);
    }
    else if(abs(maxc - abs(object_point.z)) < EPSILON) {
        object_normal = Vector( 0, 0, object_point.z);
    }
    object_normal = transform.inverse().T() * object_normal;
    object_normal.w = 0;
    return object_normal.normalize();
}

Cube::Cube() : origin(Point(0,0,0)){}

std::vector<double> Cube::check_axis(double origin, double direction) const {
    double tmin_num = (-1 - origin);
    double tmax_num = (1 - origin);

    double tmin, tmax;
    if( abs(direction) >= EPSILON) {
        tmin = tmin_num / direction;
        tmax = tmax_num / direction;  
    } else {
        tmin = tmin_num > 0 ? INFINITY : -INFINITY; 
        tmax = tmax_num > 0 ? INFINITY : -INFINITY; 
    }

    if(tmin > tmax) {
        double temp = tmin;
        tmin = tmax;
        tmax = temp;
    }

    return {tmin, tmax};
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



Group::Group() {};

void Group::add_child(const std::shared_ptr<Shape> shape) {
    shapes.push_back(shape);
    shape->set_transform(transform * shape->transform);
}

std::vector<double> Group::intersect(const Ray& r) const { 
    Ray transformed = r.transform(transform.inverse());
    Tuple transformed_origin = transformed.origin;
    Tuple transformed_direction = transformed.direction;

    std::vector<double> res = {};
    for(auto shape : shapes) {
        std::vector<double> temp = shape->intersect(transformed);
        res.insert(res.end(), temp.begin(), temp.end());
    }
    return res;
}

Tuple Group::normal_at(const Tuple& point) const {
    return Tuple(0,0,0,1);
}

void Group::set_transform(const Matrix& t)  {
    for(auto shape : shapes) {
        shape->set_transform(t * this->transform.inverse() * shape->transform); 
    }
    this->transform = t; 
}

void Sphere::set_transform(const Matrix& t)  {
    this->transform = t;
}

void Cube::set_transform(const Matrix& t)  {
    this->transform = t;
}

void Plane::set_transform(const Matrix& t)  {
    this->transform = t;
}