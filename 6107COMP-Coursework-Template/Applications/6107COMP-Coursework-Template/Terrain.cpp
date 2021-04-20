#include "Terrain.h"

#include "TerrainGenerator.h"
#include "MaterialGenerator.h"

Terrain::Terrain(int terrainResolution, int terrainSpacing, float heightScale, Scene* pScene)
	:m_terrainResolution(terrainResolution)
	,m_terrainSpacing(terrainSpacing)
	,m_heightScale(heightScale)
	,m_pScene(pScene)
{
}

Terrain::~Terrain()
{
}

void Terrain::setMaterial(MaterialPtr material)
{
	for (auto& Chunk : this->m_chunks)
	{
		Chunk->SetMaterial(material);
	}
}

void Terrain::addTerrainIntoScene(Scene* pScene)
{
	//if (pScene)
	//{
	//	for (auto& Chunk : this->m_chunks)
	//	{
	//			pScene->AddActor(Chunk);
	//	}
	//}
}

void Terrain::removeTerrainFromScene(Scene* pScene)
{
	//if (pScene)
	//{
	//	for (auto& Chunk : this->m_chunks)
	//	{
	//		pScene->RemoveActor(Chunk);
	//	}
	//}
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
			MaterialPtr tMaterial = Chunk->GetMaterial();

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
	// Delete the existing terrain mesh
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	TerrainGenerator::generateBasicTerrainMesh(vertices, resolution, terrainSpacing, heightScale, majorHeightFrequency, majorHeight, minorHeightFrequency, minorHeight);
	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(vertices, this->m_heightScale, 0.1f);
	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->SetGeometry(tMesh);

	this->m_chunks.push_back(tChunk);
}

void Terrain::generateTerrainFromNoise(int terrainResolution, float spacing, float heightScale, FastNoise::NoiseType noiseType, int seed, float frequency)
{
	// Delete the existing terrain mesh
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	TerrainGenerator::generateTerrainMeshVerticesFromNoise(vertices, terrainResolution, spacing, heightScale, noiseType, seed, frequency);
	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(vertices, this->m_heightScale, 0.1f);
	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->SetGeometry(tMesh);

	this->m_chunks.push_back(tChunk);
}

void Terrain::generateTerrainFromHeightmap(const char* filename, int actualTerrainWidth, int actualterrainLength, int terrainWidth, int terrainLength, int terrainSpacing, float heightScale)
{
	// Delete the existing terrain mesh
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	TerrainGenerator::generateTerrainMeshVerticesFromHeightmap(vertices, filename, actualTerrainWidth, actualterrainLength, terrainWidth, terrainLength, terrainSpacing, heightScale);
	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(vertices, this->m_heightScale, 0.1f);
	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->SetGeometry(tMesh);

	this->m_chunks.push_back(tChunk);
}

void Terrain::generateBasicChunkedTerrain(bool extend, int resolution, int terrainSpacing, float heightScale, float majorHeightFrequency,
	float majorHeight,
	float minorHeightFrequency,
	float minorHeight)
{
	// Delete the existing terrain mesh
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	std::vector<std::vector<Vector3f>> tChunkVertices;
	if (extend)
	{
		for (int OffsetX = -1; OffsetX < 1; OffsetX++)
		{
			for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
			{
				TerrainGenerator::generateBasicTerrainMesh(vertices, resolution, terrainSpacing, heightScale, majorHeightFrequency, majorHeight, minorHeightFrequency, minorHeight, OffsetX, OffsetZ);

				tChunkVertices.push_back(vertices);
				vertices.clear();
			}
		}

		return;
	}
	else
	{
		TerrainGenerator::generateBasicTerrainMesh(vertices, resolution, terrainSpacing, heightScale, majorHeightFrequency, majorHeight, minorHeightFrequency, minorHeight);
		// Divide the terrain mesh into 4 equal chunks
		TerrainGenerator::divideTerrainMesh(vertices, tChunkVertices, resolution, resolution);
	}

	// Generate terrain mesh chunks from the groups of vertices
	this->generateTerrainChunks(tChunkVertices, heightScale);
}

