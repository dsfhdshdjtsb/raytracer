#ifndef SHAPE_INCLUDE
#define SHAPE_INCLUDE


#include "math.h"
#include "light.h"
#include "ray.h"
#include "pattern.h"
#include <math.h>
#include <set>
#include <memory>

struct Ray;
struct Intersection;
struct Intersections;

struct Material {
    std::shared_ptr<Pattern> pattern;
    double ambient, diffuse, specular, shininess, reflective, transparency, refractive_index;
    Material();

    Material(const Tuple& c, double a, double d, double sp, double sh, double rf, double tr, double re_i);
    Material(std::shared_ptr<Pattern> pattern, double a, double d, double sp, double sh, double rf, double tr, double re_i );

    Tuple lighting(PointLight light, std::shared_ptr<Shape> object, Tuple point, Tuple eyev, Tuple normalv, bool in_shadow) const;
    void set_color(Tuple color);
};

struct Shape {
    Material material;
    Matrix transform;
    Shape();
    virtual ~Shape() {}

    virtual bool operator==(const Shape& other) const;
    void set_transform(const Matrix& t);
    virtual std::vector<double> intersect(const Ray& r) const;
    virtual Tuple normal_at(const Tuple& point) const;
    void set_material(const Material& material);
    Tuple color_at(const Tuple& point) const;
};


struct Sphere : public Shape {
    double r;
    Tuple center;

    Sphere();
    bool operator==(const Shape& other) const;
    std::vector<double> intersect(const Ray& r) const;
    Tuple normal_at(const Tuple& point) const;
};

std::shared_ptr<Shape> GlassSphere();

struct Plane : public Shape {
    Plane();
    std::vector<double> intersect(const Ray& r) const;
    Tuple normal_at(const Tuple& point) const;
};

struct Computations {
    double t;
    std::shared_ptr<Shape> object;
    Tuple point, eyev, normalv, over_point, reflectv, under_point;
    double n1,n2;
    bool inside;

    Computations(double t, std::shared_ptr<Shape> object, Tuple point, Tuple over_point, Tuple eyev, Tuple normalv, Tuple reflectv, Tuple under_point, bool inside, double n1, double n2);
    double schlick() const;
};

struct IntersectionComparator {
    bool operator()(const Intersection& a, const Intersection& b) const;
};

struct Intersections {
    std::set<Intersection, IntersectionComparator> pos;
    std::set<Intersection, IntersectionComparator> neg; 

    Intersections(std::initializer_list<Intersection> list);
    Intersections();
    Intersections(std::vector<Intersection> list);

    void insert(const Intersection& intersection);
    Intersection hit() const;
    bool has_hit() const;
    int size() const;

    struct Iterator {
        using SetIter = std::set<Intersection, IntersectionComparator>::iterator;
        SetIter pos_it, pos_end;
        SetIter neg_it, neg_end;

        Iterator(SetIter pos_it, SetIter pos_end, SetIter neg_it, SetIter neg_end);
        Intersection operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;
    };
    Iterator begin() const;
    Iterator end() const;
};

struct Intersection {
    std::shared_ptr<Shape> object;
    double t;

    Intersection(double t, std::shared_ptr<Shape> c);
    Intersection(const Intersection& other);
    bool operator==(const Intersection& other) const;
    Computations prepare_computations(const Ray& r, const Intersections& xs = {}) const;
};

#endif