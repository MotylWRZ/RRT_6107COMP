#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

#include <map>

#include "InstancedStaticMesh.h"
#include "RRTVertexDX11.h"

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

class SolarSystem
{
public:
	SolarSystem();
	~SolarSystem();

	void addISM(BasicMeshPtr mesh, Vector3f origin, RendererDX11* renderer, Scene* scene,  std::wstring texture1, std::wstring texture2 = L"none", std::wstring texture3 = L"none", std::wstring texture4 = L"none");
	const InstancedStaticMesh* getISM(int index) const { return m_ISMs.at(index); }
	const std::vector<InstancedStaticMesh*>& getISMs() { return m_ISMs; }


	void addPlanet(int ISMindex, Vector3f position, EInstanceTexture planetTexture);

	void Update(float deltaTime);

private:


	std::vector<InstancedStaticMesh*> m_ISMs;


	float m_rotationSpeed;
	Vector3f m_rotationDirection;
};

