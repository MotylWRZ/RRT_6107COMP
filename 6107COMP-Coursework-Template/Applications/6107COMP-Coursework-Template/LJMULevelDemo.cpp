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
#include "Terrain.h"
#include "CinematicCamera.h"

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

#include "PathFollowingActor.h"

#include <cmath>

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
	this->setupPaths();
	this->setupTerrain();

	MaterialReflectanceInfo tMatInfo;
	tMatInfo.SurfaceEmissiveColour = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	tMatInfo.Ambient = 0.0f;
	tMatInfo.Diffuse = 0.0f;
	tMatInfo.Specular = 0.0f;
	tMatInfo.Shininess = 1.0f;


	this->setupSpaceships();




	this->m_moonBase = new Actor();
	BasicMeshPtr tMoonBaseMesh = MeshImporter::generateMeshOBJ(L"Emergency_Backup_Generator.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tMoonBaseMat = MaterialGenerator::createLitBumpTexturedMaterial(*this->m_pRenderer11, std::wstring(L"14006_Moon_Building_Science_Module_diff1.jpg"), std::wstring(L"Emergency Backup Generator_Default_nmap_Blender.jpg"), this->m_lights, tMatInfo);
	//MaterialGenerator::createLitTexturedMaterial(*this->m_pRenderer11, L"14006_Moon_Building_Science_Module_diff1.jpg", this->m_lights, tMatInfo);
	this->m_moonBase->GetBody()->SetGeometry(tMoonBaseMesh);
	this->m_moonBase->GetBody()->SetMaterial(tMoonBaseMat);
	this->m_moonBase->GetBody()->Scale() = Vector3f(10.1f, 10.1f, 10.1f);
	this->m_moonBase->GetNode()->Position() = Vector3f(300.0f, 10.0f, 100.0f);
	this->m_pScene->AddActor(this->m_moonBase);

	this->m_actors.push_back(this->m_moonBase);


	//Actor* tLanding = new Actor();
	//BasicMeshPtr tLandingMesh = MeshImporter::generateMeshOBJ(L"Landing.obj", Vector4f(1, 1, 1, 1));
	//MaterialPtr tLandingMat = MaterialGenerator::createLitBumpTexturedMaterial(*this->m_pRenderer11, std::wstring(L"HeliPad_Base_Color.png"), std::wstring(L"HeliPad_Normal_DirectX.png"), this->m_lights, tMatInfo);
	////MaterialGenerator::createLitTexturedMaterial(*this->m_pRenderer11, L"14006_Moon_Building_Science_Module_diff1.jpg", this->m_lights, tMatInfo);
	//tLanding->GetBody()->SetGeometry(tLandingMesh);
	//tLanding->GetBody()->SetMaterial(tLandingMat);
	//tLanding->GetBody()->Scale() = Vector3f(10.1f, 10.1f, 10.1f);
	//tLanding->GetNode()->Position() = Vector3f(500.0f, 10.0f, 100.0f);
	//this->m_pScene->AddActor(tLanding);

	this->m_planet = new Planet();
	this->m_planet->Initialize(*this->m_pRenderer11, std::wstring(L"rocks_ground_06_diff_2k.tiff"), this->m_lights, tMatInfo, std::wstring(L"rocks_ground_06_nor_2k.tiff"));
	this->m_planet->GetNode()->Position() = Vector3f(2000.0f, 1000.0f, 2000.0f);
	this->m_planet->GetBody()->Scale() = Vector3f(10.0f, 10.0f, 10.0f);
	this->m_pScene->AddActor(this->m_planet);

	this->m_actors.push_back(this->m_planet);

	this->m_sun = new Planet();
	this->m_sun->Initialize(*this->m_pRenderer11, std::wstring(L"2k_sun.jpg"));
	this->m_sun->GetNode()->Position() = Vector3f(3000.0f, 3000.0f, 3000.0f);
	this->m_sun->GetBody()->Scale() = Vector3f(10.0f, 10.0f, 10.0f);
	this->m_sun->setRotationSpeed(10.0f);
	this->m_pScene->AddActor(this->m_sun);

	this->m_actors.push_back(this->m_planet);



	this->setupSolarSystem();
	this->setupSkySphere();

}

