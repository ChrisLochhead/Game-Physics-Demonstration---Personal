#ifndef SHAPE_H
#define SHAPE_H

//#include "GameObject.h"
//#include "IEMath.h"
//#include "glut.h"

#define MaxPolyVertexCount 64


struct Mat2
{
	union
	{
		struct
		{
			float m00, m01;
			float m10, m11;
		};

		float m[2][2];
		float v[4];
	};

	Mat2() {}
	//Mat2(float radians)
	//{
	//	float c = std::cos(radians);
	//	float s = std::sin(radians);

	//	m00 = c; m01 = -s;
	//	m10 = s; m11 = c;
	//}

	Mat2(float a, float b, float c, float d)
		: m00(a), m01(b)
		, m10(c), m11(d)
	{
	}


	//Mat2 Transpose(void) const
	//{
	//	return Mat2(m00, m10, m01, m11);
	//}

	//const glm::vec2 operator*(const glm::vec2& rhs) const
	//{
	//	return glm::vec2(m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y);
	//}

};

struct Shape
{
	enum Type
	{
		eCircle,
		ePoly,
		eCount
	};

	Shape() {}
	virtual Shape *Clone(void) const = 0;
	virtual void Initialize(void) = 0;
	virtual void ComputeMass(float density) = 0;
	virtual void SetOrient(float radians) = 0;
	virtual void Draw(void) const = 0;
	virtual Type GetType(void) const = 0;

	GameObject *gameobject;

	// For circle shape
	float radius;

	// For Polygon shape
	//glm::mat2 u;
	glm::mat2 u; // Orientation matrix from model to world
};

struct Circle : public Shape
{
	Circle(float r)
	{
		radius = r;
	}
	Shape *Clone(void) const
	{
		return new Circle(radius);
	}

	void Initialize(void)
	{
		ComputeMass(1.0f);
	}

	void ComputeMass(float density)
	{
		gameobject->m = 3.1415 * radius * radius * density;
		gameobject->im = (gameobject->m) ? 1.0f / gameobject->m : 0.0f;
		gameobject->I = gameobject->m * radius * radius;
		gameobject->iI = (gameobject->I) ? 1.0f / gameobject->I : 0.0f;
	}

	void SetOrient(float radians)
	{
	}

	void Draw(void) const
	{
		const int k_segments = 20;

		// Render a circle with a bunch of lines
		glColor3f(gameobject->r, gameobject->g, gameobject->b);
		glBegin(GL_LINE_LOOP);
		float theta = gameobject->orient;
		float inc = 3.1415 * 2.0f / (float)k_segments;
		for (int i = 0; i < k_segments; ++i)
		{
			theta += inc;
			glm::vec2 p(std::cos(theta), std::sin(theta));
			p *= radius;
			p += gameobject->position;
			glVertex2f(p.x, p.y);
		}
		glEnd();

		// Render line within circle so orientation is visible
		glBegin(GL_LINE_STRIP);
		glm::vec2 r(0, 1.0f);
		float c = std::cos(gameobject->orient);
		float s = std::sin(gameobject->orient);
		r = glm::vec2(r.x * c - r.y * s, r.x * s + r.y * c);
		r *= radius;
		r = r + gameobject->position;
		glVertex2f(gameobject->position.x, gameobject->position.y);
		glVertex2f(r.x, r.y);
		glEnd();
	}

	Type GetType(void) const
	{
		return eCircle;
	}
};

struct PolygonShape : public Shape
{

	PolygonShape(bool iscannon)
	{
		isCannon = iscannon;
	}
	void Initialize(void)
	{
		ComputeMass(1.0f);
	}

	Shape *Clone(void) const
	{
		PolygonShape *poly = new PolygonShape(isCannon);
		poly->u = u;
		for (int i = 0; i < m_vertexCount; ++i)
		{
			poly->m_vertices[i] = m_vertices[i];
			poly->m_normals[i] = m_normals[i];
		}
		poly->m_vertexCount = m_vertexCount;
		return poly;
	}

