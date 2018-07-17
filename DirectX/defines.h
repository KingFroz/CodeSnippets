#define RASTER_W 500
#define RASTER_H 500
#define PI       3.14159265358979323846
#define numPixels (RASTER_W * (RASTER_H))

unsigned int Raster[numPixels];

float Depth[numPixels];

struct A_PIXEL
{
	unsigned int color = 0xFFFFFFFF;
};

struct MY_VERTEX
{
	float axis[4];
};

struct MY_MATRIX_3X3
{
	struct
	{
		float p11, p12, p13,
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

//unsigned int ARGB(unsigned int _color)
//{
//	unsigned int A, R, G, B;
//
//	unsigned int argb;
//	A = _color & 0x000000FF;
//	R = _color & 0x0000FF00;
//	G = _color & 0x00FF0000;
//	B = _color & 0xFF000000;
//
//	A <<= 24;
//	B >>= 24;
//	R >>= 8;
//	G >>= 16;
//
//	argb = A | R << 16 | G << 8 | B;
//
//	return argb;
//}

//int Lerp(int v0, int v1, float R)
//{
//	return (v1 - v0) * R + v0;
//}

//unsigned int ColorLerp(unsigned int v0, unsigned int v1, float _ratio)
//{
//	int prvA = (v0 & 0xFF000000) >> 24;
//	int prvR = (v0 & 0x00FF0000) >> 16;
//	int prvG = (v0 & 0x0000FF00) >> 8;
//	int prvB = (v0 & 0x000000FF);
//
//	int next_A = (v1 & 0xFF000000) >> 24;
//	int next_R = (v1 & 0x00FF0000) >> 16;
//	int next_G = (v1 & 0x0000FF00) >> 8;
//	int next_B = (v1 & 0x000000FF);
//
//	int finA = Lerp(prvA, next_A, _ratio) << 24;
//	int finG = Lerp(prvG, next_G, _ratio) << 8;
//	int finB = Lerp(prvB, next_B, _ratio);
//	int finR = Lerp(prvR, next_R, _ratio) << 16;
//
//	return finA | finR | finG | finB;
//}
//
//unsigned int C2DID(unsigned int x, unsigned int y, unsigned int width)
//{
//	return ((y * width) + x);
//}