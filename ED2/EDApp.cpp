#include "EDApp.h"

#include <windows.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "extgl.h"

#include "EDDemo.h"
#include "BMPFile.h"

#include "EDCommon.h"

#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif

bool EDApp::LineSegment2Triangle(vec3f &vOut, unsigned int &uiTriIndex, EDTriangle *pTris, unsigned int uiTriCount, const vec3f &vStart, const vec3f &vEnd)
{
	unsigned int vertIndex = 0;
	bool col = false;
	vOut = vEnd;
	/*Reminder: Find the NEAREST interesecting triangle*/
	for (unsigned int i = 0; i < uiTriCount; ++i)
	{
		//Exclude Backfacing triangles
		if (dot_product(vStart, pTris[i].m_Normal) - dot_product(pTris[i].m_Vertices[vertIndex], pTris[i].m_Normal) < 0) {
			continue;
		}

		if (dot_product(vOut, pTris[i].m_Normal) - dot_product(pTris[i].m_Vertices[vertIndex], pTris[i].m_Normal) > 0) {
			continue;
		}

		//Finding Collision Point
		float D0 = dot_product(pTris[i].m_Normal, vStart);
		float D1 = dot_product(pTris[i].m_Normal, pTris[i].m_Vertices[vertIndex]);
		float D2 = D0 - D1;

		vec3f L = vOut - vStart;

		float D3 = dot_product(pTris[i].m_Normal, L);
		float DF = -(D2 / D3);

		vec3f CP = vStart + (L * DF);

		//Determine if CP is inside the triangle | Edge0
		vec3f Edge0 = pTris[i].m_Vertices[1] - pTris[i].m_Vertices[0];
		vec3f Normal0;
		cross_product(Normal0, Edge0, pTris[i].m_Normal);

		if (dot_product(CP - pTris[i].m_Vertices[0], Normal0) > 0)
		{
			continue;
		}

		//Determine if CP is inside the triangle | Edge1
		vec3f Edge1 = pTris[i].m_Vertices[2] - pTris[i].m_Vertices[1];
		vec3f Normal1;
		cross_product(Normal1, Edge1, pTris[i].m_Normal);

		if (dot_product(CP - pTris[i].m_Vertices[1], Normal1) > 0)
		{
			continue;
		}

		//Determine if CP is inside the triangle | Edge2
		vec3f Edge2 = pTris[i].m_Vertices[0] - pTris[i].m_Vertices[2];
		vec3f Normal2;
		cross_product(Normal2, Edge2, pTris[i].m_Normal);

		if (dot_product(CP - pTris[i].m_Vertices[2], Normal2) > 0)
		{
			continue;
		}

		vOut = CP;
		uiTriIndex = i;

		col = true;
	}

	return col;
}

//•	Perform ground clamping on the Buggy’s body using the line - to - triangle intersection test and the ground clamping algorithm.

//•	Make sure there is a call to m_Buggy.m_Frames[CBuggy::BODY].Update(); 
// after this step to force an update of the matrix hierarchy of the buggy and its children the next time you call GetWorldMatrix() on any of them.

