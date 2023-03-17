#include "pch.h"
#include "ExpandingSimplex.h"

ExpandingSimplex::ExpandingSimplex(const vector<Vector3>& simplex)
{
    size_t size = simplex.size();
    for (uint32_t i = 0; i < size; i++) {
        int j = i + 1 == size ? 0 : i + 1;

        Vector3 a = simplex[i];
        Vector3 b = simplex[j];
        edgeQueue.push(make_shared<SimplexEdge>(a,b));
    }
}

const shared_ptr<SimplexEdge>& ExpandingSimplex::findClosestEdge()
{
    return edgeQueue.top();
}

void ExpandingSimplex::expand(Vector3 point)
{
    shared_ptr<SimplexEdge> edge = edgeQueue.top();
    edgeQueue.pop();


    shared_ptr<SimplexEdge> edge1(make_shared<SimplexEdge>(edge->getPoint1(), point));
    edgeQueue.push(edge1);
    shared_ptr<SimplexEdge> edge2(make_shared<SimplexEdge>(edge->getPoint2(), point));
    edgeQueue.push(edge2);
}
