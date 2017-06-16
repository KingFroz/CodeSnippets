#include <iostream>
#include "Windows.h"
#include "RasterSurface.h"
#include "Xtime.h"

#define RASTER_W 500
#define RASTER_H 500
#define PI 3.14159265359f
#define numPixels (RASTER_W * (RASTER_H))

unsigned int Raster[numPixels];

struct SCREEN_XY
{
	float x, y;
};

struct A_PIXEL
{
	unsigned int color = 0xFFFFFFFF;
};

struct MY_VERTEX
{
	float axis[4];

	unsigned int color;
};

struct MY_MATRIX_3X3
{
	struct
	{
		float	p11, p12, p13,
				p21, p22, p23,
				p31, p32, p33;
	};
};

struct MY_MATRIX_4X4
{
	union
	{
		float m[4][4];

		struct
		{
			float p11, p12, p13, p14,
				p21, p22, p23, p24,
				p31, p32, p33, p34,
				p41, p42, p43, p44;
		};
	};
};

struct VEC
{
	float x, y, z, w;
};