#pragma once



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
	Vector3f InstancePosition;
	float padding;
};

struct InstancedMeshMatCBuffData
{
	StaticMeshInstance Instances[32];
};

class InstancedStaticMesh : public Actor
{
public:
	InstancedStaticMesh(RendererDX11& renderer);
	~InstancedStaticMesh();

	void initialise();
	void addInstance(Vector3f instancePosition);
	const StaticMeshInstance& getInstance(int instanceId) const;
	void updateInstance(int instanceId, Vector3f newInstancePosition);



	const std::vector<StaticMeshInstance>& getInstances() const { return m_instances; }

private:
	 void setupMaterial();

private:
	std::vector<StaticMeshInstance> m_instances;
	RendererDX11* m_pRenderer;
	MaterialPtr m_Material;
};

