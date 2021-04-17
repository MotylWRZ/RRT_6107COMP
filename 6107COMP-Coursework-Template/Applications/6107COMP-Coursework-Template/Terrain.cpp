#include "Terrain.h"

#include "TerrainGenerator.h"
#include "MaterialGenerator.h"

Terrain::Terrain(int terrainResolution, int terrainSpacing, float heightScale, Scene* pScene)
	:m_terrainResolution(terrainResolution)
	,m_terrainSpacing(terrainSpacing)
	,m_heightScale(heightScale)
	,m_pScene(pScene)
{
	//this->generateTerrainChunks();
}

Terrain::~Terrain()
{
}

void Terrain::setMaterial(MaterialPtr material)
{
	for (auto& Chunk : this->m_chunks)
	{
		Chunk->GetBody()->SetMaterial(material);
	}
}

void Terrain::addTerrainIntoScene(Scene* pScene)
{
	if (pScene)
	{
		for (auto& Chunk : this->m_chunks)
		{
				pScene->AddActor(Chunk);
		}
	}
}

void Terrain::updateLighting(RendererDX11* pRenderer, const std::vector<LightBasePtr>& lights)
{
	if (!pRenderer)
	{
		return;
	}

	for (auto& Chunk : this->m_chunks)
	{
		if (Chunk)
		{
			MaterialPtr tMaterial = Chunk->GetBody()->GetMaterial();

			if (tMaterial)
			{
				MaterialGenerator::updateMaterialLight(*pRenderer, tMaterial, lights);
			}
		}
	}
}

void Terrain::generateBasicTerrain(int resolution, int terrainSpacing, float heightScale, float majorHeightFrequency,
	float majorHeight,
	float minorHeightFrequency,
	float minorHeight)
{
	std::vector<Vector3f> vertices;
	TerrainGenerator::generateBasicTerrainMesh(vertices, resolution, terrainSpacing, heightScale, majorHeightFrequency, majorHeight, minorHeightFrequency, minorHeight);
	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(vertices, this->m_heightScale, 0.1f);
	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->GetBody()->SetGeometry(tMesh);

	this->m_chunks.push_back(tChunk);
}

void Terrain::generateTerrainFromNoise(int terrainResolution, float spacing, float heightScale, FastNoise::NoiseType noiseType, int seed, float frequency)
{
	std::vector<Vector3f> vertices;
	TerrainGenerator::generateTerrainMeshVerticesFromNoise(vertices, terrainResolution, spacing, heightScale, noiseType, seed, frequency);
	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(vertices, this->m_heightScale, 0.1f);
	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->GetBody()->SetGeometry(tMesh);

	this->m_chunks.push_back(tChunk);
}

void Terrain::generateTerrainFromHeightmap(const char* filename, int actualTerrainWidth, int actualterrainLength, int terrainWidth, int terrainLength, int terrainSpacing, float heightScale)
{
	std::vector<Vector3f> vertices;
	TerrainGenerator::generateTerrainMeshVerticesFromHeightmap(vertices, filename, actualTerrainWidth, actualterrainLength, terrainWidth, terrainLength, terrainSpacing, heightScale);
	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(vertices, this->m_heightScale, 0.1f);
	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->GetBody()->SetGeometry(tMesh);

	this->m_chunks.push_back(tChunk);
}

void Terrain::initializeBasicChunkedTerrain(bool extend, int resolution, int terrainSpacing, float heightScale, float majorHeightFrequency,
	float majorHeight,
	float minorHeightFrequency,
	float minorHeight)
{
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	if (extend)
	{
		std::vector<std::vector<Vector3f>> tChunkVertices;

		for (int OffsetX = -1; OffsetX < 1; OffsetX++)
		{
			for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
			{
				TerrainGenerator::generateBasicTerrainMesh(vertices, resolution, terrainSpacing, heightScale, majorHeightFrequency, majorHeight, minorHeightFrequency, minorHeight, OffsetX, OffsetZ);

				tChunkVertices.push_back(vertices);
				vertices.clear();
			}
		}

		// Instantiate chunks
		for (auto& VertsArray : tChunkVertices)
		{
			TerrainChunk* tChunk = new TerrainChunk();

			//  generate mesh from vertices
			BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(VertsArray, heightScale, 0.1f);

			tChunk->GetBody()->SetGeometry(tMesh);

			this->m_chunks.push_back(tChunk);
		}
	}
}

void Terrain::generateChunkedTerrainFromNoise(bool extend, int terrainResolution, float spacing, float heightScale, FastNoise::NoiseType noiseType, int seed, float frequency)
{
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	if (extend)
	{
		std::vector<std::vector<Vector3f>> tChunkVertices;

		for (int OffsetX = -1; OffsetX < 1; OffsetX++)
		{
			for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
			{
				TerrainGenerator::generateTerrainMeshVerticesFromNoise(vertices, terrainResolution, spacing, heightScale, noiseType, seed, frequency, OffsetX, OffsetZ);

				tChunkVertices.push_back(vertices);
				vertices.clear();
			}
		}

		// Instantiate chunks
		for (auto& VertsArray : tChunkVertices)
		{
			TerrainChunk* tChunk = new TerrainChunk();

			//  generate mesh from vertices
			BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(VertsArray, heightScale, 0.1f);

			tChunk->GetBody()->SetGeometry(tMesh);

			this->m_chunks.push_back(tChunk);
		}
	}
}

void Terrain::generateTerrainFromHeightmap(bool extend)
{
}

void Terrain::deleteTerrainMesh()
{
	for (int i = 0; i < static_cast<int>(this->m_chunks.size()); i++)
	{
		this->m_pScene->RemoveActor(this->m_chunks[i]);
		delete this->m_chunks[i];
	}

	this->m_chunks.clear();
}

void Terrain::generateTerrainChunks()
{
	/*int tLandscapeOffsetX;
	int tLandscapeOffsetZ;

	for (int LandscapeOffsetX = -1; LandscapeOffsetX < this->m_chunksNum; LandscapeOffsetX++)
	{
		for (int LandscapeOffsetZ = -1; LandscapeOffsetZ < this->m_chunksNum; LandscapeOffsetZ++)
		{
			TerrainChunk* tTerrainChunk = TerrainGenerator::generateTerrainChunk(*this, LandscapeOffsetX, LandscapeOffsetZ);

			tTerrainChunk->GetBody()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
			tTerrainChunk->GetBody()->Scale() = Vector3f(1.0f, 1.0f, 1.0f);

			this->m_chunks.push_back(tTerrainChunk);
		}
	}*/
}
