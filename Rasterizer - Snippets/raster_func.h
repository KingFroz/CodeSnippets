#include "shaders.h"

unsigned int C2DID(unsigned int x, unsigned int y, unsigned int width)
{
	return ((y * width) + x);
}

float Lerp(float v0, float v1, float R)
{
	return (v1 - v0) * R + v0;
}

unsigned int Lerp(unsigned int A, unsigned int B, float R)
{
	return (unsigned int)(((int)B - (int)A) * R + A);
}

void Draw(unsigned int _color, unsigned int x, unsigned y)
{
	if (x >= RASTER_W || y >= RASTER_H) { return; }

	Raster[C2DID(x, y, RASTER_W)] = _color;
}

SCREEN_XY CartesianToScreen(MY_VERTEX _vert)
{
	SCREEN_XY screen;

	screen.x = (_vert.axis[0] + 1) * (RASTER_W >> 1);
	screen.y = (1 - _vert.axis[1]) * (RASTER_H >> 1);

	return screen;
}

void ParametricLine(SCREEN_XY P1, SCREEN_XY P2, unsigned int color)
{
	float deltaX = abs(P2.x - P1.x);
	float deltaY = abs(P2.y - P1.y);

	float TP = max(deltaX, deltaY);

	A_PIXEL currColor;
	currColor.color = color;
	for (unsigned int i = 0; i < TP; ++i)
	{
		float R = i / TP;

		float x, y;
		x = (P2.x - P1.x) * R + P1.x;
		y = (P2.y - P1.y) * R + P1.y;

		if (PixelShader)
			PixelShader(currColor); // Modify copy. 

		Draw(currColor.color, (unsigned int)x, (unsigned int)y);
	}
}

// Draws a line using one of the line equations.
void DrawLine(const MY_VERTEX &start, const MY_VERTEX &end)
{
	// Copy input data and send through shaders
	MY_VERTEX copy_start = start;
	MY_VERTEX copy_end = end;

	// Use vertex shader to modify incoming copies only.
	if (VertexShader)
	{
		VertexShader(copy_start);
		VertexShader(copy_end);
	}
	// original plotting variables adapted to use new cartesian data
	SCREEN_XY screen_start = CartesianToScreen(copy_start);
	SCREEN_XY screen_end = CartesianToScreen(copy_end);
	// Standard line drawing code follows using integer coordinates...
	
	ParametricLine(screen_start, screen_end, start.color);
}