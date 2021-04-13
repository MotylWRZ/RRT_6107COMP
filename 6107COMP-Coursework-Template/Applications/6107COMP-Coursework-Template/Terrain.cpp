#include "Terrain.h"

#include "TerrainGenerator.h"
#include "MaterialGenerator.h"

Terrain::Terrain(int terrainResolution, int terrainSpacing, int chunksNum)
	:m_terrainResolution(terrainResolution)
	,m_terrainSpacing(terrainSpacing)
	,m_chunks((chunksNum > 0)?chunksNum:1)
{
	this->generateTerrainChunks();
}

Terrain::~Terrain()
{
}

void Terrain::setMaterial(MaterialPtr material)
{
	for (auto& Chunk : this->m_chunks)
	{
		if (Chunk)
		{
			Chunk->GetBody()->SetMaterial(material);
		}
	}
}

void Terrain::addTerrainIntoScene(Scene* pScene)
{
	if (pScene)
	{
		for (auto& Chunk : this->m_chunks)
		{
			if (Chunk)
			{
				pScene->AddActor(Chunk);
			}
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

void Terrain::generateTerrainChunks()
{
	int tLandscapeOffsetX;
	int tLandscapeOffsetZ;

	for (int LandscapeOffsetX = -1; LandscapeOffsetX < 1; LandscapeOffsetX++)
	{
		for (int LandscapeOffsetZ = -1; LandscapeOffsetZ < 1; LandscapeOffsetZ++)
		{
			TerrainChunk* tTerrainChunk = TerrainGenerator::generateTerrainChunk(*this, LandscapeOffsetX, LandscapeOffsetZ);

			tTerrainChunk->GetBody()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
			tTerrainChunk->GetBody()->Scale() = Vector3f(1.0f, 1.0f, 1.0f);

			this->m_chunks.push_back(tTerrainChunk);
		}
	}
}
