#include "Terrain.h"

#include "TerrainGenerator.h"
#include "MaterialGenerator.h"

Terrain::Terrain(int terrainResolution, int terrainSpacing, float heightScale)
	:m_terrainResolution(terrainResolution)
	,m_terrainSpacing(terrainSpacing)
	,m_heightScale(heightScale)
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

void Terrain::initializeBasic()
{
	std::vector<Vector3f> vertices;
	TerrainGenerator::generateBasicTerrainMesh(vertices, this->m_terrainResolution, m_terrainSpacing);
	BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(vertices, this->m_heightScale, 0.1f);
	TerrainChunk* tChunk = new TerrainChunk();

	tChunk->GetBody()->SetGeometry(tMesh);

	this->m_chunks.push_back(tChunk);
}

void Terrain::initializeBasicChunkedTerrain(bool extend)
{
	std::vector<Vector3f> vertices;
	if (extend)
	{
		std::vector<std::vector<Vector3f>> tChunkVertices;

		TerrainGenerator::generateBasicTerrainMesh(vertices, this->m_terrainResolution, this->m_terrainSpacing);
		// generate vertices for each chunk
		TerrainGenerator::extendTerrainMesh(vertices, tChunkVertices);

		// Instantiate chunks
		for (auto& VertsArray : tChunkVertices)
		{
			vertices.clear();

			TerrainChunk* tChunk = new TerrainChunk();

			//  generate mesh from vertices
			BasicMeshPtr tMesh = TerrainGenerator::generateTerrainMeshFromVertices(VertsArray, this->m_heightScale, 0.1f);

			tChunk->GetBody()->SetGeometry(tMesh);

			this->m_chunks.push_back(tChunk);
		}
	}

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
