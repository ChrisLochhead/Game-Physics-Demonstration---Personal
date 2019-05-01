#include "PreCompiled.h"
#include <iostream>

using namespace std;

#define ESC_KEY 27
#define SPACEBAR 32

Scene scene(1.0f / 60.0f, 10);
glm::vec2 Zero = { 0, 0 };
GameClock g_Clock;
bool frameStepping = false;
bool canStep = false;
float cannonAngle = 3.1415;
bool isShooting = false;
int power = 0;
int numShots = 5;

	float Random(float l, float h)
	{
		float a = (float)rand();
		a /= RAND_MAX;
		a = (h - l) * a + l;
		return a;
	}

void Mouse(int button, int state, int x, int y)
{
	x /= 10.0f;
	y /= 10.0f;


	if (state == GLUT_DOWN)
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
		{

			PolygonShape poly(false);
			int count = (int)Random(3, MaxPolyVertexCount);
			glm::vec2 *vertices = new glm::vec2[count];
			float e = Random(5, 10);
			for (int i = 0; i < count; ++i)
				vertices[i] = glm::vec2(Random(-e, e), Random(-e, e));
			poly.Set(vertices, count);
			GameObject *b = scene.Add(&poly, x, y, Random(0.1f, 0.4f), Random(0.1f, 0.4f), Random(0.1f, 0.4f), 0, 0);
			b->SetOrient(Random(-PI, PI));
			b->restitution = 0.2f;
			b->dynamicFriction = 0.2f;
			b->staticFriction = 0.4f;
			delete[] vertices;
		}
		break;
		case GLUT_RIGHT_BUTTON:
		{
			Circle c(Random(1.0f, 3.0f));
			GameObject *b = scene.Add(&c, x, y, Random(0.5f,1.0f), Random(0.5f, 1.0f), Random(0.5f, 1.0f), 0, 0);
		}
		break;
		}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ESC_KEY:
		exit(0);
		break;
	case 'a':
		cannonAngle -= 0.15;
		scene.moveCannon(cannonAngle);
		break;
	case 'd':
		cannonAngle += 0.15;
		scene.moveCannon(cannonAngle);
	break;
	case 'f':
		frameStepping = frameStepping ? false : true;
		break;
	case ' ':
		canStep = true;
		break;
	}
}

void keysUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case SPACEBAR:

		if (isShooting == false) {       // if reset function has been called or game has just been initialised
			isShooting = true;           // call function in update to create a new object
		}
		break;
	}
}

void PhysicsLoop(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	if (GetAsyncKeyState(VK_SPACE) & 0x80000000) // charging up the cannon
	{
		if (isShooting == false) {
			if (power < 1000)
				power = power+ 1;
			cout << "power: " << power << endl;
		}

	}

	if (isShooting == true)
	{

		Circle cannonBall(1.25f);
		cannonBall.SetOrient(cannonAngle);
		if (power > 200) {
			GameObject *c = scene.Add(&cannonBall, scene.GetPosition('x'), scene.GetPosition('y'), 0.5f, 0.5f, 0.5f, power / 5 * sin(-cannonAngle), power / 5 * cos(cannonAngle)); // first strength, second angle
		}
		else {
			GameObject *c = scene.Add(&cannonBall, scene.GetPosition('x'), scene.GetPosition('y'), 0.5f, 0.5f, 0.5f, 200 / 5 * sin(-cannonAngle), 200 / 5 * cos(cannonAngle));
		}
		numShots++;
		isShooting = false;
		power = 0;

	}

	char buf[100] = { 0 };
	sprintf_s(buf, "%s%i", "Power Level = ", power);
	renderbitmap(2, 3, GLUT_BITMAP_TIMES_ROMAN_24, buf);

	static double accumulator = 0;

	// Different time mechanisms for Linux and Windows
	#ifdef WIN32
	accumulator += g_Clock.Elapsed();
	#else
	accumulator += g_Clock.Elapsed() / static_cast<double>(std::chrono::duration_cast<clock_freq>(std::chrono::seconds(1)).count());
	#endif

	g_Clock.Start();

	if (accumulator > 0.1)
		accumulator = 0.1;
	else
	if (accumulator < 0.0)
		accumulator = 0.0;

	while (accumulator >= dt)
	{
		if (!frameStepping)
			scene.Step();
		else
		{
			if (canStep)
			{
				scene.Step();
				canStep = false;
			}
		}
		accumulator -= dt;
	}

	g_Clock.Stop();

	scene.Render();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1400, 600);
	glutCreateWindow("PhyEngine");
	glutDisplayFunc(PhysicsLoop);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(keysUp);
	glutMouseFunc(Mouse);
	glutIdleFunc(PhysicsLoop);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 140, 60, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	PolygonShape ground(false);
	ground.SetBox(140.0f, 1.0f);
	GameObject *o = scene.Add(&ground, 0, 60, 0.3f, 1.0f, 0.2f, 0, 0);
	o->SetStatic();
	o->SetOrient(0);

	PolygonShape LeftWall(false);
	LeftWall.SetBox(60.0f, 1.0f);
	o = scene.Add(&LeftWall, 0, 60, 0.5f, 0.5f, 0.5f, 0, 0);
	o->SetStatic();
	o->SetOrient(1.571);

	PolygonShape RightWall(false);
	RightWall.SetBox(60.0f, 1.0f);
	o = scene.Add(&RightWall, 140, 60, 0.5f, 0.5f, 0.5f, 0, 0);
	o->SetStatic();
	o->SetOrient(1.571);

	PolygonShape player(false);
	player.SetCannon();
	o = scene.Add(&player, 10, 57, 0.5f, 0.5f, 0.5f, 0, 0);
	o->SetStatic();
	o->SetOrient(3.1415);

	PolygonShape cannonBarrel(true);
	cannonBarrel.SetCannonBarrel();
	o = scene.Add(&cannonBarrel, 10, 55, 0.5f, 0.5f, 0.5f, 0, 0);
	o->SetStatic();
	o->SetOrient(cannonAngle);

	PolygonShape Obstacle1(true);
	Obstacle1.SetCannonBarrel();
	o = scene.Add(&Obstacle1, 92.5f, 55, 0.5f, 0.5f, 0.5f, 0, 0);
	o->SetOrient(0);

	PolygonShape Obstacle2(true);
	Obstacle1.SetCannonBarrel();
	o = scene.Add(&Obstacle1, 98.5f, 55, 0.5f, 0.5f, 0.5f, 0, 0);
	o->SetOrient(0);

	PolygonShape Obstacle3(true);
	Obstacle1.SetCannonBarrel();
	o = scene.Add(&Obstacle1, 95, 35, 0.5f, 0.5f, 0.5f, 0, 0);
	o->SetOrient(3.14/2);


	srand(1);
	glutMainLoop();

	return 0;
}
