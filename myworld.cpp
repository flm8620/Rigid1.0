#include "myworld.h"
#include "multicollision.h"
#include "userinterface.h"
namespace World {
vector<MyShape*> ShapeList;
clock_t currentTime, lastTime,lastFrameTime;
double simulateSpeed=1;
double dt=0.02;
Vector2 gravityAcc(0.0,-1);
clock_t frameTime=dt*double(CLOCKS_PER_SEC)/simulateSpeed;//除以模拟速度放大比例
Button buttonDrag,buttonPoly,buttonCircle;
}
using namespace World;
void stop_all_shape()
{
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        (*i)->vel=Vector2(0,0);
        (*i)->angVel=0;
    }
    //cout<<"stop!"<<endl;
}
void myinit() {
    readFile(ShapeList);
    lastTime=lastFrameTime=clock();
    buttonDrag.state=false;
    buttonDrag.x=10;
    buttonDrag.y=10;
    buttonDrag.w=20;
    buttonDrag.h=20;
    buttonDrag.name="Drag";
    buttonPoly.state=false;
    buttonPoly.x=buttonDrag.x;
    buttonPoly.y=buttonDrag.y+30;
    buttonPoly.w=20;
    buttonPoly.h=20;
    buttonPoly.name="Poly";
    buttonCircle.x=buttonDrag.x;
    buttonCircle.y=buttonPoly.y+30;
    buttonCircle.w=20;
    buttonCircle.h=20;
    buttonCircle.name="Circle";
}
void oneStep() {

    for_each(ShapeList.begin(),ShapeList.end(),calc_force);
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        (*i)->calc_AABB();
    }
    multiCollision(ShapeList);
    eventTreatment();
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        (*i)->makeMove(dt);
    }

}
void calc_force(MyShape* s) {
    s->force.x=s->force.y=0;
    s->torque=0;
    s->force=s->force+gravityAcc*s->mass;
}
/*
void collide_test() {
    vector<MyShape*>::iterator i;
    vector<MyShape*>::iterator j;
    MyShape *shape_inserter=0, *shape_inserted=0;
    Vector2 insert_point, collide_point, line1, line2;
    for (i=ShapeList.begin(); i<ShapeList.end()-1; i++) {
        for (j=i+1; j<ShapeList.end(); j++) {
            if (TestAABBAABB((*i)->aabb,(*j)->aabb)) {
                //对两碰撞刚体类型的分别讨论
                if ((*i)->shape_type==POLYGON&&(*j)->shape_type==POLYGON) {

                    if (collide_testPolygonPolygon(*static_cast<MyPolygon*>(*i),*static_cast<MyPolygon*>(*j),insert_point,line1,
                                                   line2,shape_inserter,shape_inserted)) {
                        Matrix2 rotate90inv(0,1,-1,0);//顺时针90度
                        Vector2 n0;
                        n0=rotate90inv*(line2-line1);
                        n0=n0/sqrt(n0*n0); //穿透单位向量
                        //collide_point=GetNearestPointToLine(insert_point,line1,line2);
                        collide_act(*shape_inserter,*shape_inserted,n0,insert_point);
                        //stop_all_shape();
                    }
                    else {
                        //cout<<"non!"<<endl;
                    } //无碰撞

                }
            }
        }
    }
}

void collide_act(MyShape &a, MyShape &b, Vector2 n, Vector2 insert_point) { //a-其顶点刺入b的几何体,b-被刺入的几何体,n-穿透向量 指向a, insert_point-穿入的顶点
    Vector2 n0, ra=insert_point-a.center, rb=insert_point-b.center, va=a.vel, vb=b.vel, vap, vbp, vr, Jvec, vr_n, vr_t, t;
    double J, Jt, e, wa=a.ang_vel, wb=b.ang_vel, t1, t2, t3, t4, miu;
    double ma_inv=a.inv_mass, mb_inv=b.inv_mass, Ia_inv=1/a.I, Ib_inv=1/b.I;
    e=std::min(a.restitution,b.restitution);
    if (a.fixToGround) {
        ma_inv=0;
        Ia_inv=0;
    }
    if (b.fixToGround) {
        mb_inv=0;
        Ib_inv=0;
    }

    Matrix2 rotate90(0,-1,1,0);//逆时针90度
    //n0=p-insert_point; //穿透向量 指向a
    //n=n0*(1./sqrt(n0*n0)); //穿透单位向量


    Vector2 temp1, temp2, temp3, temp4;
    temp1=rotate90*(wa*ra);
    vap=va+temp1;
    temp1=rotate90*(wb*rb);
    vbp=vb+temp1;

    vr=vap-vbp;
    vr_n=(vr*n)*n;
    if (vr*n>0) {
        cout<<a.ID<<"vs"<<b.ID<<"重叠但远离!"<<endl;
        return;
    } //两物体互相远离，不应产生冲量
    vr_t=vr-vr_n;
    t=rotate90*n;
    if (vr.x*n.y-vr.y*n.x>0) t=-t;
    t1=n*(rotate90*((ra.x*n.y-ra.y*n.x)*ra));
    t2=n*(rotate90*((rb.x*n.y-rb.y*n.x)*rb));
    t3=-(e+1)/(ma_inv+mb_inv+t1*Ia_inv+t2*Ib_inv);
    miu=sqrt(a.friction*b.friction);
    Jvec=t3*vr;
    J=Jvec*n;
    Jt=Jvec*t;
    if ((vr_t*vr_t)/(vr_n*vr_n)<a.friction*b.friction) { //摩擦力未超过最大静摩擦
        temp3=Jt*ma_inv*t;
        temp4=-Jt*mb_inv*t;
        t3=(ra.x*t.y-ra.y*t.x)*Jt*Ia_inv;
        t4=-(ra.x*t.y-ra.y*t.x)*Jt*Ib_inv;
    }
    else if (Jt>0) {
        temp3=J*ma_inv*miu*t;
        temp4=-J*mb_inv*miu*t;
        t3=(ra.x*t.y-ra.y*t.x)*J*miu*Ia_inv;
        t4=-(ra.x*t.y-ra.y*t.x)*J*miu*Ib_inv;
    }
    else {
        temp3=-J*ma_inv*miu*t;
        temp4=J*mb_inv*miu*t;
        t3=-(ra.x*t.y-ra.y*t.x)*J*miu*Ia_inv;
        t4=(ra.x*t.y-ra.y*t.x)*J*miu*Ib_inv;
    }

    //------
    temp1=(J*ma_inv)*n+temp3;
    temp2=(-J*mb_inv)*n+temp4;

    t1=(ra.x*n.y-ra.y*n.x)*J*Ia_inv+t3;
    t2=-(rb.x*n.y-rb.y*n.x)*J*Ib_inv+t4;
    a.vel+=temp1;
    b.vel+=temp2;
    a.ang_vel+=t1;
    b.ang_vel+=t2;
    PositionalCorrection(a,b,n0); //防穿透纠正
    //cout<<"collide!"<<endl;
}

void PositionalCorrection(MyShape &A, MyShape &B, Vector2 penetrationDepthN) {
    const double percent=0.2; // usually 20% to 80%
    const double sq_slop=0.01*0.01;
    if (penetrationDepthN*penetrationDepthN<sq_slop) {
        cout<<A.ID<<"vs"<<B.ID<<"slightly penetration"<<endl;
        return;
    }
    double ma_inv=A.inv_mass, mb_inv=B.inv_mass;
    if (A.fixToGround) ma_inv=0;
    if (B.fixToGround) mb_inv=0;
    Vector2 correction=penetrationDepthN/(ma_inv+mb_inv)*percent;
    cout<<A.ID<<"vs"<<B.ID<<"correction："<<correction.x<<'\t'<<correction.y<<endl;
    A.center+=ma_inv*correction;
    B.center-=mb_inv*correction;
}
*/

const Vector2 GetNearestPointToLine(const Vector2& c, const Vector2& a, const Vector2& b) {
    Vector2 ab=b-a;
    double t=((c-a)*ab)/(ab*ab);
    return a+t*ab;
}



