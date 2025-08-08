#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include "../src/utils/light.h"
#include "../src/scene/world.h"
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"
#include "../src/scene/camera.h"
#include "../src/screen/canvas.h"
#include "../src/utils/pattern.h"

TEST(Cameras, Construction) {
    Camera c(200, 125, M_PI / 2) ;
    EXPECT_EQ(true, c.pixel_size - 0.01 < 0.0001);
    c = Camera(125, 200, M_PI/2); 
    EXPECT_EQ(true, c.pixel_size - 0.01 < 0.0001);
}

TEST(Cameras, Pixels) {
    Camera c(201, 101, M_PI/2);
    Ray r = c.ray_for_pixel(100, 50);
    EXPECT_EQ(Point(0,0,0), r.origin);
    EXPECT_EQ(Vector(0,0,-1), r.direction);

    r = c.ray_for_pixel(0,0);
    EXPECT_EQ(Point(0,0,0), r.origin);
    EXPECT_EQ(Vector(0.66519,0.33259,-0.66851), r.direction);

    c.transform = Rotation_y(M_PI / 4) * Translation(0,-2,5);
    r = c.ray_for_pixel(100,50);
    EXPECT_EQ(Point(0,2,-5), r.origin);
    EXPECT_EQ(Vector(1,0,-1).normalize(), r.direction);
}

//TEST(Cameras, Render) {
    //World w = DefaultWorld();
    //w.camera = Camera(11,11,M_PI/2);
    //Tuple from = Point(0,0,-5);
    //Tuple to = Point(0,0,0);
    //Tuple up = Point(0,1,0);
    //w.camera.transform = ViewTransform(from, to, up);
    //Canvas image = w.render();
    //EXPECT_EQ(Color(0.38066, 0.47583, 0.2855), image[5][5]);
//}

TEST(World, isShadow) {
    World w = DefaultWorld();

    Tuple p = Point(0,10,0);
    EXPECT_EQ(false, w.is_shadowed(p));

    p = Point(10, -10, 10);
    EXPECT_EQ(true, w.is_shadowed(p));

    p = Point(-20, 20, -20);
    EXPECT_EQ(false, w.is_shadowed(p));

    p = Point(-2, 2, -2);
    EXPECT_EQ(false, w.is_shadowed(p));
}

TEST(World, shadows) {
    World w;
    w.light = PointLight(Color(1,1,1), Point(0,0,-10));
    std::shared_ptr<Shape> s1 = std::make_shared<Sphere>();
    std::shared_ptr<Shape> s2 = std::make_shared<Sphere>();
    s2->transform = Translation(0,0,10);

    w.objects.push_back(s1);
    w.objects.push_back(s2);

    Ray r(Point(0,0,5), Vector(0,0,1));
    Intersection i(4, s2);
    std::cout<< "here";
    Computations comps = i.prepare_computations(r);
    Tuple c = w.shade_hit(comps, 5);
    EXPECT_EQ(Color(0.1,0.1,0.1), c);
}

TEST(World, ShadowOffset) {
    Ray r(Point(0,0,-5), Vector(0,0,1));
    std::shared_ptr<Shape> s1 = std::make_shared<Sphere>();
    s1->transform = Translation(0,0,1);
    Intersection i(5, s1);
    Computations comps = i.prepare_computations(r) ;
    EXPECT_LT( comps.over_point.z, -EPSILON / 2);
    EXPECT_GT(comps.point.z, comps.over_point.z);
}

TEST(Patterns, Patterns) {
    std::shared_ptr<Pattern> pat = std::make_shared<Gradient>(Color(1,1,1), Color(0,0,0));
    EXPECT_EQ(pat->color_at(Point(0,0,0)), Color(1,1,1)); 
    EXPECT_EQ(pat->color_at(Point(0.5,0,0)), Color(0.5,0.5,0.5));
}

TEST(Reflection, simple) {
    World w = DefaultWorld();
    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    std::shared_ptr<Shape> s = w.objects[1];
    s->material.ambient = 1;
    Intersection i = Intersection(1, s);
    Computations comps = i.prepare_computations(r);
    Tuple c = w.reflected_color(comps, 5);
    EXPECT_EQ(Color(0,0,0), c);
}

TEST(Reflection, infRecursion) {
    World w = DefaultWorld();
    w.light = PointLight(Color(1,1,1), Point(0,0,0));

    std::shared_ptr<Shape> lower = std::make_shared<Plane>();
    lower->material.reflective = 1;
    lower->transform = Translation(0,-1,0);

    std::shared_ptr<Shape> upper = std::make_shared<Plane>();
    upper->material.reflective = 1;
    upper->transform = Translation(0,1,0);

    w.objects = {lower, upper};

    Ray r = Ray(Point(0,0,0), Vector(0,1,0));
    Tuple c = w.color_at(r, 5);
    //testing for no error
}

