#ifndef ALGEBRA_H
#define ALGEBRA_H

#include "matrix2.h"
#include "vector2.h"
const Vector2 operator*(const Matrix2&,const Vector2&);
const Vector2 operator*(const Vector2&,const Matrix2&);
inline void rotate90clockwise(Vector2 &v)
{
    double t=v.x;
    v.x=v.y;v.y=-t;
}
inline void rotate90anticlockwise(Vector2 &v)
{
    double t=v.x;
    v.x=-v.y;v.y=t;
}
inline double crossProd2D(const Vector2 &a, const Vector2 &b)
{
    return a.x*b.y-a.y*b.x;
}
#endif // ALGEBRA_H
