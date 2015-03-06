#include "matrix.h"

using namespace std;
Matrix::Matrix(const double *Array, int m, int n, bool rowGoFirst)
{
    this->m=m;
    this->n=n;
    M=new double[m*n];
    if(rowGoFirst){
        int k=0;
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                M[i+j*m]=Array[k];
                k++;
            }
        }
    }else{
        int k=0;
        for(int j=0;j<n;j++){
            for(int i=0;i<m;i++){
                M[i+j*m]=Array[k];
                k++;
            }
        }
    }
}

Matrix::Matrix(int m, int n)
{
    this->m=m;this->n=n;
    M=new double[m*n];
}

Matrix::Matrix(int m, int n, int value)
{
    this->m=m;this->n=n;
    M=new double[m*n];
    for(int i=0;i<m*n;i++)M[i]=value;
}

Matrix::~Matrix()
{
    delete[] M;
}

Matrix::Matrix(const Matrix &mat)
{
    //std::cout<<"copied"<<std::endl;
    m=mat.m;n=mat.n;
    M=new double[m*n];
    for(int i=0;i<m*n;i++)M[i]=mat.M[i];
}

Matrix& Matrix::operator=(const Matrix &mat)
{
    //std::cout<<"assigned"<<std::endl;
    double *temp=new double[mat.m*mat.n];
    for(int i=0,total=mat.m*mat.n; i<total; ++i){
        temp[i]=mat.M[i];
    }
    delete[] M;
    M=temp;
    m=mat.m;
    n=mat.n;
    return *this;
}

const Matrix Matrix::operator+(const Matrix &rhs)const
{
    if(m!=rhs.m||n!=rhs.n){
        std::cout<<"operator+ needs m==m n==n."<<std::endl;
        throw;
    }
    Matrix R(m,n);
    for(int i=0, total = m*n;i<total;++i){
        R.M[i]=M[i]+rhs.M[i];
    }
    return R;
}

const Matrix Matrix::operator-(const Matrix &rhs)const
{
    if(m!=rhs.m||n!=rhs.n){
        std::cout<<"operator+ needs m==m n==n."<<std::endl;
        throw;
    }
    Matrix R(m,n);
    for(int i=0, total = m*n;i<total;++i){
        R.M[i]=M[i]-rhs.M[i];
    }
    return R;
}

const Matrix Matrix::operator-() const
{
    Matrix R(m,n);
    for(int i=0, total = m*n;i<total;++i){
        R.M[i]=-M[i];
    }
    return R;
}

const Matrix Matrix::operator*(const Matrix &rhs)const
{
    if(n!=rhs.m){
        std::cout<<"operator*: n!=m"<<std::endl;
        throw;
    }
    Matrix R(m,rhs.n);
    double sum;
    int i,j,k,l;
    l=0;
    for(j=0;j<R.n;j++){
        for(i=0;i<R.m;i++){
            sum=0;
            for(k=0;k<n;k++)sum+=M[i+k*m]*rhs.M[k+j*m];
            R.M[l]=sum;
            l++;
        }
    }
    return R;
}


void Matrix::removeRow(int row)
{
    if(row>=m){
        std::cout<<"removeRow: row exceed the size"<<std::endl;
        throw;
    }
    double *temp=new double[(m-1)*n];
    int i,j,k=0;
    for(j=0;j<n;j++){
        for(i=0;i<row;i++){
            temp[k]=M[i+j*m];
            k++;
        }
        for(i=row+1;i<m;i++){
            temp[k]=M[i+j*m];
            k++;
        }
    }
    delete[] M;
    M=temp;
    m--;
}

void Matrix::removeCol(int col)
{
    if(col>=n){
        std::cout<<"removeCol: col exceed the size"<<std::endl;
        throw;
    }
    double *temp=new double[(n-1)*m];
    int i,j,k=0;
    for(j=0;j<col;j++){
        for(i=0;i<m;i++){
            temp[k]=M[i+j*m];
            k++;
        }
    }
    for(j=col+1;j<n;j++){
        for(i=0;i<m;i++){
            temp[k]=M[i+j*m];
            k++;
        }
    }
    delete[] M;
    M=temp;
    n--;
}

const Matrix Matrix::transpose() const
{
    Matrix R(n,m);
    int i,j,k=0;
    for(i=0;i<m;i++){
        for(j=0;j<n;j++){
            R.M[k]=M[i+j*m];
            k++;
        }
    }
    return R;
}

void Matrix::pasteFrom(const Matrix &mSource, int startRow, int startCol)
{
    if(mSource.m+startRow>m||mSource.n+startCol>n){
        std::cout<<"pasteFrom:source too large"<<std::endl;
        throw;
    }
    int i,j,ii,jj;
    for(i=0,ii=startRow;i<mSource.m;i++,ii++){
        for(j=0,jj=startCol;j<mSource.n;j++,jj++){
            M[ii+jj*m]=mSource.M[i+j*mSource.m];
        }
    }
}

