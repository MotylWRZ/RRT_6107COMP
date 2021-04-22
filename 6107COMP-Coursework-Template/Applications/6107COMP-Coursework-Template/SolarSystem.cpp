#include "SolarSystem.h"

SolarSystem::SolarSystem(Vector3f origin)
	:m_rotationSpeed(100.f)
	, m_rotationDirection(0.0f, 1.0f, 0.0f)
	, m_circularMovementSpeed(0.1f)
	, m_originPoint(origin)
	, m_circularMovementRadius(1000.0f)
	, m_angle(0.0f)

{
}

SolarSystem::~SolarSystem()
{
}

void SolarSystem::addISM(BasicMeshPtr mesh, Vector3f origin, RendererDX11* renderer, Scene* scene, std::wstring texture1, std::wstring texture2, std::wstring texture3, std::wstring texture4)
{
	InstancedStaticMesh* tISM = new InstancedStaticMesh(mesh, origin, *renderer);
	tISM->loadTextures(texture1, texture2, texture3, texture4);
	tISM->initialise();

	this->m_ISMs.push_back(tISM);
	scene->AddActor(tISM);
}

void SolarSystem::addPlanet(int ISMindex, Vector3f position, EInstanceTexture planetTexture)
{
	if (static_cast<int>(this->m_ISMs.size()) > ISMindex)
	{
		this->m_ISMs[ISMindex]->addInstance(position, planetTexture);
	}
}

void SolarSystem::Update(float deltaTime)
{
	for (int i = 0; i < static_cast<int>(this->m_ISMs.size()); i++)
	{
		float tRadius = this->m_circularMovementRadius;

		if (i > 0)
		{
			tRadius /= i * 2;
		}

		// Rotate instnces around the Y axis
		float tAngle = (this->m_rotationSpeed * (GLYPH_PI / 180.0f)) * deltaTime;

		Matrix3f tRotationMatrix;

		tRotationMatrix.RotationEuler(this->m_rotationDirection, tAngle);
		this->m_ISMs[i]->GetBody()->Rotation() *= tRotationMatrix;

		float x = 100.0f;
		float z = 100.0f;

		// Move around the solar system origin point
		m_angle += this->m_circularMovementSpeed * deltaTime;


		float offset = 1.0f;
		for (int j = 0; j < this->m_ISMs[i]->getInstances().size(); j++)
		{

			Vector3f tNewPos = Vector3f(this->m_originPoint.x + (tRadius * cos(m_angle + offset)), this->m_ISMs[i]->GetNode()->Position().y, this->m_originPoint.z + (tRadius * sin(m_angle + offset)));
			this->m_ISMs[i]->updateInstance(j, tNewPos);
			offset++;
		}

		offset = 0;
	}
}
