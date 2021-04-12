//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

// API Header Files
#include "TerrainGenerator.h"
#include "GeometryGenerator.h"
#include "MaterialGenerator.h"
#include "Light_Directional.h"
#include "Light_Point.h"
#include "Light_Spot.h"
#include "MeshImporter.h"
#include "InstancedStaticMesh.h"
#include "SolarSystem.h"

//------------DX TK AND STD/STL Includes-------------------------------------
#include <sstream>

//------------Include Hieroglyph Engine Files--------------------------------

//Include the Logging System
#include "Log.h"

//Include the Event System
#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"
#include "ScriptManager.h"

//Include the DirectX Rendering Components
#include "PipelineManagerDX11.h"
#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "MaterialGeneratorDX11.h"

#include "FirstPersonCamera.h"

//Add a Using Directive to avoid typing Glyph3 for basic constructs
using namespace Glyph3;
//Include our own application Namespace
using namespace LJMUDX;

LJMULevelDemo AppInstance;
//---------CONSTRUCTORS-------------------------------------------------------

///////////////////////////////////////
//
///////////////////////////////////////
LJMULevelDemo::LJMULevelDemo():
m_pRender_text(nullptr),
m_pRenderView(nullptr),
m_pCamera(nullptr),
m_pRenderer11(nullptr),
m_pWindow(nullptr),
m_iSwapChain(0),
m_DepthTarget(nullptr),
m_RenderTarget(nullptr)
{

}

//---------METHODS------------------------------------------------------------

//////////////////////////////////////
// Get the Window Name of this Application
//////////////////////////////////////
std::wstring LJMULevelDemo::GetName()
{
	return(std::wstring(L"6107COMP: Coursework Template"));
}

/////////////////////////////////////
// Assemble our Input Layouts for this
// Stage of the Pipeline.
/////////////////////////////////////
void LJMULevelDemo::inputAssemblyStage()
{
	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------
	this->setupLighting();
	this->setupGeometry();
}

