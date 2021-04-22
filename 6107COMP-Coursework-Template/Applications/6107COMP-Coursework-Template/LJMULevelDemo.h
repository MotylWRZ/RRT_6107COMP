#pragma once

#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

//LJMU Framework Includes
#include "LJMUTextOverlay.h"

#include "RRTVertexDX11.h"
#include "Planet.h"
#include "Path.h"

class Light_Base;
class InstancedStaticMesh;
class SolarSystem;
class Terrain;
class PathFollowingActor;
class CinematicCamera;

using namespace Glyph3;

const float DEG_TO_RAD = GLYPH_PI / 180.0f;

typedef  std::shared_ptr<Light_Base> LightBasePtr;

namespace LJMUDX
{
	//////////////////////////////////////
	//LJMULevelDemo.H
	//Class Application for a DirectX 11
	//Driven Application using the DirectX Toolkit
	//Hieroglyph 3 Rendering Engine and LUA.
	//
	//
	//AUTHORS:  DR PO YANG
	//			DR CHRIS CARTER
	//////////////////////////////////////

	typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

	class LJMULevelDemo : public Application //Inherit from the Hieroglyph Base Class
	{

	public:
		//------------CONSTRUCTORS------------------------------------------------
		LJMULevelDemo();	//Standard Empty Constructor which builds the object

	public:
		//------------INHERITED METHODS-------------------------------------------
		virtual void Initialize();					//Initialise the DirectX11 Scene
		virtual void Update();						//Update the DirectX Scene
		virtual void Shutdown();					//Shutdown the DirectX11 Scene

		virtual bool ConfigureEngineComponents();	//Initialise Hieroglyph and DirectX TK Modules
		virtual void ShutdownEngineComponents();	//Destroy Hieroglyph and DirectX TK Modules

		virtual void TakeScreenShot();				//Allow a screenshot to be generated

		virtual bool HandleEvent(EventPtr pEvent);	//Handle an I/O Event
		virtual std::wstring GetName();				//Get the Name of this App Instance

		//------------CUSTOM METHODS-----------------------------------------------
		void inputAssemblyStage();					//Stage to setup our VB and IB Info

		void setupGeometry();
		void animateGeometry(float DT);
		void setupCamera();
		void addLight(LightBasePtr pLight);
		void setupLighting();
		void setupSkySphere();
		void setupPaths();
		void setupSolarSystem();
		void setupTerrain();
		void switchTerrainRendering();
		void switchCamera();
		void setupSpaceships();
		void animateLights(float DT);
		std::wstring outputFPSInfo();				//Convert the timer's Frames Per Second to a formatted string

	protected:
		//-------------CLASS MEMBERS-----------------------------------------------
		RendererDX11*			m_pRenderer11;		//Pointer to our DirectX 11 Device
		Win32RenderWindow*		m_pWindow;			//Pointer to our Windows-Based Window

		int						m_iSwapChain;		//Index of our Swap Chain
		ResourcePtr				m_RenderTarget;		//Pointer to the GPU Render Target for Colour
		ResourcePtr				m_DepthTarget;		//Pointer to the GPU Render Target for Depth

		// Actors
		Actor* m_LandscapeActor;

		Actor* m_CubeActor;
		Actor* m_pCubeActor;
		Actor* m_moonBase;
		InstancedStaticMesh* m_pInstancedStaticMesh;
		/*PathFollowingActor* m_pSpaceship;
		PathFollowingActor* m_pSpaceship2;*/

		std::vector<PathFollowingActor*> m_pathFollowingActors;

		CinematicCamera* m_pCinematicCamera;
		int m_currentCameraId;

		std::vector<Terrain*> m_terrains;
		int m_currentTerrainIndex;

		std::vector<Path*> m_paths;
		std::shared_ptr<SolarSystem> m_solarSystem;

		Planet* m_planet;
		Planet* m_sun;

		std::vector<Actor*> m_actors;
		std::vector<LightBasePtr> m_lights;

		//--------------HIEROGLYPH OBJECTS-----------------------------------------
		ViewPerspective*		m_pRenderView;		//3D Output View - DirectX 11 Accelerated
		ViewPerspective*		m_pRenderView2;		//3D Output View - DirectX 11 Accelerated
		LJMUTextOverlay*        m_pRender_text;		//2D Output View - DirectX 11 Accelerated
		Camera*					m_pCamera;			//Camera Object

		Actor* m_Actor;
		float					m_iscreenWidth = 1920.0f;
		float					m_iscreenHeight = 1080.0f;
	};

}
