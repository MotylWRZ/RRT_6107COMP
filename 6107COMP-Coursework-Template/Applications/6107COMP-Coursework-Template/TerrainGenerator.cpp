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

	for (int i = 0; i < tTerrainWidth / 5; i++)
	{
		for (int j = 0; j < tTerrainLength / 5; j++)
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

			float tPosX = (X * tSpacing) + tOffsetX;
			float tPosY = tHeight;
			float tPosZ = (Z * tSpacing) + tOffsetZ;

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
	TerrainGenerator::createTerrainIndexArray(tTerrainWidth / 5, tTerrainLength / 5, true, tIndices);

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

void TerrainGenerator::generateterainMeshVertices(std::vector<Vector3f>& verticesOut, int offsetX, int offsetZ, int terrainResolution, int terrainSpacing)
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

	verticesOut = tVertices;
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
	int tChunkSize = (terrain.getTerrainResoultion() - 1) / terrain.getChunksNum();

	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMesh(offsetX, offsetZ, tChunkSize, terrain.getTerrainSpacing());

	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->SetGeometry(tMesh);

	return tChunk;
}

void TerrainGenerator::generateEqualChunksFromVertices(std::vector<Vector3f>& meshVertices, std::vector<TerrainChunk>& chunksOut)
{
	// Calculate terrain Resolution from the number of vertices
	int tTerrainResolution = sqrt(static_cast<int>(meshVertices.size()));

	int terrainWidth = tTerrainResolution;
	int terrainLength = tTerrainResolution;

	int currVerWidth = 0;
	int currVerLength = 0;
	int maxVerWidth = 0;
	int maxVerLength = 0;

	int tChunkId = 0;

	static int IndexOffset = 2;

	std::vector<TerrainChunk> tChunks;

	// Divide to equal 4 chunks
	for (int OffsetX = -1; OffsetX < 1; OffsetX++)
	{
		for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
		{

			int Id = tChunkId;

			std::vector<Vector3f> vertices;

			if (Id == 0)
			{
				currVerWidth = 0;
				currVerLength = 0;

				maxVerWidth = terrainWidth / 2;
				maxVerLength = terrainLength / 2;


			}
			if (Id == 1)
			{
				currVerWidth = (terrainWidth / 2) - IndexOffset;
				currVerLength = 0;

				maxVerWidth = terrainWidth - IndexOffset;
				maxVerLength = terrainLength / 2;
			}
			if (Id == 2)
			{
				currVerWidth = (terrainWidth / 2) - IndexOffset;
				currVerLength = (terrainLength / 2) - IndexOffset;

				maxVerWidth = terrainWidth - IndexOffset;
				maxVerLength = terrainLength - IndexOffset;
			}
			if (Id == 3)
			{
				currVerWidth = 0;
				currVerLength = (terrainLength / 2) - IndexOffset;

				maxVerWidth = terrainWidth / 2;
				maxVerLength = terrainLength - IndexOffset;
			}

			tChunkId++;

			for (int i = currVerWidth; i < maxVerWidth; i++)
			{
				for (int p = currVerLength; p < maxVerLength; p++)
				{
					Vector3f tVert = meshVertices[i * terrainLength + p];

					vertices.push_back(tVert);
				}
			}
			// Create a new chunk instance
			TerrainChunk tNewChunk;
			// Add vertices for the chunk mesh
			tNewChunk.Vertices = vertices;

			// add the chunk to the out array of chunks
			chunksOut.push_back(tNewChunk);
		}
	}
}

void TerrainGenerator::generateBasicTerrainMesh(std::vector<Vector3f>& verticesOut, int resolution, int terrainSpacing, float heightScale, float majorHeightFrequency,
	float majorHeight,
	float minorHeightFrequency,
	float minorHeight, int OffsetX, int OffsetZ)
{
	float tMajorHeightFrequency = majorHeightFrequency;
	float tMajorHeight = majorHeight;

	float tMinorHeightFrequency = minorHeightFrequency;
	float tMinorHeight = minorHeight;

	float tSpacing = terrainSpacing;
	float tHeightScale = heightScale;

	int tTerrainWidth = resolution;
	int tTerrainLength = resolution;

	int tOffsetX = OffsetX * (tTerrainWidth - 1);
	int tOffsetZ = OffsetZ * (tTerrainLength - 1);

	std::vector<Vector3f> tVertices;

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
		}
	}

	verticesOut = tVertices;
}