void LJMULevelDemo::animateGeometry(float DT)
{
}

void LJMULevelDemo::setupCamera()
{
	// Setup a cinematic camera (automatic camera)
	this->m_pCinematicCamera = new CinematicCamera();
	Vector3f tCameraPos(200.0f, 30.0f, -10.0f);
	this->m_pCinematicCamera->Spatial().SetTranslation(tCameraPos);
	this->m_pCinematicCamera->Spatial().RotateXBy(20 * DEG_TO_RAD);

	this->m_pRenderView = new ViewPerspective(*this->m_pRenderer11, this->m_RenderTarget,
		this->m_DepthTarget);
	this->m_pRenderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	this->m_pCinematicCamera->SetCameraView(this->m_pRenderView);
	this->m_pCinematicCamera->SetProjectionParams(0.1f, 1000000.0f, this->m_iscreenWidth / this->m_iscreenHeight,
		static_cast<float>(GLYPH_PI) / 2.0f);


	this->m_pCinematicCamera->setFocusObject(this->m_pathFollowingActors[0]);
	this->m_pCinematicCamera->Spatial().SetRotation(Vector3f(1.4f, -2.0f, 0.0f));

	// Add the cinematic camera into a scene as a default camera
	this->m_currentCameraId = 0;
	this->m_pScene->AddCamera(this->m_pCinematicCamera);

	//// Setup manual camera
	this->m_pCamera = new FirstPersonCamera();
	this->m_pCamera->SetEventManager(&this->EvtManager);
	Vector3f tCameraPos2(100.0f, 30.0f, -5.0f);
	this->m_pCamera->Spatial().SetTranslation(tCameraPos2);
	this->m_pCamera->Spatial().RotateXBy(20 * DEG_TO_RAD);

	this->m_pRender_text = new LJMUTextOverlay(*this->m_pRenderer11, this->m_RenderTarget,
		std::wstring(L"Cambria"),
		25);
	this->m_pCamera->SetOverlayView(this->m_pRender_text);
	this->m_pCamera->SetProjectionParams(0.1f, 1000000.0f, this->m_iscreenWidth / this->m_iscreenHeight,
		static_cast<float>(GLYPH_PI) / 2.0f);
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
	Vector3f tLightPosition(100.0f, -70.0f, 300.0f);
	Vector2f tLightRange(100.0f, 0.0f);

	Vector4f tLightColour2(1.0f, 0.0f, 0.9f, 1.0f);
	Vector3f tLightPosition2(300.0f, -70.0f, 400.0f);
	Vector2f tLightRange2(100.0f, 0.0f);

	Vector4f tLightColour3(1.0f, 1.0f, 1.0f, 1.0f);
	Vector3f tLightPosition3(300.0f, -70.0f, 100.0f);
	Vector2f tLightRange3(100.0f, 0.0f);


	//Vector4f tDirLightColour(0.1f, 0.1f, 0.7f, 1.0f);
	Vector4f tDirLightColour(0.2f, 0.2f, 0.3f, 1.0f);
	tDirLightColour /= 4;
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
	LightBasePtr tSpotLight1 = std::make_shared<Light_Spot>(SpotLightColour, SpotLightPosition, SpotLightDirection, SpotLightRange, SpotLightFocus);
	LightBasePtr tSpotLight2 = std::make_shared<Light_Spot>(SpotLightColour, SpotLightPosition, SpotLightDirection, SpotLightRange, SpotLightFocus);
	LightBasePtr tSpotLight3 = std::make_shared<Light_Spot>(SpotLightColour, SpotLightPosition, SpotLightDirection, SpotLightRange, SpotLightFocus);

	this->addLight(tSpotLight1);
	this->addLight(tSpotLight2);
	this->addLight(tPointLight);
	this->addLight(tPointLight2);
	this->addLight(tPointLight3);
	this->addLight(tDirectionalLight);
	this->addLight(tSpotLight3);


	for (int i = 0; i < 200;  i++)
	{
		if (tLightPosition.x > 500.0f)
		{
			tLightPosition.z += 70.0f;
			tLightPosition.x = -100.0f;
		}
		tLightPosition.x += 70.0f;
		//tLightColour.x += 0.1f;
		tLightColour.x += 0.1;
		//tLightColour.z += 0.01;

		LightBasePtr tPointLight = std::make_shared<Light_Point>(tLightColour, tLightPosition, tLightRange);

		this->addLight(tPointLight);
	}
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

void LJMUDX::LJMULevelDemo::setupPaths()
{
	Path* tSpaceshipPath = new Path();
//	tSpaceshipPath->GetBody()->Position() = Vector3f(200.0f, 50.0f, 100.0f);
	tSpaceshipPath->generatePath(EPathType::Path_CatmullRom, 0.0f, 0.0f, 1000.0f, 200.0f, -180.0f, 180.0f, 1);
	this->m_paths.push_back(tSpaceshipPath);

	Path* tSpaceshipPath2 = new Path();
	tSpaceshipPath2->generatePath(EPathType::Path_Hermite, 0.0f, 0.0f, 300.0f, 200.0f, -180.0f, 180.0f, 2);
	this->m_paths.push_back(tSpaceshipPath2);

	Path* tSpaceshipPath3 = new Path();
	tSpaceshipPath3->generatePath(EPathType::Path_CatmullRom, 0.0f, 0.0f, 2000.0f, 100.0f, -180.0f, 180.0f, 1);
	this->m_paths.push_back(tSpaceshipPath3);


	tSpaceshipPath->generatePathMesh(this->m_pRenderer11);
	this->m_pScene->AddActor(tSpaceshipPath);
	this->m_pScene->AddActor(tSpaceshipPath3);
}

void LJMUDX::LJMULevelDemo::setupSolarSystem()
{
	BasicMeshPtr tMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"geosphere.obj", Vector4f(1, 1, 1, 1));

	this->m_solarSystem = std::make_shared<SolarSystem>(Vector3f(3000.0f, 3000.0f, 3000.0f));
	this->m_solarSystem->setCircularMovementRadius(1000.0f);


	// Add outer part of the solar system
	this->m_solarSystem->addISM(tMesh, Vector3f(0.0f, 3000.0f, 0.0f), this->m_pRenderer11, this->m_pScene,
		L"rocks_ground_06_diff_2k.tiff",
		L"brown_mud_dry_diff_2k.tiff", L"mars.tif");

	Vector3f tOrigin(400.0f, 200.0f, 300.0f);
	float tAngle = 10.0f;
	float tAngleDiff = 10.0f;
	float tRadius = 1000.0f;

	std::vector<Vector3f> tPositions;

	for (int i = 0; i < 32; i++)
	{

		Vector3f tPos(tOrigin.x + (tRadius * cos(tAngle)), tOrigin.y, tOrigin.z + (tRadius * sin(tAngle)));

		tAngle += tAngleDiff;

		this->m_solarSystem->addPlanet(0, tPos, EInstanceTexture::TEXTURE1);
	}

	tOrigin = Vector3f(400.0f, 200.0f, 300.0f);
	tRadius = 500.0f;
	tAngle = 10.0f;

	// Add inner part of the solar system
	this->m_solarSystem->addISM(tMesh, Vector3f(0.0f, 3000.0f, 0.0f), this->m_pRenderer11, this->m_pScene,
		L"rocks_ground_06_diff_2k.tiff",
		L"brown_mud_dry_diff_2k.tiff", L"mars.tif");

	for (int i = 0; i < 32; i++)
	{

		Vector3f tPos(tOrigin.x + (tRadius * cos(tAngle)), tOrigin.y, tOrigin.z + (tRadius * sin(tAngle)));

		tAngle += tAngleDiff;

		this->m_solarSystem->addPlanet(1, tPos, EInstanceTexture::TEXTURE1);
	}
}