void Terrain::generateChunkedTerrainFromNoise(bool extend, int terrainResolution, float spacing, float heightScale, FastNoise::NoiseType noiseType, int seed, float frequency)
{
	// Delete the existing terrain mesh
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	std::vector<std::vector<Vector3f>> tChunkVertices;
	if (extend)
	{
		for (int OffsetX = -1; OffsetX < 1; OffsetX++)
		{
			for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
			{
				TerrainGenerator::generateTerrainMeshVerticesFromNoise(vertices, terrainResolution, spacing, heightScale, noiseType, seed, frequency, OffsetX, OffsetZ);

				tChunkVertices.push_back(vertices);
				vertices.clear();
			}
		}
	}
	else
	{

		TerrainGenerator::generateTerrainMeshVerticesFromNoise(vertices, terrainResolution, spacing, heightScale, noiseType, seed, frequency);
		// Divide the terrain mesh into 4 equal chunks
		TerrainGenerator::divideTerrainMesh(vertices, tChunkVertices, terrainResolution, terrainResolution);
	}

	// Generate terrain mesh chunks from the groups of vertices
	this->generateTerrainChunks(tChunkVertices, heightScale);
}

void Terrain::generateChunkedTerrainFromHeightmap(bool extend, const char* filename, int actualTerrainWidth, int actualterrainLength, int terrainWidth, int terrainLength, int terrainSpacing, float heightScale)
{
	// Delete the existing terrain mesh
	this->deleteTerrainMesh();

	std::vector<Vector3f> vertices;
	std::vector<std::vector<Vector3f>> tChunkVertices;
	if (extend)
	{
		// Duplicate the current terrain mesh
		for (int OffsetX = -1; OffsetX < 1; OffsetX++)
		{
			for (int OffsetZ = -1; OffsetZ < 1; OffsetZ++)
			{
				TerrainGenerator::generateTerrainMeshVerticesFromHeightmap(vertices, filename, actualTerrainWidth, actualterrainLength, terrainWidth, terrainLength, terrainSpacing, heightScale, OffsetX, OffsetZ);

				tChunkVertices.push_back(vertices);
				vertices.clear();
			}
		}
	}
	else
	{

		TerrainGenerator::generateTerrainMeshVerticesFromHeightmap(vertices, filename, actualTerrainWidth, actualterrainLength, terrainWidth, terrainLength, terrainSpacing, heightScale);
		// Divide the terrain mesh into 4 equal chunks
		TerrainGenerator::divideTerrainMesh(vertices, tChunkVertices, terrainWidth, terrainLength);
	}

	// Generate terrain mesh chunks from the groups of vertices
	this->generateTerrainChunks(tChunkVertices, heightScale);
}

void Terrain::deleteTerrainMesh()
{
	// Destroy all terrainChunks
	for (int i = 0; i < static_cast<int>(this->m_chunks.size()); i++)
	{
		this->RemoveElement(this->m_chunks[i]);
		delete this->m_chunks[i];
	}

	this->m_chunks.clear();
}

void Terrain::generateTerrainChunks(const std::vector<std::vector<Vector3f>>& chunksVertices, float heightScale)
{
	// Instantiate chunks
	for (auto& VertsArray : chunksVertices)
	{
		TerrainChunk* tChunk = new TerrainChunk();

		//this->root

		Node3D* tTerrainNode= this->GetNode();

		//TerrainChunk* tChunk = new TerrainChunk();
		tTerrainNode->AttachChild(tChunk);

		//this->m_pScene->AddActor(this->skysphereActor);

		//  generate mesh from vertices
		BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(VertsArray, heightScale, this->m_textureMappingFactor);

		//tChunk->GetBody()->SetGeometry(tMesh);
		tChunk->SetGeometry(tMesh);

		this->m_chunks.push_back(tChunk);
	}
}
