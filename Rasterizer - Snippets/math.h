#include "defines.h"


MY_MATRIX_4X4 MatrixIdentity(void)
{
	MY_MATRIX_4X4 identity;
	identity.p11 = 1;
	identity.p12 = 0;
	identity.p13 = 0;
	identity.p14 = 0;

	identity.p21 = 0;
	identity.p22 = 1;
	identity.p23 = 0;
	identity.p24 = 0;

	identity.p31 = 0;
	identity.p32 = 0;
	identity.p33 = 1;
	identity.p34 = 0;

	identity.p41 = 0;
	identity.p42 = 0;
	identity.p43 = 0;
	identity.p44 = 1;

	return identity;
}


VEC MultiplyVectorByMatrix(VEC V, MY_MATRIX_4X4 M)
{
	float x, y, z, w;

	x = M.p11 * V.x + M.p21 * V.y + M.p31 * V.z + M.p41 * V.w;
	y = M.p12 * V.x + M.p22 * V.y + M.p32 * V.z + M.p42 * V.w;
	z = M.p13 * V.x + M.p23 * V.y + M.p33 * V.z + M.p43 * V.w;
	w = M.p14 * V.x + M.p24 * V.y + M.p34 * V.z + M.p44 * V.w;

	V.x = x;
	V.y = y;
	V.z = z;
	V.w = w;

	return V;
}

VEC MultiplyVectorByMatrix(VEC V, MY_MATRIX_3X3 M)
{
	float x, y, z;

	x = M.p11 * V.x + M.p21 * V.y + M.p31 * V.z;
	y = M.p12 * V.x + M.p22 * V.y + M.p32 * V.z;
	z = M.p13 * V.x + M.p23 * V.y + M.p33 * V.z;

	V.x = x;
	V.y = y;
	V.z = z;

	return V;
}

MY_VERTEX MultiplyVertexByMatrix(MY_VERTEX V, MY_MATRIX_4X4 M)
{
	VEC vect;
	vect.x = V.axis[0];
	vect.y = V.axis[1];
	vect.z = V.axis[2];
	vect.w = V.axis[3];

	V.axis[0] = M.p11 * vect.x + M.p21 * vect.y + M.p31 * vect.z + M.p41 * vect.w;
	V.axis[1] = M.p12 * vect.x + M.p22 * vect.y + M.p32 * vect.z + M.p42 * vect.w;
	V.axis[2] = M.p13 * vect.x + M.p23 * vect.y + M.p33 * vect.z + M.p43 * vect.w;
	V.axis[3] = M.p14 * vect.x + M.p24 * vect.y + M.p34 * vect.z + M.p44 * vect.w;


	return V;
}


MY_MATRIX_3X3 MultiplyMatrixByMatrix(MY_MATRIX_3X3 M1, MY_MATRIX_3X3 M2)
{
	MY_MATRIX_3X3 newMatrix = { M1.p11 * M2.p11, M1.p12 * M2.p12, M1.p13 * M2.p13,
		M1.p21 * M2.p21, M1.p22 * M2.p22, M1.p23 * M2.p23,
		M1.p31 * M2.p31, M1.p32 * M2.p32, M1.p33 * M2.p33 };

	return newMatrix;
}

