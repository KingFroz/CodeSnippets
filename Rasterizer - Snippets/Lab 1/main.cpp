#include <iostream>
#include "RasterSurface.h"
#include "tiles_12.h"
#include <ctime>
#include "XTime.h"
#include "fire_02.h"

#define RASTER_H 500
#define RASTER_W 500
#define NUM_PIXELS ((RASTER_W) * (RASTER_H))

unsigned int Raster[NUM_PIXELS];

struct Tile
{
	Tile() {};
	unsigned int T, B, L, R;
	Tile(unsigned int _t, unsigned int _b, unsigned int _l, unsigned int _r) : T(_t), B(_b), L(_l), R(_r) {}
};

unsigned int C2DID(unsigned int x, unsigned int y, unsigned int width)
{
	return ((y * width) + x);
}

void Draw(unsigned int _color, unsigned int x, unsigned y)
{
	if (x >= RASTER_W || y >= RASTER_H) { return; }

	Raster[C2DID(x, y, RASTER_W)] = _color;
}

unsigned int Lerp(unsigned int A, unsigned int B, float R)
{
	return (unsigned int)(((int)B - (int)A) * R + A);
}

unsigned int ARGB(unsigned int _source, unsigned int _des)
{
	unsigned int A, R, G, B;
	unsigned int argb;

	A = _source & 0x000000FF;
	R = _source & 0x0000FF00;
	B = _source & 0xFF000000;
	G = _source & 0x00FF0000;

	A <<= 24;
	B >>= 24;
	R <<= 8;
	G >>= 8;

	argb = A | R | G | B;

	float ratio = 1 - (((float)(A >> 24)) / 255.0f);

	unsigned int desA = _des & 0xFF000000;
	unsigned int desR = _des & 0x00FF0000;
	unsigned int desG = _des & 0x0000FF00;
	unsigned int desB = _des & 0x000000FF;

	unsigned int finA = Lerp(A >> 24, desA >> 24, ratio);
	unsigned int finR = Lerp(R >> 16, desR >> 16, ratio);
	unsigned int finG = Lerp(G >> 8, desG >> 8, ratio);
	unsigned int finB = Lerp(B, desB, ratio);

	finA <<= 24;
	finR <<= 16;
	finG <<= 8;

	unsigned int fin = finA | finR | finG | finB;

	return fin;
}

void BLIT(const unsigned int *source, unsigned int source_w, Tile tile, unsigned int draw_x, unsigned int draw_y)
{
	unsigned int y, x;
	for (y = tile.T; y < tile.B; y++)
	{
		for (x = tile.L; x < tile.R; x++)
		{
			unsigned int color = source[C2DID(x, y, source_w)];
			unsigned int des = Raster[C2DID(x + draw_x - tile.L, y + draw_y - tile.T, RASTER_W)];
			Draw(ARGB(color, des), x + draw_x - tile.L, y + draw_y - tile.T);
		}
	}
}

unsigned int Rand(unsigned int _min, unsigned int _max)
{
	unsigned int num = rand() % _max - _min;
	return num;
}

int main(void)
{
	RS_Initialize(RASTER_W, RASTER_H);

	XTime xtime;
	unsigned int frame = 0;
	double cap = 1/30.0f;
	double counter = 0;

	srand((unsigned int)(time(NULL)));

	Tile backg(128, 159, 288, 319);
	Tile tree(15, 95, 316, 386);
	Tile Fire[64];

	for (unsigned int i = 0; i < 8; i++)
	{
		for (unsigned int j = 0; j < 8; j++)
		{
			Tile anim(i * 128, i * 128 + 128, j * 128, j * 128 + 128);
			Fire[i * 8 + j] = anim;
		}
	}

	unsigned int drawW[10] = {};
	unsigned int drawH[10] = {};

	for (unsigned int i = 0; i < 10; i++)
	{
		drawW[i] = Rand(0, RASTER_W);
		drawH[i] = Rand(0, RASTER_H);
	}

	do
	{
		xtime.Signal();

		counter += xtime.Delta();

		memset(Raster, 0, sizeof(Raster));

		for (int x = 0; x < RASTER_W; x += 31) {
			for (int y = 0; y < RASTER_H; y += 31) {
				BLIT(tiles_12_pixels, tiles_12_width, backg, x, y);
			}
		}

		for (int i = 0; i < 10; i++)
		{
			BLIT(tiles_12_pixels, tiles_12_width, tree, drawW[i], drawH[i]);
		}

		BLIT(fire_02_pixels, fire_02_width, Fire[frame], 250, 250);

		if (counter > cap)
		{
			counter = 0;
			frame = (frame + 1);

			if (frame >= 64)
				frame = 0;
			xtime.Restart();
		}
	} while (RS_Update(Raster, NUM_PIXELS));

	RS_Shutdown();
	return 0;
}