void TerrainGenerator::generateTerrainMeshVerticesFromNoise(std::vector<Vector3f>& verticesOut, int terrainResolution, float spacing, float heightScale, FastNoise::NoiseType noiseType, int seed, float frequency, int OffsetX, int OffsetZ)
{
	FastNoise noiseGenerator;
	noiseGenerator.SetNoiseType(noiseType);
	noiseGenerator.SetSeed(seed);
	noiseGenerator.SetFrequency(frequency);

	int terrainWidth = terrainResolution;
	int terrainLength = terrainResolution;

	int tOffsetX = OffsetX * (terrainWidth - 1);
	int tOffsetZ = OffsetZ * (terrainLength - 1);

	std::vector<Vector3f> vertices;

	for (int i = 0; i < terrainWidth; i++)
	{
		for (int p = 0; p < terrainLength; p++)
		{
			int X = i + tOffsetX;
			int Z = p + tOffsetZ;

			float height = noiseGenerator.GetNoise(X, Z) * heightScale;

			vertices.push_back(Vector3f(X * spacing, height, Z * spacing));
		}
	}
	verticesOut = vertices;
}

void TerrainGenerator::generateTerrainMeshVerticesFromHeightmap(std::vector<Vector3f>& verticesOut, const char* filename, int actualTerrainWidth, int actualterrainLength, int terrainLength, int terrainWidth, int terrainSpacing, float heightScale, int OffsetX, int OffsetZ)
{
	int tOffsetX = OffsetX * (terrainWidth - 1);
	int tOffsetZ = OffsetZ * (terrainLength - 1);

	// Load the heightmap
	double* tHeightmap = TerrainGenerator::loadRawHightMap(filename, actualTerrainWidth, actualterrainLength, terrainWidth, terrainLength);

	std::vector<Vector3f> tVertices;

	for (int i = 0; i < terrainWidth; i++)
	{
		for (int p = 0; p < terrainLength; p++)
		{
			int X = i + tOffsetX;
			int Z = p + tOffsetZ;

			float height = tHeightmap[i * terrainLength + p] * heightScale;

			float tPosX = (X * terrainSpacing);
			float tPosY = height;
			float tPosZ = (Z * terrainSpacing);

			Vector3f tVert(tPosX, tPosY, tPosZ);

			tVertices.push_back(tVert);
		}
	}
	verticesOut = tVertices;
}

double* TerrainGenerator::loadRawHightMap(const char* filename, int actualTerrainWidth, int actualterrainLength, int terrainWidth, int terrainLength)
{
	// Calculate the size of the raw image data
	int imageSize = terrainLength * terrainWidth;
	int actualSize = actualTerrainWidth * actualterrainLength;

	// Allocate memory for the raw image data
	unsigned short* rawImage = new unsigned short[actualSize];

	if (!rawImage)
	{
		return nullptr;
	}

	double* heightmap = new double[imageSize];

	if (!heightmap)
	{
		return nullptr;

	}

	FILE* filePtr;

	// Open the 16 bit raw height map file for reading in binary
	int error = fopen_s(&filePtr, filename, "rb");

	if (error != 0)
	{
		return nullptr;
	}

	// Read the raw image data
	int count = fread(rawImage, sizeof(unsigned short), actualSize, filePtr);

	if (count != actualSize)
	{
		return nullptr;
	}

	// Close the file
	error = fclose(filePtr);

	if (error != 0)
	{
		return nullptr;
	}

	double maxHeight = 0;
	double minHeight = 1000000;

	int i, j, index;

	for (j = 0; j < terrainLength; j++)
	{
		for (i = 0; i < terrainWidth; i++)
		{
			int index = (terrainWidth * j) + i;
			int actualindex = (actualTerrainWidth * j * 2) + i * 2;

			//Store the height at this point in the height map array
			heightmap[index] = static_cast<double>(rawImage[actualindex]);
			if (maxHeight < heightmap[index])
				maxHeight = heightmap[index];

			if (minHeight > heightmap[index])
				minHeight = heightmap[index];

		}
	}


	double range = maxHeight - minHeight;

	for (i = 0; i < imageSize; i++)
	{
		heightmap[i] = (heightmap[i] - minHeight) / range;
	}
	// Release the biutmap image data
	delete[] rawImage;
	rawImage = 0;
	return heightmap;
}

