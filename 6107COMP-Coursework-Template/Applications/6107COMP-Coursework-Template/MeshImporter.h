#pragma once


//Hieroglyph Includes
#include "GeometryActor.h"

#include "RRTVertexDX11.h"

using namespace Glyph3;

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

class MeshImporter
{
public:
	MeshImporter() {};
	~MeshImporter() {};

	static BasicMeshPtr generateMeshOBJ(std::wstring meshOBJFile, Vector4f meshColour);
	static BasicMeshPtr	generateMeshOBJWithSurfaceVectors(std::wstring meshOBJFile, Vector4f meshColour);

	// generate a mesh with additional information used for explosion in geometry shader
	static BasicMeshPtr generateOBJMeshForGSExplosion(std::wstring MeshName, Vector4f MeshColour);
};

