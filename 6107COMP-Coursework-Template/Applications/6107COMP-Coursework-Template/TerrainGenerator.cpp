#include "TerrainGenerator.h"

// API Header Files
#include "GeometryGenerator.h"

//Include the DirectX Rendering Components
#include "MaterialGeneratorDX11.h"

#include "Terrain.h"
#include "TerrainChunk.h"

TerrainGenerator::TerrainGenerator()
{
}

TerrainGenerator::~TerrainGenerator()
{
}

BasicMeshPtr TerrainGenerator::generateTerrainMesh(int offsetX, int offsetZ, int terrainResolution, int terrainSpacing)
{

	float tMajorHeightFrequency = 5.0f;
	float tMajorHeight = 1.0f;

	float tMinorHeightFrequency = 75.0f;
	float tMinorHeight = 0.25f;

	float tSpacing = terrainSpacing;
	float tHeightScale = 12.0f;

	int tTerrainWidth = terrainResolution;
	int tTerrainLength = terrainResolution;

	int tOffsetX = offsetX * (terrainResolution - 1);
	int tOffsetZ = offsetZ * (terrainResolution - 1);

	std::vector<Vector3f> tVertices;
	std::vector<Vector4f> tVertexColors;

	//this->m_vTextureBoundary = Vector4f(0.5f, 0.0f, 0.0f, 0.0f) * tHeightScale;

	// Texture
	std::vector<Vector2f> tTextureCoords;
	float tTextureMappingFactor = 0.1f;

	for (int i = 0; i < tTerrainWidth; i++)
	{
		for (int j = 0; j < tTerrainLength; j++)
		{
			int X = i + tOffsetX;
			int Z = j + tOffsetZ;

			float tMajorPeriodicHeightX = sin(static_cast<float>(X) / tTerrainWidth * tMajorHeightFrequency * GLYPH_PI) * tMajorHeight;
			float tMajorPeriodicHeightZ = cos(static_cast<float>(Z) / tTerrainWidth * tMajorHeightFrequency * GLYPH_PI) * tMajorHeight;
			float tMajorPeriodicHeight = tMajorPeriodicHeightX * tMajorPeriodicHeightZ;

			float tMinorPeriodicHeightX = sin(static_cast<float>(X) / tTerrainWidth * tMinorHeightFrequency * GLYPH_PI) * tMinorHeight;
			float tMinorPeriodicHeightZ = cos(static_cast<float>(Z) / tTerrainWidth * tMinorHeightFrequency * GLYPH_PI) * tMinorHeight;
			float tMinorPeriodicHeight = tMinorPeriodicHeightX * tMinorPeriodicHeightZ;

			float tHeight = (tMajorPeriodicHeight + tMinorPeriodicHeight) * tHeightScale;

			float tPosX = (X * tSpacing);
			float tPosY = tHeight;
			float tPosZ = (Z * tSpacing);

			tVertices.push_back(Vector3f(tPosX, tPosY, tPosZ));

			float tShade = (tHeight / tHeightScale + 1) / 2.0f;

			if (tShade < 0)
			{
				tShade = 0;
			}
			else if (tShade > 1)
			{
				tShade = 1;
			}

			tVertexColors.push_back(Vector4f(tShade, 1 - tShade, tShade / 2, 1));

			// Calculate Texture Coordinate and store them in the array
			tTextureCoords.push_back(Vector2f(i * tTextureMappingFactor, j * tTextureMappingFactor));
		}
	}

	std::vector<int> tIndices;
	TerrainGenerator::createTerrainIndexArray(tTerrainWidth, tTerrainLength, true, tIndices);

	int tNumberOfIndices = tIndices.size();
	int tNumberOfTriangles = tNumberOfIndices / 3;

	BasicMeshPtr tTerrainMesh = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	tTerrainMesh->SetLayoutElements(RRTVertexDX11::GetElementCount(), RRTVertexDX11::Elements);
	tTerrainMesh->SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tTerrainMesh->SetMaxVertexCount(tNumberOfIndices);

	RRTVertexDX11::Vertex tVertex;

	for (int i = 0; i < tNumberOfIndices; i++)
	{
		tVertex.position = tVertices[tIndices[i]];
		tVertex.color = tVertexColors[tIndices[i]];
		tVertex.texcoords = tTextureCoords[tIndices[i]];

		tTerrainMesh->AddVertex(tVertex);
	}

	return tTerrainMesh;
}

