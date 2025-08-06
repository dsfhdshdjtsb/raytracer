#ifndef MATH_INCLUDE
#define MATH_INCLUDE

#define EPSILON 0.0001 //this is a hyperparameter when you think about it (dont think about it)

#include <iostream>
#include <vector>

struct Tuple {
    double x, y, z, w;
    
    Tuple(double x, double y, double z, double w);
    Tuple();
    
    bool isPoint() const;
    bool isVector() const;
    double magnitude() const;
    Tuple normalize() const;
    double dot(const Tuple& other) const;
    
    double& operator[](int i);
    const double& operator[](int i) const;

    bool operator==(const Tuple& other) const;
    Tuple operator+(const Tuple& other) const;
    Tuple operator-(const Tuple& other) const;
    Tuple operator*(const Tuple& other) const;
    Tuple operator*(double scalar) const;
    //Tuple operator*(int scalar) const;
    Tuple operator/(double scalar) const;
    Tuple operator-() const;

    Tuple cross(const Tuple& other) const;
    Tuple reflect(const Tuple& normal) const;
};

Tuple operator*(double scalar, const Tuple& tuple);
Tuple operator*(int scalar, const Tuple& tuple);
std::ostream& operator<<(std::ostream& os, const Tuple& p);

Tuple Point(double x, double y, double z);
Tuple Vector(double x, double y, double z);
Tuple Color(double r, double g, double b);


struct Matrix {
    std::vector<std::vector<double>> v;
    int height, width;

    Matrix(int height, int width);
    Matrix(std::vector<std::vector<double>>& v);
    Matrix(std::initializer_list<std::vector<double>> list);
  
    std::vector<double>& operator[](int i);
    const std::vector<double>& operator[](int i ) const;

    bool operator==(const Matrix& other) const;

    Matrix operator*(const Matrix& other) const;
    Tuple operator*(const Tuple& other) const;

    Matrix T() const;

    double det() const;
    Matrix submatrix(int row, int col) const;
    double minor(int row, int col) const;
    double cofactor(int row, int col) const;
    Matrix inverse() const;
};

Matrix Translation(double x, double y, double z);
Matrix Scaling(double x, double y, double z);
Matrix Rotation_x(double rads);
Matrix Rotation_y(double rads);
Matrix Rotation_z(double rads);
Matrix Shearing(double xy, double xz, double yx, double yz, double zx, double zy);
Matrix ViewTransform(const Tuple& from, const Tuple& to, const Tuple& up);

extern const Matrix IDENTITY_MATRIX;

std::ostream& operator<<(std::ostream& os, const Matrix& p);

#endif

