#pragma once
#include "PathFollowingActor.h"
#include "FirstPersonCamera.h"



using namespace Glyph3;
const float DEG_TO_RAD2 = GLYPH_PI / 180.0f;
class CinematicCamera : public FirstPersonCamera
{
public:
	CinematicCamera();
	~CinematicCamera();

	void updateCinematicCamera(float deltaTime, RendererDX11* renderer);

	inline void setFocusObject(const Actor* pFocusObject) { m_focusObject = pFocusObject; };
	inline const Actor* getFocusObject() const { return m_focusObject; }

private:
	const Actor* m_focusObject;
	float m_angle;
	float m_rotationSpeed;
	Vector3f m_PositionOffset;
	float m_circularMovementRadius;
};

