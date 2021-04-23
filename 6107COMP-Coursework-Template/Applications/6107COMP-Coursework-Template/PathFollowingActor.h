#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"

#include "RRTVertexDX11.h"
#include "Path.h"

class PathFollowingActor : public Actor
{
public:
	PathFollowingActor(float movementSpeed = 160.0f, float RotationSpeed = 1.0f);
	~PathFollowingActor();

	virtual void update(float deltaTime);

	inline const Path* getPath() const { return m_pPath; }
	void setPath(const Path* pPath);


private:
	void moveActor(float deltaTime);
	void resetActorPosition();

private:
	const Path* m_pPath;
	Vector3f m_actorDirection;
	Vector3f m_actorRefDirection;
	Vector3f m_actorTargetDirection;
	float m_actorMovementSpeed;
	float m_actorRotationSpeed;
	float m_actorPrevDist2Target;
	int m_actorState;

	int m_currentCheckpointID;
	int m_nextCheckpointID;
};

