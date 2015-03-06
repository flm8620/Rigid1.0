#include "algebra.h"

const Vector2 operator*(const Vector2& v,const Matrix2& m){
    Vector2 r;
    r.x=m.m11*v.x+m.m12*v.y;
    r.y=m.m21*v.x+m.m22*v.y;
    return r;
}
const Vector2 operator*(const Matrix2& m,const Vector2& v){
    Vector2 r;
    r.x=m.m11*v.x+m.m12*v.y;
    r.y=m.m21*v.x+m.m22*v.y;
    return r;
}


