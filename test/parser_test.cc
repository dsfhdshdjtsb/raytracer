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
