#include "math.h"
#include <cmath>

/*--------------------globals and shit--------------*/
const Matrix IDENTITY_MATRIX({
    {1,0,0,0},
    {0,1,0,0},
    {0,0,1,0},
    {0,0,0,1}
});

/*--------------------TUPLE-----------------*/
Tuple::Tuple(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Tuple::Tuple() : x(0), y(0), z(0), w(0) {}

bool Tuple::isPoint() const {
    return w == 1.0f;
}

bool Tuple::isVector() const {
    return w == 0.0f;
}

float& Tuple::operator[](int i) {
    switch(i) {
        case 0:
            return this->x;
            break;
        case 1:
            return this->y;
            break;
        case 2:
            return this->z;
            break;
        case 3: 
            return this->w;
            break;
        default:
            throw std::invalid_argument("invalid tuple index");
    }
}
const float& Tuple::operator[](int i) const {
    switch(i) {
        case 0:
            return this->x;
            break;
        case 1:
            return this->y;
            break;
        case 2:
            return this->z;
            break;
        case 3: 
            return this->w;
            break;
        default:
            throw std::invalid_argument("invalid tuple index");
    }
}

float Tuple::magnitude() const {
    return sqrt((x * x) + (y * y) + (z * z)); //ignores w
}

Tuple Tuple::normalize() const {
    float mag = this->magnitude();
    return Tuple(x / mag, y / mag, z / mag, w / mag); //w should be 0
}

float Tuple::dot(const Tuple& other) const {
    return x * other.x + y * other.y + z * other.z + w * other.w;
}

bool Tuple::operator==(const Tuple& other) const {
    return (std::abs(x - other.x) < EPSILON) && 
           (std::abs(y - other.y) < EPSILON) && 
           (std::abs(z - other.z) < EPSILON) && 
           (std::abs(w - other.w) < EPSILON);
}

Tuple Tuple::operator+(const Tuple& other) const {
    return Tuple(x + other.x, y + other.y, z + other.z, w + other.w);
}

Tuple Tuple::operator-(const Tuple& other) const {
    return Tuple(x - other.x, y - other.y, z - other.z, w - other.w);
}

Tuple Tuple::operator*(const Tuple& other) const {
    return Tuple(x * other.x, y * other.y, z * other.z, w * other.w);
}

Tuple Tuple::operator*(float scalar) const {
    return Tuple(x * scalar, y * scalar, z * scalar, w * scalar);
}

//Tuple Tuple::operator*(int scalar) const {
    //return Tuple(x * scalar, y * scalar, z * scalar, w * scalar);
//}

Tuple Tuple::operator/(float scalar) const {
    return Tuple(x / scalar, y / scalar, z / scalar, w / scalar);
}

Tuple Tuple::operator-() const {
    return Tuple(-x, -y, -z, -w);
}

Tuple Vector(float x, float y, float z) {
    return Tuple(x, y, z, 0);
}
Tuple Point(float x, float y, float z) {
    return Tuple(x,y,z, 1);
}

Tuple Color(float x, float y, float z) {
    return Tuple(x, y, z, 0);
}

Tuple Tuple::cross(const Tuple& other) const {
    return Vector(y * other.z - z * other.y,
                  z * other.x - x * other.z,
                  x * other.y - y * other.x);
}

Tuple operator*(float scalar, const Tuple& tuple) {
    return Tuple(tuple.x * scalar, tuple.y * scalar, tuple.z * scalar, tuple.w * scalar);
}

Tuple operator*(int scalar, const Tuple& tuple) {
    return Tuple(tuple.x * scalar, tuple.y * scalar, tuple.z * scalar, tuple.w * scalar);
}

std::ostream& operator<<(std::ostream& os, const Tuple& p) {
    os << "(" << p.x << ", " << p.y << ", " << p.z << ", " << p.w << ")";
    return os;
}

/*--------------------MATRIX-----------------*/
Matrix::Matrix(int height, int weight) {
    v = std::vector(height, std::vector<float>(weight));
    this->height = height;
    this->width = weight;
}

Matrix::Matrix(std::vector<std::vector<float>>& v) {
    this->v = v;
    this->height = v.size();
    this->width = v[0].size();
}

Matrix::Matrix(std::initializer_list<std::vector<float>> list) {
    this->v = list;
    this->height = v.size();
    this->width = v[0].size();
}

bool Matrix::operator==(const Matrix& other) const {
    if(this->height != other.height) return false;
    if(this->width != other.width) return false;

    for(int i = 0; i < this->height; i++ ) {
        for(int j = 0; j < this->width; j++) {
            if(std::abs(this->v[i][j] - other.v[i][j]) >= EPSILON) return false;
        }
    }
    return true;
}
std::vector<float> & Matrix::operator[](int i) {
    return v[i];
}
const std::vector<float>& Matrix::operator[](int i) const {
    return v[i];
}
Matrix Matrix::operator*(const Matrix& other) const {
    if(this->height != other.height) throw std::invalid_argument("matrix multiplication of mismatched sizes");
    if(this->width != other.width) throw std::invalid_argument("matrix multiplication of mismatched sizes");
    if(this->width != 4 || this->height != 4) throw std::invalid_argument("matrix multiplication only defined for size 4");

    std::vector<std::vector<float>> result(4, std::vector<float>(4, 0.0f));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += this->v[i][k] * other.v[k][j];
            }
            result[i][j] = sum;
        }
    }
    return Matrix(result);
}

