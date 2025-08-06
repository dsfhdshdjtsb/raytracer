#include <gtest/gtest.h>
#include "../src/utils/math.h"
#include <math.h>
TEST(Tuples, TupleWithW1IsPoint) {
    Tuple a(4.3, -4.2, 3.1, 1.0);
    EXPECT_DOUBLE_EQ(a.x, 4.3);
    EXPECT_DOUBLE_EQ(a.y, -4.2);
    EXPECT_DOUBLE_EQ(a.z, 3.1);
    EXPECT_DOUBLE_EQ(a.w, 1.0);
    EXPECT_TRUE(a.isPoint());
    EXPECT_FALSE(a.isVector());
}
TEST(Tuples, TupleWithW0IsVector) {
    Tuple a(4.3, -4.2, 3.1, 0.0);
    EXPECT_DOUBLE_EQ(a.x, 4.3);
    EXPECT_DOUBLE_EQ(a.y, -4.2);
    EXPECT_DOUBLE_EQ(a.z, 3.1);
    EXPECT_DOUBLE_EQ(a.w, 0.0);
    EXPECT_FALSE(a.isPoint());
    EXPECT_TRUE(a.isVector());
}
TEST(Tuples, PointHasW1) {
    Tuple a = Point(4, -4, 3);
    Tuple t(4, -4, 3, 1);
    EXPECT_EQ(a,t);
}
TEST(Tuples, VectorHasW0) {
    Tuple a = Vector(4, -4, 3);
    Tuple t(4, -4, 3, 0);
    EXPECT_EQ(a,t);
}

TEST(Tuples, Opers) {
    Tuple a = Vector(4,-4, 0);
    Tuple t = Vector(4, -4, 3);

    EXPECT_EQ(a+t, Tuple(8, -8, 3, 0));

    EXPECT_EQ(a-t, Tuple(0, 0, -3, 0));

    Tuple b = Vector(1,2,3);

    EXPECT_EQ(-b, Vector(-1,-2,-3));
    EXPECT_EQ(2 * b, Vector(2,4,6));
    std::cout << (a.dot(t)) << std::endl;
    EXPECT_EQ(32, a.dot(t));
}
TEST(Tuples, Cross) {
    Tuple a = Vector(1,2,3);
    Tuple b = Vector(2,3,4);
    EXPECT_EQ(Vector(1, -2, 1) ,b.cross(a) );
}

TEST(Tuples, HadamardProduct) {
    Tuple c = Color(1, 0.2, 0.4);
    Tuple d = Color(0.9, 1, 0.1);
    EXPECT_EQ(c * d, Color(0.9, 0.2, 0.04));
}

TEST(Matrices, MatrixOpers) {
    Matrix m(4,4);

    std::vector<std::vector<double>> vec = {
        {1,2,3},
        {4,5,6},
        {7,8,9},
    };
    std::vector<std::vector<double>> vec2 = {
        {1,2,3},
        {4,0,6},
        {7,8,9},
    }; 

    Matrix m1(vec);
    Matrix m2(vec2);
    Matrix m3(3, 3);

    EXPECT_EQ(false, m1 == m3);
    EXPECT_EQ(false, m2 == m3);
    EXPECT_EQ(false, m1 == m2);
    m2[1][1] = 5;
    EXPECT_EQ(true, m1 == m2);

    std::vector<std::vector<double>> vec3 = {
        {1,2,3,4},
        {2,4,4,2},
        {8,6,4,1},
        {0,0,0,1},
    };
    std::vector<std::vector<double>> vec3t = {
        {1,2,8,0},
        {2,4,8,0},
        {3,4,4,0},
        {4,2,1,1},
    };

    Matrix m4(vec3);
    Tuple t(1,2,3,1);
    Tuple a(18,24,33,1);

    std::cout << m4;
    EXPECT_EQ(a,m4 * t ); 
}

TEST(Matrices, Determinant) {
    Matrix m({
        {1,5, 0},
        {-3, 2, 7},
        {0, 6, -3}
    });
    Matrix sm = m.submatrix(0,2);
    EXPECT_EQ(Matrix({{-3,2}, {0,6}}), sm);
    EXPECT_EQ(-18, sm.det());
    EXPECT_EQ(-18, m.minor(0,2));

    Matrix m2({
        {3,5,0},
        {2, -1, -7},
        {6, -1, 5},
    });
    EXPECT_EQ(-12, m2.cofactor(0,0));
    EXPECT_EQ(-25, m2.cofactor(1, 0));

    Matrix m3({
        {-2, -8, 3, 5},
        {-3, 1, 7, 3},
        {1, 2, -9, 6},
        {-6, 7, 7, -9},
    });

    EXPECT_EQ(-4071, m3.det());
}

TEST(Matrices, Inverse) {
    Matrix m({
        {8, -5, 9, 2},
        {7, 5, 6, 1},
        {-6, 0, 9, 6},
        {-3, 0, -9, -4},
    });
    Matrix mi({
        {-0.15385, -0.15385, -0.28205,-0.53846 },
        { -0.07692, 0.12308, 0.02564, 0.03077},
        { 0.35897, 0.35897, 0.43590, 0.92308}, 
        { -0.69231, -0.69231, -0.76923,-1.92308}
    });
    EXPECT_EQ(mi, m.inverse());
    EXPECT_EQ(IDENTITY_MATRIX, mi * m);
}

TEST(Transformations, Translation) {
    Matrix transformation = Translation(5.0f, -3.0f, 2.0f);
    Tuple point = Point(-3, 4, 5);
    Tuple vec = Vector(-3, 4, 5);
    EXPECT_EQ(Point(2,1,7), transformation * point);
    EXPECT_EQ(Point(-8, 7, 3), transformation.inverse() * point);
    EXPECT_EQ(vec, transformation * vec);

    //TODO: SCALING AND OTHER TRANSFOMRATIONS
}

TEST(Vectors, Reflections) {
    Tuple v = Vector(0, -1, 0);
    Tuple n = Vector(sqrt(2)/2, sqrt(2)/2, 0);
    Tuple r = v.reflect(n);
    EXPECT_EQ(Vector(1,0,0), r);
}

TEST(Matrices, ViewSpace) {
    Tuple from = Point(1, 3, 2);
    Tuple to = Point(4, -2, 8);
    Tuple up = Point(1, 1, 0);

    Matrix t = ViewTransform(from, to, up);

    EXPECT_EQ(Matrix({
        {-0.50709 , 0.50709, 0.67612,-2.36643 },
        { 0.76772, 0.60609, 0.12122, -2.82843},
        { -0.35857, 0.59761, -0.71714, 0.00000},
        { 0.00000, 0,0 , 1.00000}
    }), t);
}