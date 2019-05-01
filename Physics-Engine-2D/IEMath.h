//#pragma once

#ifndef IEMATH_H
#define IEMATH_H

#include <cmath> // abs, sqrt
#include <cassert> // assert
#include <algorithm> // max, min

#include "glm/glm.hpp"

const float PI = 3.141592741f;
const float EPSILON = 0.0001f;


//struct ToolBox {
//
//	ToolBox()
//	{
//	}
//	~ToolBox()
//	{
//	}
//
//	float LenSqr(glm::vec2 a)
//	{
//		return a.x * a.x + a.y * a.y;
//	}
//
//	float Dot(glm::vec2 a, glm::vec2 b)
//	{
//		return a.x * b.x + a.y * b.y;
//	}
//
//	float DistSqr(glm::vec2 a, glm::vec2 b)
//	{
//		glm::vec2 c = a - b;
//		return Dot(c, c);
//	}
//
//	glm::vec2 Cross(glm::vec2 v, float a)
//	{
//		return glm::vec2(a * v.y, -a * v.x);
//	}
//
//	glm::vec2 Cross(float a, glm::vec2 v)
//	{
//		return glm::vec2(-a * v.y, a * v.x);
//	}
//
//	float Cross(glm::vec2 a, glm::vec2 b)
//	{
//		return a.x * b.y - a.y * b.x;
//	}
//
//	// Comparison with tolerance of EPSILON
//	inline bool Equal(float a, float b)
//	{
//		// <= instead of < for NaN comparison safety
//		return std::abs(a - b) <= EPSILON;
//	}
//
//	inline float Sqr(float a)
//	{
//		return a * a;
//	}
//
//	inline float Clamp(float min, float max, float a)
//	{
//		if (a < min) return min;
//		if (a > max) return max;
//		return a;
//	}
//
//	inline int Round(float a)
//	{
//		return (int)(a + 0.5f);
//	}
//
//	inline float Random(float l, float h)
//	{
//		float a = (float)rand();
//		a /= RAND_MAX;
//		a = (h - l) * a + l;
//		return a;
//	}
//
//	inline bool BiasGreaterThan(float a, float b)
//	{
//		const float k_biasRelative = 0.95f;
//		const float k_biasAbsolute = 0.01f;
//		return a >= b * k_biasRelative + a * k_biasAbsolute;
//	}
//};
const float gravityScale = 5.0f;
const glm::vec2 gravity(0, 10.0f * gravityScale);
const float dt = 1.0f / 60.0f;

#endif