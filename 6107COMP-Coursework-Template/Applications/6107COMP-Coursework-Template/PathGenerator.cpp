#include "PathGenerator.h"

Vector3f PathGenerator::LinearInterp(Vector3f p0, Vector3f p1, float t)
{
    Vector3f result;
    result = p0 * (1 - t) + p1 * t;
    return result;
}

void PathGenerator::createLinearPath(float centerX, float centerY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut)
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

	pathOut = tPath;
}

Vector3f PathGenerator::CatmullRom(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, float t)
{
	// Calculate Catmull-Rom basis
	Vector3f cm0 = p1;
	Vector3f cm1 = (p2 - p0) * 0.5f;
	Vector3f cm2 = (p0 * 2 - p1 * 5 + p2 * 4 - p3) * 0.5f;
	Vector3f cm3 = ((p1 - p2) * 3 + p3 - p0) * 0.5f;

	float t2 = t * t;
	float t3 = t2 * t;

	Vector3f result;

	result = cm0 + cm1 * t + cm2 * t2 + cm3 * t3;

	return result;
}

void PathGenerator::createCatmullRomPath(float centerX, float centerY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut)
{

	std::vector<Vector3f> CatmullRomPoints;
	std::vector<Vector3f> tPath;

	for (int i = start; i < end; i = i + increment)
	{
		float x = centerX + radius * cos(i * DEG_TO_RAD);
		float z = centerY + radius * sin(i * DEG_TO_RAD);
		float y = height + 100 * cos(i * DEG_TO_RAD * 10);
		CatmullRomPoints.push_back(Vector3f(x, y, z));
	}

	int numPoints = CatmullRomPoints.size();

	for (int pt = 1; pt <= numPoints; pt++)
	{
		int p0 = (pt - 1);
		int p1 = pt % numPoints;
		int p2 = (pt + 1) % numPoints;
		int p3 = (pt + 2) % numPoints;
		int numInterpoints = 10;

		for (int i = 0; i < numInterpoints; i++)
		{
			float time = ((float)i) / numInterpoints;
			Vector3f pt = CatmullRom(CatmullRomPoints[p0], CatmullRomPoints[p1],
				CatmullRomPoints[p2], CatmullRomPoints[p3], time);
			tPath.push_back(pt);
		}
	}

	pathOut = tPath;
}

HermitePoint PathGenerator::Hermite(const HermitePoint& start, const HermitePoint& end, float t)
{
	Vector3f position0 = start.Position;   // Enter position
	Vector3f tangentOut0 = start.TangentOut; // Enter tangent
	Vector3f position1 = end.Position; // Exit position
	Vector3f tangentIn1 = end.TangentIn; // Exit tangent

	float t2 = t * t; //t-squared
	float t3 = t2 * t; // t-cubed

	// Calculate Hermite basis
	float h1 = 2.0f * t3 - 3.0f * t2 + 1.0f;
	float h2 = -2.0f * t3 + 3.0f * t2;
	float h3 = t3 - 2.0f * t2 + t;
	float h4 = t3 - t2;

	HermitePoint result;

	result.Position.x = h1 * position0.x + h3 * tangentOut0.x + h2 * position1.x + h4 * tangentIn1.x;
	result.Position.y = h1 * position0.y + h3 * tangentOut0.y + h2 * position1.y + h4 * tangentIn1.y;
	result.Position.z = h1 * position0.z + h3 * tangentOut0.z + h2 * position1.z + h4 * tangentIn1.z;

	return result;
}

void PathGenerator::createHermitePath(float centerX, float centerY, float radius, float height, float start, float end, float increment, std::vector<Vector3f>& pathOut)
{
	std::vector<HermitePoint> HermitePoints;
	std::vector<Vector3f> tPath;

	for (int i = start; i < end; i = i + increment)
	{
		float x = centerX + radius * cos(i * DEG_TO_RAD);
		float z = centerY + radius * sin(i * DEG_TO_RAD);
		float y = height + 100 * cos(i * DEG_TO_RAD * 10);

		HermitePoint tNewPoint;
		tNewPoint.Position = Vector3f(x, y, z);

		HermitePoints.push_back(tNewPoint);
	}

	int numPoints = HermitePoints.size();

	for (int pt = 1; pt <= numPoints; pt++)
	{
		int p0 = (pt - 1);
		int p1 = pt % numPoints;
		int numInterpoints = 10;

		HermitePoints[p0].TangentOut = HermitePoints[p1].Position - HermitePoints[p0].Position;
		HermitePoints[p1].TangentIn = HermitePoints[p1].Position - HermitePoints[p0].Position;

		HermitePoints[p0].TangentOut.Normalize();
		HermitePoints[p1].TangentIn.Normalize();

		for (int i = 0; i < numInterpoints; i++)
		{
			float time = ((float)i) / numInterpoints;
			HermitePoint pt = Hermite(HermitePoints[p0], HermitePoints[p1], time);
			tPath.push_back(pt.Position);
		}
	}

	pathOut = tPath;
}

BasicMeshPtr PathGenerator::generatePathMesh(const std::vector<Vector3f>& pathPoints)
{
	if (pathPoints.size() == 0)
	{
		return nullptr;
	}

	auto pathMesh = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	pathMesh->SetLayoutElements(RRTVertexDX11::GetElementCount(), RRTVertexDX11::Elements);
	pathMesh->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pathMesh->SetMaxVertexCount(pathPoints.size());

	RRTVertexDX11::Vertex tv;

	for (int i = 0; i < pathPoints.size() + 1; i++)
	{
		int j = i % pathPoints.size();
		tv.position = pathPoints[j];
		tv.color = Vector4f(1, 0, 0, 1);
		pathMesh->AddVertex(tv);
	}

	return pathMesh;
}
