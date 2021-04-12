#include "Planet.h"

#include "MeshImporter.h"

Planet::Planet()
	:m_rotationSpeed(100.f)
	,m_rotationDirection(0.0f, 1.0f, 0.0f)
{
}

void Planet::Initialize(MaterialPtr planetMaterial, std::wstring planetMesh)
{
	this->GetBody()->SetGeometry(MeshImporter::generateMeshOBJWithSurfaceVectors(planetMesh, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
	this->GetBody()->SetMaterial(planetMaterial);
}

void Planet::Update(float deltaTime)
{
	float tAngle = (this->m_rotationSpeed * (GLYPH_PI / 180.0f)) * deltaTime;

	Matrix3f tRotationMatrix;

	tRotationMatrix.RotationEuler(Vector3f(0.0f, 1.0f, 0.0f), tAngle);
	this->GetBody()->Rotation() *= tRotationMatrix;
}

void Planet::UpdateEntities(float deltaTime)
{
}