BasicMeshPtr TerrainGenerator::generateTerrainMeshFromVertices(const std::vector<Vector3f>& meshVertices, float heightScale, float textureMappingFactor)
{
	// Calculate terrain Resolution from the number of vertices
	int tTerrainResolution = sqrt(static_cast<int>(meshVertices.size()));

	float tHeightScale = heightScale;

	int tTerrainWidth = tTerrainResolution;
	int tTerrainLength = tTerrainResolution;

	std::vector<Vector3f> tVertices;
	std::vector<Vector4f> tVertexColors;

	// Texture
	std::vector<Vector2f> tTextureCoords;
	float tTextureMappingFactor = 0.1f;

	for (int i = 0; i < tTerrainWidth; i++)
	{
		for (int j = 0; j < tTerrainLength; j++)
		{
			Vector3f tPos = meshVertices[i * tTerrainLength + j];

			float tHeight = tPos.y;

			tVertices.push_back(tPos);

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

void TerrainGenerator::extendTerrainMesh(const std::vector<Vector3f>& meshVertices, std::vector<std::vector<Vector3f>>& verticesOut)
{
	int res = sqrt(meshVertices.size());

	// cache mesh vertices locally
	std::vector<Vector3f> verts;// = meshVertices;

	for (int OffsetX = -1; OffsetX < 1; OffsetX++)
	{
		for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
		{
			//TerrainGenerator::generateTerrainMesh(OffsetX, OffsetZ, res, 3);
			//TerrainGenerator::generateterainMeshVertices(verts, OffsetX, OffsetZ, res, 3);



			///*for (int Width = 0; Width < res; Width++)
			//{
			//	for (int Length = 0; Length < res; Length++)
			//	{
			//		Vector3f& tVert = verts[Width * res + Length];
			//	}

			//}*/

			int tOffsetX = OffsetX * (res - 1);
			int tOffsetZ = OffsetZ * (res - 1);

			for (int i = 0; i < res; i++)
			{
				for (int p = 0; p < res; p++)
				{
					int X = i + tOffsetX;
					int Z = p + tOffsetZ;

					float tHeight = meshVertices[i * res + p].y;

					Vector3f tVec;// = verts[i * res + p];

				/*	if (OffsetX == -1)
					{
						X += 700;
						Z += 700;
					}*/

					tVec.x = X * 3;
					tVec.y = tHeight;
					tVec.z = Z * 3;

					verts.push_back(tVec);

				}
			}

			/*for (auto& Vertex : verts)
			{
				Vertex.x += OffsetX * res;
				Vertex.z += OffsetZ * res;
			}*/

			verticesOut.push_back(verts);
			verts.clear();
		}
	}
}

void TerrainGenerator::divideTerrainMesh(const std::vector<Vector3f>& meshVertices, std::vector<std::vector<Vector3f>>& verticesOut, int terrainWidth, int terrainLength)
{
	int currVerWidth = 0;
	int currVerLength = 0;
	int maxVerWidth = terrainWidth;
	int maxVerLength = terrainLength;

	int tChunkId = 0;
	int IndexOffset = 1;

		// Divide to equal 4 chunks
	for (int OffsetX = -1; OffsetX < 1; OffsetX++)
	{
		for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
		{
			int Id = tChunkId;
			std::vector<Vector3f> vertices;

			if (Id == 0)
			{
				currVerWidth = 0;
				currVerLength = 0;

				maxVerWidth = terrainWidth / 2;
				maxVerLength = terrainLength / 2;
			}
			if (Id == 1)
			{
				currVerWidth = (terrainWidth / 2) - IndexOffset;
				currVerLength = 0;

				maxVerWidth = terrainWidth - IndexOffset;
				maxVerLength = terrainLength / 2;
			}
			if (Id == 2)
			{
				currVerWidth = (terrainWidth / 2) - IndexOffset;
				currVerLength = (terrainLength / 2) - IndexOffset;

				maxVerWidth = terrainWidth - IndexOffset;
				maxVerLength = terrainLength - IndexOffset;
			}
			if (Id == 3)
			{
				currVerWidth = 0;
				currVerLength = (terrainLength / 2) - IndexOffset;

				maxVerWidth = terrainWidth / 2;
				maxVerLength = terrainLength - IndexOffset;
			}

			tChunkId++;

			for (int i = currVerWidth; i < maxVerWidth; i++)
			{
				for (int p = currVerLength; p < maxVerLength; p++)
				{
					Vector3f tVert = meshVertices[i * terrainLength + p];

					vertices.push_back(tVert);
				}
			}
			verticesOut.push_back(vertices);
		}
	}
}
