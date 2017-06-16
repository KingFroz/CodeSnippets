#include "math.h"

// The active vertex shader. Modifies an incoming vertex. Pre-Rasterization. 
void(*VertexShader)(MY_VERTEX&) = 0;
// The active pixel shader. Modifies an outgoing pixel. Post-Rasterization.
void(*PixelShader)(A_PIXEL&) = 0;

// All Shader Variables (Always Pre-fixed by “SV_”)
MY_MATRIX_4X4 SV_WorldMatrix;
MY_MATRIX_4X4 View_Matrix;
MY_MATRIX_4X4 Proj_Matrix;

// Various custom vertex and pixel shaders, (Pre-fixed by “VS_” & “PS_”)
// Can be swapped using above function pointers as needed for flexibility. 

// Applys the current world matrix to all
void VS_World(MY_VERTEX &multiplyMe)
{
	multiplyMe = MultiplyVertexByMatrix(multiplyMe, SV_WorldMatrix);
	multiplyMe = MultiplyVertexByMatrix(multiplyMe, View_Matrix);
	multiplyMe = MultiplyVertexByMatrix(multiplyMe, Proj_Matrix);

	unsigned int i = 0;
	for (; i < 3; i++)
	{
		multiplyMe.axis[i] = multiplyMe.axis[i] / multiplyMe.axis[3];
	}
}

void VS_Grid(MY_VERTEX &multiplyMe)
{
	multiplyMe = MultiplyVertexByMatrix(multiplyMe, SV_WorldMatrix);
	multiplyMe = MultiplyVertexByMatrix(multiplyMe, View_Matrix);
	multiplyMe = MultiplyVertexByMatrix(multiplyMe, Proj_Matrix);

	unsigned int i = 0;
	for (; i < 3; i++)
	{
		multiplyMe.axis[i] = multiplyMe.axis[i] / multiplyMe.axis[3];
	}
}
// Basic pixel shader returns the color white
void PS_White(A_PIXEL &makeWhite)
{
	makeWhite.color = 0xFFFFFFFF;
}

void PS_Green(A_PIXEL &makeWhite)
{
	makeWhite.color = 0xFF00FF00;
}