void LJMUDX::LJMULevelDemo::setupTerrain()
{

	MaterialReflectanceInfo tMatInfo;
	tMatInfo.SurfaceEmissiveColour = Vector4f(0.0f, 1.0f, 1.0f, 10.0f);
	tMatInfo.Ambient = 0.0f;
	tMatInfo.Diffuse = 0.0f;
	tMatInfo.Specular = 0.0f;
	tMatInfo.Shininess = 0.3f;

	// Create test Landscape
	MaterialPtr tTerrainMaterial = MaterialGenerator::createLitTerrainMultiTextureMaterial(*this->m_pRenderer11, L"rocks_ground_06_diff_2k.tiff", L"brown_mud_dry_diff_2k.tiff", this->m_lights, tMatInfo);
	MaterialPtr tTerrainMaterial2 = MaterialGenerator::createLitTerrainMultiTextureMaterial(*this->m_pRenderer11, L"rock_ground_02_diff_1k.png", L"aerial_sand_diff_1k.png", this->m_lights, tMatInfo);

	// Terrain from Heightmap
	Terrain* tTerrainHeightmap = new Terrain(254, 3, 12, this->m_pScene, 0.01f);
	tTerrainHeightmap->generateChunkedTerrainFromHeightmap(false, "heightmap.r16",1025, 1025, 512, 512, 10, 300);
	tTerrainHeightmap->setMaterial(tTerrainMaterial2);
	tTerrainHeightmap->GetNode()->Position() = Vector3f(-1000.0f, -100.0f, -1000.0f);

	// Terrain procedurally generated
	Terrain* tTerrainProcedural = new Terrain(254, 3, 12, this->m_pScene, 0.03f);
	tTerrainProcedural->generateChunkedTerrainFromNoise(true, 254, 10, 128);
	tTerrainProcedural->setMaterial(tTerrainMaterial);
	tTerrainProcedural->GetNode()->Position() = Vector3f(-100.0f, 0.0f, -100.0f);

	this->m_terrains.push_back(tTerrainHeightmap);
	this->m_terrains.push_back(tTerrainProcedural);

	this->m_currentTerrainIndex = 0;
	this->switchTerrainRendering();
}

