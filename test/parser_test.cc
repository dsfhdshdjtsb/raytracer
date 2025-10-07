#include <gtest/gtest.h>
#include "../src/file/parser.h"
#include "../src/utils/math.h"

TEST(Parser, parse_vertices) {
    Parser parser;
    
    std::string obj_content = 
        "# Test OBJ file\n"
        "v 0.0 1.0 0.0\n"
        "v -1.0 0.0 0.0\n"
        "v 1.0 0.0 0.0\n"
        "f 1 2 3\n";
    
    std::shared_ptr<Group> group = parser.parse_string(obj_content);
    
    EXPECT_EQ(1, group->shapes.size());
    
    // Check if we can cast to Triangle
    auto triangle = std::dynamic_pointer_cast<Triangle>(group->shapes[0]);
    EXPECT_TRUE(triangle != nullptr);
    
    if (triangle) {
        EXPECT_EQ(Point(0, 1, 0), triangle->p1);
        EXPECT_EQ(Point(-1, 0, 0), triangle->p2);
        EXPECT_EQ(Point(1, 0, 0), triangle->p3);
    }
}

TEST(Parser, parse_complex_obj) {
    Parser parser;
    
    std::string obj_content = 
        "# More complex test\n"
        "v 0.0 1.0 0.0\n"
        "v -1.0 0.0 0.0\n"
        "v 1.0 0.0 0.0\n"
        "v 0.0 0.0 1.0\n"
        "# A comment line\n"
        "\n"
        "f 1 2 3\n"
        "f 1 3 4\n"
        "f 1 4 2\n"
        "f 2 4 3\n";
    
    std::shared_ptr<Group> group = parser.parse_string(obj_content);
    
    EXPECT_EQ(4, group->shapes.size());
    
    // All shapes should be triangles
    for (const auto& shape : group->shapes) {
        auto triangle = std::dynamic_pointer_cast<Triangle>(shape);
        EXPECT_TRUE(triangle != nullptr);
    }
}

TEST(Parser, ignore_unsupported_commands) {
    Parser parser;
    
    std::string obj_content = 
        "# Test ignoring unsupported commands\n"
        "v 0.0 1.0 0.0\n"
        "v -1.0 0.0 0.0\n"
        "v 1.0 0.0 0.0\n"
        "vn 0.0 0.0 1.0\n"   // normal - should be ignored
        "vt 0.5 0.5\n"       // texture coordinate - should be ignored
        "usemtl material1\n"  // material - should be ignored
        "s 1\n"              // smooth shading - should be ignored
        "f 1 2 3\n";
    
    std::shared_ptr<Group> group = parser.parse_string(obj_content);
    
    EXPECT_EQ(1, group->shapes.size());
}

// Test: Faces with normals
TEST(Parser, FacesWithNormals) {
    std::string obj_content = R"(
v 0 1 0
v -1 0 0
v 1 0 0

vn -1 0 0
vn 1 0 0
vn 0 1 0

f 1//3 2//1 3//2
f 1/0/3 2/102/1 3/14/2
)";

    Parser parser;
    std::shared_ptr<Group> g = parser.parse_string(obj_content);
    
    ASSERT_EQ(g->shapes.size(), 2);
    
    //print type of first triangle
    std::shared_ptr<SmoothTriangle> t1 = std::dynamic_pointer_cast<SmoothTriangle>(g->shapes[0]);
    ASSERT_NE(t1, nullptr) << "First shape should be a SmoothTriangle";
    
    // Check vertices
    EXPECT_EQ(t1->p1, Point(0, 1, 0));  // parser.vertices[1] (1-indexed)
    EXPECT_EQ(t1->p2, Point(-1, 0, 0)); // parser.vertices[2] 
    EXPECT_EQ(t1->p3, Point(1, 0, 0));  // parser.vertices[3]
    
    // Check normals
    EXPECT_EQ(t1->n1, Vector(0, 1, 0));  // parser.normals[3] (1-indexed)
    EXPECT_EQ(t1->n2, Vector(-1, 0, 0)); // parser.normals[1]
    EXPECT_EQ(t1->n3, Vector(1, 0, 0));  // parser.normals[2]
    
    // Second triangle - should also be SmoothTriangle  
    std::shared_ptr<SmoothTriangle> t2 = std::dynamic_pointer_cast<SmoothTriangle>(g->shapes[1]);
    ASSERT_NE(t2, nullptr) << "Second shape should be a SmoothTriangle";
    
    // Second triangle should have same properties as first (t2 = t1)
    EXPECT_EQ(t2->p1, t1->p1);
    EXPECT_EQ(t2->p2, t1->p2);
    EXPECT_EQ(t2->p3, t1->p3);
    EXPECT_EQ(t2->n1, t1->n1);
    EXPECT_EQ(t2->n2, t1->n2);
    EXPECT_EQ(t2->n3, t1->n3);
}

// Test: Faces without normals should create regular triangles
TEST(Parser, FacesWithoutNormals) {
    std::string obj_content = R"(
v 0 1 0
v -1 0 0
v 1 0 0

f 1 2 3
)";

    Parser parser;
    std::shared_ptr<Group> g = parser.parse_string(obj_content);
    
    ASSERT_EQ(g->shapes.size(), 1);
    
    // Should be regular Triangle, not SmoothTriangle
    std::shared_ptr<Triangle> t = std::dynamic_pointer_cast<Triangle>(g->shapes[0]);
    ASSERT_NE(t, nullptr) << "Shape should be a regular Triangle";
    
    // Should NOT be a SmoothTriangle
    std::shared_ptr<SmoothTriangle> st = std::dynamic_pointer_cast<SmoothTriangle>(g->shapes[0]);
    EXPECT_EQ(st, nullptr) << "Shape should NOT be a SmoothTriangle";
}

// Test: Mixed format - some faces with normals, some without
TEST(Parser, MixedFaceFormats) {
    std::string obj_content = R"(
v 0 1 0
v -1 0 0
v 1 0 0
v 0 0 1

vn 0 1 0
vn -1 0 0

f 1 2 3
f 1//1 2//2 4//1
)";

    Parser parser;
    std::shared_ptr<Group> g = parser.parse_string(obj_content);
    
    ASSERT_EQ(g->shapes.size(), 2);
    
    // First triangle - regular Triangle (no normals)
    std::shared_ptr<Triangle> t1 = std::dynamic_pointer_cast<Triangle>(g->shapes[0]);
    ASSERT_NE(t1, nullptr) << "First shape should be a regular Triangle";
    std::shared_ptr<SmoothTriangle> st1 = std::dynamic_pointer_cast<SmoothTriangle>(g->shapes[0]);
    EXPECT_EQ(st1, nullptr) << "First shape should NOT be a SmoothTriangle";
    
    // Second triangle - SmoothTriangle (with normals)
    std::shared_ptr<SmoothTriangle> t2 = std::dynamic_pointer_cast<SmoothTriangle>(g->shapes[1]);
    ASSERT_NE(t2, nullptr) << "Second shape should be a SmoothTriangle";
}
