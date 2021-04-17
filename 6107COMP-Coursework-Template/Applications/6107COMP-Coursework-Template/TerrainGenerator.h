#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

#include "RRTVertexDX11.h"
#include "TerrainChunk.h"
#include "FastNoise.h"

class TerrainChunk;
class Terrain;
class Light_Base;

using namespace Glyph3;
//typedef  std::shared_ptr<Light_Base> LightBasePtr;

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

class TerrainGenerator
{
	TerrainGenerator();
	~TerrainGenerator();

public:
	static BasicMeshPtr generateTerrainMesh(int offsetX, int offsetZ, int terrainResolution, int terrainSpacing);
	static void generateterainMeshVertices(std::vector<Vector3f>& verticesOut, int offsetX, int offsetZ, int terrainResolution, int terrainSpacing);
	static void createTerrainIndexArray(int terrainWidth, int terrainLength,
		bool winding, std::vector<int>& indices);

	static void createSegmentedTerrainActor(std::vector<Actor*>& terrainSegments);
	static Actor* createTerrainActor(int offsetX, int offsetZ, int terrainResolution, int terrainSpacing);

	static TerrainChunk* generateTerrainChunk(const Terrain& terrain, int offsetX, int offsetZ);

	// Divid terrain mesh into 4 equal chunks
	// Returns 4 equal array of vertices
	static void generateEqualChunksFromVertices(std::vector<Vector3f>& meshVertices, std::vector<TerrainChunk>& chunksOut);

	static void generateBasicTerrainMesh(std::vector<Vector3f>& verticesOut,
		int resolution,
		int terrainSpacing,
		float heightScale = 12.0f,
		float majorHeightFrequency = 5.0f,
		float majorHeight = 1.0f,
		float minorHeightFrequency = 75.0f,
		float minorHeight = 0.25f, int OffsetX = 0, int OffsetZ = 0);

	static void generateTerrainMeshVerticesFromNoise(std::vector<Vector3f>& verticesOut,
		int terrainResolution,
		float spacing,
		float heightScale,
		FastNoise::NoiseType noiseType = FastNoise::NoiseType::SimplexFractal,
		int seed = 123456,
		float frequency = 0.01f,
		int OffsetX = 0,
		int OffsetZ = 0);

	static void generateTerrainMeshVerticesFromHeightmap(std::vector<Vector3f>& verticesOut,
		const char* filename,
		int actualTerrainWidth,
		int actualterrainLength,
		int terrainWidth,
		int terrainLength,
		int terrainSpacing,
		float heightScale,
		int OffsetX = 0,
		int OffsetZ = 0);

	static double* loadRawHightMap(const char* filename, int actualTerrainWidth, int actualterrainLength, int terrainWidth, int terrainLength);

	static BasicMeshPtr generateTerrainMeshFromVertices(const std::vector<Vector3f>& meshVertices, float heightScale, float textureMappingFactor);

	// Create 4 equal arrays of vertices that can be used to create 4 duplicated meshes with position offsets
	static void extendTerrainMesh(const std::vector<Vector3f>& meshVertices, std::vector<std::vector<Vector3f>>& verticesOut);

	//Terrain* generateBasicTerrain();


};

