#ifndef MATRIX2_H
#define MATRIX2_H
#include "vector2.h"

class Matrix2
{
public:
    double m11,m12,m21,m22;
    Matrix2();
    Matrix2(double,double,double,double);
    const Matrix2 operator+(const Matrix2)const;
    const Matrix2 operator-(const Matrix2)const;
    const Matrix2 operator*(const Matrix2)const;
};

#endif // MATRIX2_H