void EDApp::Drive(float fTime)
{
	matrix4f &BuggyMat = m_Buggy.m_Frames[CBuggy::BODY].GetLocalMat();
	matrix4f &BuggyGun = m_Buggy.m_Frames[CBuggy::GUN].GetLocalMat();

	if( m_Camera.GetCameraMode() != EDCamera::MOUSE_LOOK && GetAsyncKeyState( VK_RBUTTON ) )
	{
		
		MouseLook(BuggyGun, fTime);
		glutSetCursor( GLUT_CURSOR_NONE );
	}
	else
	{
		glutSetCursor( GLUT_CURSOR_INHERIT );
	}

	// Move the buggy forward along it's Z-Axis
	if( GetAsyncKeyState( 'W' ) )
	{
		vec3f v = { 1,0,0 };

		float rot = dot_product(BuggyGun.axis_z, v);
		float turnRate = rot * fTime;

		BuggyGun.rotate_y_pre(-turnRate);
		BuggyMat.rotate_y_pre(turnRate);
		BuggyMat.axis_pos += BuggyMat.axis_z * fTime;
	}

	// Move the buggy backward along it's Z-Axis
	if( GetAsyncKeyState( 'S' ) )
	{
		vec3f v = { 1,0,0 };

		float rot = dot_product(BuggyGun.axis_z, v);
		float turnRate = rot * fTime;

		BuggyGun.rotate_y_pre(-turnRate);
		BuggyMat.rotate_y_pre(turnRate);
		BuggyMat.axis_pos += BuggyMat.axis_z * -fTime;
	}
	
	m_Buggy.m_Frames[CBuggy::BODY].Update();

	vec3f newPos;
	unsigned int triIndex;
	
	vec3f start, end;
	float offset = 25.0f;
	start = BuggyMat.axis_pos;
	start.y += offset;
	end = BuggyMat.axis_pos;
	end.y -= offset;

	EDTriangle * triangles;
	unsigned int triCount;
	EDApp::GetTriangles(&triangles, &triCount, start, end);

	bool clamp;
	clamp = LineSegment2Triangle(newPos, triIndex, triangles, triCount, start, end);

	if (clamp)
	{
		BuggyMat.axis_pos = newPos;
	}

	////FLT////
	vec3f flPos;
	unsigned int fltindex;

	vec3f fl_start, fl_end;
	fl_start = EDApp::m_Buggy.m_Frames[CBuggy::FLWHEEL].GetWorldMat().axis_pos;
	fl_start.y += offset;
	fl_end = EDApp::m_Buggy.m_Frames[CBuggy::FLWHEEL].GetWorldMat().axis_pos;
	fl_end.y -= offset;

	EDTriangle * fl_triangles = nullptr;
	unsigned int fl_triCount = 0;
	EDApp::GetTriangles(&fl_triangles, &fl_triCount, fl_start, fl_end);

	bool flClamp;
	flClamp = LineSegment2Triangle(flPos, fltindex, fl_triangles, fl_triCount, fl_start, fl_end);

	if (!flClamp)
	{
		flPos = fl_start;
	}
	////FRT////
	vec3f frPos;
	unsigned int frtindex;

	vec3f fr_start, fr_end;
	fr_start = EDApp::m_Buggy.m_Frames[CBuggy::FRWHEEL].GetWorldMat().axis_pos;
	fr_start.y += offset;
	fr_end = EDApp::m_Buggy.m_Frames[CBuggy::FRWHEEL].GetWorldMat().axis_pos;
	fr_end.y -= offset;

	EDTriangle * fr_triangles = nullptr;
	unsigned int fr_triCount = 0;
	EDApp::GetTriangles(&fr_triangles, &fr_triCount, fr_start, fr_end);

	bool frClamp;
	frClamp = LineSegment2Triangle(frPos, frtindex, fr_triangles, fr_triCount, fr_start, fr_end);

	if (!frClamp)
	{
		frPos = fr_start;
	}
	////REAR////
	vec3f rPos;
	unsigned int rtindex;

	vec3f r_start, r_end;
	r_start = EDApp::m_Buggy.m_Frames[CBuggy::BLWHEEL].GetWorldMat().axis_pos + EDApp::m_Buggy.m_Frames[CBuggy::BRWHEEL].GetWorldMat().axis_pos;
	r_start /= 2;
	r_start.y += offset;
	r_end = EDApp::m_Buggy.m_Frames[CBuggy::BLWHEEL].GetWorldMat().axis_pos + EDApp::m_Buggy.m_Frames[CBuggy::BRWHEEL].GetWorldMat().axis_pos;
	r_end /= 2;
	r_end.y -= offset;

	EDTriangle * r_triangles = nullptr;
	unsigned int r_triCount = 0;
	EDApp::GetTriangles(&r_triangles, &r_triCount, r_start, r_end);

	bool rClamp;
	rClamp = LineSegment2Triangle(rPos, rtindex, r_triangles, r_triCount, r_start, r_end);
	
	if (!rClamp)
	{
		rPos = r_start;
	}
	//....
	vec3f xAxis = (flPos - frPos).normalize();
	vec3f tempZ = frPos - rPos;
	vec3f yAxis;
	cross_product(yAxis, tempZ, xAxis);
	yAxis.normalize();
	vec3f zAxis;
	cross_product(zAxis, xAxis, yAxis);
	zAxis.normalize();

	BuggyMat.axis_x = xAxis;
	BuggyMat.axis_y = yAxis;
	BuggyMat.axis_z = zAxis;

	// We moved the Buggy, so update it's frame.
	m_Buggy.m_Frames[CBuggy::BODY].Update();

	// Fire a mortar...
	if( GetAsyncKeyState( VK_SPACE ) & 0x0001 )
	{
		for( unsigned int i = 0; i < 20; ++i )
		{
			if( !m_Mortars[i].IsAlive() )
			{
				matrix4f temp = m_Buggy.m_Frames[CBuggy::GUN].GetWorldMat();
				temp.axis_pos += temp.axis_y * 0.075f;
				temp.axis_pos += temp.axis_z * 0.25f;

				m_Mortars[i].Fire( temp );
				break;
			}
		}
	}
	
	// Fire a bullet...
	if( GetAsyncKeyState( VK_LBUTTON ) & 0x0001 )
	{
			for( unsigned int i = 0; i < 20; ++i )
			{
				if( !m_Bullets[i].IsAlive() )
				{
					matrix4f temp = m_Buggy.m_Frames[CBuggy::GUN].GetWorldMat();
					temp.axis_pos += temp.axis_y * 0.075f;
					temp.axis_pos += temp.axis_z * 0.25f;

					m_Bullets[i].Fire( temp );
					break;
				}
			}
	}
}

