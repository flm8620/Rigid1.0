#include "lcp.h"
#include <vector>

using namespace std;
enum Clamp{Ignored,Clamped,NonClamped};
void driveToZero(int d, vector<Clamp> &clamp, Matrix &f, Matrix &a, const Matrix &A);
const Matrix fDirection(int d, vector<Clamp> &clamp, const Matrix&A);
double maxStep(Matrix &f, Matrix &a, Matrix &delta_f, Matrix& delta_a, vector<Clamp> clamp, int d, int &index);
void LCPsolver(const Matrix &A, const Matrix &b, Matrix &f)
{
    if(!A.isSquare()){cout<<"LCP:A is not square"<<endl;throw;}
    if(A.getCol()!=f.getRow()){cout<<"LCP:f size invalid"<<endl;throw;}
    if(A.getCol()!=b.getRow()){cout<<"LCP:b size invalid"<<endl;throw;}
    if(b.getCol()!=1){cout<<"LCP:b.col!=1"<<endl;throw;}
    if(f.getCol()!=1){cout<<"LCP:f.col!=1"<<endl;throw;}
    vector<Clamp> clamp(A.getCol(),Ignored);//clamp set
    for(int i=0;i<f.getRow();i++)f.set(i,0)=0;
    Matrix a=b;
    for(int d=0, total = f.getRow();d<total;++d){
        if(a.get(d,0)<-eps){
            driveToZero(d,clamp,f,a,A);
        }else{
            clamp[d]=NonClamped;
        }
    }
}
void driveToZero(int d,vector<Clamp>& clamp,Matrix &f,Matrix &a,const Matrix &A){
    double s;
    int j;
    while(1){
        Matrix delta_f=fDirection(d,clamp,A);
        Matrix delta_a=A*delta_f;
        s=maxStep(f,a,delta_f,delta_a,clamp,d,j);
        for(int i=0, total = f.getRow();i<total;++i){
            f.set(i,0)+=s*delta_f.get(i,0);
            a.set(i,0)+=s*delta_a.get(i,0);
        }
        if(clamp[j]==Clamped){
            clamp[j]=NonClamped;
        }else if(clamp[j]==NonClamped){
            clamp[j]=Clamped;
        }else{
            clamp[j]=Clamped;
            return;
        }
    }
}
const Matrix fDirection(int d,vector<Clamp>& clamp, const Matrix&A){
    Matrix delta_f(A.getCol(),1,0);
    delta_f.set(d,0)=1;
    int *a=new int[A.getCol()];
    int i,j,size=0;
    for(i=0;i<A.getCol();i++){
        if(clamp[i]==Clamped){
            a[size]=i;
            size++;
        }
    }
    Matrix Acc(size,size),x(size,1),v(size,1);
    int k=0;
    for(i=0;i<size;i++){
        v.set(i,0)=-A.get(a[i],d);
        for(j=0;j<size;j++){
            Acc.set(i,j)=A.get(a[i],a[j]);
        }
    }
    solveAxb(Acc,x,v);
    for(i=0;i<size;i++){
        delta_f.set(a[i],0)=x.get(i,0);
    }
    delete[] a;
    return delta_f;
}
double maxStep(Matrix &f,Matrix &a,Matrix &delta_f,Matrix& delta_a,vector<Clamp> clamp,int d,int &index){
    double s,s1;
    s=1e10;
    if(delta_a.get(d,0)>eps){
        index=d;
        s=-a.get(d,0)/delta_a.get(d,0);
    }
    for(int i=0;i<d;i++){
        if(clamp[i]==Clamped){
            if(delta_f.get(i,0)<-eps){
                s1=-f.get(i,0)/delta_f.get(i,0);
                if(s1<s){
                    s=s1;
                    index=i;
                }
            }
        }else{
            if(delta_a.get(i,0)<-eps){
                s1=-a.get(i,0)/delta_a.get(i,0);
                if(s1<s){
                    s=s1;
                    index=i;
                }
            }
        }
    }
    return s;
}
