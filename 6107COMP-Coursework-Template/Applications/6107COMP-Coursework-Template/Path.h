#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"

#include "RRTVertexDX11.h"

enum class EPathType
{
	Path_Linear,
	Path_CatmullRom,
	Path_Hermite
};

using namespace Glyph3;

//const float DEG_TO_RAD = GLYPH_PI / 180.0f;


typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

class Path : public Actor
{
public:
	Path();
	~Path();

	void generatePath(EPathType pathType, float centerX, float centerY, float radius, float height, float start, float end, float increment);

	void addPathActorToScene(Scene* pScene, RendererDX11* pRenderer);
	void removePathActorFromScene(Scene* pScene);

	inline const std::vector<Vector3f> getPathPoints() const { return m_pathPoints; }

private:
	std::vector<Vector3f> m_pathPoints;
	EPathType m_pathType;
};

