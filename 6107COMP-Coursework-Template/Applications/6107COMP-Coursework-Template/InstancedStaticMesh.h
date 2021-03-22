#pragma once

#include "MaterialGenerator.h"

#include "RendererDX11.h"
#include "MaterialDX11.h"

#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

#include <vector>

using namespace Glyph3;


struct StaticMeshInstance
{
	int InstanceId;
	Vector3f InstancePosition;
};

//class InstancedStaticMesh : Actor
//{
//public:
//	InstancedStaticMesh(RendererDX11& renderer);
//	~InstancedStaticMesh();
//
//	void addInstance(Vector3f instancePosition);
//	void getInstance(int instanceId);
//	void updateInstance(int instanceId, Vector3f newInstancePosition);
//
//	const std::vector<StaticMeshInstance>& getInstances() const { return m_instances; }
//
//private:
//	std::vector<StaticMeshInstance> m_instances;
//};
//
