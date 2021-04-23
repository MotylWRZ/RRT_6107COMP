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

void SolarSystem::addPlanet(int ISMindex, Vector3f position, float circularMovementSpeed, float circularMovementRadius, float planetScale, Matrix4f planetRotation, float planetRotationSpeed, Vector3f planetRotationDirection, EInstanceTexture planetTexture)
{
	if (static_cast<int>(this->m_ISMs.size()) > ISMindex)
	{
		// Create a new instance of the specified ISM
		this->m_ISMs[ISMindex]->addInstance(position, planetScale, planetRotation, planetTexture);

		// Create a new planet struct and fill the relevant info
		PlanetInfo tNewPlanet;
		tNewPlanet.CircularMovementSpeed = circularMovementSpeed;
		tNewPlanet.CircularMovementRadius = circularMovementRadius;
		tNewPlanet.RotationSpeed = planetRotationSpeed;
		tNewPlanet.RotationDirection = planetRotationDirection;
		tNewPlanet.PlanetScale = planetScale;

		// Check if an element with specified ID already exists
		if(this->m_planets.find(ISMindex) == this->m_planets.end())
		{
			std::vector<PlanetInfo> tPlanetInfos;
			tPlanetInfos.push_back(tNewPlanet);

			this->m_planets.emplace(ISMindex, tPlanetInfos);
			return;
		}

		// If an element already exists and vector is not empty, add a planet info struct into a vector with planet infos
		// Add a planet info into the vector
		this->m_planets.at(ISMindex).push_back(tNewPlanet);
	}

}

void SolarSystem::Update(float deltaTime)
{
	float offset = 1.0f;
	for (int i = 0; i < static_cast<int>(this->m_ISMs.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(this->m_ISMs[i]->getInstances().size()); j++)
		{
			PlanetInfo& tPlanetInfo = this->m_planets.at(i)[j];
			const ISMInstanceInfo& tInstanceInfo = this->m_ISMs[i]->getInstance(j);

			// Calculate movement and rotation angles
			tPlanetInfo.MovementAngle += tPlanetInfo.CircularMovementSpeed * deltaTime;
			tPlanetInfo.RotationAngle += (tPlanetInfo.RotationSpeed * (GLYPH_PI / 180.0f)) * deltaTime;

			// Construct Matrix3f which wil be use for planet rotation in Geometry Shader
			Matrix3f tRotationMatrix;
			tRotationMatrix.RotationEuler(tPlanetInfo.RotationDirection, tPlanetInfo.RotationAngle);

			// Construct Matrix4f and set it's rotation
			Matrix4f tRotMatrix = Matrix4f::Identity();
			tRotMatrix.SetRotation(tRotationMatrix);

			// Calculate planet position
			Vector3f tNewPos = Vector3f(this->m_originPoint.x + (tPlanetInfo.CircularMovementRadius * cos(tPlanetInfo.MovementAngle + offset)), this->m_ISMs[i]->GetNode()->Position().y, this->m_originPoint.z + (tPlanetInfo.CircularMovementRadius * sin(tPlanetInfo.MovementAngle + offset)));
			this->m_ISMs[i]->updateInstance(j, tNewPos, tPlanetInfo.PlanetScale, tRotMatrix);
			offset++;
		}
	}
}
