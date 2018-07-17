#include <iostream>
#include <ctime>
#include "Windows.h"
#include "RasterSurface.h"

#define RASTER_W 500
#define RASTER_H 500
#define NUM_PIXELS (RASTER_W * (RASTER_H))

unsigned int Raster[NUM_PIXELS];

struct Point
{
	unsigned int x, y;

};

Point pix[2000];

unsigned int Rand(unsigned int _min, unsigned int _max)
{
	unsigned int num = rand() % _max - _min;
	return num;
}

unsigned int C2DID(unsigned int x, unsigned int y, unsigned int width)
{
	return ((y * width) + x);
}

void Draw(unsigned int _color, unsigned int x, unsigned y)
{
	if (x >= RASTER_W || y >= RASTER_H) { return; }

	Raster[C2DID(x, y, RASTER_W)] = _color;
}

unsigned int Blend(float _ratio)
{
	int start, end, finish;

	finish = 0xFF000000;
	start = 0x00FF0000 >> 16;
	end = 0x00000000;

	finish |= (unsigned int)((end - start) * _ratio + start) << 16;

	start = 0x00000000;
	end = 0x0000FF00 >> 8;

	finish |= (unsigned int)((end - start) * _ratio + start) << 8;

	start = 0x000000FF;
	end = 0x000000FF;

	finish |= (unsigned int)((end - start) * _ratio + start);

	return finish;
}

void Parametric(float _1x, float _1y, float _2x, float _2y)
{
	unsigned int blend;

	float deltaX = abs(_2x - _1x);
	float deltaY = abs(_2y - _1y);

	float TP = max(deltaX, deltaY);

	for (unsigned int i = 0; i < TP; ++i)
	{
		float R = i / TP;
		blend = Blend(R);

		float x, y;
		x = (_2x - _1x) * R + _1x;
		y = (_2y - _1y) * R + _1y;

		Draw(blend, (unsigned int)x, (unsigned int)y);
	}
}

void Bresenham(unsigned int _startX, unsigned int _startY, unsigned int _endX, unsigned int _endY)
{
	unsigned int currY = _startY;
	unsigned int currX = _startX;

	int dirX = (_startX < _endX) ? 1 : -1;
	int dirY = (_startY < _endY) ? 1 : -1;

	dirX = (_startX == _endX) ? 0 : dirX;
	dirY = (_startY == _endY) ? 0 : dirY;

	float deltaX = (float)_endX - (float)_startX;
	float deltaY = (float)_endY - (float)_startY;

	float slopeY = deltaY / deltaX;
	float slopeX = deltaX / deltaY;

	slopeX = abs(slopeX);
	slopeY = abs(slopeY);

	float ErrorX = 0, ErrorY = 0;


	while (currX != _endX || currY != _endY)
	{
		Draw(0xFFFF0000, currX, currY);

		ErrorX += slopeX;
		ErrorY += slopeY;

		if (ErrorX > 0.5)
		{
			currX += dirX;
			ErrorX -= 1;
		}
		if (ErrorY > 0.5)
		{
			currY += dirY;
			ErrorY -= 1;
		}
	}
}

float ILE(float x, float y, float _endx, float _endy, float _mx, float _my)
{
	float sum = (float)((y - _endy) * _mx) + (float)((_endx - x) * _my) + (x * _endy) - y * _endx;
	return sum;
}

