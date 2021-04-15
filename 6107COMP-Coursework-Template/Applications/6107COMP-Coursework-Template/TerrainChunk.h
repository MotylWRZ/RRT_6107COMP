#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

#include <unordered_map>

#include "RRTVertexDX11.h"

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

class TerrainChunk : public Actor
{
public:
	TerrainChunk();
	~TerrainChunk();

	BasicMeshPtr ChunkMesh;
	std::vector<Vector3f> Vertices;
	std::vector<Vector4f> Colors;
	std::vector<Vector2f> TextureCoords;
};

