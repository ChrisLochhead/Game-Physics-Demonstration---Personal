#ifndef SCENE_H
#define SCENE_H

#include "IEMath.h"

struct Scene
{
	Scene(float dt, int iterations)
		: m_dt(dt)
		, m_iterations(iterations)
	{
	}

	float GetPosition(char coord)
	{
		if (coord == 'x')
		{
			return (float)bodies[4]->position.x;
		}
		else {
			return (float)bodies[4]->position.y;
		}
	}

	void Step(void);
	void Render(void);
	GameObject *Add(Shape *shape, int x, int y, float r, float g, float b, float velx, float vely);
	void moveCannon(float angle) {
		bodies[4]->SetOrient(angle);
	}
	void Clear(void);

	float m_dt;
	int m_iterations;
	std::vector<GameObject*> bodies;
	std::vector<Manifold> contacts;
};

#endif