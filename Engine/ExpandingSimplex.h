#pragma once

#pragma once

#include <vector>
#include <queue>
#include <memory>

class SimplexEdge
{
public:
    SimplexEdge(Vector3 point1, Vector3 point2)
    {
        Vector3 diff = point1 - point2;
        normal = diff.Cross(point1).Cross(diff);
        normal.Normalize();

        this->distance = abs(point1.Dot(normal));
        this->point1 = point1;
        this->point2 = point2;
    }

    const Vector3& getPoint1() const
    {
        return point1;
    }

    const Vector3& getPoint2() const
    {
        return point2;
    }

    const Vector3& getNormal() const
    {
        return normal;
    }

    const float getDistance() const
    {
        return distance;
    }

private:
    Vector3 point1;
    Vector3 point2;
    Vector3 normal;
    float distance;
};

struct DistancePredicate
{
    bool operator()(const shared_ptr<SimplexEdge>& lhs, const shared_ptr<SimplexEdge>& rhs) const
    {
        return lhs->getDistance() > rhs->getDistance();
    }
};

class ExpandingSimplex
{
public:
    ExpandingSimplex(const vector<Vector3>& simplex);
    const shared_ptr<SimplexEdge>& findClosestEdge();
    void expand(Vector3 Vector3);

private:
    priority_queue<shared_ptr<SimplexEdge>, vector<shared_ptr<SimplexEdge>>, DistancePredicate> edgeQueue;
};
