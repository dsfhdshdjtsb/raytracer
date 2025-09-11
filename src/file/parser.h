#ifndef PARSER_INCLUDE
#define PARSER_INCLUDE

#include "../utils/shape.h"
#include <memory>
#include <string>
#include <vector>

struct Parser {
    std::vector<Tuple> vertices;
    
    Parser();
    
    std::shared_ptr<Group> parse(const std::string& path) const;
    std::shared_ptr<Group> parse_string(const std::string& obj_content) const;
    
private:
    std::vector<std::string> split(const std::string& str, char delimiter) const;
    std::shared_ptr<Triangle> parse_face_line(const std::string& line, const std::vector<Tuple>& vertices) const;
};

#endif