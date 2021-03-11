//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

// API Header Files
#include "TerrainGenerator.h"
#include "GeometryGenerator.h"

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
m_RenderTarget(nullptr),
m_sceneActive(ESceneActive::UNIVERSE)
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
	this->setupGeometry();
}

void LJMULevelDemo::setupGeometry()
{
	// Create test Landscape
	this->m_LandscapeActor = TerrainGenerator::createTerrainActor();
	MaterialPtr tTerrainMaterial = this->createBasicMaterial();

	this->m_LandscapeActor->GetBody()->SetMaterial(tTerrainMaterial);
	this->m_LandscapeActor->GetNode()->Position() = Vector3f(100.0f, 30.0f, -5.0f);
	this->m_LandscapeActor->GetNode()->Scale() = Vector3f(1, 1, 1);
	this->m_pScene->AddActor(this->m_LandscapeActor);

	// Create test Cube Mesh and Actor
	this->m_CubeActor = new Actor();
	BasicMeshPtr tMesh = GeometryGenerator::generateRectangle();
	this->m_CubeActor->GetBody()->SetGeometry(tMesh);
	MaterialPtr tCubeMaterial = this->createBasicMaterial();
	this->m_CubeActor->GetBody()->SetMaterial(tCubeMaterial);
	this->m_CubeActor->GetNode()->Position() = Vector3f(100.0f, 30.0f, -5.0f);
	this->m_CubeActor->GetNode()->Scale() = Vector3f(1, 1, 1);
	this->m_pScene->AddActor(this->m_CubeActor);

	// Scene2
	this->m_ActorScene2 = new Actor();
	BasicMeshPtr tMesh2 = GeometryGenerator::generateCube();
	this->m_ActorScene2->GetBody()->SetGeometry(tMesh2);
	MaterialPtr tCubeMaterial2 = this->createBasicMaterial();
	this->m_ActorScene2->GetBody()->SetMaterial(tCubeMaterial2);
	this->m_ActorScene2->GetNode()->Position() = Vector3f(100.0f, 30.0f, -5.0f);
	this->m_ActorScene2->GetNode()->Scale() = Vector3f(1, 1, 1);
	this->m_scenePlanet->AddActor(this->m_ActorScene2);
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
	this->m_pCamera->SetProjectionParams(0.1f, 1000.0f, this->m_iscreenWidth / this->m_iscreenHeight,
		static_cast<float>(GLYPH_PI) / 2.0f);
	this->m_pScene->AddCamera(this->m_pCamera);

	this->m_pCamera2 = new FirstPersonCamera();
	*this->m_pCamera2 = *this->m_pCamera;

	this->m_scenePlanet->AddCamera(this->m_pCamera2);
}

void LJMULevelDemo::setupScenes()
{
	// Main scene is just a pointer to the pScene.
	//This scene wil be used for Universe rendering
	this->m_sceneMain = this->m_pScene;
	// Planet scene is an additional scene which should render planet terrain upclose
	this->m_scenePlanet = new Scene();
}

MaterialPtr LJMULevelDemo::createBasicMaterial()
{
	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	// Setup shader here
	tEffect->SetVertexShader(this->m_pRenderer11->LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(L"Basic.hlsl"),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(this->m_pRenderer11->LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(L"Basic.hlsl"),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));

	DepthStencilStateConfigDX11 tDSConfig;
	int tDepthStencilState = this->m_pRenderer11->CreateDepthStencilState(&tDSConfig);

	if (tDepthStencilState == -1)
	{
		Log::Get().Write(L"Failed to create light depth stencil state");
		assert(false);
	}

	tEffect->m_iDepthStencilState = tDepthStencilState;
	tEffect->m_uStencilRef = tDepthStencilState;

	BlendStateConfigDX11 blendConfig;
	blendConfig.AlphaToCoverageEnable = false;
	blendConfig.IndependentBlendEnable = false;

	for (int i = 0; i < 8; ++i)
	{
		blendConfig.RenderTarget[i].BlendEnable = true;
		blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	tEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState(&blendConfig);

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_BACK;
	int iRasterizerState = m_pRenderer11->CreateRasterizerState(&rsConfig);

	if (iRasterizerState == -1)
	{
		Log::Get().Write(L"Failed to create light ratserizer state");
		assert(false);
	}

	tEffect->m_iRasterizerState = iRasterizerState;

	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].bRender = true;
	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].pEffect = tEffect;



	return tNewMaterial;
}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	this->setupScenes();
	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	this->inputAssemblyStage();
	this->setupCamera();
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
}

///////////////////////////////////
// Update the State of our Game and
// Output the Results to Screen (Render)
///////////////////////////////////
void LJMULevelDemo::Update()
{
	this->m_pTimer->Update();
	EvtManager.ProcessEvent(EvtFrameStartPtr( new EvtFrameStart(this->m_pTimer->Elapsed())));

	//----------START RENDERING--------------------------------------------------------------

	switch (this->m_sceneActive)
	{
	case ESceneActive::UNIVERSE:
	{
		this->m_pScene->Update(m_pTimer->Elapsed());
		this->m_pScene->Render(this->m_pRenderer11);
		break;
	}
	case ESceneActive::PLANET:
	{
		this->m_scenePlanet->Update(m_pTimer->Elapsed());
		this->m_scenePlanet->Render(this->m_pRenderer11);
		break;
	}
	default:
		break;
	}


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


		if (tkeycode == VK_UP)
		{
			this->m_sceneActive = ESceneActive::PLANET;
		}
		if (tkeycode == VK_DOWN)
		{
			this->m_sceneActive = ESceneActive::UNIVERSE;

		}
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