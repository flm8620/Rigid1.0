#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>
#include <cmath>
const double eps=1e-10;
class Matrix//index starts from 0
{
    double *M;
    int m,n;
public:
    Matrix(int m,int n);
    Matrix(int m,int n,int value);
    Matrix(const double *Array,int m,int n,bool rowGoFirst=true);
    ~Matrix();
    //copy constructor
    Matrix(const Matrix &mat);
    //operator
    Matrix& operator=(const Matrix& mat);
    const Matrix operator+(const Matrix &rhs)const;
    const Matrix operator-(const Matrix &rhs)const;
    const Matrix operator-()const;
    const Matrix operator*(const Matrix &rhs)const;
    friend const Matrix operator*(const Matrix &mat,double lambda);
    friend const Matrix operator*(double lambda,const Matrix &mat);
    //operation
    void removeRow(int row);
    void removeCol(int col);
    const Matrix transpose()const;
    void pasteFrom(const Matrix& mSource,int startRow,int startCol);
    int reduced_row_echelon();
    const Matrix inverse()const;
    //access

    const Matrix getSection(int startRow, int startCol, int rowSize, int colSize)const;
    double get(int i,int j)const{return M[i+j*m];}
    double& set(int i,int j){return M[i+j*m];}//editable
    int getRow()const{return m;}
    int getCol()const{return n;}
    void print()const;
    bool isSquare()const{return m==n;}
};
inline bool isNearlyZero(double x);
Matrix identity(int size);
void solveAxb(const Matrix &A,Matrix &x,const Matrix &b);
#endif // MATRIX_H
