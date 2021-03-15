#include "GeometryGenerator.h"


void GeometryGenerator::convertQuadToTriangles(std::vector<int>& Indices, int Vert0, int Vert1, int Vert2, int Vert3)
{
	Indices.push_back(Vert3);
	Indices.push_back(Vert1);
	Indices.push_back(Vert0);

	Indices.push_back(Vert3);
	Indices.push_back(Vert2);
	Indices.push_back(Vert1);
}

BasicMeshPtr GeometryGenerator::generateTriangle()
{
	float alpha = 1.0f;

	DirectX::XMFLOAT4 Red = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, alpha);
	DirectX::XMFLOAT4 Green = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, alpha);
	DirectX::XMFLOAT4 Blue = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, alpha);
	DirectX::XMFLOAT4 Yellow = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, alpha);

	Vertex Vertices[] =
	{
		{DirectX::XMFLOAT3(-1.0f, 0.0f, -1.0f), Red}, 	//0 (Bottom Left)
		{DirectX::XMFLOAT3(1.0f, 0.0f, -1.0f), Green}, //1 (Bottom Right)
		{DirectX::XMFLOAT3(0.0f, 1.0f, -1.0f), Yellow},  //2 (Top Left)
	};

	int tVertexSize = 4;

	// Populate Index Array
	UINT  Indices[] =
	{
		2,1,0, // Triangle
	};
	int tIndexSize = 3;

	auto tTriangleMesh = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	tTriangleMesh->SetLayoutElements(RRTVertexDX11::GetElementCount(), RRTVertexDX11::Elements);
	tTriangleMesh->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tTriangleMesh->SetMaxVertexCount(tIndexSize);

	RRTVertexDX11::Vertex tVertex;
	for (size_t i = 0; i < tIndexSize; i++)
	{
		UINT tIndex = Indices[i];
		tVertex.position = Vector3f(Vertices[tIndex].Pos.x,
			Vertices[tIndex].Pos.y,
			Vertices[tIndex].Pos.z);

		tVertex.color = Vector4f(Vertices[tIndex].Color.x,
			Vertices[tIndex].Color.y,
			Vertices[tIndex].Color.z,
			Vertices[tIndex].Color.w);

		tTriangleMesh->AddVertex(tVertex);
	}
	return tTriangleMesh;
}

BasicMeshPtr GeometryGenerator::generateRectangle()
{
	float alpha = 1.0f;

	DirectX::XMFLOAT4 Red = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, alpha);
	DirectX::XMFLOAT4 Green = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, alpha);
	DirectX::XMFLOAT4 Blue = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, alpha);
	DirectX::XMFLOAT4 Yellow = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, alpha);

	Vertex Vertices[] =
	{
		{DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), Red}, 	//0 (Bottom Left)
		{DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), Green}, //1 (Bottom Right)
		{DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), Yellow},  //2 (Top Left)
		{DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), Yellow},	//3 (Top Right)
	};

	int tVertexSize = 4;

	// Populate Index Array
	UINT  Indices[] =
	{
		2,1,0, // Front faces (Rectangle)
		3,1,2,
	};
	int tIndexSize = 6;

	auto tRectangleMesh = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	tRectangleMesh->SetLayoutElements(RRTVertexDX11::GetElementCount(), RRTVertexDX11::Elements);
	tRectangleMesh->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tRectangleMesh->SetMaxVertexCount(tIndexSize);

	RRTVertexDX11::Vertex tVertex;
	for (size_t i = 0; i < tIndexSize; i++)
	{
		UINT tIndex = Indices[i];
		tVertex.position = Vector3f(Vertices[tIndex].Pos.x,
			Vertices[tIndex].Pos.y,
			Vertices[tIndex].Pos.z);

		tVertex.color = Vector4f(Vertices[tIndex].Color.x,
			Vertices[tIndex].Color.y,
			Vertices[tIndex].Color.z,
			Vertices[tIndex].Color.w);

		tRectangleMesh->AddVertex(tVertex);
	}
	return tRectangleMesh;
}

BasicMeshPtr GeometryGenerator::generateCube()
{
	float alpha = 1.0f;

	DirectX::XMFLOAT4 Red = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, alpha);
	DirectX::XMFLOAT4 Green = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, alpha);
	DirectX::XMFLOAT4 Blue = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, alpha);
	DirectX::XMFLOAT4 Yellow = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, alpha);

	Vertex Vertices[] =
	{
		{DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), Red}, 	//0 (Bottom Left)
		{DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), Green}, //1 (Bottom Right)
		{DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), Yellow},  //2 (Top Left)
		{DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), Yellow},	//3 (Top Right)
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), Red}, 	//4 (Bottom Left)
		{DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), Green}, //5 (Bottom Right)
		{DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), Yellow},  //6 (Top Left)
		{DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), Yellow}	//7 (Top Right)
	};

	int tVertexSize = 8;

	// Populate Index Array
	UINT  Indices[] =
	{
		2,1,0, // Front faces (Rectangle)
		3,1,2,

		7,4,5, //Back faces (Rectangle)
		6,4,7,

		3,5,1, // Right Side Faces (Rectangle)
		7,5,3,

		6,0,4, // Left Side Faces (Rectangle)
		2,0,6,

		6,3,2, // Top Faces (Rectangle)
		7,3,6,

		0,5,4, // Bottom Faces (Rectangle)
		1,5,0
	};
	int tIndexSize = 36;

	auto tCubeMesh = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	tCubeMesh->SetLayoutElements(RRTVertexDX11::GetElementCount(), RRTVertexDX11::Elements);
	tCubeMesh->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tCubeMesh->SetMaxVertexCount(tIndexSize);

	RRTVertexDX11::Vertex tVertex;
	for (size_t i = 0; i < tIndexSize; i++)
	{
		UINT tIndex = Indices[i];
		tVertex.position = Vector3f(Vertices[tIndex].Pos.x,
			Vertices[tIndex].Pos.y,
			Vertices[tIndex].Pos.z);

		tVertex.color = Vector4f(Vertices[tIndex].Color.x,
			Vertices[tIndex].Color.y,
			Vertices[tIndex].Color.z,
			Vertices[tIndex].Color.w);

		tCubeMesh->AddVertex(tVertex);
	}
	return tCubeMesh;
}
