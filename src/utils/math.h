#ifndef MATH_INCLUDE
#define MATH_INCLUDE

#define EPSILON 0.0001 //this is a hyperparameter when you think about it (dont think about it)

#include <iostream>
#include <vector>

struct Tuple {
    float x, y, z, w;
    
    Tuple(float x, float y, float z, float w);
    Tuple();
    
    bool isPoint() const;
    bool isVector() const;
    float magnitude() const;
    Tuple normalize() const;
    float dot(const Tuple& other) const;
    
    float& operator[](int i);
    const float& operator[](int i) const;

    bool operator==(const Tuple& other) const;
    Tuple operator+(const Tuple& other) const;
    Tuple operator-(const Tuple& other) const;
    Tuple operator*(const Tuple& other) const;
    Tuple operator*(float scalar) const;
    //Tuple operator*(int scalar) const;
    Tuple operator/(float scalar) const;
    Tuple operator-() const;

    Tuple cross(const Tuple& other) const;
    Tuple reflect(const Tuple& normal) const;
};

Tuple operator*(float scalar, const Tuple& tuple);
Tuple operator*(int scalar, const Tuple& tuple);
std::ostream& operator<<(std::ostream& os, const Tuple& p);

Tuple Point(float x, float y, float z);
Tuple Vector(float x, float y, float z);
Tuple Color(float r, float g, float b);


struct Matrix {
    std::vector<std::vector<float>> v;
    int height, width;

    Matrix(int height, int width);
    Matrix(std::vector<std::vector<float>>& v);
    Matrix(std::initializer_list<std::vector<float>> list);
  
    std::vector<float>& operator[](int i);
    const std::vector<float>& operator[](int i ) const;

    bool operator==(const Matrix& other) const;

    Matrix operator*(const Matrix& other) const;
    Tuple operator*(const Tuple& other) const;

    Matrix T() const;

    float det() const;
    Matrix submatrix(int row, int col) const;
    float minor(int row, int col) const;
    float cofactor(int row, int col) const;
    Matrix inverse() const;
};

Matrix Translation(float x, float y, float z);
Matrix Scaling(float x, float y, float z);
Matrix Rotation_x(float rads);
Matrix Rotation_y(float rads);
Matrix Rotation_z(float rads);
Matrix Shearing(float xy, float xz, float yx, float yz, float zx, float zy);
Matrix ViewTransform(const Tuple& from, const Tuple& to, const Tuple& up);

extern const Matrix IDENTITY_MATRIX;

std::ostream& operator<<(std::ostream& os, const Matrix& p);

#endif