void LJMULevelDemo::setupGeometry()
{
	MaterialReflectanceInfo tMatInfo;
	tMatInfo.SurfaceEmissiveColour = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	tMatInfo.Ambient = 0.0f;
	tMatInfo.Diffuse = 0.0f;
	tMatInfo.Specular = 0.0f;
	tMatInfo.Shininess = 1.0f;





	// Create test Landscape
	this->m_LandscapeActor = TerrainGenerator::createTerrainActor(0, 0, 200, 3);
	MaterialPtr tTerrainMaterial = MaterialGenerator::createLitBumpTexturedMaterial(*this->m_pRenderer11, std::wstring(L"rocks_ground_06_diff_2k.tiff"), std::wstring(L"rocks_ground_06_nor_2k.tiff"), this->m_lights, tMatInfo);

	this->m_LandscapeActor->GetBody()->SetMaterial(tTerrainMaterial);
	this->m_LandscapeActor->GetNode()->Position() = Vector3f(100.0f, 30.0f, -5.0f);
	this->m_LandscapeActor->GetNode()->Scale() = Vector3f(1, 1, 1);
	this->m_pScene->AddActor(this->m_LandscapeActor);


	// Create test Cube Mesh and Actor
	this->m_CubeActor = new Actor();
	BasicMeshPtr tMesh = GeometryGenerator::generateRectangle();
	this->m_CubeActor->GetBody()->SetGeometry(tMesh);
	MaterialPtr tCubeMaterial = MaterialGenerator::createBasicMaterial(*this->m_pRenderer11);
	this->m_CubeActor->GetBody()->SetMaterial(tCubeMaterial);
	this->m_CubeActor->GetNode()->Position() = Vector3f(200.0f, 50.0f, 100.0f);
	this->m_CubeActor->GetNode()->Scale() = Vector3f(1, 1, 1);
	this->m_pScene->AddActor(this->m_CubeActor);


	Actor* tPlanetActor = new Actor();
	BasicMeshPtr tMesh2 = MeshImporter::generateMeshOBJWithSurfaceVectors(L"geosphere.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tPlanetMaterial = MaterialGenerator::createLitBumpTexturedMaterial(*this->m_pRenderer11, std::wstring(L"rocks_ground_06_diff_2k.tiff"), std::wstring(L"rocks_ground_06_nor_2k.tiff"), this->m_lights, tMatInfo);// MaterialGenerator::createTextureMaterial(*this->m_pRenderer11, L"RRTTextureMapping.hlsl", L"brown_mud_dry_diff_2k.tiff");
	tPlanetActor->GetBody()->SetGeometry(tMesh2);
	tPlanetActor->GetBody()->SetMaterial(tPlanetMaterial);
	tPlanetActor->GetNode()->Position() = Vector3f(200.0f, 50.0f, 100.0f);
	tPlanetActor->GetNode()->Scale() = Vector3f(1, 1, 1);
	//this->m_pScene->AddActor(tPlanetActor);



	//MaterialPtr tInstMaterial = MaterialGenerator::createGSInstancingMaterial(*this->m_pRenderer11, std::wstring(L"rocks_ground_06_diff_2k.tiff"));
	this->m_planet = new Planet();
	this->m_planet->Initialize(tPlanetMaterial);
	this->m_planet->GetBody()->Position() = Vector3f(200.0f, 50.0f, 100.0f);
	this->m_pScene->AddActor(this->m_planet);


	this->m_solarSystem = std::make_shared<SolarSystem>();

	this->m_solarSystem->addISM(tMesh2, Vector3f(400.0f, 300.0f, 300.0f), this->m_pRenderer11, this->m_pScene,
		L"rocks_ground_06_diff_2k.tiff",
		L"brown_mud_dry_diff_2k.tiff", L"mars.tif");

	Vector3f tInstancePos1(30.0f, 30.0f, 30.0f);
Vector3f tInstancePos2(30.0f, -30.0f, 30.0f);
Vector3f tInstancePos3(30.0f, 30.0f, -30.0f);
Vector3f tInstancePos4(30.0f, -30.0f, -30.0f);
Vector3f tInstancePos5(130.0f, 130.0f, 130.0f);
Vector3f tInstancePos6(130.0f, -130.0f, 130.0f);
Vector3f tInstancePos7(130.0f, 130.0f, -130.0f);
Vector3f tInstancePos8(130.0f, -130.0f, -130.0f);


	this->m_solarSystem->addPlanet(0, tInstancePos1, EInstanceTexture::TEXTURE1);
	this->m_solarSystem->addPlanet(0, tInstancePos2, EInstanceTexture::TEXTURE1);
	this->m_solarSystem->addPlanet(0, tInstancePos3, EInstanceTexture::TEXTURE3);
	this->m_solarSystem->addPlanet(0, tInstancePos4, EInstanceTexture::TEXTURE3);
	this->m_solarSystem->addPlanet(0, tInstancePos5, EInstanceTexture::TEXTURE2);
	this->m_solarSystem->addPlanet(0, tInstancePos6, EInstanceTexture::TEXTURE2);
	this->m_solarSystem->addPlanet(0, tInstancePos7, EInstanceTexture::TEXTURE1);


	this->setupSkySphere();

}

void LJMULevelDemo::animateGeometry(float DT)
{
}

void LJMULevelDemo::setupCamera()
{
	this->m_pCamera = new FirstPersonCamera();
	this->m_pCamera->SetEventManager(&this->EvtManager);
	Vector3f tCameraPos(100.0f, 30.0f, -5.0f);
	this->m_pCamera->Spatial().SetTranslation(tCameraPos);
	this->m_pCamera->Spatial().RotateXBy(20 * DEG_TO_RAD);

	this->m_pRenderView = new ViewPerspective(*this->m_pRenderer11, this->m_RenderTarget,
		this->m_DepthTarget);
	this->m_pRenderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	this->m_pCamera->SetCameraView(this->m_pRenderView);

	this->m_pRender_text = new LJMUTextOverlay(*this->m_pRenderer11, this->m_RenderTarget,
		std::wstring(L"Cambria"),
		25);
	this->m_pCamera->SetOverlayView(this->m_pRender_text);
	this->m_pCamera->SetProjectionParams(0.1f, 1000000.0f, this->m_iscreenWidth / this->m_iscreenHeight,
		static_cast<float>(GLYPH_PI) / 2.0f);
	this->m_pScene->AddCamera(this->m_pCamera);
}

void LJMUDX::LJMULevelDemo::addLight(LightBasePtr pLight)
{
	// Check if the maximum number of lights is not exceeded
	if (this->m_lights.size() < LIGHTS_NUM_MAX)
	{
		this->m_lights.push_back(pLight);
	}
}

void LJMUDX::LJMULevelDemo::setupLighting()
{
	Vector4f tLightColour(0.0f, 1.0f, 0.9f, 1.0f);
	Vector3f tLightPosition(100.0f, 30.0f, 300.0f);
	Vector2f tLightRange(100.0f, 0.0f);

	Vector4f tLightColour2(1.0f, 0.0f, 0.9f, 1.0f);
	Vector3f tLightPosition2(300.0f, 0.0f, 400.0f);
	Vector2f tLightRange2(100.0f, 0.0f);

	Vector4f tLightColour3(1.0f, 1.0f, 1.0f, 1.0f);
	Vector3f tLightPosition3(300.0f, 30.0f, 100.0f);
	Vector2f tLightRange3(40.0f, 0.0f);


	//Vector4f tDirLightColour(0.1f, 0.1f, 0.7f, 1.0f);
	Vector4f tDirLightColour(0.2f, 0.2f, 0.3f, 1.0f);
	Vector3f tDirLightDirection(0.0f, 0.0f, 1.0f);
	tDirLightDirection.Normalize();

	Vector4f SpotLightColour(0.0f, 1.0f, 0.0f, 1.0f);
	Vector3f SpotLightDirection(0.0f, -1.0f, 0.0f);
	Vector3f SpotLightPosition(400.0f, 100.0f, 100.0f);
	Vector2f SpotLightRange(100.0f, 0.0f);
	Vector2f SpotLightFocus(1.0f, 0.0f);
	//SpotLightDirection.Normalize();

	LightBasePtr tPointLight = std::make_shared<Light_Point>(tLightColour, tLightPosition, tLightRange);
	LightBasePtr tPointLight2 = std::make_shared<Light_Point>(tLightColour2, tLightPosition2, tLightRange2);
	LightBasePtr tPointLight3 = std::make_shared<Light_Point>(tLightColour3, tLightPosition3, tLightRange3);
	LightBasePtr tDirectionalLight = std::make_shared<Light_Directional>(tDirLightColour, tDirLightDirection);
	LightBasePtr tSpotLight = std::make_shared<Light_Spot>(SpotLightColour, SpotLightPosition, SpotLightDirection, SpotLightRange, SpotLightFocus);

	this->addLight(tPointLight);
	this->addLight(tPointLight2);
	this->addLight(tPointLight3);
	this->addLight(tDirectionalLight);
	this->addLight(tSpotLight);

	//for (int i = 0; i < 200;  i++)
	//{
	//	tLightPosition.x += 30.0f;
	//	//tLightColour.x += 0.1f;
	//	tLightColour.x += 0.1;
	//	tLightColour.z -= 0.1;

	//	LightBasePtr tPointLight = std::make_shared<Light_Point>(tLightColour, tLightPosition, tLightRange);

	//	this->addLight(tPointLight);
	//}
}

void LJMUDX::LJMULevelDemo::setupSkySphere()
{
	// Setup Skyphere
	Actor* tSkySphereActor = new Actor();

	tSkySphereActor->GetNode()->Position() = Vector3f(200.0f, 50.0f, 100.0f);;
	BasicMeshPtr tSkySphereMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"geosphere.obj", Vector4f(1, 1, 1, 1));
	tSkySphereActor->GetBody()->SetGeometry(tSkySphereMesh);
	MaterialPtr tSkySphereMaterial = MaterialGenerator::createTextureMaterial(*this->m_pRenderer11, L"RTRSkySphere.hlsl", L"stars.tif");


	tSkySphereActor->GetBody()->SetMaterial(tSkySphereMaterial);
	tSkySphereActor->GetBody()->Scale() = Vector3f(10000.0f, 10000.0f, 10000.0f);

	// Modify ratserizer state
	MaterialPtr tMat = tSkySphereActor->GetBody()->GetMaterial();
	RenderEffectDX11* tEffect = tMat->Params[VT_PERSPECTIVE].pEffect;

	// Modify the rasterizer state
	RasterizerStateConfigDX11 rsConfig;
	RasterizerStateComPtr rstatePtr = this->m_pRenderer11->GetRasterizerState(tEffect->m_iRasterizerState);
	rstatePtr->GetDesc(&rsConfig);

	// Set FrontCulling
	rsConfig.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

	int iRasterizerState = this->m_pRenderer11->CreateRasterizerState(&rsConfig);
	tEffect->m_iRasterizerState = iRasterizerState;
	tMat->Params[VT_PERSPECTIVE].bRender = true;
	tMat->Params[VT_PERSPECTIVE].pEffect = tEffect;

	this->m_actors.push_back(tSkySphereActor);

	this->m_pScene->AddActor(tSkySphereActor);
}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	this->inputAssemblyStage();
	this->setupCamera();

	///////////////////////////////////////////////////////////////////////////////////////
	/*this->m_pCamera = new Camera();

	Vector3f tcamerapos(0.0f, 20.0f, -50.0f);
	this->m_pCamera->Spatial().SetTranslation(tcamerapos);

	this->m_pRenderView = new ViewPerspective(*this->m_pRenderer11,
		                                     this->m_RenderTarget, this->m_DepthTarget);
	this->m_pRenderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	this->m_pCamera->SetCameraView(this->m_pRenderView);

	this->m_pRender_text = new LJMUTextOverlay(*this->m_pRenderer11,
		                                      this->m_RenderTarget,
											  std::wstring(L"Cambria"),
											  25);

	this->m_pCamera->SetOverlayView(this->m_pRender_text);


	this->m_pCamera->SetProjectionParams(0.1f, 1000.0f, m_iscreenWidth / m_iscreenHeight,
		                                   static_cast<float>(GLYPH_PI) / 2.0f);

	this->m_pScene->AddCamera(this->m_pCamera);*/

	//LightInfo tLights[10];

	//BufferConfigDX11 tBuffConfig;
	////tBuffConfig.SetDefaultConstantBuffer(LIGHTS_NUM_MAX * sizeof(LightInfo), true);
	//tBuffConfig.SetByteWidth(LIGHTS_NUM_MAX * sizeof(LightInfo));
	//tBuffConfig.SetBindFlags(D3D11_BIND_CONSTANT_BUFFER);
	//tBuffConfig.SetMiscFlags(0);
	//tBuffConfig.SetStructureByteStride(0);
	//tBuffConfig.SetUsage(D3D11_USAGE_DEFAULT);
	//tBuffConfig.SetCPUAccessFlags(0);

	//D3D11_SUBRESOURCE_DATA dataLights;
	//dataLights.pSysMem = tLights;
	//dataLights.SysMemPitch = 0;
	//dataLights.SysMemSlicePitch = 0;

	//Vec = this->m_pRenderer11->m_pParamMgr->GetVectorParameterRef(L"SurfaceConstants");
	//EmissiveCol = this->m_pRenderer11->m_pParamMgr->GetVectorParameterRef(L"SurfaceEmissiveColour");

	//m_Buf2 = this->m_pRenderer11->CreateConstantBuffer(&tBuffConfig, &dataLights);
	//this->m_pRenderer11->m_pParamMgr->SetConstantBufferParameter(L"cLights", m_Buf2);
	//m_Buf = this->m_pRenderer11->m_pParamMgr->GetConstantBufferParameterRef(L"cLights");
}

