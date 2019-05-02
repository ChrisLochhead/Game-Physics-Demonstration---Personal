//#pragma once
#ifndef PRECOMPILED_H
#define PRECOMPILED_H

//If the compiler defines one of these items we can assume that it is windows running
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
//so we will standardize on WIN32 for windows specific code
#ifndef WIN32
#define WIN32
#endif
#endif

#ifdef WIN32
#include <Windows.h>
#endif

#undef min
#undef max

#include <cstring> // strlen, memcpy, etc.
#include <cstdlib> // exit
#include <cfloat>  // FLT_MAX
#include <vector>

#include "glm/glm.hpp"

const float PI = 3.141592741f;
const float EPSILON = 0.0001f;
const float gravityScale = 5.0f;
const glm::vec2 gravity(0, 10.0f * gravityScale);
const float dt = 1.0f / 60.0f;

#include "glut.h"
#include "GameClock.h"
#include "Render.h"
#include "GameObject.h"
#include "Shape.h"
#include "CollisionDetection.h"
#include "Manifold.h"
#include "Scene.h"

#endif // PRECOMPILED_H