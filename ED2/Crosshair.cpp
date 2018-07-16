#include <windows.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "extgl.h"

#include "EDApp.h"
#include "EDCommon.h"
#include "Crosshair.h"

unsigned int CCrosshair::m_uiRefCount = 0;
unsigned int CCrosshair::m_uiDrawList = 0;

//•	Hard attach the crosshair to the buggy’s gun, offsetting it by 25 units on the Z - Axis.

//•	Perform a ray to sphere intersection test against all the targets in the world.
//  If there is an intersection, snap the crosshair to that target and set EDApp’s m_pTarget to that target.
//  If there is no intersection set EDApp’s m_pTarget to NULL.For the sphere, use the position and radius of the target.
//  For the ray, use the position of the gun and the normalized vector from the gun to the crosshair.

void CCrosshair::Update(float fTime)
{
	vec3f center_p;
	float radius;
	vec3f start_p;

	GetMatrix() = EDApp::GetInstance().m_Buggy.m_Frames[CBuggy::GUN].GetWorldMat();
	EDApp::GetInstance().m_pTarget = NULL;
	GetMatrix().axis_pos += GetMatrix().axis_z * 25.0f;
	for (int i = 0; i < 20; i++)
	{
		center_p = EDApp::GetInstance().m_Targets[i].GetPosition();
		radius = EDApp::GetInstance().m_Targets[i].GetRadius();
		
		start_p = EDApp::GetInstance().m_Buggy.m_Frames[CBuggy::GUN].GetWorldMat().axis_pos;
		vec3f N = EDApp::GetInstance().m_Buggy.m_Frames[CBuggy::GUN].GetWorldMat().axis_z;
		//z axis of gun it's normalized yay
		vec3f vec = center_p - start_p;
		float distance = dot_product(N, vec);

		if (distance < 0)
			continue;
	
		N = { (N.x * distance), (N.y * distance), (N.z * distance) };

		vec3f cp = N + start_p;
		float D = sqrt((center_p.x - cp.x) * (center_p.x - cp.x) + (center_p.y - cp.y) * (center_p.y - cp.y) + (center_p.z - cp.z) * (center_p.z - cp.z));

		if (D <= radius)
		{
			EDApp::GetInstance().m_pTarget = &EDApp::GetInstance().m_Targets[i];
			GetMatrix() = EDApp::GetInstance().m_Targets[i].GetMatrix();
		}

	}
	
//•	Perform a line - to - triangle test with the crosshair and the terrain.
//  For the start point of the line segment, use the position of the buggy’s gun.
//  For the end point of the line segment, use the position of the crosshair.
//	If there is a collision, set the crosshair’s position to the collision point.
	vec3f newPos;
	unsigned int triIndex;

	vec3f start, end;
	float offset = 25.0f;
	start = EDApp::GetInstance().m_Buggy.m_Frames[CBuggy::GUN].GetWorldMat().axis_pos;
	end = EDApp::GetInstance().m_Crosshair.GetMatrix().axis_pos;

	EDTriangle * triangles;
	unsigned int triCount;
	EDApp::GetInstance().GetTriangles(&triangles, &triCount, start, end);

	bool clamp;
	clamp = EDApp::GetInstance().LineSegment2Triangle(newPos, triIndex, triangles, triCount, start, end);

	if (clamp)
	{
		EDApp::GetInstance().m_Crosshair.GetMatrix().axis_pos = newPos;
	}
}

CCrosshair::CCrosshair(void)
{
	m_fRadius = 0.375f;

	if( m_uiRefCount == 0 )
	{
		m_uiDrawList = glGenLists(1);

		GLUquadric *pQuad = gluNewQuadric();
		glNewList( m_uiDrawList, GL_COMPILE );
			// innner ring
			gluCylinder( pQuad, 0.125f, 0.125f, 0.125f, 15, 15 );

			glPushMatrix();
				glTranslatef( 0.0f, 0.0f, 0.125f );
				gluDisk( pQuad, 0.0f, 0.125f, 15, 15 );
			glPopMatrix();
			glPushMatrix();
				glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
				gluDisk( pQuad, 0.0f, 0.125f, 15, 15 );
			glPopMatrix();

			// outer ring
			gluCylinder( pQuad, 0.375f, 0.375f, 0.125f, 15, 15 );
			gluCylinder( pQuad, 0.25f, 0.25f, 0.125f, 15, 15 );

			glPushMatrix();
				glTranslatef( 0.0f, 0.0f, 0.125f );
				gluDisk( pQuad, 0.25f, 0.375f, 15, 15 );
			glPopMatrix();
			glPushMatrix();
				glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
				gluDisk( pQuad, 0.25f, 0.375f, 15, 15 );
			glPopMatrix();
		glEndList();
		gluDeleteQuadric(pQuad);
	}
	
	++m_uiRefCount;
	m_Matrix.make_identity();
}

CCrosshair::~CCrosshair()
{
	--m_uiRefCount;

	if( m_uiRefCount == 0 )
	{
		glDeleteLists( m_uiDrawList, 1 );
		m_uiDrawList = 0;
	}
}

void CCrosshair::Render(void)
{
	glColor3f( 0.5f, 0.5f, 0.5f );	
	//glDisable( GL_LIGHTING );
	glDisable( GL_CULL_FACE );

	glPushMatrix();
		glMultMatrixf( m_Matrix.ma );
		glCallList( m_uiDrawList );
	glPopMatrix();
	//glEnable( GL_LIGHTING );
	glEnable( GL_CULL_FACE );
}