void LJMUDX::LJMULevelDemo::switchTerrainRendering()
{
	if (this->m_terrains.size() == 0)
	{
		return;
	}

	// Clear the scene from the terrain meshes
	for (auto& Terrain : this->m_terrains)
	{
		//Terrain->removeTerrainFromScene(this->m_pScene);
		this->m_pScene->RemoveActor(Terrain);
	}

	this->m_currentTerrainIndex++;

	if (this->m_currentTerrainIndex >= static_cast<int>(this->m_terrains.size()))
	{
		this->m_currentTerrainIndex = 0;
	}

	// Add the terrain to the scene
	//this->m_terrains[this->m_currentTerrainIndex]->addTerrainIntoScene(this->m_pScene);
	this->m_pScene->AddActor(this->m_terrains[this->m_currentTerrainIndex]);
}

void LJMUDX::LJMULevelDemo::switchCamera()
{
	this->m_currentCameraId++;

	this->m_currentCameraId = (this->m_currentCameraId > 1) ? 0 : this->m_currentCameraId;

	if (this->m_currentCameraId != 0)
	{
		this->m_pScene->RemoveCamera(this->m_pCinematicCamera);
		this->m_pScene->AddCamera(this->m_pCamera);
		this->m_pCamera->SetEventManager(&this->EvtManager);
		this->m_pCamera->SetCameraView(this->m_pRenderView);
	}
	else
	{
		this->m_pScene->RemoveCamera(this->m_pCamera);
		this->m_pScene->AddCamera(this->m_pCinematicCamera);
		this->m_pCinematicCamera->SetCameraView(this->m_pRenderView);
	}
}

void LJMUDX::LJMULevelDemo::setupSpaceships()
{

	MaterialReflectanceInfo tMatInfo;
	tMatInfo.SurfaceEmissiveColour = Vector4f(0.0f, 0.5f, 0.5f, 30.0f);
	tMatInfo.Ambient = 0.0f;
	tMatInfo.Diffuse = 0.0f;
	tMatInfo.Specular = 0.0f;
	tMatInfo.Shininess = 1.0f;


	PathFollowingActor* tSpaceship = new PathFollowingActor(160.0f, 1.0f);
	BasicMeshPtr tSpaceshipMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"spaceship.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tSpaceshipMat = MaterialGenerator::createLitTexturedMaterial(*this->m_pRenderer11, L"wedge_p1_diff_v1.png", this->m_lights, tMatInfo);

	tSpaceship->GetBody()->SetGeometry(tSpaceshipMesh);
	tSpaceship->GetBody()->SetMaterial(tSpaceshipMat);
	tSpaceship->GetBody()->Scale() = Vector3f(0.1f, 0.1f, 0.1f);
	this->m_pScene->AddActor(tSpaceship);

	PathFollowingActor* tSpaceship2 = new PathFollowingActor(360.0f, 3.0f);
	tSpaceship2->GetBody()->SetGeometry(tSpaceshipMesh);
	tSpaceship2->GetBody()->SetMaterial(tSpaceshipMat);
	tSpaceship2->GetBody()->Scale() = Vector3f(0.1f, 0.1f, 0.1f);
	this->m_pScene->AddActor(tSpaceship2);

	tSpaceship->setPath(this->m_paths[0]);
	tSpaceship2->setPath(this->m_paths[1]);

	this->m_pathFollowingActors.push_back(tSpaceship);
	this->m_pathFollowingActors.push_back(tSpaceship2);
}

