#include "PreCompiled.h"

void Manifold::Solve(void)
{
	Dispatch[A->shape->GetType()][B->shape->GetType()](this, A, B);
}

void Manifold::Initialize(void)
{
	// Calculate average restitution
	e = std::min(A->restitution, B->restitution);

	// Calculate static and dynamic friction
	sf = std::sqrt(A->staticFriction * B->staticFriction);
	df = std::sqrt(A->dynamicFriction * B->dynamicFriction);

	for (int i = 0; i < contact_count; ++i)
	{
		// Calculate radii from COM to contact
		glm::vec2 ra = contacts[i] - A->position;
		glm::vec2 rb = contacts[i] - B->position;
		glm::vec2 rv = B->velocity + glm::vec2(B->angularVelocity * rb.y, -B->angularVelocity * rb.x) -
			A->velocity - glm::vec2(A->angularVelocity * ra.y, -A->angularVelocity * ra.x);


		// Determine if we should perform a resting collision or not
		// The idea is if the only thing moving this object is gravity,
		// then the collision should be performed without any restitution
		glm::vec2 tmp = dt * gravity;
		if (rv.x * rv.x + rv.y * rv.y < tmp.x * tmp.x + tmp.y * tmp.y + EPSILON)
			e = 0.0f;
	}
}

void Manifold::ApplyImpulse(void)
{
	// Early out and positional correct if both objects have infinite mass
	if (std::abs((A->im + B->im) - 0) <= EPSILON)
	{
		InfiniteMassCorrection();
		return;
	}

	for (int i = 0; i < contact_count; ++i)
	{
		// Calculate radii from COM to contact
		glm::vec2 ra = contacts[i] - A->position;
		glm::vec2 rb = contacts[i] - B->position;

		// Relative velocity
		glm::vec2 rv = B->velocity + glm::vec2(-B->angularVelocity * rb.y, B->angularVelocity * rb.x)-
			A->velocity - glm::vec2(-A->angularVelocity * ra.y, A->angularVelocity * ra.x);

		// Relative velocity along the normal
		float contactVel = rv.x * normal.x + rv.y * normal.y;

		// Do not resolve if velocities are separating
		if (contactVel > 0)
			return;

		float raCrossN = ra.x * normal.y - ra.y * normal.x;
		float rbCrossN = rb.x * normal.y - rb.y * normal.x;
		float invMassSum = A->im + B->im + (raCrossN * raCrossN) * A->iI + (rbCrossN * rbCrossN) * B->iI;

		// Calculate impulse scalar
		float j = -(1.0f + e) * contactVel;
		j /= invMassSum;
		j /= (float)contact_count;

		// Apply impulse
		glm::vec2 impulse = normal * j;
		A->ApplyImpulse(-impulse, ra);
		B->ApplyImpulse(impulse, rb);

		// Friction impulse
		rv = B->velocity + glm::vec2(-B->angularVelocity * rb.y, B->angularVelocity * rb.x) -
			A->velocity - glm::vec2(-A->angularVelocity * ra.y, A->angularVelocity * ra.x);

		glm::vec2 tb = rv - (normal * rv.x * normal.x + rv.y * normal.y);
		tb = glm::normalize(tb);

		// j tangent magnitude
		float jt = rv.x * tb.x + rv.y * tb.y;
		jt /= invMassSum;
		jt /= (float)contact_count;

		// Don't apply tiny friction impulses
		if (std::abs(jt - 0.0) <= EPSILON)
			return;

		// Coulumb's law
		glm::vec2 tangentImpulse;
		if (std::abs(jt) < j * sf)
			tangentImpulse = tb * jt;
		else
			tangentImpulse = tb * -j * df;

		// Apply friction impulse
		A->ApplyImpulse(-tangentImpulse, ra);
		B->ApplyImpulse(tangentImpulse, rb);
	}
}

void Manifold::PositionalCorrection(void)
{
	const float k_slop = 0.05f; // Penetration allowance
	const float percent = 0.4f; // Penetration percentage to correct
	glm::vec2 correction = (std::max(penetration - k_slop, 0.0f) / (A->im + B->im)) * normal * percent;
	A->position -= correction * A->im;
	B->position += correction * B->im;
}

void Manifold::InfiniteMassCorrection(void)
{
	A->velocity = glm::vec2(0, 0);
	B->velocity = glm::vec2(0, 0);
}
