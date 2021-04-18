#include "Path.h"

#include "RasterizerStateConfigDX11.h"

#include "PathGenerator.h"
#include "MaterialGenerator.h"

Path::Path()
{
}

Path::~Path()
{
}

void Path::generatePath(EPathType pathType, float centerX, float centerY, float radius, float height, float start, float end, float increment)
{
	switch (pathType)
	{
	case EPathType::Path_Linear:
	{
		PathGenerator::createLinearPath(centerX, centerY, radius, height, start, end, increment, this->m_pathPoints);
		break;
	}
	case EPathType::Path_CatmullRom:
	{
		PathGenerator::createCatmullRomPath(centerX, centerY, radius, height, start, end, increment, this->m_pathPoints);
		break;
	}
	case EPathType::Path_Hermite:
	{
		PathGenerator::createHermitePath(centerX, centerY, radius, height, start, end, increment, this->m_pathPoints);
		break;
	}
	}
}

void Path::addPathActorToScene(Scene* pScene, RendererDX11* pRenderer)
{
	if (!pScene || !pRenderer)
	{
		return;
	}

	BasicMeshPtr tMesh = PathGenerator::generatePathMesh(this->m_pathPoints);
	MaterialPtr tMat = MaterialGenerator::createBasicMaterial(*pRenderer);



	//// Modify ratserizer state
	//RenderEffectDX11* tEffect = tMat->Params[VT_PERSPECTIVE].pEffect;

	//RasterizerStateConfigDX11 rsConfig;
	//RasterizerStateComPtr rstatePtr = pRenderer->GetRasterizerState(tEffect->m_iRasterizerState);
	//rstatePtr->GetDesc(&rsConfig);

	//// Set FrontCulling
	//rsConfig.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

	//int iRasterizerState = pRenderer->CreateRasterizerState(&rsConfig);
	//tEffect->m_iRasterizerState = iRasterizerState;
	//tMat->Params[VT_PERSPECTIVE].bRender = true;
	//tMat->Params[VT_PERSPECTIVE].pEffect = tEffect;


	this->GetBody()->SetGeometry(tMesh);
	this->GetBody()->SetMaterial(tMat);

	pScene->AddActor(this);
}

void Path::removePathActorFromScene(Scene* pScene)
{
	pScene->RemoveActor(this);
}
