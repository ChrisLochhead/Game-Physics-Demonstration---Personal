#ifndef COLLISION_H
#define COLLISION_H

#include "Shape.h"

struct Manifold;
struct GameObject;

typedef void(*CollisionCallback)(Manifold *m, GameObject *a, GameObject *b);

extern CollisionCallback Dispatch[Shape::eCount][Shape::eCount];

void CircletoCircle(Manifold *m, GameObject *a, GameObject *b);
void CircletoPolygon(Manifold *m, GameObject *a, GameObject *b);
void PolygontoCircle(Manifold *m, GameObject *a, GameObject *b);
void PolygontoPolygon(Manifold *m, GameObject *a, GameObject *b);

#endif