void LJMUDX::LJMULevelDemo::animateLights(float DT)
{
	LightBasePtr tLight = this->m_lights[1];
	LightInfo tInfo = tLight->getLightInfo();
	tInfo.LightPosition.x = this->m_pathFollowingActors[0]->GetNode()->Position().x;
	tInfo.LightPosition.y = this->m_pathFollowingActors[0]->GetNode()->Position().y - 30.0f;
	tInfo.LightPosition.z = this->m_pathFollowingActors[0]->GetNode()->Position().z;
	tLight->setLightInfo(tInfo);

	LightBasePtr tLight2 = this->m_lights[2];
	LightInfo tInfo2 = tLight->getLightInfo();
	tInfo2.LightPosition.x = this->m_pathFollowingActors[1]->GetNode()->Position().x;
	tInfo2.LightPosition.y = this->m_pathFollowingActors[1]->GetNode()->Position().y - 30.0f;
	tInfo2.LightPosition.z = this->m_pathFollowingActors[1]->GetNode()->Position().z;
	tLight2->setLightInfo(tInfo2);
}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	this->inputAssemblyStage();
	this->setupCamera();
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

	/*LightBasePtr tLight = this->m_lights[2];
	static float speed = 0.0f;
	speed += 0.001f;
	LightInfo tInfo;
	tInfo = tLight->getLightInfo();
	tInfo.LightPosition.x += cos(speed) * 0.2f;

	tLight->setLightInfo(tInfo);*/

	for (int i = 2; i < this->m_lights.size(); i++)
	{
		LightBasePtr tLight = this->m_lights[i];
		static float speed = 0.0f;
		speed += 0.001f;
		LightInfo tInfo;
		tInfo = tLight->getLightInfo();
		tInfo.LightPosition.x += cos(speed / 2) * 4.7f;

		tLight->setLightInfo(tInfo);
	}

	for (auto& Actor : this->m_actors)
	{
		MaterialPtr tMat = Actor->GetBody()->GetMaterial();
		MaterialGenerator::updateMaterialLight(*this->m_pRenderer11, tMat, this->m_lights);
	}


	for (auto& Terrain : this->m_terrains)
	{
		Terrain->updateLighting(this->m_pRenderer11, this->m_lights);
	}

	this->m_planet->Update(tDT, this->m_pTimer);
	this->m_sun->Update(tDT, this->m_pTimer);
	this->m_solarSystem->Update(tDT);

	for (auto& Actor : this->m_pathFollowingActors)
	{
		Actor->update(tDT);

		MaterialPtr tMat = Actor->GetBody()->GetMaterial();
		MaterialGenerator::updateMaterialLight(*this->m_pRenderer11, tMat, this->m_lights);
	}

	this->animateLights(tDT);


	this->m_pCinematicCamera->updateCinematicCamera(tDT, this->m_pRenderer11);

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

		// Checkif Up key is pressed
		if (tkeycode == VK_UP)
		{
			this->switchTerrainRendering();
		}

		// Check if C key is pressed
		if (tkeycode == 0x43)
		{
			this->switchCamera();
		}

		// Check if G key is pressed
		if (tkeycode == 0x0047)
		{
			if (this->m_planet->isExplosionActive())
			{
				this->m_planet->setExplosion(false);
			}
			else
			{
				this->m_planet->setExplosion(true);
			}

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