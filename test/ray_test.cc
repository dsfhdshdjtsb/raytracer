#include <gtest/gtest.h>
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"
#include <memory>

TEST(Rays, init) {
    Ray r(Point(2,3,4), Vector(1,0,0));

    EXPECT_EQ(Point(2,3,4), r.position(0));
    EXPECT_EQ(Point(3,3,4), r.position(1));
    EXPECT_EQ(Point(1,3,4), r.position(-1));
}

TEST(Rays, intersection) {
    Ray r(Point(0,0,-5), Vector(0,0,1));
    std::shared_ptr<Shape> s = std::make_shared<Sphere>();
    std::vector<Intersection> xs = r.intersect(s);

    EXPECT_EQ(xs[0].t, 4.0); //should be expect to equal actual but wtv
    EXPECT_EQ(xs[1].t, 6.0);

    r = Ray(Point(0,1,-5), Vector(0,0,1));
    xs = r.intersect(s);

    EXPECT_EQ(xs[0].t, 5);
    EXPECT_EQ(xs[0].t, 5);

    r = Ray(Point(0, 10000, -5), Vector(0,0,1));
    xs = r.intersect(s);

    EXPECT_EQ(xs.size(), 0);

    r = Ray(Point(0,0,0), Vector(0,0,1));
    xs = r.intersect(s);

    EXPECT_EQ(xs[0].t, -1);
    EXPECT_EQ(xs[1].t, 1);

    r = Ray(Point(0,0,5), Vector(0,0,1));
    xs = r.intersect(s);

    EXPECT_EQ(xs[0].t, -6.0);
    EXPECT_EQ(xs[1].t, -4);
}

TEST(Rays, intersections) {
    Sphere s;
    std::shared_ptr<Sphere> s_ptr = std::make_shared<Sphere>(s);
    Intersection i1 = Intersection(5, s_ptr);
    Intersection i2 = Intersection(7, s_ptr);
    Intersection i3 = Intersection(-3, s_ptr);
    Intersection i4 = Intersection(2, s_ptr);
    Intersections xs = {i1, i2, i3, i4};
    EXPECT_EQ(i4, xs.hit());
}


TEST(Rays, Transformations) {
    Ray r = Ray(Point(1,2,3), Vector(0,1,0));
    Matrix m = Translation(3,4,5);
    Ray r2 = r.transform(m);

    EXPECT_EQ(Point(4,6,8), r2.origin);
    EXPECT_EQ(Vector(0,1,0), r2.direction);

    m = Scaling(2,3,4);
    r2 = r.transform(m);


    EXPECT_EQ(Point(2,6,12), r2.origin);
    EXPECT_EQ(Vector(0,3,0), r2.direction);
}

TEST(Rays, TransformedIntersection) {
    Ray r(Point(0,0,-5), Vector(0,0,1));
    std::shared_ptr<Shape> s = std::make_shared<Sphere>();
    s->set_transform(Scaling(2,2,2));
    std::vector<Intersection> xs = r.intersect(s);
    EXPECT_EQ(3, xs[0].t);
    EXPECT_EQ(7, xs[1].t);
}