void EDApp::Update(void)
{
	// Get the elapsed seconds since the last frame
	float fTimeElapsed = m_Timer.GetElapsedTime() * 4.0f;
	m_Timer.Reset();

	// Update/Drive the buggy
	Drive(fTimeElapsed);

	// Clear our screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the identity
	glPushMatrix();
		
		// Update our camera
		m_Camera.Update(fTimeElapsed);
		m_Camera.ApplyCameraTransform();

		// Set LIGHT0's position
		float fPos[4] = { 0.707f, 0.707f, 0.0f, 0.0f };
		glLightfv( GL_LIGHT0, GL_POSITION, fPos );

		// Render the buggy
		glBindTexture( GL_TEXTURE_2D, m_Buggy.m_uiTexID );
		glColor3f( 1.0f, 1.0f, 1.0f );
		glEnable( GL_TEXTURE_2D );
			m_Buggy.Render();
		glDisable( GL_TEXTURE_2D );

		#if 1 // Render the terrain or don't render the terrain
				glPushMatrix();
					glBindTexture( GL_TEXTURE_2D, m_TerTex );
					glColor3f( 1.0f, 1.0f, 1.0f );
				
					glEnable( GL_TEXTURE_2D );

					glVertexPointer( 3, GL_FLOAT, 0, m_verts[0].v );
					glNormalPointer( GL_FLOAT, 0, m_norms[0].v );
					glTexCoordPointer( 2, GL_FLOAT, 0, &m_uvs[0].s );

					glDrawArrays( GL_TRIANGLES, 0, m_verts.size() );
					glDisable( GL_TEXTURE_2D );
				glPopMatrix();
		#endif

		// UPDATE TARGETS
		for( unsigned int i = 0; i < 20; ++i )
		{
			m_Targets[i].Update(fTimeElapsed);
			m_Targets[i].Render();
		}

		// Draw the world's coordinate axes
		glDisable(GL_LIGHTING);
			glDepthMask( 0 );
				glColor3f( 0.0f, 0.5f, 0.0f );
				DrawGround();
			glDepthMask( 1 );
			DrawAxes();
		glEnable(GL_LIGHTING);

		// UPDATE THE CROSSHAIR
		m_Crosshair.Update(fTimeElapsed);
		m_Crosshair.Render();

		// UPDATE ACTIVE MORTARS
		for( unsigned int i = 0; i < 20; ++i )
		{
			if( m_Mortars[i].IsAlive() )
			{
				m_Mortars[i].Update(fTimeElapsed);
				m_Mortars[i].Render();
			}
		}

		// UPDATE ACTIVE EXPLOSIONS
		glDisable( GL_LIGHTING );
		glDisable( GL_CULL_FACE );
		glDepthMask( 0 );

		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_BullTex );

		for( unsigned int i = 0; i < 20; ++i )
		{
			if( m_Explosions[i].IsAlive() )
			{
				m_Explosions[i].Update( fTimeElapsed );
				m_Explosions[i].Render();
			}
		}

		glDisable( GL_TEXTURE_2D );
		glEnable( GL_LIGHTING );
		glEnable( GL_CULL_FACE );
		glDepthMask( 1 );

		glEnable( GL_TEXTURE_2D );
		glDisable( GL_LIGHTING );
		glDepthMask( 0 );

		// UPDATE ACTIVE BULLETS
		glBindTexture( GL_TEXTURE_2D, m_BullTex );
		for( unsigned int i = 0; i < 20; ++i )
		{
			if( m_Bullets[i].IsAlive() )
			{
				m_Bullets[i].Update(fTimeElapsed);
				m_Bullets[i].Render();
			}
		}
		glEnable( GL_LIGHTING );
		glDepthMask( 1 );
		glDisable( GL_TEXTURE_2D );

	// Restore the identity
	glPopMatrix();

	// Swap the buffer
	glutSwapBuffers();

	// Tell glut to render again
	glutPostRedisplay();
}

