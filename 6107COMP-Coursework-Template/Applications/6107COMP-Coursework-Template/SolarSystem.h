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
	SolarSystem(Vector3f origin);
	~SolarSystem();

	void addISM(BasicMeshPtr mesh, Vector3f origin, RendererDX11* renderer, Scene* scene, std::wstring texture1, std::wstring texture2 = L"none", std::wstring texture3 = L"none", std::wstring texture4 = L"none");
	const InstancedStaticMesh* getISM(int index) const { return m_ISMs.at(index); }
	const std::vector<InstancedStaticMesh*>& getISMs() { return m_ISMs; }


	void addPlanet(int ISMindex, Vector3f position, EInstanceTexture planetTexture);

	void Update(float deltaTime);

	inline void setCircularMovementSpeed(float speed) { m_circularMovementSpeed = speed; }
	inline void setOrigin(Vector3f origin){m_originPoint = origin;}
	inline void setCircularMovementRadius(float radius) { m_circularMovementRadius = radius; }

	inline float getCircularMovementSpeed() { return m_circularMovementSpeed; }
	inline const Vector3f& getOrigin() const { return m_originPoint; }
	inline float getCircularMovementRadius() { return m_circularMovementRadius; }

private:
	float m_angle;

	std::vector<InstancedStaticMesh*> m_ISMs;
	float m_circularMovementSpeed;
	Vector3f m_originPoint;
	float m_circularMovementRadius;

	float m_rotationSpeed;
	Vector3f m_rotationDirection;
};

