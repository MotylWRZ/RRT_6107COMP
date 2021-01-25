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

using namespace Glyph3;

namespace LJMUDX
{
	//////////////////////////////////////
	//LJMULevelDemo.H
	//Class Application for a DirectX 11
	//Driven Application using the DirectX Toolkit
	//Hieroglyph 3 Rendering Engine and LUA.
	//
	//
	//AUTHORS:  DR Po Yang
	//			DR CHRIS CARTER
	//////////////////////////////////////
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
		
		std::wstring outputFPSInfo();				//Convert the timer's Frames Per Second to a formatted string

	    protected:
		//-------------CLASS MEMBERS-----------------------------------------------
		RendererDX11*			_obj_renderer11;	//Pointer to our DirectX 11 Device
		Win32RenderWindow*		_obj_window;		//Pointer to our Windows-Based Window

		int						_swap_index;		//Index of our Swap Chain 
		ResourcePtr				_tgt_render;		//Pointer to the GPU Render Target for Colour
		ResourcePtr				_tgt_depth;			//Pointer to the GPU Render Target for Depth

		//--------------HIEROGLYPH OBJECTS-----------------------------------------
		ViewPerspective*			_render_view;	//3D Output View - DirectX 11 Accelerated
		LJMUTextOverlay*            _render_text;	//2D Output View - DirectX 11 Accelerated
		Camera*						_obj_camera;	//Camera Object
	};

}
