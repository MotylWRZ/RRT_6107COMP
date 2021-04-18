#include "Path.h"

#include "RasterizerStateConfigDX11.h"

#include "PathGenerator.h"
#include "MaterialGenerator.h"

Path::Path()
{
}

Path::~Path()
{
}

void Path::generatePath(EPathType pathType, float centerX, float centerY, float radius, float height, float start, float end, float increment)
{
	switch (pathType)
	{
	case EPathType::Path_Linear:
	{
		PathGenerator::createLinearPath(centerX, centerY, radius, height, start, end, increment, this->m_pathPoints);
		break;
	}
	case EPathType::Path_CatmullRom:
	{
		PathGenerator::createCatmullRomPath(centerX, centerY, radius, height, start, end, increment, this->m_pathPoints);
		break;
	}
	case EPathType::Path_Hermite:
	{
		PathGenerator::createHermitePath(centerX, centerY, radius, height, start, end, increment, this->m_pathPoints);
		break;
	}
	}
}

void Path::generatePathMesh(RendererDX11* pRenderer)
{
	if (!pRenderer)
	{
		return;
	}

	BasicMeshPtr tMesh = PathGenerator::generatePathMesh(this->m_pathPoints);
	MaterialPtr tMat = MaterialGenerator::createBasicMaterial(*pRenderer);

	this->GetBody()->SetGeometry(tMesh);
	this->GetBody()->SetMaterial(tMat);
}