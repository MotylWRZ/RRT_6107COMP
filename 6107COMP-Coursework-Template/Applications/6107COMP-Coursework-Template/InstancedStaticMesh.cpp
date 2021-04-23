#include "InstancedStaticMesh.h"
#include "LJMULevelDemo.h"

#include "ConstantBufferDX11.h"

#include "MaterialGenerator.h"
#include "MeshImporter.h"

InstancedStaticMesh::InstancedStaticMesh(BasicMeshPtr mesh, Vector3f origin, RendererDX11& renderer)// , LJMULevelDemo& Level)
	:m_pRenderer(&renderer)
{
	this->GetBody()->SetGeometry(mesh);
	this->GetBody()->Position() = origin;
}

InstancedStaticMesh::~InstancedStaticMesh()
{
}

void InstancedStaticMesh::initialise()
{
	this->setupMaterial();
	//this->GetBody()->SetGeometry(MeshImporter::generateMeshOBJWithSurfaceVectors(L"geosphere.obj", Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
	this->GetBody()->SetMaterial(this->m_Material);


}

void InstancedStaticMesh::addInstance(Vector3f instancePosition, float instanceScale, EInstanceTexture instanceTexture)
{
	// Do not add another instance if the limit of instances has been reached
	if (this->m_instances.size() + 1 > INSTANCE_NUM_MAX)
	{
		return;
	}

	ISMInstanceInfo tNewInstance(this->m_instances.size());
	tNewInstance.InstancePosition = instancePosition;
	tNewInstance.InstanceScale = instanceScale;
	tNewInstance.InstanceTexture = static_cast<float>(instanceTexture);

	this->m_instances.push_back(tNewInstance);

	this->updateMaterial();
}

const ISMInstanceInfo& InstancedStaticMesh::getInstance(int instanceId) const
{
	return this->m_instances[instanceId];
}

void InstancedStaticMesh::updateInstance(int instanceId, Vector3f newInstancePosition, float newInstanceScale)
{
	if (this->m_instances.size() > instanceId)
	{
		this->m_instances[instanceId].InstancePosition = newInstancePosition;
		this->m_instances[instanceId].InstanceScale = newInstanceScale;
	}

	this->updateMaterial();
}

void InstancedStaticMesh::loadTextures(std::wstring texture1, std::wstring texture2, std::wstring texture3, std::wstring texture4)
{
	ResourcePtr tTexture1 = RendererDX11::Get()->LoadTexture(texture1);
	ResourcePtr tTexture2 = RendererDX11::Get()->LoadTexture(texture2);
	ResourcePtr tTexture3 = RendererDX11::Get()->LoadTexture(texture3);
	ResourcePtr tTexture4 = RendererDX11::Get()->LoadTexture(texture4);

	this->m_textures.emplace(EInstanceTexture::TEXTURE1, tTexture1);
	this->m_textures.emplace(EInstanceTexture::TEXTURE2, tTexture2);
	this->m_textures.emplace(EInstanceTexture::TEXTURE3, tTexture3);
	this->m_textures.emplace(EInstanceTexture::TEXTURE4, tTexture4);

	if (this->m_Material)
	{
		this->m_Material->Parameters.SetShaderResourceParameter(L"DiffuseTexture1", this->m_textures.at(EInstanceTexture::TEXTURE1));
		this->m_Material->Parameters.SetShaderResourceParameter(L"DiffuseTexture2", this->m_textures.at(EInstanceTexture::TEXTURE2));
		this->m_Material->Parameters.SetShaderResourceParameter(L"DiffuseTexture3", this->m_textures.at(EInstanceTexture::TEXTURE3));
		this->m_Material->Parameters.SetShaderResourceParameter(L"DiffuseTexture4", this->m_textures.at(EInstanceTexture::TEXTURE4));
	}

}

void InstancedStaticMesh::setupMaterial()
{
	MaterialPtr tMaterial = MaterialGenerator::createMaterialWithGS(*this->m_pRenderer, L"RTR_MultiTextureInstancing2.hlsl");
	//MaterialPtr tMaterial = MaterialGenerator::createMaterialWithGS(*this->m_pRenderer, L"RTR_MultiLitTextureInstancing.hlsl");


	if (this->m_textures.size() > 0)
	{
		tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture1", this->m_textures.at(EInstanceTexture::TEXTURE1));
		tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture2", this->m_textures.at(EInstanceTexture::TEXTURE2));
		tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture3", this->m_textures.at(EInstanceTexture::TEXTURE3));
		tMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture4", this->m_textures.at(EInstanceTexture::TEXTURE4));
	}

	InstancedMeshMatCBuffData tData;

	for (int i = 0; i < this->m_instances.size(); i++)
	{
		tData.ISMInstances[i] = this->m_instances[i];
	}


	BufferConfigDX11 tBuffConfig;
	tBuffConfig.SetByteWidth(sizeof(tData.ISMInstances));
	tBuffConfig.SetBindFlags(D3D11_BIND_CONSTANT_BUFFER);
	tBuffConfig.SetMiscFlags(0);
	tBuffConfig.SetStructureByteStride(0);
	tBuffConfig.SetUsage(D3D11_USAGE_DYNAMIC);
	tBuffConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);

	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = &tData;
	dataLights.SysMemPitch = 0;
	dataLights.SysMemSlicePitch = 0;

	ResourcePtr res = this->m_pRenderer->CreateConstantBuffer(&tBuffConfig, &dataLights);

	tMaterial->Parameters.SetConstantBufferParameter(L"cInstances", res);

	this->m_Material = tMaterial;
}

void InstancedStaticMesh::updateMaterial()
{
	if (!this->m_Material)
	{
		return;
	}

	// Get CBuffer Paramameter Writer from the material
	ConstantBufferParameterWriterDX11* tWriter = this->m_Material->Parameters.GetConstantBufferParameterWriter(L"cInstances");

	if (!tWriter)
	{
		return;
	}

	// Get actual constant buffer from the renderer
	ConstantBufferDX11* tBuffer = this->m_pRenderer->GetConstantBufferByIndex(tWriter->GetValue()->m_iResource);

	// Create a pointer to the DeviceContext
	ID3D11DeviceContext* tContext;
	this->m_pRenderer->GetDevice()->GetImmediateContext(&tContext);

	// Create a pointer to the source resource
	ID3D11Resource* srcResource;
	srcResource = tBuffer->GetResource();

	// Create a pointer to the source constant buffer
	ID3D11Buffer* srcBuffer;
	srcResource->QueryInterface(IID_ID3D11Buffer, (void**)&srcBuffer);

	// Download the data from the source buffer and save it in MappedResource
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	tContext->Map(srcBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	// Create a pointer to the BufferData from the source buffer
	InstancedMeshMatCBuffData* b = &((InstancedMeshMatCBuffData*)(MappedResource.pData))[0];

	// Modify the data from the source buffer by changing the values in the mapped resource
	for (int i = 0; i < this->m_instances.size(); i++)
	{
		b->ISMInstances[i] = this->m_instances[i];
	}

	// Finish operation by unmapping the buffer
	tContext->Unmap(srcBuffer, 0);
}
