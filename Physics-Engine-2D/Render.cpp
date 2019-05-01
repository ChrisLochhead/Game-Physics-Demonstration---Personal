#include "PreCompiled.h"

void RenderString(int x, int y,  int s, float r, float g, float b)
{


}

void renderbitmap(float x, float y, void *font, char *string) {

	char *c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}