TEST(Reflection, shade_hit) {
    World w = DefaultWorld();
    std::shared_ptr<Shape> shape = std::make_shared<Plane>();
    shape->material.reflective = 0.5;
    shape->transform = Translation(0, -1, 0);
    w.objects.push_back(shape);

    Ray r = Ray(Point(0,0,-3), Vector(0, -sqrt(2)/2, sqrt(2)/2));
    Intersection i  = Intersection(sqrt(2), shape);
    Computations comps = i.prepare_computations(r);
    Tuple color = w.shade_hit(comps);
    EXPECT_EQ(Color(0.87677, 0.92436, 0.82918), color);
}

TEST(Reflection, reflected_color_for_reflective_material) {
    World w = DefaultWorld();
    std::shared_ptr<Shape> shape = std::make_shared<Plane>();
    shape->material.reflective = 0.5;
    shape->transform = Translation(0, -1, 0);
    w.objects.push_back(shape);

    Ray r = Ray(Point(0, 0, -3), Vector(0, -sqrt(2)/2, sqrt(2)/2));
    Intersection i = Intersection(sqrt(2), shape);
    Computations comps = i.prepare_computations(r);
    Tuple color = w.reflected_color(comps);
    EXPECT_EQ(Color(0.19032, 0.2379, 0.14274), color);
}


TEST(Refraction, n1_n2_at_various_intersections) {

    auto A = GlassSphere();
    A->transform = Scaling(2,2,2);
    A->material.refractive_index = 1.5;

    auto B = GlassSphere();
    B->transform = Translation(0,0,-0.25);
    B->material.refractive_index = 2.0;

    auto C = GlassSphere();
    C->transform = Translation(0,0,0.25);
    C->material.refractive_index = 2.5;

    Ray r = Ray(Point(0,0,-4), Vector(0,0,1));

    std::vector<Intersection> xs_vec = {
        Intersection(2.0, A),
        Intersection(2.75, B),
        Intersection(3.25, C),
        Intersection(4.75, B),
        Intersection(5.25, C),
        Intersection(6.0, A)
    };
    Intersections xs = xs_vec;

    struct Example { int index; double n1; double n2; };
    std::vector<Example> examples = {
        {0, 1.0, 1.5},
        {1, 1.5, 2.0},
        {2, 2.0, 2.5},
        {3, 2.5, 2.5},
        {4, 2.5, 1.5},
        {5, 1.5, 1.0}
    };

    for (const auto& ex : examples) {
        Computations comps = xs_vec[ex.index].prepare_computations(r, xs);
        EXPECT_DOUBLE_EQ(comps.n1, ex.n1);
        EXPECT_DOUBLE_EQ(comps.n2, ex.n2);
    }
}

TEST(Refraction, refracted_color_under_total_internal_reflection) {
    World w = DefaultWorld();
    std::shared_ptr<Shape> shape = w.objects[0];
    shape->material.transparency = 1.0;
    shape->material.refractive_index = 1.5;
    
    Ray r = Ray(Point(0, 0, sqrt(2)/2), Vector(0, 1, 0));
    Intersections xs = {
        Intersection(-sqrt(2)/2, shape),
        Intersection(sqrt(2)/2, shape)
    };
    
    auto it = xs.begin();
    ++it;
    Intersection second_intersection = *it;
    
    Computations comps = second_intersection.prepare_computations(r, xs);
    Tuple c = w.refracted_color(comps, 5);
    EXPECT_EQ(Color(0, 0, 0), c);
}

TEST(Refraction, refracted) {
    World w = DefaultWorld();
    
    std::shared_ptr<Shape> plane = std::make_shared<Plane>();

    plane->transform = Translation(0,-1,0);
    plane->material.reflective = 0;
    plane->material.transparency = 0.5;
    plane->material.refractive_index= 1.5;
    w.objects.push_back(plane);

    std::shared_ptr<Shape> ball = std::make_shared<Sphere>();
    ball->material.set_color(Color(1,0,0));
    ball->material.ambient = 0.5;
    ball->transform = Translation(0, -3.5, -0.5);

    w.objects.push_back(ball);

    Ray r = Ray(Point(0,0,-3), Vector(0, -1, 1).normalize());
    Intersections xs = {Intersection(sqrt(2),plane )};
    Computations comps = xs.hit().prepare_computations(r, xs);

    Tuple color = w.shade_hit(comps, 5);
 
    EXPECT_EQ(Color(0.93642, 0.68642, 0.68642), color);
}

TEST(Refraction, Schlick) {
    World w = DefaultWorld();
    
    std::shared_ptr<Shape> plane = std::make_shared<Plane>();

    plane->transform = Translation(0,-1,0);
    plane->material.reflective = 0.5;
    plane->material.transparency = 0.5;
    plane->material.refractive_index= 1.5;
    w.objects.push_back(plane);

    std::shared_ptr<Shape> ball = std::make_shared<Sphere>();
    ball->material.set_color(Color(1,0,0));
    ball->material.ambient = 0.5;
    ball->transform = Translation(0, -3.5, -0.5);

    w.objects.push_back(ball);

    Ray r = Ray(Point(0,0,-3), Vector(0, -1, 1).normalize());
    Intersections xs = {Intersection(sqrt(2),plane )};
    Computations comps = xs.hit().prepare_computations(r, xs);

    Tuple color = w.shade_hit(comps, 5);

    EXPECT_EQ(Color(0.93391, 0.69643, 0.69243), color);
}