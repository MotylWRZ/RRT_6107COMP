#include "SolarSystem.h"

SolarSystem::SolarSystem()
	:m_rotationSpeed(100.f)
	, m_rotationDirection(0.0f, 1.0f, 0.0f)
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
	for (auto& ISM : this->m_ISMs)
	{
		for (int i = 0; i < static_cast<int>(ISM->getInstances().size()); ++i)
		{
			// Updare instance(planet) rotation here
			/*Vector3f tPos = ISM->getInstance(i).InstancePosition;
			tPos.x += 100.0f * deltaTime;
			ISM->updateInstance(i, tPos);*/
		}

		float tAngle = (this->m_rotationSpeed * (GLYPH_PI / 180.0f)) * deltaTime;

		Matrix3f tRotationMatrix;

		tRotationMatrix.RotationEuler(this->m_rotationDirection, tAngle);
		ISM->GetBody()->Rotation() *= tRotationMatrix;
	}
}
