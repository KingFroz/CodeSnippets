#include <windows.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "extgl.h"

#include "Bullet.h"
#include "EDCommon.h"
#include "EDApp.h"

//•	If the bullet lives for more than 30 seconds, Kill() it.

//•	Translate the bullet along its Z - Axis by(fTime * 0.5f) units.

//•	If EDApp’s m_pTarget points to a valid target(not NULL),
//  then perform the full turn - to algorithm(same as ED1)
//  on the bullet to make it turn to m_pTarget’s position.

//•	For all the targets in the world, perform a sphere to sphere intersection test with the sphere of the bullet.
//  Use the position and radius of each target for the target’s sphere.
//  Use the the position and radius of the bullet for the bullet’s sphere.
//  If there is a collision, Kill() the bullet and Spin() the target.
//  The spinning will not be seen until you finish CTarget::Update().

void CBullet::Update(float fTime)
{
	m_fAge += fTime;
	m_fTrailTimer += fTime;

	for (int i = 0; i < 20; i++)
	{
		vec3f vec_d = (m_Matrix.axis_pos - EDApp::GetInstance().m_Targets[i].GetPosition());
		float distance = dot_product(vec_d, vec_d);
		float radius = (m_fRadius + EDApp::GetInstance().m_Targets[i].GetRadius());

		radius = radius * radius;
		if (distance <= radius)
		{
			EDApp::GetInstance().m_Targets[i].Spin();
			Kill();
		}
	}

	if (EDApp::GetInstance().m_pTarget != NULL)
	{
		vec3f homing = EDApp::GetInstance().m_pTarget->GetPosition() - m_Matrix.axis_pos;
		homing.normalize();
		m_Matrix.axis_pos += homing * (fTime * 0.5f);
	}
	else
	{
		m_Matrix.axis_pos += m_Matrix.axis_z * (fTime * 0.5f);
	}

	if (m_fAge > 30.0f)
		Kill();

	// Update the bullet trail...
	if( m_fTrailTimer >= 0.0666f )
	{
		memmove( m_trail, &m_trail[1], sizeof(vec3f) * (TRAILLEN-1) );
		m_trail[TRAILLEN-1] = m_Matrix.axis_pos;
		m_fTrailTimer = 0.0f;
	}

}

void CBullet::Render(void)
{
	float fScale = (rand() % 100) / 100.0f;

	GLUquadric * pQuad = gluNewQuadric();

	gluQuadricTexture( pQuad, true );

	glColor4f( 0.0f, 1.0f, 1.0f, 1.0f );

	glPushMatrix();
		glMultMatrixf( m_Matrix.ma );
		gluSphere( pQuad, fScale * 0.0625f, 15, 15 );
	glPopMatrix();

	glLineWidth( 2.0f );
	glBegin( GL_LINE_STRIP );
		for( unsigned int i = 0; i < TRAILLEN; ++i )
		{
			float fTemp = i/(float)TRAILLEN;

			glColor4f( fTemp*fTemp*fTemp*fTemp, fTemp*fTemp, fTemp*fTemp, fTemp );
			glVertex3fv( m_trail[i].v );
		}
	glEnd();
	glLineWidth( 1.0f );

	gluDeleteQuadric( pQuad );
}