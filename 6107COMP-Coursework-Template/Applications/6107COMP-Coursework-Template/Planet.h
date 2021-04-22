#pragma once

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

//LJMU Framework Includes
#include "LJMUTextOverlay.h"

#include "RRTVertexDX11.h"
#include "MaterialGenerator.h"

class Light_Base;

using namespace Glyph3;

class Planet : public Actor
{
public:

	Planet();
	virtual ~Planet() {};

	void Initialize(RendererDX11& renderer, std::wstring diffuseTextureFile, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo, std::wstring bumpTextureFile = L"none", std::wstring planetMesh = L"geosphere.obj");
	void Update(float deltaTime, Timer* pTimer);
	void setExplosion(bool enabled);

	inline bool isExplosionActive() { return m_explosionActive; }

private:
	void UpdateEntities(float deltaTime);

private:
	MaterialPtr m_mainMaterial;
	MaterialPtr m_explosionMaterial;
	bool m_explosionActive;

	float m_rotationSpeed;
	Vector3f m_rotationDirection;
	float m_totalTime;
};

