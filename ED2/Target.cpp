#include <windows.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "extgl.h"

#include "EDApp.h"
#include "EDCommon.h"
#include "Target.h"

unsigned int CTarget::m_uiRefCount = 0;
unsigned int CTarget::m_uiDrawList = 0;

//•	If the target is spinning, increment its spin time(m_fSpinTime) by fTime.
//  If the target spins for more than 3.0f seconds, stop its spinning and reset its spin time.
//  Otherwise, rotate the target about its Y - Axis by(3.0f * fTime) radians.

//•	If the target is not spinning, perform the Look - 
//  At algorithm to make the target look at the body of the Buggy.

void CTarget::Update(float fTime)
{
	if (m_bSpinning)
	{
		m_fSpinTime += fTime;

		if (m_fSpinTime > 3.0f)
		{
			m_bSpinning = false;
			m_fSpinTime = 0.0f;
		}
		else
		{
			m_Matrix.rotate_y_pre(3.0f * fTime);
		}
	}
	else
	{
		LookAt(m_Matrix, EDApp::GetInstance().m_Buggy.m_Frames[CBuggy::BODY].GetLocalMat().axis_pos);
	}
}

void CTarget::Render()
{
	glColor3f( 0.75f, 0.0f, 0.0f );			
	glDisable( GL_CULL_FACE );

	glPushMatrix();
		glMultMatrixf( m_Matrix.ma );
		glCallList( m_uiDrawList );
	glPopMatrix();

	glEnable( GL_CULL_FACE );
}

CTarget::CTarget(void)
{
	m_fRadius = 0.5f;
	m_fSpinTime = 0.0f;

	if( m_uiRefCount == 0 )
	{
		m_uiDrawList = glGenLists(1);

		GLUquadric *pQuad = gluNewQuadric();
		glNewList( m_uiDrawList, GL_COMPILE );
			
			// innner ring
			gluCylinder( pQuad, 0.25f, 0.25f, 0.125f, 15, 15 );
			gluCylinder( pQuad, 0.125f, 0.125f, 0.125f, 15, 15 );

			glPushMatrix();
				glTranslatef( 0.0f, 0.0f, 0.125f );
				gluDisk( pQuad, 0.125f, 0.25f, 15, 15 );
			glPopMatrix();
			glPushMatrix();
				glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
				gluDisk( pQuad, 0.125f, 0.25f, 15, 15 );
			glPopMatrix();

			// outer ring
			gluCylinder( pQuad, 0.5f, 0.5f, 0.125f, 15, 15 );
			gluCylinder( pQuad, 0.375f, 0.375f, 0.125f, 15, 15 );

			glPushMatrix();
				glTranslatef( 0.0f, 0.0f, 0.125f );
				gluDisk( pQuad, 0.375f, 0.5f, 15, 15 );
			glPopMatrix();
			glPushMatrix();
				glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
				gluDisk( pQuad, 0.375f, 0.5f, 15, 15 );
			glPopMatrix();
		glEndList();
		gluDeleteQuadric(pQuad);
	}

	++m_uiRefCount;
	m_Matrix.make_identity();
}

CTarget::~CTarget()
{
	--m_uiRefCount;

	if( m_uiRefCount == 0 )
	{
		glDeleteLists( m_uiDrawList, 1 );
		m_uiDrawList = 0;
	}
}