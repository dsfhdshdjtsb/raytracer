#ifndef SHAPE_INCLUDE
#define SHAPE_INCLUDE


#include "math.h"
#include "light.h"
#include <math.h>
#include <set>
#include <memory>

struct Ray;
struct Intersection;
struct Intersections;

struct Material {
    Tuple color;
    float ambient, diffuse, specular, shininess;
    Material();

    Material(Tuple c, float a, float d, float sp, float sh);

    Tuple lighting(PointLight light, Tuple point, Tuple eyev, Tuple normalv) const;
};

struct Shape {
    Material material;
    Matrix transform;
    Shape();

    virtual bool operator==(const Shape& other) const;
    void set_transform(const Matrix& t);
    virtual std::vector<float> intersect(const Ray& r) const;
    virtual Tuple normal_at(const Tuple& point) const;
    void set_material(Material material);
};


struct Sphere : public Shape {
    float r;
    Tuple center;

    Sphere();
    bool operator==(const Shape& other) const;
    std::vector<float> intersect(const Ray& r) const;
    Tuple normal_at(const Tuple& point) const;
};

struct Intersection {
    std::shared_ptr<Shape> object;
    float t;

    Intersection(float t, std::shared_ptr<Shape> c);
    Intersection(const Intersection& other);
    bool operator==(const Intersection& other) const;
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
    Intersection hit();
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
#endif