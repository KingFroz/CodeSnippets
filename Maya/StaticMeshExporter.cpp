#include "StaticMeshExporter.h"

//includes needed for texture function//
#include <maya/MObjectArray.h>
#include <maya/MFnSet.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItDag.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MFnTransform.h>
////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Function	: ClearAndReset
// Takes In	: Nothing
// Returns	: MStatus
// Comments	: Clears the exported mesh vector
//////////////////////////////////////////////////////////////////////////
MStatus CStaticMeshExporter::ClearAndReset(void)
{
	//TODO: make sure you clear out any vectors
	m_exportedMeshes.clear();

	//return success
	return MStatus::kSuccess;
}

//////////////////////////////////////////////////////////////////////////
//	Function Name: ExportMeshes
//	Purpose: Exports the mesh(es) from the Maya scene
//	Parameters: bool bExportAll
//	Return: void
//	Notes: bExportAll will be true to export all meshes, false to export only selected
//////////////////////////////////////////////////////////////////////////
void CStaticMeshExporter::ExportMeshes(bool bExportAll)
{
	//your goal for this function is to iterate through the dag appropriately
	//find the MDagPath for a mesh, Create an MFnMesh, and then export the mesh.
	//Creates dag iter, depth first trav
	

	if (bExportAll)
	{
		MItDag dagIt(MItDag::kDepthFirst, MFn::kMesh);
		for (; !dagIt.isDone(); dagIt.next())
		{
			//Get path through currMesh
			MDagPath currPath;
			dagIt.getPath(currPath);

			//Attach a mesh func set to the obj at the end of the path
			MFnMesh currMesh(currPath);

			//Make sure the mesh isn't intermediate
			if (currMesh.isIntermediateObject())
			{
				continue;
			}

			CMesh c_Mesh;
			//Begin exporting mesh data
			ExportMesh(currMesh, c_Mesh);
			m_exportedMeshes.push_back(c_Mesh);
		}
	}
	else
	{
		MSelectionList list;
		MGlobal::getActiveSelectionList(list);
		MDagPath currPath;
		int mslSize = list.length();
		int x = 0;

		unsigned int i = 0;
		for (; i < mslSize; ++i)
		{
			list.getDagPath(i, currPath);

			if (MFn::kTransform == currPath.apiType())
			{
				MFnTransform pos(currPath);
				int childNum = pos.childCount();

				for (; x < childNum; ++x)
				{
					MObject kid = pos.child(x);

					if (MFn::kMesh == kid.apiType())
					{
						MFnDagNode node;
						node.setObject(kid);

						MDagPath dagP;
						node.getPath(dagP);

						MFnMesh currM(dagP);
						if (currM.isIntermediateObject())
							continue;

						CMesh c_Mesh;
						ExportMesh(currM, c_Mesh);
						m_exportedMeshes.push_back(c_Mesh);
					}
				}
			}

		}
	}
}
//////////////////////////////////////////////////////////////////////////
//	Function Name: ExportMesh
//	Purpose: to export a particular mesh
//	Parameters: MFnMesh & currMesh, CMesh<VertType> & outMesh
//	Return: void
//	Notes: Exports all the appropriate mesh data (ex. unique verts, tris, mesh name, texture names)
//			for the MFnMesh passed in and stores it in the mesh structure passed in
//////////////////////////////////////////////////////////////////////////
void CStaticMeshExporter::ExportMesh(MFnMesh & currMesh, CMesh & outMesh)
{
	//this function will take care of exporting the actual data needed to draw a 3d mesh
	//such as verts, normals, texture coords, and the triangles for indexed geometry, as well
	//as the mesh name, and texture names for this mesh.
	unsigned int texturenames = 0;

	MStringArray string;
	MPointArray points;
	MFloatVectorArray normals;
	MFloatArray U, V;

	tTriangle tri;
	tVertex test;

	outMesh.m_strName = currMesh.name().asChar();
	getTextureNamesFromMesh(currMesh, string, texturenames);

	for (unsigned int x = 0; x < texturenames; x++)
	{
		outMesh.m_vTextureNames.push_back(string[x].asChar());
	}

	currMesh.getPoints(points);
	currMesh.getNormals(normals);
	currMesh.getUVs(U, V);

	MItMeshPolygon iter(currMesh.dagPath());
	for (; !iter.isDone(); iter.next())
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			int uvI;
			int vertI = iter.vertexIndex(i);
			int normI = iter.normalIndex(i);
			iter.getUVIndex(i, uvI);

			test.fX = points[vertI].x;
			test.fY = points[vertI].y;
			test.fZ = points[vertI].z;

			test.fNX = normals[normI].x;
			test.fNY = normals[normI].y;
			test.fNZ = normals[normI].z;

			test.fU = U[uvI];
			test.fV = V[uvI];

			unsigned int v = 0;
			for (; v < outMesh.m_vUniqueVerts.size(); v++)
			{
				if (outMesh.m_vUniqueVerts[v] == test)
				{
					tri.uIndices[i] = v;
					break;
				}
			}

			if (outMesh.m_vUniqueVerts.size() == v)
			{
				outMesh.m_vUniqueVerts.push_back(test);
				tri.uIndices[i] = v;
			}
		}
		outMesh.m_vTriangles.push_back(tri);
	}
}
//////////////////////////////////////////////////////////////////////////
//	Function Name: WriteMesh_Binary
//	Purpose: write out the mesh in a binary format
//	Parameters: const CMesh<VertType> &
//	Return: void
//	Notes: Writes out a mesh to a binary file "the mesh's name".mesh
//////////////////////////////////////////////////////////////////////////
void CStaticMeshExporter::WriteMesh_Binary( const CMesh& writeOutMesh )
{
	// When opening the binary file, be sure to open it inside of the same folder
	// specified when exporting from Maya (m_exportDirectory).
	
	//the binary format
	/*
		--length of the mesh name including null terminator (unsigned int)
		--the mesh name including null terminator (char *)
		--the number of textures (unsigned int)
			--for each texture
			--the length of the texture name including null terminator (unsigned int)
			--the texture name (char *)		
		--the number of verts (unsigned int)
			--for each vert
			--the position (3 floats)
			--the normal (3 floats)
			--the texture coordinates (2 floats)
		--the number of triangles (unsigned int)
			--for each triangle
			--the three vertex indices (3 unsigned)

	*/
	fstream fout;
	fout.open(m_exportDirectory + writeOutMesh.m_strName + ".mesh", ios_base::binary | ios_base::out);
	if (fout.is_open())
	{
		unsigned int len;
		unsigned int i;

		len = strlen(writeOutMesh.m_strName.c_str()) + 1;
		fout.write((char*)&len, sizeof(len));
		fout.write(writeOutMesh.m_strName.c_str(), len);

		len = writeOutMesh.m_vTextureNames.size();
		fout.write((char*)&len, sizeof(len));

		i = 0;
		for (; i < writeOutMesh.m_vTextureNames.size(); i++)
		{
			len = (unsigned int)writeOutMesh.m_vTextureNames[i].length() + 1;
			fout.write((char*)&len, sizeof(len));
			fout.write(writeOutMesh.m_vTextureNames[i].c_str(), len);
		}

		len = writeOutMesh.m_vUniqueVerts.size();
		fout.write((char*)&len, sizeof(len));

		i = 0;
		for (; i < writeOutMesh.m_vUniqueVerts.size(); ++i)
		{
			//Verts
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fX), sizeof(float));
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fY), sizeof(float));
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fZ), sizeof(float));
			//Norm
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fNX), sizeof(float));
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fNY), sizeof(float));
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fNZ), sizeof(float));
			//UVs
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fU), sizeof(float));
			fout.write((char*)(&writeOutMesh.m_vUniqueVerts[i].fV), sizeof(float));
		}
		len = writeOutMesh.m_vTriangles.size();
		fout.write((char*)&len, sizeof(len));

		i = 0;
		for (; i < writeOutMesh.m_vTriangles.size(); ++i)
		{
			//Triangles
			fout.write((char*)(&writeOutMesh.m_vTriangles[i].uIndices[0]), sizeof(unsigned int));
			fout.write((char*)(&writeOutMesh.m_vTriangles[i].uIndices[1]), sizeof(unsigned int));
			fout.write((char*)(&writeOutMesh.m_vTriangles[i].uIndices[2]), sizeof(unsigned int));
		}

		fout.close();
	}
}
//////////////////////////////////////////////////////////////////////////
//	Function Name: WriteOutMeshes
//	Purpose: Calls the appropriate write functions for exporting meshes to a file
//	Parameters: bool bWriteBinary
//	Return: void
//	Notes: bWriteBinary will be true if we want to write out the binary files along with the xml
//////////////////////////////////////////////////////////////////////////
void CStaticMeshExporter::WriteOutMeshes( bool bWriteBinary )
{
	for(size_t i = 0; i < m_exportedMeshes.size(); ++i)
	{
		WriteMesh_XML( m_exportedMeshes[i] );

		if(bWriteBinary)
			WriteMesh_Binary( m_exportedMeshes[i] );
	}
}
//////////////////////////////////////////////////////////////////////////
//	Function Name: WriteMesh_XML
//	Purpose: write out the mesh in an xml format
//	Parameters: const CMesh<VertType> &
//	Return: void
//	Notes: Writes out a mesh to xml creating a file "the mesh's name".xml
//////////////////////////////////////////////////////////////////////////
void CStaticMeshExporter::WriteMesh_XML( const CMesh& writeOutMesh )
{
	//must be the first thing written out to xml file, makes xml file legal
	char XMLProlog[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

	char buffer[256];

	std::string fileName = m_exportDirectory + writeOutMesh.m_strName.c_str();
	fileName += ".xml";

	std::ofstream xmlOut;
	//open xml file
	xmlOut.open( fileName.c_str(), std::ios_base::trunc);

	//write out prolog
	xmlOut << XMLProlog;

	//write out the root element <Model>
	xmlOut << "<Model>\n";
	//the mesh element with name attribute <Mesh name="">
	xmlOut << "<Mesh name=\"" << writeOutMesh.m_strName.c_str() << "\">\n";
	//the trackback <TrackBack>
	xmlOut << "<TrackBack>" << "Insert Trackback here" << "</TrackBack>\n";
	//VertCount
	xmlOut << "<VertCount>" << writeOutMesh.m_vUniqueVerts.size() << "</VertCount>\n";
	//PolyCount
	xmlOut << "<PolyCount>" << writeOutMesh.m_vTriangles.size() << "</PolyCount>\n";
	
	//the textures
	if(writeOutMesh.m_vTextureNames.size())
	{
		for(size_t currTex = 0; currTex < writeOutMesh.m_vTextureNames.size(); ++currTex)
		{
			std::string tempTex = writeOutMesh.m_vTextureNames[currTex].c_str();
			size_t index = tempTex.find_last_of('/');
			
			if(index > 0)
				tempTex.erase(0, index + 1);

			xmlOut << "<Texture>" << tempTex.c_str() << "</Texture>\n";
		}
		

	}
	else
	{
		xmlOut << "<Texture/>\n";
	}

	//write out the vertList
	xmlOut << "<vertList>\n";

		//loop through verts <Vert id=""><x></x><y></y><z></z><Nx></Nx><Ny></Ny><Nz></Nz><Tu></Tu><Tv></Tv></Vert>
		for(size_t currVert = 0; currVert < writeOutMesh.m_vUniqueVerts.size(); ++currVert)
		{
			xmlOut << "<Vert id=\"" << currVert << "\">\n";
			memset(buffer, 0, 256);

			sprintf_s(buffer, "<x>%f</x><y>%f</y><z>%f</z><Nx>%f</Nx><Ny>%f</Ny><Nz>%f</Nz><Tu>%f</Tu><Tv>%f</Tv>\n",
						writeOutMesh.m_vUniqueVerts[currVert].fX, writeOutMesh.m_vUniqueVerts[currVert].fY, writeOutMesh.m_vUniqueVerts[currVert].fZ,
						writeOutMesh.m_vUniqueVerts[currVert].fNX, writeOutMesh.m_vUniqueVerts[currVert].fNY, writeOutMesh.m_vUniqueVerts[currVert].fNZ, 
						writeOutMesh.m_vUniqueVerts[currVert].fU, writeOutMesh.m_vUniqueVerts[currVert].fV);

			xmlOut << buffer;

			xmlOut << "</Vert>";
		}

	//close the vertList
	xmlOut << "</vertList>\n";

	//open polyList
	xmlOut << "<polyList>\n";
	
	//loop through triangles and write out indices <Polygon><i1></i1><i2></i2><i3></i3></Polygon>
	for(size_t currTri = 0; currTri < writeOutMesh.m_vTriangles.size(); ++currTri)
	{
		xmlOut << "<Polygon>\n";
		
		xmlOut << "<i1>" << writeOutMesh.m_vTriangles[currTri].uIndices[0] << "</i1><i2>" << writeOutMesh.m_vTriangles[currTri].uIndices[1] << "</i2><i3>" << writeOutMesh.m_vTriangles[currTri].uIndices[2] << "</i3>\n";

		xmlOut << "</Polygon>\n";
	}

	//close polyList
	xmlOut << "</polyList>\n";

	//close the mesh element
	xmlOut << "</Mesh>\n";
	//close the root element <Model>
	xmlOut << "</Model>\n";

	xmlOut.close();

}

//////////////////////////////////////////////////////////////////////////
//	Function Name: getTextureNamesFromMesh
//	Purpose: Gets a list of textures associated with a mesh
//	Parameters: MFnMesh & mesh, MString Array & names, unsigned int & count
//	Return: void
//	Notes: This function has two output parameters:
//			names -- this stores the texture names
//			count -- this will contain the number of (elements) in the names variable.
//////////////////////////////////////////////////////////////////////////
void CStaticMeshExporter::getTextureNamesFromMesh( MFnMesh &mesh, MStringArray &names, unsigned &count )
{
	// local vars
	MStatus         status;
	MObjectArray    meshSets;
	MObjectArray    meshComps;
	unsigned        meshSetCount = 0;

	// get sets & components from mesh
	mesh.getConnectedSetsAndMembers( 0, meshSets, meshComps, true );
	meshSetCount = meshSets.length();
	if ( meshSetCount > 1 ) meshSetCount--;

	// init in-vars
	names.clear();
	count = 0;

	// get info from sets
	for ( unsigned i = 0; i < meshSetCount; i++ )
	{
		MObject set  = meshSets[i];
		MObject comp = meshComps[i];
		MFnSet  fnSet( set );

		// make sure we have a polygon set
		MItMeshPolygon meshSetIterator( mesh.dagPath(), comp, &status );
		if ( !status ) continue;

		//
		MFnDependencyNode   fnNode( set ); 
		MPlugArray          connectedPlugs;            

		// get shader plug
		MPlug shaderPlug = fnNode.findPlug( "surfaceShader", &status );
		//_MAYA_ASSERT( status, MString( "Failed to get shader plug for: " + mesh.partialPathName() ) );
		if ( !status ) continue;
		if ( shaderPlug.isNull() ) continue;

		// get shader node
		shaderPlug.connectedTo( connectedPlugs, true, false, &status );
		//_MAYA_ASSERT( status, MString( "Failed to get shader node for: " + mesh.partialPathName() ) );
		if ( !status ) continue;
		if ( connectedPlugs.length() != 1 ) continue;

		// get color plug
		MPlug colorPlug = MFnDependencyNode( connectedPlugs[0].node() ).findPlug("color", &status);
		//_MAYA_ASSERT( status, MString( "Failed to get color plug for: " + mesh.partialPathName() ) );
		if ( !status ) continue;

		// get iterator for traversing our color plug
		MItDependencyGraph itDG( colorPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &status );
		if ( !status ) continue;

		// disable automatic pruning so that we can locate a specific plug 
		itDG.disablePruningOnFilter();
		if ( itDG.isDone() ) continue;

		// get file plug
		MObject textureNode = itDG.thisNode();
		MPlug   filePlug    = MFnDependencyNode( textureNode ).findPlug( "fileTextureName" );
		MString textureName( "" );

		// log texture name
		if ( filePlug.getValue( textureName ) && textureName != "" )
		{
			names.append( textureName );            
			count++;
		}		
	}
}

//////////////////////////////////////////////////////////////////////////
//	Function Name: SetExportDirectory
//	Purpose: Sets the m_exportDirectory variable to the given string
//	Parameters: string path
//	Return: void
//////////////////////////////////////////////////////////////////////////	
void CStaticMeshExporter::SetExportDirectory( const char * path )
{
	m_exportDirectory = path;
}