//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

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
_render_text(nullptr),
_render_view(nullptr),
_obj_camera(nullptr),
_obj_renderer11(nullptr),
_obj_window(nullptr),
_swap_index(0),
_tgt_depth(nullptr),
_tgt_render(nullptr)
{
	
}

//---------METHODS------------------------------------------------------------

//////////////////////////////////////
// Get the Window Name of this Application
//////////////////////////////////////
std::wstring LJMULevelDemo::GetName()
{
	return(std::wstring(L"5108COMP: Coursework Template"));
}

/////////////////////////////////////
// Assemble our Input Layouts for this
// Stage of the Pipeline.
/////////////////////////////////////
void LJMULevelDemo::inputAssemblyStage()
{			
	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------

}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	this->inputAssemblyStage();

	this->_obj_camera = new Camera();

	Vector3f tcamerapos(0.0f, 20.0f, -50.0f);
	this->_obj_camera->Spatial().SetTranslation(tcamerapos);

	this->_render_view = new ViewPerspective(*this->_obj_renderer11,
		                                     this->_tgt_render, this->_tgt_depth);
	this->_render_view->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	this->_obj_camera->SetCameraView(this->_render_view);

	this->_render_text = new LJMUTextOverlay(*this->_obj_renderer11, 
		                                      this->_tgt_render, 
											  std::wstring(L"Cambria"), 
											  25);	

	this->_obj_camera->SetOverlayView(this->_render_text);
	
	float twidth = 1024.0f;
	float theight = 768.0f;
	this->_obj_camera->SetProjectionParams(0.1f, 1000.0f, twidth/ theight, 
		                                   static_cast<float>(GLYPH_PI) / 2.0f);
	
	this->m_pScene->AddCamera(this->_obj_camera);	
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

	this->m_pScene->Update(m_pTimer->Elapsed());
	this->m_pScene->Render(this->_obj_renderer11);

	//--------END RENDERING-------------------------------------------------------------
	this->_obj_renderer11->Present(this->_obj_window->GetHandle(), this->_obj_window->GetSwapChain());
}

///////////////////////////////////
// Configure the DirectX 11 Programmable
// Pipeline Stages and Create the Window
// Calls 
///////////////////////////////////
bool LJMULevelDemo::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int twidth = 1024;
	int theight = 768;

	// Set the render window parameters and initialize the window
	this->_obj_window = new Win32RenderWindow();
	this->_obj_window->SetPosition(25, 25);
	this->_obj_window->SetSize(twidth, theight);
	this->_obj_window->SetCaption(this->GetName());
	this->_obj_window->Initialize(this);


	// Create the renderer and initialize it for the desired device
	// type and feature level.
	this->_obj_renderer11 = new RendererDX11();

	if (!this->_obj_renderer11->Initialize(D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0))
	{
		Log::Get().Write(L"Could not create hardware device, trying to create the reference device...");

		if (!this->_obj_renderer11->Initialize(D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0))
		{
			ShowWindow(this->_obj_window->GetHandle(), SW_HIDE);
			MessageBox(this->_obj_window->GetHandle(), L"Could not create a hardware or software Direct3D 11 device!", L"5108COMP Coursework Template", MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
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
	tconfig.SetWidth(this->_obj_window->GetWidth());
	tconfig.SetHeight(this->_obj_window->GetHeight());
	tconfig.SetOutputWindow(this->_obj_window->GetHandle());
	this->_swap_index = this->_obj_renderer11->CreateSwapChain(&tconfig);
	this->_obj_window->SetSwapChain(this->_swap_index);
	
	//Create Colour and Depth Buffers
	this->_tgt_render = this->_obj_renderer11->GetSwapChainResource(this->_swap_index);

	Texture2dConfigDX11 tdepthconfig;
	tdepthconfig.SetDepthBuffer(twidth, theight);
	this->_tgt_depth = this->_obj_renderer11->CreateTexture2D(&tdepthconfig, 0);

	// Bind the swap chain render target and the depth buffer for use in rendering.  
	this->_obj_renderer11->pImmPipeline->ClearRenderTargets();
	this->_obj_renderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, this->_tgt_render->m_iResourceRTV);
	this->_obj_renderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState(this->_tgt_depth->m_iResourceDSV);
	this->_obj_renderer11->pImmPipeline->ApplyRenderTargets();

	D3D11_VIEWPORT tviewport;
	tviewport.Width = static_cast< float >(twidth);
	tviewport.Height = static_cast< float >(theight);
	tviewport.MinDepth = 0.0f;
	tviewport.MaxDepth = 1.0f;
	tviewport.TopLeftX = 0;
	tviewport.TopLeftY = 0;

	int tvpindex = this->_obj_renderer11->CreateViewPort(tviewport);
	this->_obj_renderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState(1);
	this->_obj_renderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState(0, tvpindex);
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
	if (this->_obj_renderer11)
	{
		this->_obj_renderer11->Shutdown();
		delete this->_obj_renderer11;
	}

	if (this->_obj_window)
	{
		this->_obj_window->Shutdown();
		delete this->_obj_window;
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
		this->_obj_renderer11->pImmPipeline->SaveTextureScreenShot(0, this->GetName());
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