///////////////////////////////////
// Update the State of our Game and
// Output the Results to Screen (Render)
///////////////////////////////////
void LJMULevelDemo::Update()
{
	this->m_pTimer->Update();
	EvtManager.ProcessEvent(EvtFrameStartPtr( new EvtFrameStart(this->m_pTimer->Elapsed())));

	float tDT = this->m_pTimer->Elapsed();

	LightBasePtr tLight = this->m_lights[2];
	static float speed = 0.0f;
	speed += 0.001f;
	LightInfo tInfo;
	tInfo = tLight->getLightInfo();
	tInfo.LightPosition.x += cos(speed) * 0.2f;

	tLight->setLightInfo(tInfo);

	MaterialReflectanceInfo tMatInfo;
	tMatInfo.SurfaceEmissiveColour = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	tMatInfo.Ambient = 0.0f;
	tMatInfo.Diffuse = 0.0f;
	tMatInfo.Specular = 0.0f;
	tMatInfo.Shininess = 1.0f;

	MaterialPtr tMat = this->m_LandscapeActor->GetBody()->GetMaterial();
	MaterialPtr tPlanetMat = this->m_planet->GetBody()->GetMaterial();

	if (tMat && tPlanetMat)
	{
		MaterialGenerator::updateMaterialLight(*this->m_pRenderer11, tPlanetMat, this->m_lights, tMatInfo);
		MaterialGenerator::updateMaterialLight(*this->m_pRenderer11, tMat, this->m_lights, tMatInfo);
	}

	this->m_planet->Update(tDT);
	this->m_solarSystem->Update(tDT);
	//this->m_pInstancedStaticMesh->GetNode()->Position() += Vector3f(0.01f, 0.01f, 0.01f);

	//----------START RENDERING--------------------------------------------------------------
		this->m_pScene->Update(m_pTimer->Elapsed());
		this->m_pScene->Render(this->m_pRenderer11);
	//--------END RENDERING-------------------------------------------------------------
	this->m_pRenderer11->Present(this->m_pWindow->GetHandle(), this->m_pWindow->GetSwapChain());
}

