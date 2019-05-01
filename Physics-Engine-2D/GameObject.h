#ifndef BODY_H
#define BODY_H


struct Shape;



struct GameObject
{

	GameObject(Shape *shape_, int x, int y, float red, float green, float blue, float velx, float vely);

	void ApplyForce(const glm::vec2& f)
	{
		force += f;
	}

	void ApplyImpulse(const glm::vec2& impulse, const glm::vec2& contactVector)
	{
		velocity += im * impulse;
		angularVelocity += iI * Cross(contactVector, impulse);
	}

	void SetStatic(void)
	{
		I = 0.0f;
		iI = 0.0f;
		m = 0.0f;
		im = 0.0f;
	}

	void SetOrient(float radians);

	glm::vec2 position;
	glm::vec2 velocity;

	float angularVelocity;
	float torque;
	float orient; // radians
	glm::vec2 force;

	// Set by shape
	float I;  // moment of inertia
	float iI; // inverse inertia
	float m;  // mass
	float im; // inverse mass

	float staticFriction;
	float dynamicFriction;
	float restitution;

	// Shape interface
	Shape *shape;

	// Store a color in RGB format
	float r, g, b;
};

#endif