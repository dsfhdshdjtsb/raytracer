#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

Parser::Parser() {
    // Reserve space for common case to avoid frequent reallocations
    vertices.reserve(1000);
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
    
    std::istringstream stream(obj_content);
    std::string line;
    int line_number = 0;
    
    while (std::getline(stream, line)) {
        line_number++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Remove trailing whitespace
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
        // Parse face lines (f v1 v2 v3)
        else if (line.substr(0, 2) == "f ") {
            std::shared_ptr<Triangle> triangle = parse_face_line(line, file_vertices);
            if (triangle != nullptr) {
                group->add_child(triangle);
            } else {
                std::cerr << "Warning: Invalid face at line " << line_number << ": " << line << std::endl;
            }
        }
        // Ignore all other commands (vn, vt, etc.)
    }
    
    std::cout << "Parsed OBJ file: " << file_vertices.size() << " vertices, " 
              << group->shapes.size() << " triangles" << std::endl;
    
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

std::shared_ptr<Triangle> Parser::parse_face_line(const std::string& line, const std::vector<Tuple>& vertices) const {
    std::vector<std::string> parts = split(line, ' ');
    
    // Need at least "f v1 v2 v3" (4 parts)
    if (parts.size() < 4) {
        return nullptr;
    }
    
    try {
        // Parse vertex indices (OBJ files use 1-based indexing)
        std::vector<int> vertex_indices;
        
        for (size_t i = 1; i < parts.size(); ++i) {
            std::string vertex_ref = parts[i];
            
            // Handle vertex/texture/normal format (v/vt/vn) - we only want the vertex index
            size_t slash_pos = vertex_ref.find('/');
            if (slash_pos != std::string::npos) {
                vertex_ref = vertex_ref.substr(0, slash_pos);
            }
            
            int vertex_index = std::stoi(vertex_ref);
            
            // Convert from 1-based to 0-based indexing
            vertex_index--;
            
            // Validate index
            if (vertex_index < 0 || vertex_index >= static_cast<int>(vertices.size())) {
                std::cerr << "Warning: Vertex index " << (vertex_index + 1) << " out of range" << std::endl;
                return nullptr;
            }
            
            vertex_indices.push_back(vertex_index);
        }
        
        // For now, we only handle triangular faces
        // If there are more than 3 vertices, we'll triangulate by creating multiple triangles
        if (vertex_indices.size() == 3) {
            // Simple triangle
            return std::make_shared<Triangle>(
                vertices[vertex_indices[0]],
                vertices[vertex_indices[1]], 
                vertices[vertex_indices[2]]
            );
        } else if (vertex_indices.size() > 3) {
            // For polygons with more than 3 vertices, create triangle fan
            // This assumes convex polygons - for complex polygons, more sophisticated triangulation would be needed
            std::cerr << "Warning: Polygon with " << vertex_indices.size() << " vertices found. Using simple fan triangulation." << std::endl;
            
            // Create the first triangle (v0, v1, v2)
            return std::make_shared<Triangle>(
                vertices[vertex_indices[0]],
                vertices[vertex_indices[1]], 
                vertices[vertex_indices[2]]
            );
            
            // Note: In a complete implementation, you'd want to create multiple triangles
            // for polygons with >3 vertices, but for simplicity we're just taking the first triangle
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Warning: Error parsing face line: " << line << " - " << e.what() << std::endl;
        return nullptr;
    }
    
    return nullptr;
}
