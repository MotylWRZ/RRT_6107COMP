#include "Planet.h"

#include "MeshImporter.h"

Planet::Planet()
	:m_rotationSpeed(1.0f)
{
}

void Planet::Initialize(MaterialPtr planetMaterial, std::wstring planetMesh)
{
	this->GetBody()->SetGeometry(MeshImporter::generateMeshOBJWithSurfaceVectors(planetMesh, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
	this->GetBody()->SetMaterial(planetMaterial);
}

void Planet::Update(float deltaTime)
{
	this->m_planetrotation += this->m_rotationSpeed * deltaTime;
	Matrix3f tRotationMatrix;
	tRotationMatrix.RotationZYX(Vector3f(GLYPH_PI, this->m_planetrotation, 0.0f));
	this->GetBody()->Rotation() = tRotationMatrix;
}

void Planet::UpdateEntities(float deltaTime)
{
}
