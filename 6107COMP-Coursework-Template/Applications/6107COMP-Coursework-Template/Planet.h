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

class Light_Base;

using namespace Glyph3;

class Planet : public Actor
{
public:

	Planet();
	virtual ~Planet() {};

	void Initialize(MaterialPtr planetMaterial, std::wstring planetMesh = L"geosphere.obj");
	void Update(float deltaTime);

private:
	void UpdateEntities(float deltaTime);

private:
	float m_planetrotation;
	float m_rotationSpeed;
	Vector3f m_rotationDirection;
};

