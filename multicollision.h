#ifndef MULTICOLLISION_H
#define MULTICOLLISION_H

#include "algebra.h"
#include "MyShape.h"
#include "matrix.h"
#include <vector>
class ContactPoint{
public:
    Vector2 pos;
    int shape1,shape2;
    Vector2 n;
    ContactPoint(Vector2 position, Vector2 normalvec, int shape1, int shape2);
};


class Graph
{
    Matrix G;
    int size;
public:
    Graph(int n);
    int getsize(){return size;}
    void linkTwoNodes(int i,int j){G.set(i,j)=G.set(j,i)=1;}
    bool islinked(int i,int j){return G.get(i,j)==1;}
    std::vector<int> getNeighbours(int node);
};
void multiCollision(std::vector<MyShape *> &Shapes);

std::vector<std::vector<int> > groupingGraph(Graph G);
#endif // MULTICOLLISION_H
