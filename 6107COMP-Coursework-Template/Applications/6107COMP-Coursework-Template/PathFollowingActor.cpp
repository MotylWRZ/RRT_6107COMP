#include "PathFollowingActor.h"

PathFollowingActor::PathFollowingActor(Scene* pScene, float movementSpeed, float rotationSpeed, bool generateDefaultPath)
	: m_actorMovementSpeed(movementSpeed)
	, m_actorRotationSpeed(rotationSpeed)
	, m_actorState(0)
	, m_pScene(pScene)
{
	if (generateDefaultPath)
	{
		this->generateNewPath(EPathType::Path_CatmullRom, 1250.0f, 1250.0f, 1000.0f, 256.0f, -180.0f, 180.0f, 15);
	}
}

PathFollowingActor::~PathFollowingActor()
{
}

void PathFollowingActor::Update(float deltaTime)
{
	if (!this->m_pPath)
	{
		return;
	}

	this->moveActor(deltaTime);

}

void PathFollowingActor::generateNewPath(EPathType pathType, float centerX, float centerY, float radius, float height, float start, float end, float increment)
{
	if (!this->m_pPath)
	{
		this->m_pPath = new Path();// std::make_shared<Path>();
	}

	this->m_pPath->generatePath(pathType, centerX, centerY, radius, height, start, end, increment);


	// Placing the actor at the start checkpoint and facing the next checkpoint
	m_currentCheckpointID = 0;
	m_nextCheckpointID = m_currentCheckpointID + 1;
	m_actorState = 0;

	Matrix3f tstartRotation;
	m_actorRefDirection = Vector3f(-1, 0, 0);
	m_actorDirection = m_actorRefDirection;
	m_actorTargetDirection = this->m_pPath->getPathPoints()[m_nextCheckpointID] - this->m_pPath->getPathPoints()[m_currentCheckpointID];
	m_actorPrevDist2Target = m_actorTargetDirection.Magnitude();
	m_actorTargetDirection.Normalize();

	float angle = acos(m_actorRefDirection.Dot(m_actorTargetDirection));

	Vector3f axis = m_actorRefDirection.Cross(m_actorTargetDirection);
	axis.Normalize();

	tstartRotation.RotationEuler(axis, angle);

	this->GetNode()->Rotation() = tstartRotation;
	this->GetNode()->Position() = this->m_pPath->getPathPoints()[m_currentCheckpointID];
}

//void PathFollowingActor::setPath(std::shared_ptr<Path> pPath)
//{
//	this->m_pPath = pPath;
//}

void PathFollowingActor::addPathActorToScene(Scene* pScene, RendererDX11* pRenderer)
{
	if (this->m_pPath)
	{
		this->m_pPath->addPathActorToScene(pScene, pRenderer);
	}
}

void PathFollowingActor::removePathActorFromScene(Scene* pScene)
{
	if (this->m_pPath)
	{
		this->m_pPath->removePathActorFromScene(pScene);
	}
}

void PathFollowingActor::moveActor(float deltaTime)
{
	const std::vector<Vector3f>& tPathPoints = this->m_pPath->getPathPoints();

	// #13: Get the elapsed time since the last update
	float tpf = deltaTime;

	if (tpf > 10.0f / 60.0f)
	{
		// if this is triggered, most likely the user is debugging the code,
		// hence time per frame is much larger than 10x 60fps then

		tpf = 1 / 60.0f;
	}

	// #14: When the spacehip is moving towards the next checkpoint
	if (m_actorState == 0)
	{
		m_actorTargetDirection = tPathPoints[m_nextCheckpointID] - this->GetNode()->Position();
		float cur_dist = m_actorTargetDirection.Magnitude();
		m_actorTargetDirection.Normalize();
		m_actorDirection = m_actorTargetDirection;

		if (cur_dist > m_actorPrevDist2Target)
		{
			m_currentCheckpointID++;
			m_nextCheckpointID++;
			m_currentCheckpointID = m_currentCheckpointID % tPathPoints.size();
			m_nextCheckpointID = m_nextCheckpointID % tPathPoints.size();
			this->GetNode()->Position() = tPathPoints[m_currentCheckpointID];

			m_actorTargetDirection = tPathPoints[m_nextCheckpointID] - this->GetNode()->Position();
			cur_dist = m_actorTargetDirection.Magnitude();

			m_actorPrevDist2Target = cur_dist;

			m_actorTargetDirection.Normalize();

			float angle = acos(m_actorDirection.Dot(m_actorTargetDirection));
			if (angle > m_actorRotationSpeed * tpf)
			{
				m_actorState = 1;
			}
		}
		else
		{
			this->GetNode()->Position() = this->GetNode()->Position() +
				m_actorDirection *
				m_actorMovementSpeed * tpf;
			m_actorPrevDist2Target = cur_dist;
		}
	}

	// #15: When the spacehip is orienting to face the next checkpoint
	else
	{
		float angle = acos(m_actorDirection.Dot(m_actorTargetDirection));
		if (fabs(angle) < m_actorRotationSpeed * tpf)
		{
			Vector3f axis = m_actorRefDirection.Cross(m_actorTargetDirection);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, angle);
			this->GetNode()->Rotation() *= tstartRotation;

			m_actorTargetDirection = tPathPoints[m_nextCheckpointID] - this->GetNode()->Position();
			m_actorPrevDist2Target = m_actorTargetDirection.Magnitude();

			m_actorState = 0;
		}
		else
		{
			angle = angle + m_actorRotationSpeed * tpf;
			Vector3f axis = m_actorDirection.Cross(m_actorTargetDirection);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, m_actorRotationSpeed * tpf);
			this->GetNode()->Rotation() *= tstartRotation;
			m_actorDirection = this->GetNode()->Rotation() * m_actorRefDirection;
			m_actorDirection.Normalize();
		}
	}
}