int Matrix::reduced_row_echelon()
{
    int l,i,j,k,rank,imax;
    double pivot,absmax;
    int mn=min(m,n);
    rank=0;
    i=0;
    for(j=0;j<mn;j++){
        imax=i;
        absmax=abs(M[imax+j*m]);
        for(l=i+1;l<m;l++){//find abs max element in j_th col
            if(abs(M[l+j*m])>absmax){
                imax=l;
                absmax=abs(M[l+j*m]);
            }
        }
        cout<<"imax="<<imax<<"absmax="<<absmax<<endl;
        if(isNearlyZero(absmax)){
            continue;
        }

        rank++;
        for(k=j;k<n;k++){//swap row imax and row i
            swap(M[i+k*m],M[imax+k*m]);
        }
        print();
        pivot=M[i+j*m];
        for(k=j+1;k<n;k++)M[i+k*m]/=pivot;//divide row j by pivot
        M[i+j*m]=1;
        print();
        for(l=0;l<m;l++){//
            if(l==i)continue;
            for(k=j+1;k<n;k++)M[l+k*m]-=M[i+k*m]*M[l+j*m];
            M[l+j*m]=0;//in jth col, make (i,j)=0,where i>j+1
            print();
        }
        i++;

    }
    return rank;
}

const Matrix Matrix::inverse() const
{
    if(m!=n){
        cout<<"Matrix non-square"<<endl;throw;
    }
    int l,i,j,k,rank,imax;
    Matrix A(m,m*2,0);
    A.pasteFrom(*this,0,0);
    for(i=0;i<m;i++)A.M[i+(i+m)*m]=1;//indentity
    double pivot,absmax;
    n;
    i=0;
    for(j=0;j<m;j++){
        imax=i;
        absmax=abs(A.M[imax+j*m]);
        for(l=i+1;l<m;l++){//find abs max element in j_th col
            if(abs(A.M[l+j*m])>absmax){
                imax=l;
                absmax=abs(A.M[l+j*m]);
            }
        }
        if(isNearlyZero(absmax)){
            cout<<"Matrix non-invertible!"<<endl;
            print();
            throw 1;
        }
        for(k=j;k<m*2;k++){//swap row imax and row i
            swap(A.M[i+k*m],A.M[imax+k*m]);
        }
        pivot=A.M[i+j*m];
        for(k=j+1;k<m*2;k++)A.M[i+k*m]/=pivot;//divide row j by pivot
        A.M[i+j*m]=1;
        for(l=0;l<m;l++){//
            if(l==i)continue;
            for(k=j+1;k<m*2;k++)A.M[l+k*m]-=A.M[i+k*m]*A.M[l+j*m];
            A.M[l+j*m]=0;//in jth col, make (i,j)=0,where i>j+1
        }
        i++;

    }
    return A.getSection(0,m,m,m);
}



const Matrix Matrix::getSection(int startRow, int startCol, int rowSize, int colSize) const
{
    if(startRow+rowSize>m||startCol+colSize>n){
        std::cout<<"getSection error, size invalid"<<std::endl;
        throw;
    }
    Matrix R(rowSize,colSize);
    int i,j,k=0;
    for(j=0;j<colSize;j++){
        for(i=0;i<rowSize;i++){
            R.M[k]=M[i+startRow+(j+startCol)*m];
            k++;
        }
    }
    return R;
}

const Matrix operator*(const Matrix &mat, double lambda)
{
    Matrix R(mat.m,mat.n);
    for(int i=0, total = mat.m*mat.n;i<total;++i){
        R.M[i]=mat.M[i]*lambda;
    }
    return R;
}

const Matrix operator*(double lambda, const Matrix &mat)
{
    Matrix R(mat.m,mat.n);
    for(int i=0, total = mat.m*mat.n;i<total;++i){
        R.M[i]=mat.M[i]*lambda;
    }
    return R;
}



void Matrix::print() const
{
    using namespace std;
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){

            cout<<M[i+j*m]<<'\t';
        }
        cout<<endl;
    }
    cout<<endl;
}




bool isNearlyZero(double x)
{
    return x<1e-14&&x>-1e-14;
}


Matrix identity(int size)
{
    Matrix R(size,size,0);
    for(int i=0;i<size;i++)R.set(i,i)=1;
    return R;
}

void solveAxb(const Matrix &A, Matrix &x, const Matrix &b)
{
    if(!A.isSquare()){cout<<"solveAxb:A is not square"<<endl;throw;}
    if(A.getCol()!=x.getRow()){cout<<"solveAxb:A*x size error"<<endl;throw;}
    if(x.getCol()!=1){cout<<"solveAxb:Col of x must be 1"<<endl;throw;}
    if(b.getCol()!=1){cout<<"solveAxb:Col of b must be 1"<<endl;throw;}
    if(b.getRow()!=x.getRow()){cout<<"solveAxb:Row of b is invalid"<<endl;throw;}
    x=A.inverse()*b;

}
