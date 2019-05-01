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

#include "glut.h"
#include "IEMath.h"
#include "GameClock.h"
#include "Render.h"
#include "GameObject.h"
#include "Shape.h"
#include "CollisionDetection.h"
#include "Manifold.h"
#include "Scene.h"
#include "Player.h"

#endif // PRECOMPILED_H