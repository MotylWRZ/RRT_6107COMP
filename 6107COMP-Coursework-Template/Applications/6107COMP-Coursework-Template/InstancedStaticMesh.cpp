#include "InstancedStaticMesh.h"

#include "MaterialGenerator.h"

InstancedStaticMesh::InstancedStaticMesh(RendererDX11& renderer)
	:m_pRenderer(&renderer)
{
	//MaterialGenerator::createGSInstancingMultiTextureMaterial(renderer)
}

InstancedStaticMesh::~InstancedStaticMesh()
{
}

void InstancedStaticMesh::initialise()
{
	Vector4f tInstancePos1(30.0f, 30.0f, 30.0f, 1.0f);
	Vector4f tInstancePos2(30.0f, -30.0f, 30.0f, 1.0f);
	Vector4f tInstancePos3(30.0f, 30.0f, -30.0f, 1.0f);
	Vector4f tInstancePos4(30.0f, -30.0f, -30.0f, 1.0f);

	StaticMeshInstance i1;
	StaticMeshInstance i2;
	StaticMeshInstance i3;
	StaticMeshInstance i4;

	i1.InstancePosition = Vector3f(30.0f, 30.0f, 30.0f);
	i2.InstancePosition = Vector3f(30.0f, -30.0f, 30.0f);
	i3.InstancePosition = Vector3f(30.0f, 30.0f, -30.0f);
	i4.InstancePosition = Vector3f(30.0f, -30.0f, -30.0f);

	this->m_instances.push_back(i1);
	this->m_instances.push_back(i2);
	this->m_instances.push_back(i3);
	this->m_instances.push_back(i4);
	this->setupMaterial();
	this->GetBody()->SetMaterial(this->m_Material);
}

void InstancedStaticMesh::addInstance(Vector3f instancePosition)
{
	StaticMeshInstance tNewInstance;
	tNewInstance.InstancePosition = instancePosition;
	//tNewInstance.InstanceId = static_cast<int>(this->m_instances.size());

	this->m_instances.push_back(tNewInstance);
}

const StaticMeshInstance& InstancedStaticMesh::getInstance(int instanceId) const
{
	return this->m_instances[instanceId];
}

void InstancedStaticMesh::updateInstance(int instanceId, Vector3f newInstancePosition)
{
}

void InstancedStaticMesh::setupMaterial()
{
	MaterialPtr tMaterial = MaterialGenerator::createMaterialWithGS(*this->m_pRenderer, L"RTR_MultiTextureInstancing2.hlsl");

	ResourcePtr tTexture1 = RendererDX11::Get()->LoadTexture(std::wstring(L"earth.tif"));
	ResourcePtr tTexture2 = RendererDX11::Get()->LoadTexture(std::wstring(L"mars.tif"));
	ResourcePtr tTexture3 = RendererDX11::Get()->LoadTexture(std::wstring(L"TerrainGrass.tif"));
	ResourcePtr tTexture4 = RendererDX11::Get()->LoadTexture(std::wstring(L"SnowScuffedGround.tif"));

	tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture1", tTexture1);
	tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture2", tTexture2);
	tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture3", tTexture3);
	tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture4", tTexture4);


	InstancedMeshMatCBuffData tData;

	for (int i = 0; i < this->m_instances.size(); i++)
	{
		tData.Instances[i] = this->m_instances[i];
	}


	BufferConfigDX11 tBuffConfig;
	//tBuffConfig.SetDefaultConstantBuffer(LIGHTS_NUM_MAX * sizeof(LightInfo), false);
	tBuffConfig.SetByteWidth(sizeof(tData.Instances));
	tBuffConfig.SetBindFlags(D3D11_BIND_CONSTANT_BUFFER);
	tBuffConfig.SetMiscFlags(0);
	tBuffConfig.SetStructureByteStride(0);
	tBuffConfig.SetUsage(D3D11_USAGE_DYNAMIC);// D3D11_USAGE_DEFAULT);
	tBuffConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);



	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = &tData;
	dataLights.SysMemPitch = 0;
	dataLights.SysMemSlicePitch = 0;

	ResourcePtr res = this->m_pRenderer->CreateConstantBuffer(&tBuffConfig, &dataLights);

	tMaterial->Parameters.SetConstantBufferParameter(L"cInstances", res);

	this->m_Material = tMaterial;
}