	void ComputeMass(float density)
	{
		// Calculate centroid and moment of interia
		glm::vec2 c(0.0f, 0.0f); // centroid
		float area = 0.0f;
		float I = 0.0f;
		const float k_inv3 = 1.0f / 3.0f;

		for (int i1 = 0; i1 < m_vertexCount; ++i1)
		{
			// Triangle vertices, third vertex implied as (0, 0)
			glm::vec2 p1(m_vertices[i1]);
			int i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
			glm::vec2 p2(m_vertices[i2]);
			float D = p1.x * p2.y - p1.y * p2.x;
			float triangleArea = 0.5f * D;

			area += triangleArea;

			// Use area to weight the centroid average, not just vertex position
			c += triangleArea * k_inv3 * (p1 + p2);

			float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
			float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
			I += (0.25f * k_inv3 * D) * (intx2 + inty2);
		}

		c *= 1.0f / area;

		// Translate vertices to centroid (make the centroid (0, 0)
		// for the polygon in model space)
		// Not floatly necessary, but I like doing this anyway
		if (isCannon == false) {
			for (int i = 0; i < m_vertexCount; ++i)
				m_vertices[i] -= c;
		}

		gameobject->m = density * area;
		gameobject->im = (gameobject->m) ? 1.0f / gameobject->m : 0.0f;
		gameobject->I = I * density;
		gameobject->iI = gameobject->I ? 1.0f / gameobject->I : 0.0f;
	}

	void SetOrient(float radians)
	{
	float c = std::cos(radians);
	float s = std::sin(radians);
	
		u[0][0] = c; u[0][1] = -s;
		u[1][0]= s; u[1][1]= c;
		
	//u.m00 = c; u.m01 = -s;
	//u.m10 = s; u.m11 = c;

	}
	glm::vec2 MatrixMult(glm::mat2 a, glm::vec2 b)
	{
		return glm::vec2(a[0][0] * b.x + a[0][1] * b.y, a[1][0] * b.x + a[1][1] * b.y);
	}

	glm::vec2 MatrixMult(Mat2 a, glm::vec2 b)
	{
		return glm::vec2(a.m00 * b.x + a.m01 * b.y, a.m10 * b.x + a.m11 * b.y);
	}
	void Draw(void) const
	{
		glColor3f(gameobject->r, gameobject->g, gameobject->b);
		glBegin(GL_POLYGON);
		for (int i = 0; i < m_vertexCount; ++i)
		{
			glm::vec2 v = gameobject->position + glm::vec2(u[0][0] * m_vertices[i].x + u[0][1] * m_vertices[i].y, u[1][0] * m_vertices[i].x + u[1][1] * m_vertices[i].y);
			//glm::vec2 v = gameobject->position + glm::vec2(u.m00 * m_vertices[i].x + u.m01 * m_vertices[i].y, u.m10 * m_vertices[i].x + u.m11 * m_vertices[i].y);
			glVertex2f(v.x, v.y);
		}
		glEnd();
	}

	Type GetType(void) const
	{
		return ePoly;
	}

	// Half width and half height
	void SetBox(float hw, float hh)
	{
		m_vertexCount = 4;
		m_vertices[0] = glm::vec2(-hw, -hh);
		m_vertices[1] = glm::vec2(hw, -hh);
		m_vertices[2] = glm::vec2(hw, hh);
		m_vertices[3] = glm::vec2(-hw, hh);
		m_normals[0] = glm::vec2(0.0f, -1.0f);
		m_normals[1] = glm::vec2(1.0f, 0.0f);
		m_normals[2] = glm::vec2(0.0f, 1.0f);
		m_normals[3] = glm::vec2(-1.0f, 0.0f);
	}

	void SetCannon()
	{
		m_vertexCount = 4;
		m_vertices[0] = glm::vec2(3.0, 3.0f);
		m_vertices[1] = glm::vec2(0.0, 3.0f);
		m_vertices[2] = glm::vec2(-1.5f, -1.5f);
		m_vertices[3] = glm::vec2(4.5f, -1.5f);
		m_normals[0] = glm::vec2(0.0f, -1.0f);
		m_normals[1] = glm::vec2(1.0f, 0.0f);
		m_normals[2] = glm::vec2(0.0f, 1.0f);
		m_normals[3] = glm::vec2(-1.0f, 0.0f);

	}

