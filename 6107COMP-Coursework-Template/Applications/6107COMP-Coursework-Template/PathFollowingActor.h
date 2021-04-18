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
	PathFollowingActor(float movementSpeed = 160.0f, float RotationSpeed = 1.0f, bool generateDefaultPath = false);
	~PathFollowingActor();

	void Update(float deltaTime);

	inline const std::shared_ptr<Path> getPath() const { return m_pPath; }
	void generateNewPath(EPathType pathType, float centerX, float centerY, float radius, float height, float start, float end, float increment);
	void setPath(std::shared_ptr<Path> pPath);
	void addPathActorToScene(Scene* pScene);
	void removePathActorFromScene(Scene* pScene);

private:
	void moveActor(float deltaTime);

private:
	std::shared_ptr<Path>  m_pPath;
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

