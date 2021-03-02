#pragma once

#include "DirectXMath.h"

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

using namespace Glyph3;

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<BasicVertexDX11::Vertex>> BasicMeshPtr;


struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

class GeometryGenerator
{
public:

	static void convertQuadToTriangles(std::vector<int>& Indices,
		int Vert0, int Vert1, int Vert2, int Vert3);

	static BasicMeshPtr generateTriangle();
	static BasicMeshPtr generateRectangle();
	static BasicMeshPtr generateCube();
};