Tuple Matrix::operator*(const Tuple& other) const {
    if(this->width != 4 || this->height != 4) throw std::invalid_argument("matrix multiplication only defined for size 4");

    Tuple result;

    for(int i = 0; i < 4; i++) {
        float sum = 0;
        for(int j = 0; j < 4; j++) {
            sum += this->v[i][j] * other[j];
        }
        result[i] = sum; 
    }
    return result;    
}

Matrix Matrix::T() const {
    Matrix result(width, height);

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            result[j][i] = v[i][j];
        }
    }
    return result;
}
float Matrix::det() const {
    if(height == 2 && width == 2) return v[0][0] * v[1][1] - v[0][1] * v[1][0];
    float res = 0;
    for(int i = 0; i < width; i++) { //cofactor expand across the first row cause why not
        res += v[0][i] * this->cofactor(0,i);
    }
    return res;
}
Matrix Matrix::submatrix(int row, int col) const {
    Matrix result(height - 1, width -1 );
    int ri = 0;
    for(int i = 0; i < height; i++)  {
        if(i == row) continue;
        int rj = 0;
        for(int j = 0; j < width; j++) {
            if(j == col) continue;
            result[ri][rj] = v[i][j];
            rj++;
        }
        ri++;
    }
    return result;
}

float Matrix::minor(int row, int col) const {
    return this->submatrix(row, col).det();
}
float Matrix::cofactor(int row, int col) const {
    int sign = 1;
    if((row + col) % 2 == 1) {
        sign = -1;
    }
    return minor(row, col) * sign;
}

Matrix Matrix::inverse() const {
    float det = this->det(); 

    Matrix res(height, width);

    for(int i = 0; i < height; i++) { //height and width should be the same but whatever
        for(int j = 0; j < width; j++) {
            float c = this->cofactor(i, j);

            res[j][i] = c / det;
        }
    }
    return res;
}

Matrix Translation(float x, float y, float z) {
    return Matrix({
        {1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1},
    });
}

Matrix Scaling(float x, float y, float z) {
    return Matrix({
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, z, 0},
        {0, 0, 0, 1},
    });
}

Matrix Rotation_x(float rads) {
    return Matrix({
        {1, 0, 0, 0},
        {0, cosf(rads), -sinf(rads), 0},
        {0, sinf(rads), cosf(rads), 0},
        {0, 0, 0, 1},
    });
}
Matrix Rotation_y(float rads) {
    return Matrix({
        {cosf(rads), 0, sinf(rads), 0},
        {0, 1, 0, 0},
        {-sinf(rads), 0, cosf(rads), 0},
        {0, 0, 0, 1},
    });
}
Matrix Rotation_z(float rads) {
    return Matrix({
        {cosf(rads), -sinf(rads), 0, 0},
        {sinf(rads), cosf(rads), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    });
}
Matrix Shearing(float xy, float xz, float yx, float yz, float zx, float zy) {
    return Matrix({
        {1, xy, xz, 0},
        {yx, 1, yz, 0},
        {zx, 0, 1, 0},
        {0, 0, 0, 1},
    });
}

std::ostream& operator<<(std::ostream& os, const Matrix& p) {
    for(int i = 0; i < p.height; i++) {
        os << "| ";
        for(int j = 0; j < p.width; j++) {
            os << p[i][j] << " ";
        }
        os << "|";
        if(i != p.height - 1) os << std::endl;
    }
    return os;
}

