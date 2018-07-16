#include <windows.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "extgl.h"

#include "EDApp.h"
#include "EDCommon.h"

#include "Mortar.h"

unsigned int CMortar::m_uiRefCount = 0;
unsigned int CMortar::m_uiDrawList = 0;

//•	If the mortar lives for more than 60 seconds, Kill() it.

//•	Store the current position of the mortar in m_fvOldPos.

//•	Translate the mortar along its Z - Axis by(fTime) units.

//•	Translate the mortar by(-0.001f * fTime) units on the world - Y for gravity.

//•	Perform the “Orient in the direction the object is moving” algorithm from “3DManipulations.doc” to arc the shell.
//  Essentially you are performing a look - at from the old position to the new position but keeping the new position in your final matrix.

//•	For all the targets in the world, perform a cylinder to sphere intersection test.
//  Use the position and radius of each target for the target’s sphere.Use the old position of the mortar as the base point of the cylinder, 
//  and the current position of the mortar as the top point.Use the radius of the mortar as the radius of the cylinder.
//  If there is a collision, Kill() the mortar and Spin() the target.

void CMortar::Update( float fTime )
{
	m_fAge += fTime;
	m_fTrailTimer += fTime;

 	m_fvOldPos = m_Matrix.axis_pos;

	m_Matrix.axis_pos += m_Matrix.axis_z * (fTime);
	m_Matrix.axis_pos += worldY * (-0.001f * fTime);
	m_Velocity += vec3f(0, -1.0f, 0) * (fTime * 0.005f);

	vec3f zAxis = m_Velocity *fTime;
	zAxis.normalize();

	vec3f xAxis = worldY;
	xAxis.normalize();

	vec3f yAxis;
	cross_product(yAxis, zAxis, xAxis);
	yAxis.normalize();

	m_Matrix.axis_x = xAxis;
	m_Matrix.axis_y = yAxis;
	m_Matrix.axis_z = zAxis;

	for (int i = 0; i < 20; i++)
	{
		vec3f base = m_fvOldPos;
		vec3f top = m_Matrix.axis_pos;
		float c_Radius = GetRadius();

		vec3f center = EDApp::GetInstance().m_Targets[i].GetPosition();
		float radius = EDApp::GetInstance().m_Targets[i].GetRadius();

		vec3f vector = top - base;
		float len = vector.magnitude();
		vec3f N = vector.normalize();

		vec3f bToc = center - base;

		float D0 = dot_product(bToc, N);

		if (D0 < 0 || D0 > len)
			continue;

		vec3f n_Scaled = N * D0;
		vec3f CP = n_Scaled + base;

		float distance = sqrt((center.x - CP.x) * (center.x - CP.x) +
			(center.y - CP.y) * (center.y - CP.y) +
			(center.z - CP.z) * (center.z - CP.z));

		if (distance < c_Radius + radius)
		{
			Kill();
			EDApp::GetInstance().m_Targets[i].Spin();
		}
	}

//•	Perform a line - to - triangle intersection test with the mortar and the terrain.
//	For the start point of the line segment, use the old position of the mortar.
//	For the end point of the line segment, use the position of the mortar.
//	If there is a collision, Kill() the mortar.
	vec3f newPos;
	unsigned int triIndex;

	vec3f start, end;
	start = m_fvOldPos;
	end = m_Matrix.axis_pos;

	EDTriangle * triangles;
	unsigned int triCount;
	EDApp::GetInstance().GetTriangles(&triangles, &triCount, start, end);

	bool clamp;
	clamp = EDApp::GetInstance().LineSegment2Triangle(newPos, triIndex, triangles, triCount, start, end);

	if (clamp)
	{
		Kill();
	}

	if (m_fAge > 60.0f)
		Kill();
	// Update the mortar trail...
	if( m_fTrailTimer >= 0.0666f )
	{
		memmove( m_trail, &m_trail[1], sizeof(vec3f) * (TRAILLEN-1) );
		m_trail[TRAILLEN-1] = m_Matrix.axis_pos;
		m_fTrailTimer = 0.0f;
	}
}

CMortar::CMortar(void)
{
	m_fRadius = 0.25f;
	m_fAge = 0.0f;
	m_fTrailTimer = 0.0f;

	if( m_uiRefCount == 0 )
	{
		m_uiDrawList = glGenLists(1);

		GLUquadric *pQuad = gluNewQuadric();
		glNewList( m_uiDrawList, GL_COMPILE );

			glPushMatrix();
				
				glPushMatrix();
					glTranslatef( 0.0f, 0.0f, -0.0625f );
					glColor3f( 0.75f, 0.75f, 0.0f );
					gluCylinder( pQuad, 0.0125f, 0.0125f, 0.125f, 15, 15 );
				glPopMatrix();
				
				glPushMatrix();
					glTranslatef( 0.0f, 0.0f, 0.0625f );
					glColor3f( 0.25f, 0.25f, 0.25f );
					gluSphere( pQuad, 0.0125f, 15, 15 );
				glPopMatrix();

				glPushMatrix();
					glTranslatef( 0.0f, 0.0f, -0.09375f );
					glColor3f( 0.75f, 0.75f, 0.0f );
					gluCylinder( pQuad, 0.00625f, 0.0125f, 0.03125f, 15, 15 );
				glPopMatrix();


				glPushMatrix();
					glTranslatef( 0.0f, 0.0f, -0.125f );
					gluCylinder( pQuad, 0.0125f, 0.00625f, 0.03125f, 15, 15 );

					glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
					glColor3f( 0.5f, 0.0f, 0.0f );
					gluDisk( pQuad, 0.0f, 0.0125f, 15, 15 );
				glPopMatrix();
				
			glPopMatrix();

		glEndList();
		gluDeleteQuadric(pQuad);
	}

	++m_uiRefCount;
	m_Matrix.make_identity();
}

CMortar::~CMortar()
{
	--m_uiRefCount;

	if( m_uiRefCount == 0 )
	{
		glDeleteLists( m_uiDrawList, 1 );
		m_uiDrawList = 0;
	}
}

void CMortar::Render(void)
{
	glPushMatrix();
		glMultMatrixf( m_Matrix.ma );
		glCallList( m_uiDrawList );
	glPopMatrix();

	glDisable( GL_LIGHTING );
	glDepthMask( 0 );

	glLineWidth( 2.0f );
	glBegin( GL_LINE_STRIP );
		for( unsigned int i = 0; i < TRAILLEN; ++i )
		{
			float fTemp = i/(float)TRAILLEN;

			glColor4f( 1.0f, fTemp*fTemp, fTemp*fTemp*fTemp*fTemp, fTemp );
			glVertex3fv( m_trail[i].v );
		}
	glEnd();

	glEnable( GL_LIGHTING );
	glDepthMask( 1 );
	glLineWidth( 1.0f );
}

void CMortar::Kill(void)
{
	for( unsigned int i = 0; i < 20; ++i )
	{
		if( !EDApp::GetInstance().m_Explosions[i].IsAlive() )
		{
			EDApp::GetInstance().m_Explosions[i].Explode( m_Matrix );
			break;
		}
	}

	m_fAge = 0.0f;
	m_fTrailTimer = 0.0f;
	m_bAliveFlag = false;
}

void CMortar::Fire( const matrix4f &attachMat )
{
	m_Matrix = attachMat;
	m_bAliveFlag = true;

	m_Velocity = EDApp::GetInstance().m_Buggy.m_Frames[CBuggy::GUN].GetWorldMat().axis_z *  0.05f;

	for( unsigned int i = 0; i < TRAILLEN; ++i )
		m_trail[i] = m_Matrix.axis_pos;
}