///////////////////////////////////
// Configure the DirectX 11 Programmable
// Pipeline Stages and Create the Window
// Calls
///////////////////////////////////
bool LJMULevelDemo::ConfigureEngineComponents()
{

	// Set the render window parameters and initialize the window
	this->m_pWindow = new Win32RenderWindow();
	this->m_pWindow->SetPosition(25, 25);
	this->m_pWindow->SetSize(m_iscreenWidth, m_iscreenHeight);
	this->m_pWindow->SetCaption(this->GetName());
	this->m_pWindow->Initialize(this);


	// Create the renderer and initialize it for the desired device
	// type and feature level.
	this->m_pRenderer11 = new RendererDX11();

	if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0))
	{
		Log::Get().Write(L"Could not create hardware device, trying to create the reference device...");

		if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0))
		{
			ShowWindow(this->m_pWindow->GetHandle(), SW_HIDE);
			MessageBox(this->m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device!", L"5108COMP Coursework Template", MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
			this->RequestTermination();
			return(false);
		}
		// If using the reference device, utilize a fixed time step for any animations.
		this->m_pTimer->SetFixedTimeStep(1.0f / 10.0f);
	}

	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.
	SwapChainConfigDX11 tconfig;
	tconfig.SetWidth(this->m_pWindow->GetWidth());
	tconfig.SetHeight(this->m_pWindow->GetHeight());
	tconfig.SetOutputWindow(this->m_pWindow->GetHandle());
	this->m_iSwapChain = this->m_pRenderer11->CreateSwapChain(&tconfig);
	this->m_pWindow->SetSwapChain(this->m_iSwapChain);

	//Create Colour and Depth Buffers
	this->m_RenderTarget = this->m_pRenderer11->GetSwapChainResource(this->m_iSwapChain);

	Texture2dConfigDX11 tdepthconfig;
	tdepthconfig.SetDepthBuffer(m_iscreenWidth, m_iscreenHeight);
	this->m_DepthTarget = this->m_pRenderer11->CreateTexture2D(&tdepthconfig, 0);

	// Bind the swap chain render target and the depth buffer for use in rendering.
	this->m_pRenderer11->pImmPipeline->ClearRenderTargets();
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, this->m_RenderTarget->m_iResourceRTV);
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState(this->m_DepthTarget->m_iResourceDSV);
	this->m_pRenderer11->pImmPipeline->ApplyRenderTargets();

	D3D11_VIEWPORT tviewport;
	tviewport.Width = static_cast< float >(m_iscreenWidth);
	tviewport.Height = static_cast< float >(m_iscreenHeight);
	tviewport.MinDepth = 0.0f;
	tviewport.MaxDepth = 1.0f;
	tviewport.TopLeftX = 0;
	tviewport.TopLeftY = 0;

	int tvpindex = this->m_pRenderer11->CreateViewPort(tviewport);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState(1);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState(0, tvpindex);
	return(true);
}

