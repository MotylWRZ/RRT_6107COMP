#include "CinematicCamera.h"

CinematicCamera::CinematicCamera()
{
	this->m_pathfingActor = new PathFollowingActor();
}

CinematicCamera::~CinematicCamera()
{
	if (this->m_pathfingActor)
	{
		delete this->m_pathfingActor;
	}
}

void CinematicCamera::updateCinematicCamera(float deltaTime)
{

	this->m_pathfingActor->update(deltaTime);

	//this->Spatial().SetTranslation(this->m_pathfingActor->GetNode()->Position());

	if (!this->m_focusObject)
	{
		return;
	}












	// V1
//
//	Vector3f axisZ = Vector3f(tLookat - tPos);
//	axisZ.Normalize();
//	Vector3f axisX = Vector3f::Cross(tObjectUpVector, axisZ);
//	axisX.Normalize();
//	Vector3f axisY = Vector3f::Cross(axisZ, axisX);
//	axisY.Normalize();
//
//	Matrix4f pm = {
//	axisX.x, axisX.y, axisX.z, 0,
//	axisY.x, axisY.y, axisY.z, 0,
//	axisZ.x, axisZ.y, axisZ.z, 0,
//	0, 0, 0, 1
//	};
//
//	Matrix4f a = this->GetCameraView()->GetProjMatrix();
////	this->m_pCameraView->SetProjMatrix(pm);
//
//	this->Spatial().RotateBy(Vector3f(1.0f, 0.0f, 0.0f));

	//V2









	// v2

	Vector3f tObjectPos = this->m_pathfingActor->GetNode()->Position();
	Vector3f tPos = this->Spatial().GetTranslation();

	Vector3f tTargetDir = tObjectPos - tPos;
	tTargetDir.Normalize();

	// Calculate Camera unit vector
	float tCameraMag = tPos.Magnitude();
	Vector3f tCameraUnitVec = tPos / tCameraMag;
	tCameraUnitVec.Normalize();

	//// Calculate object unit vector
	//float tObjectMag = tObjectPos.Magnitude();
	//Vector3f tObjectUnitVec = tObjectPos / tObjectMag;
	//tObjectUnitVec.Normalize();

	//this->Spatial().SetRotation(tObjectUnitVec);

	float angle = acos(tCameraUnitVec.Dot(tTargetDir));

	Vector3f axis = tCameraUnitVec.Cross(tTargetDir);
	axis.Normalize();


	Matrix3f tstartRotation;
	tstartRotation.RotationEuler(axis, angle);
	this->GetBody()->Rotation() *= tstartRotation * deltaTime;


	// V2










	//angle = angle + 3.0f * deltaTime;
	//Vector3f axis2 = tCameraUnitVec.Cross(tObjectUnitVec);
	//axis.Normalize();

	//Matrix3f tstartRotation;
	//tstartRotation.RotationEuler(axis2, 3.0f * deltaTime);
	//this->GetNode()->Rotation() *= tstartRotation;
	///*m_actorDirection = this->GetNode()->Rotation() * m_actorRefDirection;
	//m_actorDirection.Normalize();*/







	/*Vector3f axis = m_actorRefDirection.Cross(m_actorTargetDirection);
	axis.Normalize();

	Matrix3f tstartRotation;
	tstartRotation.RotationEuler(axis, angle);
	this->GetNode()->Rotation() *= tstartRotation;

	m_actorTargetDirection = tPathPoints[m_nextCheckpointID] - this->GetNode()->Position();
	m_actorPrevDist2Target = m_actorTargetDirection.Magnitude();

	m_actorState = 0;*/

	Vector3f tDir = this->Spatial().GetTranslation() - this->m_pathfingActor->GetNode()->Position() ;
	tDir.Normalize();

	//this->Spatial().SetRotation(Vector3f(0.0f, tDir.y, 0.0f));

}

void CinematicCamera::setPath(Path* pPath)
{
	if (!this->m_pathfingActor || !pPath)
	{
		return;
	}

	this->m_pathfingActor->setPath(pPath);
}

inline const Path* CinematicCamera::getPath()
{
	if (!this->m_pathfingActor)
	{
		return nullptr;
	}

	this->m_pathfingActor->getPath();
}