	void SetCannonBarrel()
	{
		m_vertexCount = 4;
		m_vertices[0] = glm::vec2(-1.0f, -5.0f);
		m_vertices[1] = glm::vec2(1.0f, -5.0f);
		m_vertices[2] = glm::vec2(1.0f, 5.0f);
		m_vertices[3] = glm::vec2(-1.0f, 5.0f);
		m_normals[0] = glm::vec2(0.0f, -1.0f);
		m_normals[1] = glm::vec2(1.0f, 0.0f);
		m_normals[2] = glm::vec2(0.0f, 1.0f);
		m_normals[3] = glm::vec2(-1.0f, 0.0f);
	}

	void Set(glm::vec2 *vertices, int count)
	{
		// Find the right most point on the hull
		int rightMost = 0;
		float highestXCoord = vertices[0].x;
		for (int i = 1; i < count; ++i)
		{
			float x = vertices[i].x;
			if (x > highestXCoord)
			{
				highestXCoord = x;
				rightMost = i;
			}

			// If matching x then take farthest negative y
			else if (x == highestXCoord)
				if (vertices[i].y < vertices[rightMost].y)
					rightMost = i;
		}

		int hull[MaxPolyVertexCount];
		int outCount = 0;
		int indexHull = rightMost;

		for (;;)
		{
			hull[outCount] = indexHull;

			// Search for next index that wraps around the hull
			// by computing cross products to find the most counter-clockwise
			// vertex in the set, given the previos hull index
			int nextHullIndex = 0;
			for (int i = 1; i < (int)count; ++i)
			{
				// Skip if same coordinate as we need three unique
				// points in the set to perform a cross product
				if (nextHullIndex == indexHull)
				{
					nextHullIndex = i;
					continue;
				}

				// Cross every set of three unique vertices
				// Record each counter clockwise third vertex and add
				// to the output hull
				// See : http://www.oocities.org/pcgpe/math2d.html
				glm::vec2 e1 = vertices[nextHullIndex] - vertices[hull[outCount]];
				glm::vec2 e2 = vertices[i] - vertices[hull[outCount]];
				float c = e1.x * e2.y - e1.y * e2.x;
				if (c < 0.0f)
					nextHullIndex = i;

				// Cross product is zero then e vectors are on same line
				// therefor want to record vertex farthest along that line
				if (c == 0.0f && e2.x * e2.x + e2.y * e2.y > e1.x * e1.x + e1.y * e1.y)
					nextHullIndex = i;

			}

			++outCount;
			indexHull = nextHullIndex;

			// Conclude algorithm upon wrap-around
			if (nextHullIndex == rightMost)
			{
				m_vertexCount = outCount;
				break;
			}
		}

		// Copy vertices into shape's vertices
		for (int i = 0; i < m_vertexCount; ++i)
			m_vertices[i] = vertices[hull[i]];

		// Compute face normals
		for (int i1 = 0; i1 < m_vertexCount; ++i1)
		{
			int i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
			glm::vec2 face = m_vertices[i2] - m_vertices[i1];

			// Ensure no zero-length edges, because that's bad
			//assert(face.LenSqr() > EPSILON * EPSILON);

			// Calculate normal with 2D cross product between vector and scalar
			m_normals[i1] = glm::vec2(face.y, -face.x);
			glm::normalize(m_normals[i1]);
			//normalize(m_normals[i1]);
		}
	}

	// The extreme point along a direction within a polygon
	glm::vec2 GetSupport(const glm::vec2& dir)
	{
		float bestProjection = -FLT_MAX;
		glm::vec2 bestVertex;

		for (int i = 0; i < m_vertexCount; ++i)
		{
			glm::vec2 v = m_vertices[i];
			float projection = v.x * dir.x + v.y * dir.y;

			if (projection > bestProjection)
			{
				bestVertex = v;
				bestProjection = projection;
			}
		}

		return bestVertex;
	}

	bool isCannon;
	int m_vertexCount;
	glm::vec2 m_vertices[MaxPolyVertexCount];
	glm::vec2 m_normals[MaxPolyVertexCount];

};

#endif // SHAPE_H