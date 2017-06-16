#include "raster_func.h"
#include "Windows.h"
#include <Random>

VEC stars[3000];
MY_VERTEX stoneHenge[1457];
// Main program loop.
void main(void)
{
	RS_Initialize(RASTER_W, RASTER_H);
	srand((unsigned int)(time(NULL)));

	float timer = 0;
	float rotate = 0;
	XTime time;
	time.Restart();

	// Initialize constant drawing data. (BE SURE IT IS CONSTANT!!!)

	for (int i = 0; i < 3000; i++) {
		stars[i].x = static_cast <float>(rand() / static_cast<float>(RAND_MAX)) * 0.5f;
		stars[i].y = static_cast <float>(rand() / static_cast<float>(RAND_MAX)) * 0.5f;
		stars[i].z = static_cast <float>(rand() / static_cast<float>(RAND_MAX)) * 0.5f;
		stars[i].w = 1;
	}

	for (int i = 0; i < 1457; i++) {
		stoneHenge[i].axis[0] = StoneHenge_data[i].pos[0] * 0.1f;
		stoneHenge[i].axis[1] = StoneHenge_data[i].pos[1] * 0.1f;
		stoneHenge[i].axis[2] = StoneHenge_data[i].pos[2] * 0.1f;
		stoneHenge[i].axis[3] = 1.0f;

		stoneHenge[i].UVs[0] = StoneHenge_data[i].uvw[0];
		stoneHenge[i].UVs[0] = StoneHenge_data[i].uvw[1];

		stoneHenge[i].norms[0] = StoneHenge_data[i].nrm[0];
		stoneHenge[i].norms[1] = StoneHenge_data[i].nrm[1];
		stoneHenge[i].norms[2] = StoneHenge_data[i].nrm[2];
	}

	MY_MATRIX_4X4 rot = XRotation(-18);

	float zoom = -0.8f;

	MY_MATRIX_4X4 translate = MatrixIdentity();
	MY_MATRIX_4X4 Camera = MatrixIdentity();

	Translate(translate, 0.2f, 0.1f, zoom);
	Camera = MultiplyMatrixByMatrix4X4(translate, rot);

	float yScale = 1.0f / tan((3.1415f / 180.0f) * (90 >> 1));
	Proj_Matrix = BuildProjMatrix(yScale, yScale * (RASTER_W / RASTER_H), 0.1f, 10.0f);
	// Enter the main drawing loop...
	while (RS_Update(Raster, numPixels))
	{
		memset(Raster, 0, sizeof(Raster));
		time.Signal();

		timer = (float)time.Delta();
		rotate += (float)time.Delta();

		View_Matrix = Inverse(Camera);
		// Select appropriate Shaders you want to Use. REMEMBER! You can make as many as you want!
		// Only ever having one type of shader defeats the purpose! (This exists for flexibility!)
		VertexShader = VS_World; // I choose a shader that will multiply by a single matrix that I control.  
		PixelShader = PS_White; // Regardless of the original color I want all pixels to be shown white. 
		// Set any custom variables used by the shader functions so you can control shader behaviors directly! 
		//SV_WorldMatrix = BuidRotationMatrixOnAxisZ(timer);

		SV_WorldMatrix = XRotation(90);
		
		// With appropriate shaders chosen we draw the fixed line. (A copy will be modifed by shaders)
		DrawStars(stars);

		SV_WorldMatrix = MatrixIdentity();
		MY_MATRIX_4X4 trans = MatrixIdentity();
		Translate(trans, 0, 0.25f, 0);

		SV_WorldMatrix = MultiplyMatrixByMatrix4X4(trans, YRotation(rotate * 50));

		unsigned int d = 0;
		for (; d < numPixels; d++)
		{
			Depth[d] = 1;
		}

		if (GetAsyncKeyState(VK_UP))
		{
			MY_MATRIX_4X4 Cam = MatrixIdentity();
			Translate(Cam, 0, 0, 1.0f * timer);
			Camera = MultiplyMatrixByMatrix4X4(Cam, Camera);

		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			MY_MATRIX_4X4 Cam = MatrixIdentity();
			Translate(Cam, 0, 0, -1.0f * timer);
			Camera = MultiplyMatrixByMatrix4X4(Cam, Camera);
		}

		if (GetAsyncKeyState(AKey))
		{
			MY_MATRIX_4X4 Cam = MatrixIdentity();
			Translate(Cam, -1.0f * timer, 0, 0);
			Camera = MultiplyMatrixByMatrix4X4(Cam, Camera);

		}
		if (GetAsyncKeyState(DKey))
		{
			MY_MATRIX_4X4 Cam = MatrixIdentity();
			Translate(Cam, 1.0f * timer, 0, 0);
			Camera = MultiplyMatrixByMatrix4X4(Cam, Camera);
		}

		if (GetAsyncKeyState(WKey))
		{
			MY_MATRIX_4X4 Cam = MatrixIdentity();
			Translate(Cam, 0, 1.0f * timer, 0);
			Camera = MultiplyMatrixByMatrix4X4(Cam, Camera);

		}
		if (GetAsyncKeyState(SKey))
		{
			MY_MATRIX_4X4 Cam = MatrixIdentity();
			Translate(Cam, 0, -1.0f * timer, 0);
			Camera = MultiplyMatrixByMatrix4X4(Cam, Camera);
		}
	}
	// Exit program...
	RS_Shutdown();
}