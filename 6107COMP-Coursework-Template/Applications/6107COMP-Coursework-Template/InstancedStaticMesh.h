#pragma once



#include "RendererDX11.h"
#include "MaterialDX11.h"

#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

#include <vector>

#include "InstancedStaticMesh.h"
#include "RRTVertexDX11.h"

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

static constexpr int INSTANCE_NUM_MAX = 32;
using namespace Glyph3;



struct ISMInstanceInfo
{
	ISMInstanceInfo() {};
	ISMInstanceInfo(int NewInstanceId) { InstanceId = NewInstanceId; }

	Vector3f InstancePosition;
	int InstanceTexture = -1;

private:
	int InstanceId = -1;
	Vector3f padding;
};

struct InstancedMeshMatCBuffData
{
	ISMInstanceInfo ISMInstances[INSTANCE_NUM_MAX];
};

// Mesh Instances can use one from the 4 supported textures (texture have to be loaded first into the ISM)
enum class EInstanceTexture
{
	TEXTURE1,
	TEXTURE2,
	TEXTURE3,
	TEXTURE4
};

class InstancedStaticMesh : public Actor
{
public:
	InstancedStaticMesh(BasicMeshPtr mesh, Vector3f origin,  RendererDX11& renderer);
	~InstancedStaticMesh();

	void initialise();
	void addInstance(Vector3f instancePosition, EInstanceTexture instanceTexture = EInstanceTexture::TEXTURE1);

	void updateInstance(int instanceId, Vector3f newInstancePosition);

	// Load all textures
	void loadTextures(std::wstring texture1, std::wstring texture2 = L"none", std::wstring texture3 = L"none", std::wstring texture4 = L"none");

	const std::vector<ISMInstanceInfo>& getInstances() const { return m_instances; }
	const ISMInstanceInfo& getInstance(int instanceId) const;

private:
	 void setupMaterial();
	 void updateMaterial();

private:
	std::vector<ISMInstanceInfo> m_instances;
	std::map<EInstanceTexture, ResourcePtr> m_textures;
	RendererDX11* m_pRenderer;
	MaterialPtr m_Material;


};

