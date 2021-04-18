#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"

#include "RRTVertexDX11.h"

using namespace Glyph3;

const float DEG_TO_RAD = GLYPH_PI / 180.0f;

struct HermitePoint
{
	Vector3f TangentOut;
	Vector3f TangentIn;
	Vector3f Position;
};

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

class PathGenerator
{
public:
	static Vector3f LinearInterp(Vector3f p0, Vector3f p1, float t);
	static void createLinearPath(float centerX, float centerY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut);
	static Vector3f CatmullRom(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, float t);
	static void createCatmullRomPath(float centerX, float centerY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut);
	static HermitePoint Hermite(const HermitePoint& start, const HermitePoint& end, float t);
	static void createHermitePath(float centerX, float centerY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut);
	static BasicMeshPtr generatePathMesh(const std::vector<Vector3f>& pathPoints);
};

