#include "PreCompiled.h"

	float Dot(glm::vec2 a, glm::vec2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline bool BiasGreaterThan(float a, float b)
	{
		const float k_biasRelative = 0.95f;
		const float k_biasAbsolute = 0.01f;
		return a >= b * k_biasRelative + a * k_biasAbsolute;
	}

	float DistSqr(glm::vec2 a, glm::vec2 b)
	{
		glm::vec2 c = a - b;
		return Dot(c, c);

	}

	float LenSqr(glm::vec2 a)
	{
		return a.x * a.x + a.y * a.y;
	}

	glm::vec2 MatrixMultiply(glm::mat2 a, glm::vec2 b) 
	{
		return glm::vec2(a[0][0] * b.x + a[0][1] * b.y, a[1][0] * b.x + a[1][1] * b.y);
	}


	//glm::mat2 transP(glm::mat2 m) 
	//{
	//	return glm::mat2(m[0][0], m[1][0], m[0][1], m[1][1]);
	//}

CollisionCallback Dispatch[Shape::eCount][Shape::eCount] =
{
	{
		CircletoCircle, CircletoPolygon
	},
  {
	  PolygontoCircle, PolygontoPolygon
  },
};

void CircletoCircle(Manifold *m, GameObject *a, GameObject *b)
{
	Circle *A = reinterpret_cast<Circle *>(a->shape);
	Circle *B = reinterpret_cast<Circle *>(b->shape);

	// Calculate translational vector, which is normal
	glm::vec2 normal = b->position - a->position;

	float dist_sqr = LenSqr(normal);
	float radius = A->radius + B->radius;

	// Not in contact
	if (dist_sqr >= radius * radius)
	{
		m->contact_count = 0;
		return;
	}

	float distance = std::sqrt(dist_sqr);

	m->contact_count = 1;

	if (distance == 0.0f)
	{
		m->penetration = A->radius;
		m->normal = glm::vec2(1, 0);
		m->contacts[0] = a->position;
	}
	else
	{
		m->penetration = radius - distance;
		m->normal = normal / distance; // Faster than using Normalized since we already performed sqrt
		m->contacts[0] = m->normal * A->radius + a->position;
	}
}

void CircletoPolygon(Manifold *m, GameObject *a, GameObject *b)
{
	Circle *A = reinterpret_cast<Circle *>      (a->shape);
	PolygonShape *B = reinterpret_cast<PolygonShape *>(b->shape);

	m->contact_count = 0;

	// Transform circle center to Polygon model space
	glm::vec2 center = a->position;

	center = MatrixMultiply(glm::transpose(B->u) , (center - b->position));

	// Find edge with minimum penetration
	// Exact concept as using support points in Polygon vs Polygon
	float separation = -FLT_MAX;
	int faceNormal = 0;
	for (int i = 0; i < B->m_vertexCount; ++i)
	{
		float s = Dot(B->m_normals[i], center - B->m_vertices[i]);

		if (s > A->radius)
			return;

		if (s > separation)
		{
			separation = s;
			faceNormal = i;
		}
	}

	// Grab face's vertices
	glm::vec2 v1 = B->m_vertices[faceNormal];
	int i2 = faceNormal + 1 < B->m_vertexCount ? faceNormal + 1 : 0;
	glm::vec2 v2 = B->m_vertices[i2];

	// Check to see if center is within polygon
	if (separation < EPSILON)
	{
		m->contact_count = 1;
		m->normal = -MatrixMultiply(B->u , B->m_normals[faceNormal]);
		m->contacts[0] = m->normal * A->radius + a->position;
		m->penetration = A->radius;
		return;
	}

	// Determine which voronoi region of the edge center of circle lies within
	float dot1 = Dot(center - v1, v2 - v1);
	float dot2 = Dot(center - v2, v1 - v2);
	m->penetration = A->radius - separation;

	// Closest to v1
	if (dot1 <= 0.0f)
	{
		if (DistSqr(center, v1) > A->radius * A->radius)
			return;

		m->contact_count = 1;
		glm::vec2 n = v1 - center;
		n = MatrixMultiply(B->u , n);
		n = glm::normalize(n);
		m->normal = n;
		v1 = MatrixMultiply(B->u, v1) + b->position;
		m->contacts[0] = v1;
	}

	// Closest to v2
	else if (dot2 <= 0.0f)
	{
		if (DistSqr(center, v2) > A->radius * A->radius)
			return;

		m->contact_count = 1;
		glm::vec2 n = v2 - center;
		v2 = MatrixMultiply(B->u, v2) + b->position;
		m->contacts[0] = v2;
		n = MatrixMultiply(B->u, n);
		n = glm::normalize(n);
		m->normal = n;
	}

	// Closest to face
	else
	{
		glm::vec2 n = B->m_normals[faceNormal];
		if (Dot(center - v1, n) > A->radius)
			return;
		n = MatrixMultiply(B->u, n);
		m->normal = -n;
		m->contacts[0] = m->normal * A->radius + a->position;
		m->contact_count = 1;
	}
}

void PolygontoCircle(Manifold *m, GameObject *a, GameObject *b)
{
	CircletoPolygon(m, b, a);
	m->normal = -m->normal;
}

float FindAxisLeastPenetration(int *faceIndex, PolygonShape *A, PolygonShape *B)
{
	float bestDistance = -FLT_MAX;
	int bestIndex;

	for (int i = 0; i < A->m_vertexCount; ++i)
	{
		// Retrieve a face normal from A
		glm::vec2 n = A->m_normals[i];
		glm::vec2 nw = MatrixMultiply(A->u, n);

		glm::mat2 buT = glm::transpose(B->u);

		n = MatrixMultiply(buT, nw);
		//^ uses multiply operator - potentially the problem
		// Retrieve support point from B along -n
		glm::vec2 s = B->GetSupport(-n);

		// Retrieve vertex on face from A, transform into
		// B's model space
		glm::vec2 v = A->m_vertices[i];
		v = MatrixMultiply(A->u, v);
		v += A->gameobject->position;

		v -= B->gameobject->position;
		v = MatrixMultiply(buT, v);

		

		// Compute penetration distance (in B's model space)
		float d = Dot(n, s - v);

		// Store greatest distance
		if (d > bestDistance)
		{
			bestDistance = d;
			bestIndex = i;
		}
	}

	*faceIndex = bestIndex;
	return bestDistance;
}

void FindIncidentFace(glm::vec2 *v, PolygonShape *RefPoly, PolygonShape *IncPoly, int referenceIndex)
{
	glm::vec2 referenceNormal = RefPoly->m_normals[referenceIndex];

	// Calculate normal in incident's frame of reference
	referenceNormal = MatrixMultiply(RefPoly->u, referenceNormal);  
	//calculate 2D transpose
	glm::mat2 temp = glm::transpose(IncPoly->u);
	referenceNormal = MatrixMultiply(temp, referenceNormal);

																// Find most anti-normal face on incident polygon
	int incidentFace = 0;
	float minDot = FLT_MAX;
	for (int i = 0; i < IncPoly->m_vertexCount; ++i)
	{
		float dot = Dot(referenceNormal, IncPoly->m_normals[i]);
		if (dot < minDot)
		{
			minDot = dot;
			incidentFace = i;
		}
	}

	// Assign face vertices for incidentFace
	v[0] = MatrixMultiply(IncPoly->u, IncPoly->m_vertices[incidentFace]) + IncPoly->gameobject->position;
	incidentFace = incidentFace + 1 >= (int)IncPoly->m_vertexCount ? 0 : incidentFace + 1;
	v[1] = MatrixMultiply(IncPoly->u, IncPoly->m_vertices[incidentFace]) + IncPoly->gameobject->position;
}

int Clip(glm::vec2 n, float c, glm::vec2 *face)
{
	int sp = 0;
	glm::vec2 out[2] = {
		face[0],
		face[1]
	};

	// Retrieve distances from each endpoint to the line
	// d = ax + by - c
	float d1 = Dot(n, face[0]) - c;
	float d2 = Dot(n, face[1]) - c;

	// If negative (behind plane) clip
	if (d1 <= 0.0f) out[sp++] = face[0];
	if (d2 <= 0.0f) out[sp++] = face[1];

	// If the points are on different sides of the plane
	if (d1 * d2 < 0.0f) // less than to ignore -0.0f
	{
		// Push interesection point
		float alpha = d1 / (d1 - d2);
		out[sp] = face[0] + alpha * (face[1] - face[0]);
		++sp;
	}

	// Assign our new converted values
	face[0] = out[0];
	face[1] = out[1];

	assert(sp != 3);

	return sp;
}

void PolygontoPolygon(Manifold *m, GameObject *a, GameObject *b)
{
	PolygonShape *A = reinterpret_cast<PolygonShape *>(a->shape);
	PolygonShape *B = reinterpret_cast<PolygonShape *>(b->shape);
	m->contact_count = 0;

	// Check for a separating axis with A's face planes
	int faceA;
	float penetrationA = FindAxisLeastPenetration(&faceA, A, B);
	if (penetrationA >= 0.0f)
		return;

	// Check for a separating axis with B's face planes
	int faceB;
	float penetrationB = FindAxisLeastPenetration(&faceB, B, A);
	if (penetrationB >= 0.0f)
		return;

	int referenceIndex;
	bool flip; // Always point from a to b

	PolygonShape *RefPoly; // Reference
	PolygonShape *IncPoly; // Incident

						   // Determine which shape contains reference face
	if (BiasGreaterThan(penetrationA, penetrationB))
	{
		RefPoly = A;
		IncPoly = B;
		referenceIndex = faceA;
		flip = false;
	}

	else
	{
		RefPoly = B;
		IncPoly = A;
		referenceIndex = faceB;
		flip = true;
	}

	// World space incident face
	glm::vec2 incidentFace[2];
	FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);

	//        y
	//        ^  ->n       ^
	//      +---c ------posPlane--
	//  x < | i |\
		  //      +---+ c-----negPlane--
//             \       v
//              r
//
//  r : reference face
//  i : incident poly
//  c : clipped point
//  n : incident normal

// Setup reference face vertices
	glm::vec2 v1 = RefPoly->m_vertices[referenceIndex];
	referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
	glm::vec2 v2 = RefPoly->m_vertices[referenceIndex];

	// Transform vertices to world space
	v1 = MatrixMultiply(RefPoly->u , v1) + RefPoly->gameobject->position;
	v2 = MatrixMultiply(RefPoly->u, v2) + RefPoly->gameobject->position;

	// Calculate reference face side normal in world space
	glm::vec2 sidePlaneNormal = (v2 - v1);
	sidePlaneNormal = glm::normalize(sidePlaneNormal);

	// Orthogonalize
	glm::vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

	// ax + by = c
	// c is distance from origin
	float refC = Dot(refFaceNormal, v1);
	float negSide = -Dot(sidePlaneNormal, v1);
	float posSide = Dot(sidePlaneNormal, v2);

	// Clip incident face to reference face side planes
	if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
		return; // Due to floating point error, possible to not have required points

	if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
		return; // Due to floating point error, possible to not have required points

				// Flip
	m->normal = flip ? -refFaceNormal : refFaceNormal;

	// Keep points behind reference face
	int cp = 0; // clipped points behind reference face
	float separation = Dot(refFaceNormal, incidentFace[0]) - refC;
	if (separation <= 0.0f)
	{
		m->contacts[cp] = incidentFace[0];
		m->penetration = -separation;
		++cp;
	}
	else
		m->penetration = 0;

	separation = Dot(refFaceNormal, incidentFace[1]) - refC;
	if (separation <= 0.0f)
	{
		m->contacts[cp] = incidentFace[1];

		m->penetration += -separation;
		++cp;

		// Average penetration
		m->penetration /= (float)cp;
	}

	m->contact_count = cp;
}