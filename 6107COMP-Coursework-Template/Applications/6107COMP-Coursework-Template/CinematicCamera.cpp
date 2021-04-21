#include "CinematicCamera.h"
#include "Win32RenderWindow.h"
CinematicCamera::CinematicCamera()
	:m_angle(0)
	, m_PositionOffset(Vector3f(300.0f, 600.0f, 300.0f))
	, m_rotationSpeed(0.7f)
	, m_circularMovementRadius(200.0f)
{
}

CinematicCamera::~CinematicCamera()
{

}

void CinematicCamera::updateCinematicCamera(float deltaTime, RendererDX11* renderer)
{
	if (!this->m_focusObject)
	{
		return;
	}

	this->Spatial().SetTranslation(this->m_focusObject->GetNode()->Position());

	// Set the position offset
	this->Spatial().TranslateXBy(this->m_PositionOffset.x);
	this->Spatial().TranslateYBy(this->m_PositionOffset.y);
	this->Spatial().TranslateZBy(this->m_PositionOffset.z);

	// Rotate around a Y axis
	this->Spatial().RotateYBy(0.1f * DEG_TO_RAD2);

	m_angle += this->m_rotationSpeed * deltaTime;

	// Move around the focus object
	Vector3f tNewPos = Vector3f(this->m_focusObject->GetNode()->Position().x + (this->m_circularMovementRadius * cos(m_angle)), this->Spatial().GetTranslation().y, this->m_focusObject->GetNode()->Position().z + (this->m_circularMovementRadius * sin(m_angle)));
	this->Spatial().SetTranslation(tNewPos);
}
