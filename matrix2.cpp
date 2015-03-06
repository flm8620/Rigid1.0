#include "matrix2.h"

Matrix2::Matrix2()
{
    m11=m12=m21=m22=0;
}
Matrix2::Matrix2(double a11,double a12,double a21,double a22)
{
    m11=a11;m12=a12;m21=a21;m22=a22;
}
const Matrix2 Matrix2::operator+(const Matrix2 m)const{
    Matrix2 r;
    r.m11=m11+m.m11;
    r.m12=m12+m.m12;
    r.m21=m21+m.m21;
    r.m22=m22+m.m22;
    return r;
}
const Matrix2 Matrix2::operator-(const Matrix2 m)const{
    Matrix2 r;
    r.m11=m11-m.m11;
    r.m12=m12-m.m12;
    r.m21=m21-m.m21;
    r.m22=m22-m.m22;
    return r;
}
const Matrix2 Matrix2::operator*(const Matrix2 m)const{
    Matrix2 r;
    r.m11=m11*m.m11+m12*m.m21;
    r.m12=m11*m.m12+m12*m.m22;
    r.m21=m21*m.m11+m22*m.m21;
    r.m22=m21*m.m12+m22*m.m22;
    return r;
}