//////////////////////////////////
//Handle Input Events in the Application
//////////////////////////////////
bool LJMULevelDemo::HandleEvent(EventPtr pevent)
{
	eEVENT e = pevent->GetEventType();

	if (e == SYSTEM_KEYBOARD_KEYDOWN)
	{
		EvtKeyDownPtr tkey_down = std::static_pointer_cast<EvtKeyDown>(pevent);
		unsigned int  tkeycode = tkey_down->GetCharacterCode();
	}
	else if (e == SYSTEM_KEYBOARD_KEYUP)
	{
		EvtKeyUpPtr tkey_up = std::static_pointer_cast<EvtKeyUp>(pevent);
		unsigned int tkeycode = tkey_up->GetCharacterCode();
	}

	return(Application::HandleEvent(pevent));
}

//////////////////////////////////
// Destroy Resources created by the engine
//////////////////////////////////
void LJMULevelDemo::ShutdownEngineComponents()
{
	if (this->m_pRenderer11)
	{
		this->m_pRenderer11->Shutdown();
		delete this->m_pRenderer11;
	}

	if (this->m_pWindow)
	{
		this->m_pWindow->Shutdown();
		delete this->m_pWindow;
	}
}

//////////////////////////////////
// Shutdown the Application
//////////////////////////////////
void LJMULevelDemo::Shutdown()
{
	//NOTHING TO DO HERE
}

//////////////////////////////////
// Take a Screenshot of the Application
//////////////////////////////////
void LJMULevelDemo::TakeScreenShot()
{
	if (this->m_bSaveScreenshot)
	{
		this->m_bSaveScreenshot = false;
		this->m_pRenderer11->pImmPipeline->SaveTextureScreenShot(0, this->GetName());
	}
}

//////////////////////////////////////
// Output our Frame Rate
//////////////////////////////////////
std::wstring LJMULevelDemo::outputFPSInfo()
{
	std::wstringstream out;
	out << L"FPS: " << m_pTimer->Framerate();
	return out.str();
}