#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

Parser::Parser() {
    // Reserve space for common case to avoid frequent reallocations
    vertices.reserve(1000);
    normals.reserve(1000);
}

std::shared_ptr<Group> Parser::parse(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        return std::make_shared<Group>();
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return parse_string(buffer.str());
}

std::shared_ptr<Group> Parser::parse_string(const std::string& obj_content) const {
    std::shared_ptr<Group> group = std::make_shared<Group>();
    std::vector<Tuple> file_vertices; // Local vertices for this file
    std::vector<Tuple> file_normals; // Local normals for this file
    std::istringstream stream(obj_content);
    std::string line;
    int line_number = 0;
    
    while (std::getline(stream, line)) {
        line_number++;
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // Parse vertex lines (v x y z)
        if (line.substr(0, 2) == "v ") {
            std::vector<std::string> parts = split(line, ' ');
            if (parts.size() >= 4) {
                try {
                    double x = std::stod(parts[1]);
                    double y = std::stod(parts[2]);
                    double z = std::stod(parts[3]);
                    file_vertices.push_back(Point(x, y, z));
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Invalid vertex at line " << line_number << ": " << line << std::endl;
                }
            } else {
                std::cerr << "Warning: Invalid vertex format at line " << line_number << ": " << line << std::endl;
            }
        }
        else if (line.substr(0, 3) == "vn ") {
            std::vector<std::string> parts = split(line, ' ');
            if (parts.size() >= 4) {
                try {
                    double x = std::stod(parts[1]);
                    double y = std::stod(parts[2]);
                    double z = std::stod(parts[3]);
                    file_normals.push_back(Vector(x, y, z).normalize());
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Invalid normal at line " << line_number << ": " << line << std::endl;
                }
            } else {
                std::cerr << "Warning: Invalid normal format at line " << line_number << ": " << line << std::endl;
            }
        }
        // Parse face lines (f v1 v2 v3)
        else if (line.substr(0, 2) == "f ") {
            std::vector<std::shared_ptr<Shape>> shapes = parse_face_line(line, file_vertices, file_normals);
            if (shapes.size() > 0) {
                for (const auto& shape : shapes) {
                    group->add_child(shape);
                }
            } else {
                std::cerr << "Warning: Invalid face at line " << line_number << ": " << line << std::endl;
            }
        }
    }
    
    // Calculate bounding box statistics
    if (!file_vertices.empty()) {
        double min_x = file_vertices[0].x, max_x = file_vertices[0].x;
        double min_y = file_vertices[0].y, max_y = file_vertices[0].y;
        double min_z = file_vertices[0].z, max_z = file_vertices[0].z;
        
        for (const auto& vertex : file_vertices) {
            min_x = std::min(min_x, vertex.x);
            max_x = std::max(max_x, vertex.x);
            min_y = std::min(min_y, vertex.y);
            max_y = std::max(max_y, vertex.y);
            min_z = std::min(min_z, vertex.z);
            max_z = std::max(max_z, vertex.z);
        }
        
        // Calculate center (average of min/max, not average of all vertices)
        double center_x = (min_x + max_x) / 2.0;
        double center_y = (min_y + max_y) / 2.0;
        double center_z = (min_z + max_z) / 2.0;
        
        // Calculate dimensions
        double width = max_x - min_x;
        double height = max_y - min_y;
        double depth = max_z - min_z;
        
        std::cout << "Parsed OBJ file: " << file_vertices.size() << " vertices, " 
                  << group->shapes.size() << " triangles" << std::endl;
        std::cout << "Bounding Box:" << std::endl;
        std::cout << "  Min: (" << min_x << ", " << min_y << ", " << min_z << ")" << std::endl;
        std::cout << "  Max: (" << max_x << ", " << max_y << ", " << max_z << ")" << std::endl;
        std::cout << "  Center: (" << center_x << ", " << center_y << ", " << center_z << ")" << std::endl;
        std::cout << "  Dimensions: " << width << " x " << height << " x " << depth << std::endl;
        std::cout << "To center at origin: Translation(" << -center_x << ", " << -center_y << ", " << -center_z << ")" << std::endl;
    } else {
        std::cout << "Parsed OBJ file: 0 vertices, " << group->shapes.size() << " triangles" << std::endl;
    }
    
    return group;
}

std::vector<std::string> Parser::split(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::vector<std::shared_ptr<Shape>> Parser::parse_face_line(const std::string& line, const std::vector<Tuple>& vertices, const std::vector<Tuple>& normals) const {
    std::vector<std::string> parts = split(line, ' ');
    
    if (parts.size() < 4) {
        return {};
    }
    
    try {
        std::vector<int> vertex_indices;
        std::vector<int> normal_indices;
        bool has_normals = false;

        for (size_t i = 1; i < parts.size(); ++i) {
            std::string vertex_ref = parts[i];
            
            // Parse vertex/texture/normal format (v/vt/vn)
            // Use special parsing that preserves empty tokens for v//vn format
            std::vector<std::string> vertex_parts;
            std::stringstream ss(vertex_ref);
            std::string token;
            while (std::getline(ss, token, '/')) {
                vertex_parts.push_back(token); // Keep empty tokens
            }
            
            // Get vertex index
            int vertex_index = std::stoi(vertex_parts[0]) - 1;
            if (vertex_index < 0 || vertex_index >= static_cast<int>(vertices.size())) {
                std::cerr << "Warning: Vertex index " << (vertex_index + 1) << " out of range" << std::endl;
                return {};
            }
            vertex_indices.push_back(vertex_index);
            
            // Get normal index if present (third component after second slash)
            if (vertex_parts.size() >= 3 && !vertex_parts[2].empty() && !normals.empty()) {
                int normal_index = std::stoi(vertex_parts[2]) - 1;
                if (normal_index >= 0 && normal_index < static_cast<int>(normals.size())) {
                    normal_indices.push_back(normal_index);
                    has_normals = true;
                } else {
                    std::cerr << "Warning: Normal index " << (normal_index + 1) << " out of range" << std::endl;
                    has_normals = false;
                    normal_indices.clear();
                    break;
                }
            }
        }
        
        // Fan triangulation: works for any polygon size (3+ vertices)
        // Creates triangles using vertex 0 as the fan center
        std::vector<std::shared_ptr<Shape>> triangles;
        
        if (vertex_indices.size() >= 3) {
            for (size_t i = 1; i < vertex_indices.size() - 1; ++i) {
                if (has_normals && normal_indices.size() == vertex_indices.size()) {
                    //std::cout << "Smooth triangle" << std::endl;
                    triangles.push_back(std::make_shared<Triangle>(
                        vertices[vertex_indices[0]], vertices[vertex_indices[i]], vertices[vertex_indices[i + 1]]
                        //normals[normal_indices[0]], normals[normal_indices[i]], normals[normal_indices[i + 1]]
                    ));
                } else {
                    std::cout << "Regular triangle" << std::endl;
                    triangles.push_back(std::make_shared<Triangle>(
                        vertices[vertex_indices[0]],
                        vertices[vertex_indices[i]], 
                        vertices[vertex_indices[i + 1]]
                    ));
                }
            }
        }
        
        return triangles;
        
    } catch (const std::exception& e) {
        std::cerr << "Warning: Error parsing face line: " << line << " - " << e.what() << std::endl;
        return {};
    }
    
    return {};
}
