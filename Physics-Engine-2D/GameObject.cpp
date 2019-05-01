#include "PreCompiled.h"


GameObject::GameObject(Shape *shape_, int x, int y, float red, float green, float blue, float velx, float vely)
	: shape(shape_->Clone())
{
	shape->gameobject = this;
	position = glm::vec2((float)x, (float)y);
	velocity = glm::vec2(velx, vely);
	angularVelocity = 0;
	torque = 0;
	orient = Random(-PI, PI);
	force = glm::vec2(0, 0);
	staticFriction = 0.5f;
	dynamicFriction = 0.3f;
	restitution = 0.2f;
	shape->Initialize();
	r = red;
	g = green;
	b = blue;
}

void GameObject::SetOrient(float radians)
{
	orient = radians;
	shape->SetOrient(radians);
}
