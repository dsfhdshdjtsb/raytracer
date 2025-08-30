#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <algorithm>
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

TEST(Cube, intersection) {
    std::shared_ptr<Shape> c = std::make_shared<Cube>();
    
    // Test cases where ray misses the cube
    struct MissExample {
        Tuple origin;
        Tuple direction;
    };
    
    std::vector<MissExample> miss_examples = {
        {Point(-2, 0, 0), Vector(0.2673, 0.5345, 0.8018)},
        {Point(0, -2, 0), Vector(0.8018, 0.2673, 0.5345)},
        {Point(0, 0, -2), Vector(0.5345, 0.8018, 0.2673)},
        {Point(2, 0, 2), Vector(0, 0, -1)},
        {Point(0, 2, 2), Vector(0, -1, 0)},
        {Point(2, 2, 0), Vector(-1, 0, 0)}
    };
    
    for (const auto& example : miss_examples) {
        Ray r(example.origin, example.direction);
        std::vector<double> xs = c->intersect(r);
        EXPECT_EQ(0, xs.size()) << "Ray should miss cube from origin " 
                                << "(" << example.origin.x << "," << example.origin.y << "," << example.origin.z << ")"
                                << " with direction " 
                                << "(" << example.direction.x << "," << example.direction.y << "," << example.direction.z << ")";
    }
    
    struct HitExample {
        Tuple origin;
        Tuple direction;
        double t1;
        double t2;
    };
    
    std::vector<HitExample> hit_examples = {
        {Point(5, 0.5, 0), Vector(-1, 0, 0), 4, 6},
        {Point(-5, 0.5, 0), Vector(1, 0, 0), 4, 6},
        {Point(0.5, 5, 0), Vector(0, -1, 0), 4, 6},
        {Point(0.5, -5, 0), Vector(0, 1, 0), 4, 6},
        {Point(0.5, 0, 5), Vector(0, 0, -1), 4, 6},
        {Point(0.5, 0, -5), Vector(0, 0, 1), 4, 6},
        {Point(0, 0.5, 0), Vector(0, 0, 1), -1, 1}  
    };
    
    for (const auto& example : hit_examples) {
        Ray r(example.origin, example.direction);
        std::vector<double> xs = c->intersect(r);
        EXPECT_EQ(2, xs.size()) << "Ray should hit cube twice from origin " 
                               << "(" << example.origin.x << "," << example.origin.y << "," << example.origin.z << ")";
        EXPECT_DOUBLE_EQ(example.t1, xs[0]) << "First intersection t value incorrect";
        EXPECT_DOUBLE_EQ(example.t2, xs[1]) << "Second intersection t value incorrect";
    }
}

TEST(Cube, normals) {
    std::shared_ptr<Shape> c = std::make_shared<Cube>();
    
    struct NormalExample {
        Tuple point;
        Tuple expected_normal;
    };
    
    std::vector<NormalExample> examples = {
        {Point(1, 0.5, -0.8), Vector(1, 0, 0)},
        {Point(-1, -0.2, 0.9), Vector(-1, 0, 0)},
        {Point(-0.4, 1, -0.1), Vector(0, 1, 0)},
        {Point(0.3, -1, -0.7), Vector(0, -1, 0)},
        {Point(-0.6, 0.3, 1), Vector(0, 0, 1)},
        {Point(0.4, 0.4, -1), Vector(0, 0, -1)},
        {Point(1, 1, 1), Vector(1, 0, 0)},
        {Point(-1, -1, -1), Vector(-1, 0, 0)}
    };
    
    for (const auto& example : examples) {
        Tuple normal = c->normal_at(example.point);
        EXPECT_EQ(example.expected_normal, normal) 
            << "Normal at point (" << example.point.x << ", " << example.point.y << ", " << example.point.z << ")"
            << " should be (" << example.expected_normal.x << ", " << example.expected_normal.y << ", " << example.expected_normal.z << ")"
            << " but got (" << normal.x << ", " << normal.y << ", " << normal.z << ")";
    }
}

TEST(Group, intersect_nonempty) {
    std::shared_ptr<Group> g = std::make_shared<Group>();

    std::shared_ptr<Shape> s1 = std::make_shared<Sphere>();
    std::shared_ptr<Shape> s2 = std::make_shared<Sphere>();
    s2->set_transform(Translation(0, 0, -3));
    std::shared_ptr<Shape> s3 = std::make_shared<Sphere>();
    s3->set_transform(Translation(5, 0, 0));
    
    g->add_child(s1);
    g->add_child(s2);
    g->add_child(s3);
    
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Intersections xs = r.intersect(g);
    
    EXPECT_EQ(4, xs.size());
    
    int count = 0;
    for(auto i : xs) {
        if(count == 0 || count == 1) {
            EXPECT_EQ(s2, i.object);
        } else {
            EXPECT_EQ(s1, i.object);
        } 
        count++;
    }
}

TEST(Group, transforming_groups) {
    std::shared_ptr<Group> g1 = std::make_shared<Group>();
    g1->set_transform(Rotation_y(M_PI/2));
    
    std::shared_ptr<Group> g2 = std::make_shared<Group>();
    g2->set_transform(Scaling(1, 2, 3));
    
    g1->add_child(g2);
    
    std::shared_ptr<Shape> s = std::make_shared<Sphere>();
    s->set_transform(Translation(5, 0, 0));
    
    g2->add_child(s);
    
    // Test the normal at the specified point
    Tuple n = s->normal_at(Point(1.7321, 1.1547, -5.5774));
    
    EXPECT_EQ(Vector(0.2857, 0.4286, -0.8571), n);
}