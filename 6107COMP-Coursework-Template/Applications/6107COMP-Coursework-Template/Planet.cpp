#include "Planet.h"

#include "MeshImporter.h"

Planet::Planet()
	:m_rotationSpeed(100.f)
	,m_rotationDirection(0.0f, 1.0f, 0.0f)
	,m_explosionActive(false)
	,m_explosionMaterial(nullptr)
	,m_mainMaterial(nullptr)
{
}

void Planet::Initialize(RendererDX11& renderer, std::wstring diffuseTextureFile, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo, std::wstring bumpTextureFile, std::wstring planetMesh)
{
	MaterialPtr tMat;

	// Create main material
	if (bumpTextureFile != L"none")
	{
		tMat = MaterialGenerator::createLitBumpTexturedMaterial(renderer, diffuseTextureFile, bumpTextureFile, lights, MatReflectanceInfo);
	}
	else
	{
		tMat = MaterialGenerator::createLitTexturedMaterial(renderer, diffuseTextureFile, lights, MatReflectanceInfo);
	}

	this->m_mainMaterial = tMat;

	this->GetBody()->SetGeometry(MeshImporter::generateOBJMeshForGSExplosion(planetMesh, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
	this->GetBody()->SetMaterial(this->m_mainMaterial);

	// Create explosion material
	this->m_explosionMaterial = MaterialGenerator::createPlanetExplosionmaterial(renderer, diffuseTextureFile);
}

void Planet::Initialize(RendererDX11& renderer, std::wstring diffuseTextureFile, std::wstring planetMesh)
{
	MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(renderer, std::wstring(L"RRTTextureMapping.hlsl"), diffuseTextureFile);

	this->m_mainMaterial = tMaterial;

	this->GetBody()->SetGeometry(MeshImporter::generateOBJMeshForGSExplosion(planetMesh, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
	this->GetBody()->SetMaterial(this->m_mainMaterial);
}

void Planet::Update(float deltaTime, Timer* pTimer)
{

	// Planet rotation
	float tAngle = (this->m_rotationSpeed * (GLYPH_PI / 180.0f)) * deltaTime;

	Matrix3f tRotationMatrix;

	tRotationMatrix.RotationEuler(Vector3f(0.0f, 1.0f, 0.0f), tAngle);
	this->GetBody()->Rotation() *= tRotationMatrix;

	if (!this->m_explosionActive)
	{
		this->m_totalTime = 0.0f;
		return;
	}
	// This is used for planet explosion animation
	this->m_totalTime += pTimer->Elapsed();
	float elapsed = pTimer->Elapsed();
	Vector4f vTime = Vector4f(elapsed, this->m_totalTime, 0.0f, 0.0f);

	this->m_explosionMaterial->Parameters.SetVectorParameter(std::wstring(L"time"), vTime);
}

void Planet::setExplosion(bool enabled)
{
	if (!this->m_mainMaterial || !this->m_explosionMaterial)
	{
		return;
	}

	if (enabled)
	{
		this->GetBody()->SetMaterial(this->m_explosionMaterial);
	}
	else
	{
		this->GetBody()->SetMaterial(this->m_mainMaterial);
	}

	this->m_explosionActive = enabled;
}

void Planet::UpdateEntities(float deltaTime)
{
}
