#include "PreCompiled.h"
#include <iostream>

using namespace std;

void IntegrateForces(GameObject *b, float dt)
{
	if (b->im == 0.0f)
		return;

	b->velocity += (b->force * b->im + gravity) * (dt / 2.0f);
	b->angularVelocity += b->torque * b->iI * (dt / 2.0f);
}

void IntegrateVelocity(GameObject *b, float dt)
{
	if (b->im == 0.0f)
		return;

	b->position += b->velocity * dt;
	b->orient += b->angularVelocity * dt;
	b->SetOrient(b->orient);
	IntegrateForces(b, dt);
}

void Scene::Step(void)
{
	// Generate new collision info
	contacts.clear();
	for (int i = 0; i < bodies.size(); ++i)
	{
		GameObject *A = bodies[i];

		for (int j = i + 1; j < bodies.size(); ++j)
		{
			GameObject *B = bodies[j];
			if (A->im == 0 && B->im == 0)
				continue;
			Manifold m(A, B);
			m.Solve();
			if (m.contact_count)
				contacts.emplace_back(m);
		}
	}

	// Integrate forces
	for (int i = 0; i < bodies.size(); ++i)
		IntegrateForces(bodies[i], m_dt);

	// Initialize collision
	for (int i = 0; i < contacts.size(); ++i)
		contacts[i].Initialize();

	// Solve collisions
	for (int j = 0; j < m_iterations; ++j)
		for (int i = 0; i < contacts.size(); ++i)
			contacts[i].ApplyImpulse();

	// Integrate velocities
	for (int i = 0; i < bodies.size(); ++i)
		IntegrateVelocity(bodies[i], m_dt);

	// Correct positions
	for (int i = 0; i < contacts.size(); ++i)
		contacts[i].PositionalCorrection();

	// Clear all forces
	for (int i = 0; i < bodies.size(); ++i)
	{
		GameObject *b = bodies[i];
		b->force = glm::vec2(0, 0);
		b->torque = 0;
	}
}

void Scene::Render(void)
{
	for (int i = 0; i < bodies.size(); ++i)
	{
		GameObject *b = bodies[i];
		b->shape->Draw();
	}

	glPointSize(4.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < contacts.size(); ++i)
	{
		Manifold& m = contacts[i];
		for (int j = 0; j < m.contact_count; ++j)
		{
			glm::vec2 c = m.contacts[j];
			glVertex2f(c.x, c.y);
		}
	}
	glEnd();
	glPointSize(1.0f);

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < contacts.size(); ++i)
	{
		Manifold& m = contacts[i];
		glm::vec2 n = m.normal;
		for (int j = 0; j < m.contact_count; ++j)
		{
			glm::vec2 c = m.contacts[j];
			glVertex2f(c.x, c.y);
			n *= 0.75f;
			c += n;
			glVertex2f(c.x, c.y);
		}
	}
	glEnd();
}

GameObject *Scene::Add(Shape *shape, int x, int y, float r, float g, float b, float velx, float vely)
{
	assert(shape);
	GameObject *o = new GameObject(shape, x, y, r, g, b, velx,vely);
	bodies.push_back(o);
	cout << bodies.size() << endl;
	return o;
}
