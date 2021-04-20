#pragma once
#include "PathFollowingActor.h"
#include "FirstPersonCamera.h"

using namespace Glyph3;

class CinematicCamera : public FirstPersonCamera
{
public:
	CinematicCamera();
	~CinematicCamera();

	void updateCinematicCamera(float deltaTime);

	inline void setFocusObject(const Actor* pFocusObject) { m_focusObject = pFocusObject; };
	inline const Actor* getFocusObject() const { return m_focusObject; }

	void setPath(Path* pPath);
	const Path* getPath();

private:
	PathFollowingActor* m_pathfingActor;
	const Actor* m_focusObject;
};