void TerrainGenerator::createTerrainIndexArray(int terrainWidth, int terrainLength,
	bool winding, std::vector<int>& indices)
{
	indices.clear();

	if (terrainWidth >= 2 && terrainLength >= 2)
	{
		for (int IdxX = 0; IdxX < terrainWidth - 1; IdxX++)
		{
			for (int IdxY = 0; IdxY < terrainLength - 1; IdxY++)
			{
				const int I0 = (IdxX + 0) * terrainLength + (IdxY + 0);
				const int I1 = (IdxX + 1) * terrainLength + (IdxY + 0);
				const int I2 = (IdxX + 1) * terrainLength + (IdxY + 1);
				const int I3 = (IdxX + 0) * terrainLength + (IdxY + 1);

				if (winding)
				{
					GeometryGenerator::convertQuadToTriangles(indices, I0, I1, I2, I3);
				}
				else
				{
					GeometryGenerator::convertQuadToTriangles(indices, I0, I3, I2, I1);
				}
			}
		}
	}
}

void TerrainGenerator::createSegmentedTerrainActor(std::vector<Actor*>& terrainSegments)
{
	int tLandscapeResolution = 255;
	int tLandscapeSpacing = 10;
	int tLandscapeOffsetX;
	int tLandscapeOffsetZ;

	std::vector<Actor*> tTerrainSegments;
	Actor* tTerrainSegmentActor;
	//this->m_LandscapeMaterial = createTerrainMultiTextureMaterial(std::wstring(L"SnowScuffedGround.tif"), std::wstring(L"TerrainGrass.tif"));

	for (int LandscapeOffsetX = -1; LandscapeOffsetX < 1; LandscapeOffsetX++)
	{
		for (int LandscapeOffsetZ = -1; LandscapeOffsetZ < 1; LandscapeOffsetZ++)
		{
			BasicMeshPtr tLandscapeMesh = TerrainGenerator::generateTerrainMesh(LandscapeOffsetX, LandscapeOffsetZ,
				tLandscapeResolution, tLandscapeSpacing);

			tTerrainSegmentActor = new Actor();
			tTerrainSegmentActor->GetBody()->SetGeometry(tLandscapeMesh);

			//tLandscapeSegmentActor->GetBody()->SetMaterial(this->m_LandscapeMaterial);
			tTerrainSegmentActor->GetBody()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
			tTerrainSegmentActor->GetBody()->Scale() = Vector3f(1.0f, 1.0f, 1.0f);

			tTerrainSegments.push_back(tTerrainSegmentActor);
		}
	}

	terrainSegments = tTerrainSegments;
}

Actor* TerrainGenerator::createTerrainActor(int offsetX, int offsetZ, int terrainResolution, int terrainSpacing)
{
	BasicMeshPtr tTerrainMesh = TerrainGenerator::generateTerrainMesh(offsetX, offsetZ, terrainResolution, terrainSpacing);
	Actor* tTerrainActor = new Actor();

	tTerrainActor->GetBody()->SetGeometry(tTerrainMesh);
	tTerrainActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	tTerrainActor->GetNode()->Scale() = Vector3f(1, 1, 1);

	return tTerrainActor;
}

TerrainChunk* TerrainGenerator::generateTerrainChunk(const Terrain& terrain, int offsetX, int offsetZ)
{
	int tChunkSize = terrain.getTerrainResoultion();

	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMesh(offsetX, offsetZ, tChunkSize, terrain.getTerrainSpacing());

	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->GetBody()->SetGeometry(tMesh);

	return tChunk;
}
