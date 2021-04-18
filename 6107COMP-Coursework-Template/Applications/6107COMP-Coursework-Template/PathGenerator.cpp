#include "PathGenerator.h"

Vector3f PathGenerator::LinearInterp(Vector3f p0, Vector3f p1, float t)
{
    Vector3f result;
    result = p0 * (1 - t) + p1 * t;
    return result;
}

BasicMeshPtr PathGenerator::createLinearPath(float centerX, float centerY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut)
{

	std::vector<Vector3f> tLinearCheckpoints;
	std::vector<Vector3f> tPath;

	for (int i = start; i < end; i = i + increment)
	{
		float x = centerX + radius * cos(i * DEG_TO_RAD);
		float z = centerY + radius * sin(i * DEG_TO_RAD);
		float y = height + 100 * cos(i * DEG_TO_RAD * 10);
		tLinearCheckpoints.push_back(Vector3f(x, y, z));
	}

	int numPoints = tLinearCheckpoints.size();

	for (int pt = 0; pt < numPoints; pt++)
	{
		int p0 = pt;
		int p1 = (pt + 1) % numPoints;
		int numInterpoints = 10;

		for (int i = 0; i < numInterpoints; i++)
		{
			float time = ((float)i) / numInterpoints;
			Vector3f pt = LinearInterp(tLinearCheckpoints[p0], tLinearCheckpoints[p1], time);
			tPath.push_back(pt);
		}
	}

	auto pathMesh = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	pathMesh->SetLayoutElements(BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements);
	pathMesh->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pathMesh->SetMaxVertexCount(tPath.size());

	RRTVertexDX11::Vertex tv;

	for (int i = 0; i < tPath.size() + 1; i++)
	{
		int j = i % tPath.size();
		tv.position = tPath[j];
		tv.color = Vector4f(1, 0, 0, 1);
		pathMesh->AddVertex(tv);
	}

	tLinearCheckpoints.clear();

	pathOut = tPath;

	return pathMesh;
}

Vector3f PathGenerator::CatmullRom(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, float t)
{
    return Vector3f();
}

BasicMeshPtr PathGenerator::createCatmullRomPath(float centerX, float centeY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut)
{
    return BasicMeshPtr();
}

HermitePoint PathGenerator::Hermite(const HermitePoint& start, const HermitePoint& end, float t)
{
    return HermitePoint();
}

BasicMeshPtr PathGenerator::createHermitePath(float centerX, float centeY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut)
{
    return BasicMeshPtr();
}