void EDApp::Initialize(void)
{
	BMPFile buggyTex( "jeepbmp.BMP" );
	glGenTextures( 1, &m_Buggy.m_uiTexID );
	glBindTexture( GL_TEXTURE_2D, m_Buggy.m_uiTexID );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, (GLsizei)buggyTex.GetWidth(), (GLsizei)buggyTex.GetHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, buggyTex.GetPixelData() ); 

	BMPFile terrainTex( "tempgrass2.bmp" );
	glGenTextures( 1, &m_TerTex );
	glBindTexture( GL_TEXTURE_2D, m_TerTex );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, (GLsizei)terrainTex.GetWidth(), (GLsizei)terrainTex.GetHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, terrainTex.GetPixelData() ); 

	BMPFile bullTex( "plasma2.bmp" );
	glGenTextures( 1, &m_BullTex );
	glBindTexture( GL_TEXTURE_2D, m_BullTex );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, (GLsizei)bullTex.GetWidth(), (GLsizei)bullTex.GetHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, bullTex.GetPixelData() ); 

	size_t rowCount;

	FILE *pTerrainFile = fopen( "terrain.bin", "rb" );

	if( pTerrainFile )
	{
		fread( &rowCount, sizeof(size_t), 1, pTerrainFile );

		m_verts.resize( rowCount );
		m_norms.resize( rowCount );
		m_uvs.resize( rowCount );

		fread( m_verts[0].v, sizeof( vec3f ), rowCount, pTerrainFile );
		fread( m_norms[0].v, sizeof( vec3f ), rowCount, pTerrainFile );
		fread( &m_uvs[0].s, sizeof( uv ), rowCount, pTerrainFile );

		fclose( pTerrainFile );
	}

	vec3f highest = m_verts[0];
	for( unsigned int i = 0; i < m_verts.size(); ++i )
	{
		if( m_verts[i].x > highest.x )
			highest.x = m_verts[i].x;
		if( m_verts[i].y > highest.y )
			highest.y = m_verts[i].y;
		if( m_verts[i].z > highest.z )
			highest.z = m_verts[i].z;
	}

	highest *= 0.5f;
	for( unsigned int i = 0; i < m_verts.size(); ++i )
	{
		m_verts[i] -= highest;
	}


	m_Buggy.m_Frames[CBuggy::BODY].GetLocalMat().axis_pos = vec3f(-1.5f, 1.5f, -1.5f);

	m_Buggy.m_Frames[CBuggy::BODY].Update();


	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	srand(GetTickCount());

	for( unsigned int i = 0; i < 20; ++i )
	{
		size_t posIndex = rand() % m_verts.size();

		m_Targets[i].m_Matrix.axis_pos = m_verts[posIndex];
		m_Targets[i].m_Matrix.axis_pos.y += 2.5f;
	}

	m_Timer.Reset();

	m_pBVTree = new EDBVTree( &m_verts[0], (unsigned int)m_verts.size(), 10 );
}

bool AABB2LineSegment( const EDAABB &box, const vec3f& startPoint, const vec3f& endPoint )
{
	vec3f c = (box.GetMin() + box.GetMax()) * 0.5f;
	vec3f e = box.GetMax() - box.GetMin();
	vec3f d = endPoint - startPoint;
	vec3f m = startPoint + endPoint - box.GetMin() - box.GetMax();

	float adx = abs(d.x);
	if( abs(m.x) > e.x + adx ) return false;
	
	float ady = abs(d.y);
	if( abs(m.y) > e.y + ady ) return false;

	float adz = abs(d.z);
	if( abs(m.z) > e.z + adz ) return false;

	adx += FLT_EPSILON;
	ady += FLT_EPSILON;
	adz += FLT_EPSILON;

	if( abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady ) return false;
	if( abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx ) return false;
	if( abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx ) return false;

	return true;
}

bool AABB2LineSegmentTraverse( EDAABB *pBV, void *pVoid )
{
	vec3f* pPoints = (vec3f*)pVoid;

	if( AABB2LineSegment( *pBV, pPoints[0], pPoints[1] ) )
		return true;

	return false;
}

void EDApp::GetTriangles( EDTriangle **pTris, unsigned int *pTriCount, const vec3f &startPoint, const vec3f &endPoint )
{
	vec3f points[2];
	points[0] = startPoint;
	points[1] = endPoint;

	EDApp::GetInstance().m_pBVTree->Traverse( AABB2LineSegmentTraverse, points, pTris, pTriCount );
}

void EDApp::Shutdown(void)
{
	glDeleteTextures( 1, &m_Buggy.m_uiTexID );

	delete m_pBVTree;
}
	