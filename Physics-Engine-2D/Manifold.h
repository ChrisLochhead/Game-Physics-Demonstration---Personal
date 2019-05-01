//#pragma once
#ifndef MANIFOLD_H
#define MANIFOLD_H

//#include "IEMath.h"
//#include "GameObject.h"

struct GameObject;


	glm::vec2 Cross(float a, glm::vec2 v)
	{
		return glm::vec2(-a * v.y, a * v.x);
	}

	inline float Sqr(float a)
	{
		return a * a;
	}

struct Manifold
{
	Manifold(GameObject *a, GameObject *b)
		: A(a)
		, B(b)
	{
	}

	void Solve(void);                 // Generate contact information
	void Initialize(void);            // Precalculations for impulse solving
	void ApplyImpulse(void);          // Solve impulse and apply
	void PositionalCorrection(void);  // Naive correction of positional penetration
	void InfiniteMassCorrection(void);

	GameObject *A;
	GameObject *B;

	float penetration;     // Depth of penetration from collision
	glm::vec2 normal;          // From A to B
	glm::vec2 contacts[2];     // Points of contact during collision
	int contact_count; // Number of contacts that occured during collision
	float e;               // Mixed restitution
	float df;              // Mixed dynamic friction
	float sf;              // Mixed static friction
};

#endif