MY_MATRIX_4X4 MultiplyMatrixByMatrix4X4(MY_MATRIX_4X4 M1, MY_MATRIX_4X4 M2)
{
	MY_MATRIX_4X4 newMatrix = MatrixIdentity();

	newMatrix.m[0][0] = M1.m[0][0] * M2.m[0][0] + M1.m[0][1] * M2.m[1][0] + M1.m[0][2] * M2.m[2][0] + M1.m[0][3] * M2.m[3][0];
	newMatrix.m[0][1] = M1.m[0][0] * M2.m[0][1] + M1.m[0][1] * M2.m[1][1] + M1.m[0][2] * M2.m[2][1] + M1.m[0][3] * M2.m[3][1];
	newMatrix.m[0][2] = M1.m[0][0] * M2.m[0][2] + M1.m[0][1] * M2.m[1][2] + M1.m[0][2] * M2.m[2][2] + M1.m[0][3] * M2.m[3][2];
	newMatrix.m[0][3] = M1.m[0][0] * M2.m[0][3] + M1.m[0][1] * M2.m[1][3] + M1.m[0][2] * M2.m[2][3] + M1.m[0][3] * M2.m[3][3];
	newMatrix.m[1][0] = M1.m[1][0] * M2.m[1][0] + M1.m[1][1] * M2.m[1][0] + M1.m[1][2] * M2.m[2][0] + M1.m[1][3] * M2.m[3][0];
	newMatrix.m[1][1] = M1.m[1][0] * M2.m[1][1] + M1.m[1][1] * M2.m[1][1] + M1.m[1][2] * M2.m[2][1] + M1.m[1][3] * M2.m[3][1];
	newMatrix.m[1][2] = M1.m[1][0] * M2.m[1][2] + M1.m[1][1] * M2.m[1][2] + M1.m[1][2] * M2.m[2][2] + M1.m[1][3] * M2.m[3][2];
	newMatrix.m[1][3] = M1.m[1][0] * M2.m[1][3] + M1.m[1][1] * M2.m[1][3] + M1.m[1][2] * M2.m[2][3] + M1.m[1][3] * M2.m[3][3];
	newMatrix.m[2][0] = M1.m[2][0] * M2.m[2][0] + M1.m[2][1] * M2.m[1][0] + M1.m[2][2] * M2.m[2][0] + M1.m[2][3] * M2.m[3][0];
	newMatrix.m[2][1] = M1.m[2][0] * M2.m[2][1] + M1.m[2][1] * M2.m[1][1] + M1.m[2][2] * M2.m[2][1] + M1.m[2][3] * M2.m[3][1];
	newMatrix.m[2][2] = M1.m[2][0] * M2.m[2][2] + M1.m[2][1] * M2.m[1][2] + M1.m[2][2] * M2.m[2][2] + M1.m[2][3] * M2.m[3][2];
	newMatrix.m[2][3] = M1.m[2][0] * M2.m[2][3] + M1.m[2][1] * M2.m[1][3] + M1.m[2][2] * M2.m[2][3] + M1.m[2][3] * M2.m[3][3];
	newMatrix.m[3][0] = M1.m[3][0] * M2.m[3][0] + M1.m[3][1] * M2.m[1][0] + M1.m[3][2] * M2.m[2][0] + M1.m[3][3] * M2.m[3][0];
	newMatrix.m[3][1] = M1.m[3][0] * M2.m[3][1] + M1.m[3][1] * M2.m[1][1] + M1.m[3][2] * M2.m[2][1] + M1.m[3][3] * M2.m[3][1];
	newMatrix.m[3][2] = M1.m[3][0] * M2.m[3][2] + M1.m[3][1] * M2.m[1][2] + M1.m[3][2] * M2.m[2][2] + M1.m[3][3] * M2.m[3][2];
	newMatrix.m[3][3] = M1.m[3][0] * M2.m[3][3] + M1.m[3][1] * M2.m[1][3] + M1.m[3][2] * M2.m[2][3] + M1.m[3][3] * M2.m[3][3];

	return newMatrix;
}

MY_MATRIX_4X4 BuidRotationMatrixOnAxisZ(float angle)
{
	MY_MATRIX_4X4 identity = MatrixIdentity();
	identity.p11 = cos(angle * PI / 180);
	identity.p12 = -sin(angle * PI / 180);
	identity.p13 = 0;

	identity.p21 = sin(angle * PI / 180);
	identity.p22 = cos(angle * PI / 180);

	identity.p33 = 1;

	return identity;
}

MY_MATRIX_4X4 YRotation(float angle)
{
	MY_MATRIX_4X4 identity = MatrixIdentity();
	identity.p11 = cos(angle * (PI / 180));
	identity.p13 = sin(angle * (PI / 180));
	identity.p22 = 1;

	identity.p31 = -sin(angle * (PI / 180));
	identity.p33 = cos(angle * (PI / 180));

	identity.p44 = 1;

	return identity;
}

MY_MATRIX_4X4 XRotation(float angle)
{
	MY_MATRIX_4X4 identity = MatrixIdentity();

	identity.p22 = cos(angle * (PI / 180));
	identity.p23 = -sin(angle * (PI / 180));

	identity.p32 = sin(angle * (PI / 180));
	identity.p33 = cos(angle * (PI / 180));


	return identity;
}


MY_MATRIX_4X4 BuildProjMatrix(float view, float screenRatio, float n, float f)
{
	MY_MATRIX_4X4 m = MatrixIdentity();

	m.p11 = screenRatio;
	m.p22 = view;
	m.p33 = f / (f - n);
	m.p34 = 1;
	m.p43 = -(f * n) / (f - n);
	m.p44 = 0;

	return m;
}

void Inverse(MY_MATRIX_4X4& m)
{
	MY_MATRIX_3X3 m3;

	VEC v;
	v.x = m.p41;
	v.y = m.p42;
	v.z = m.p43;
	v.w = 1;

	m3.p11 = m.p11;
	m3.p12 = m.p21;
	m3.p13 = m.p31;

	m3.p21 = m.p12;
	m3.p22 = m.p22;
	m3.p23 = m.p32;

	m3.p31 = m.p13;
	m3.p32 = m.p23;
	m3.p33 = m.p33;

	v = MultiplyVectorByMatrix(v, m3);

	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;

	m.p11 = m3.p11;
	m.p12 = m3.p12;
	m.p13 = m3.p13;

	m.p21 = m3.p21;
	m.p22 = m3.p22;
	m.p23 = m3.p23;

	m.p31 = m3.p31;
	m.p32 = m3.p32;
	m.p33 = m3.p33;

	m.p41 = v.x;
	m.p42 = v.y;
	m.p43 = v.z;
}

void Translate(MY_MATRIX_4X4 &m, float x, float y, float z)
{
	m.p41 = x;
	m.p42 = y;
	m.p43 = z;
}

void TranslateGlobal(MY_MATRIX_4X4 &m, float x, float y, float z)
{
	m.p41 += x;
	m.p42 += y;
	m.p43 += z;
}
