#include "pch.h"
#include "RectCollider.h"
#include "CircleCollider.h"
#include "GameObject.h"
#include "Transform.h"

RectCollider::RectCollider(GameObject* obj)
    :Collider(obj)
{
    _type = ColliderType::RECT;
    _dim = 2;
}

RectCollider::~RectCollider()
{

}

bool RectCollider::Intersects(Collider* other, OUT Vector3& dis)
{
    assert(other->getDim() == this->getDim());
    dis = Vector3(1e5, 0, 0);
    const Matrix& other2world = other->GetColliderTransform();
    Vector3 centerdiff = other2world.Translation() - toWorld.Translation();

    if (other->GetType() == ColliderType::RECT)
    {
        Vector3 myUp = toWorld.Up() * 0.5f;
        Vector3 otherUp = other2world.Up() * 0.5f;
        Vector3 myRight = toWorld.Right() * 0.5f;
        Vector3 otherRight = other2world.Right() * 0.5f;

        float diff = (abs(myRight.Dot(centerdiff)) - (myRight.LengthSquared() + abs(myRight.Dot(otherUp)) + abs(myRight.Dot(otherRight)))) / myRight.Length();

        if (diff > 0)
            return false;

        else {
            if (-diff < dis.Length()) {
                dis = myRight * (diff) / myRight.Length();

                if (myRight.Dot(centerdiff) > 0)
                    dis = -dis;
            }
        }

        diff = (abs(myUp.Dot(centerdiff)) - (myUp.LengthSquared() + abs(myUp.Dot(otherUp)) + abs(myUp.Dot(otherRight)))) / myUp.Length();

        if (diff > 0)
            return false;

        else {
            if (-diff < dis.Length()) {
                dis = myUp * (diff) / myUp.Length();

                if (myUp.Dot(centerdiff) > 0)
                    dis = -dis;
            }
        }

        diff = (abs(otherRight.Dot(centerdiff)) - (otherRight.LengthSquared() + abs(otherRight.Dot(myUp)) + abs(otherRight.Dot(myRight)))) / otherRight.Length();

        if (diff > 0)
            return false;

        else {
            if (-diff < dis.Length()) {
                dis = otherRight * (diff) / otherRight.Length();

                if (otherRight.Dot(centerdiff) > 0)
                    dis = -dis;
            }
        }

        diff = (abs(otherUp.Dot(centerdiff)) - (otherUp.LengthSquared() + abs(otherUp.Dot(myUp)) + abs(otherUp.Dot(myRight)))) / otherUp.Length();

        if (diff > 0)
            return false;

        else {
            if (-diff < dis.Length()) {
                dis = otherUp * (diff) / otherUp.Length();

                if (otherUp.Dot(centerdiff) > 0)
                    dis = -dis;
            }
        }
        dis.z = 0.f;
        return true;
    }
    else if (other->GetType() == ColliderType::CIRCLE)
    {
        float radius = other->GetColliderTransform().Up().Length()/2;

        Vector3 right = toWorld.Right() * 0.5f;
        float rightLen = right.Length();
        Vector3 rightDiff = right * centerdiff.Dot(right) / right.LengthSquared();


        Vector3 up = toWorld.Up() * 0.5f;
        float upLen = up.Length();
        Vector3 upDiff = centerdiff.Dot(up) * up / up.LengthSquared();
        

        float rightIn = 0.f;
        if (rightDiff.Length() > rightLen + radius)
            return false;
        else
            rightIn = rightDiff.Length() - rightLen;

        

        float upIn = 0.f;
        if (upDiff.Length() > upLen + radius)
            return false;
        else
            upIn = upDiff.Length() - upLen;

        upDiff.Normalize();
        rightDiff.Normalize();

        if (rightIn < 0 || upIn < 0)
        {
            if (upIn > rightIn)
                dis = upDiff * (radius - upIn);
            else
                dis = rightDiff * (radius - rightIn);
        }
        else {
            dis = centerdiff - up * centerdiff.Dot(up) / abs(centerdiff.Dot(up)) - right * centerdiff.Dot(right) / abs(centerdiff.Dot(right));
            dis *= (radius - dis.Length()) / dis.Length();
        }
        dis.z = 0.f;

        return true;
    }
    else
        return false;
}

bool RectCollider::RayCast(const Vector3& origin, const Vector3& dir, OUT float& dist)
{
    BoundingOrientedBox b({ 0.f,0.f,0.f }, { 0.5f, 0.5f, 0.5f }, { 0.f, 0.f, 0.f, 1.f });
    BoundingOrientedBox bob;
    b.Transform(bob, XMLoadFloat4x4(&toWorld));

    return bob.Intersects(origin, dir, dist);
}