void MidPoint(unsigned int _startX, unsigned int _startY, unsigned int _endX, unsigned int _endY)
{
	float currY = (float)_startY;
	float currX = (float)_startX;

	float Midx = 0, Midy = 0;
	float deltax, deltay;

	deltax = (float)_endX - (float)_startX;
	deltay = (float)_endY - (float)_startY;

	bool run = true;
	while (run)
	{
		Draw(0xFF00ff00, (unsigned int)currX, (unsigned int)currY);

		if (abs(deltax) > abs(deltay))
		{
			if (deltay > 0)
			{
				if (deltax > 0)
				{
					Midx = currX + 1.0f;
					Midy = currY + 0.5f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) < 0)
					{
						currY++;
					}
					currX++;
				}
				else if (deltax < 0) {
					Midx = currX - 1.0f;
					Midy = currY + 0.5f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) > 0)
					{
						currY++;
					}
					currX--;
				}

				if (_startX > _endX)
				{
					if (abs(currX) <= abs((float)_endX))
					{
						run = false;
					}
				}
				else
				{
					if (abs(currX) >= abs((float)_endX))
					{
						run = false;
					}
				}
			}
			else if (deltay < 0)
			{
				if (deltax > 0)
				{
					Midx = currX + 1.0f;
					Midy = currY - 0.5f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) > 0)
					{
						currY--;
					}
					currX++;
				}
				else if (deltax < 0) {
					Midx = currX - 1.0f;
					Midy = currY - 0.5f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) < 0)
					{
						currY--;

					}
					currX--;
				}

				if (_startX > _endX)
				{
					if (abs(currX) <= abs((float)_endX))
					{
						run = false;
					}
				}
				else
				{
					if (abs(currX) >= abs((float)_endX))
					{
						run = false;
					}
				}
			}
			else if (deltay == 0)
			{
				if (deltax > 0)
				{
					Midx = currX + 1.0f;
					Midy = currY + 0.5f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) < 0)
					{
						currY++;
					}
					currX++;
				}
				else if (deltax < 0) {
					Midx = currX - 1.0f;
					Midy = currY + 0.5f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) > 0)
					{
						currY++;
					}
					currX--;
				}

				if (_startX > _endX)
				{
					if (abs(currX) <= abs((float)_endX))
					{
						run = false;
					}
				}
				else
				{
					if (abs(currX) >= abs((float)_endX))
					{
						run = false;
					}
				}
			}
		} else {
			if (deltay > 0)
			{
				if (deltax > 0)
				{
					Midx = currX + 0.5f;
					Midy = currY + 1.0f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) > 0)
					{
						currX++;
					}
					currY++;
				}
				else if (deltax < 0) {
					Midx = currX - 0.5f;
					Midy = currY + 1.0f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) < 0)
					{
						currX--;

					}
					currY++;
				}

				if (deltax == 0)
				{
					currY++;
				}

				if (_startY > _endY)
				{
					if (abs(currY) <= abs((float)_endY))
					{
						run = false;
					}
				}
				else 
				{
					if (abs(currY) >= abs((float)_endY))
					{
						run = false;
					}
				}
			}
			else if (deltay < 0)
			{
				if (deltax > 0)
				{
					Midx = currX + 0.5f;
					Midy = currY - 1.0f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) < 0)
					{
						currX++;
					}
					currY--;
				}
				else if (deltax < 0) {
					Midx = currX - 0.5f;
					Midy = currY - 1.0f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) > 0)
					{
						currX--;

					}
					currY--;
				}

				if (deltax == 0)
				{
					currY--;
				}

				if (_startY > _endY)
				{
					if (abs(currY) <= abs((float)_endY))
					{
						run = false;
					}
				}
				else 
				{
					if (abs(currY) >= abs((float)_endY))
					{
						run = false;
					}
				}
			}
			else if (deltax == 0)
			{
				if (deltax > 0)
				{
					Midx = currX + 0.5f;
					Midy = currY + 1.0f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) > 0)
					{
						currX++;
					}
					currY++;
				}
				else if (deltax < 0) {
					Midx = currX - 0.5f;
					Midy = currY + 1.0f;

					if (ILE((float)_startX, (float)_startY, (float)_endX, (float)_endY, Midx, Midy) < 0)
					{
						currX--;

					}
					currY++;
				}

				if (deltax == 0)
				{
					currY++;
				}

				if (_startY > _endY)
				{
					if (abs(currY) <= abs((float)_endY))
					{
						run = false;
					}
				}
				else
				{
					if (abs(currY) >= abs((float)_endY))
					{
						run = false;
					}
				}
			}
		}
	}
}

int main()
{
	srand((unsigned int)(time(NULL)));
	RS_Initialize(RASTER_W, RASTER_H);

	for (int i = 0; i < 2000; i++) {
		pix[i].x = Rand(0, RASTER_W);
		pix[i].y = Rand(0, RASTER_H);
	}

	unsigned int x;
	for (x = 0; x < 2000; x++)
	{
		Draw(0xFFFFFFFF, pix[x].x, pix[x].y);
	}

	unsigned int x1, x2, y1, y2;
	unsigned int _bres_x1, _bres_x2, _bres_y1, _bres_y2;
	unsigned int _Mid_x1, _Mid_x2, _Mid_y1, _Mid_y2;

	do
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 0x1)
		{
			memset(Raster, 0, sizeof(Raster));
			x1 = rand() % RASTER_W;
			x2 = rand() % RASTER_W;
			y1 = rand() % RASTER_H;
			y2 = rand() % RASTER_H;

			Parametric((float)x1, (float)y1, (float)x2, (float)y2);

			Draw(0xFFFFFF00, x1, y1);
			Draw(0xFFFFFF00, x2, y2);
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 0x1)
		{
			memset(Raster, 0, sizeof(Raster));

			_bres_x1 = rand() % RASTER_W;
			_bres_x2 = rand() % RASTER_W;
			_bres_y1 = rand() % RASTER_H;
			_bres_y2 = rand() % RASTER_H;

			Bresenham(_bres_x1, _bres_y1, _bres_x2, _bres_y2);

			Draw(0xFFFFFF00, _bres_x1, _bres_y1);
			Draw(0xFFFFFF00, _bres_x2, _bres_y2);
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 0x1)
		{
			memset(Raster, 0, sizeof(Raster));

			_Mid_x1 = rand() % RASTER_W;
			_Mid_x2 = rand() % RASTER_W;
			_Mid_y1 = rand() % RASTER_H;
			_Mid_y2 = rand() % RASTER_H;

			MidPoint(_Mid_x1, _Mid_y1, _Mid_x2, _Mid_y2);

			Draw(0xFFFF0000, _Mid_x1, _Mid_y1);
			Draw(0xFFFFFF00, _Mid_x2, _Mid_y2);
		}

		Parametric(0, 120, 499, 420);
		MidPoint(0, 110, 499, 410);
		Bresenham(0, 100, 499, 400);

		if (GetAsyncKeyState(VK_ESCAPE))
			break;

	} while (RS_Update(Raster, NUM_PIXELS));

	RS_Shutdown();

	return 0;
}