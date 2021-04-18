#include "Path.h"

#include "PathGenerator.h"

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

void Path::addPathActorToScene(Scene* pScene)
{
	pScene->AddActor(this);
}

void Path::removePathActorFromScene(Scene* pScene)
{
	pScene->RemoveActor(this);
}
