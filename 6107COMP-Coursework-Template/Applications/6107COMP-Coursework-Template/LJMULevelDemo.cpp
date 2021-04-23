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
#include <random>


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
	this->setupBase();

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
	Vector3f tLightPosition(150.0f, 70.0f, 700.0f);
	Vector2f tLightRange(70.0f, 0.0f);

	Vector4f tLightColour2(0.1f, 0.7f, 0.9f, 1.0f);
	Vector3f tLightPosition2(250.0f, 30.0f, 600.0f);
	Vector2f tLightRange2(100.0f, 0.0f);

	Vector4f tLightColour3(1.0f, 1.0f, 1.0f, 1.0f);
	Vector3f tLightPosition3(170.0f, 30.0f, 700.0f);
	Vector2f tLightRange3(200.0f, 0.0f);


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


	// Setup Dynamic lights (animated lights)

	Vector3f tOrigin(150.0f, 10.0f, 700.0f);
	float tAngle = 10.0f;
	float tAngleDiff = 10.0f;
	float tRadius = 700.0f;

	std::vector<Vector3f> tPositions;

	Vector2f tLightDynamicRange(17.0f, 0.0f);
	for (int i = 0; i < 22; i++)
	{
		tLightColour.x += 0.3;

		Vector3f tPos(tOrigin.x + (tRadius * cos(tAngle)), tOrigin.y, tOrigin.z + (tRadius * sin(tAngle)));

		tLightPosition = tPos;
		tAngle += tAngleDiff;

		LightBasePtr tPointLight = std::make_shared<Light_Point>(tLightColour, tLightPosition, tLightDynamicRange);

		this->addLight(tPointLight);
		this->m_dynamicLights.push_back(tPointLight);
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
	tSpaceshipPath2->generatePath(EPathType::Path_CatmullRom, 0.0f, 0.0f, 300.0f, 200.0f, -180.0f, 180.0f, 2);
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


	// Add an instance which can instantiate up to 32 instances of the specified mesh
	this->m_solarSystem->addISM(tMesh, Vector3f(0.0f, 3000.0f, 0.0f), this->m_pRenderer11, this->m_pScene,
		L"rocks_ground_06_diff_2k.tiff",
		L"brown_mud_dry_diff_2k.tiff", L"mars.tif");

	// Add another instance which can instantiate up to 32 instances of the specified mesh
	this->m_solarSystem->addISM(tMesh, Vector3f(0.0f, 3000.0f, 0.0f), this->m_pRenderer11, this->m_pScene,
		L"rocks_ground_06_diff_2k.tiff",
		L"brown_mud_dry_diff_2k.tiff", L"mars.tif");

	// Add another instance which can instantiate up to 32 instances of the specified mesh
	this->m_solarSystem->addISM(tMesh, Vector3f(0.0f, 3000.0f, 0.0f), this->m_pRenderer11, this->m_pScene,
		L"rocks_ground_06_diff_2k.tiff",
		L"brown_mud_dry_diff_2k.tiff", L"mars.tif");





	const float minX = -200.0f;
	const float maxX = 200.0f;
	const float minY = -200.0f;
	const float maxY = 200.0f;

	std::random_device rd;
	std::default_random_engine eng(rd());

	// Generate random float between Min and Max values
	auto randRangeFloat = [&eng](float rangeMin, float rangeMax)
	{
		std::uniform_real_distribution<> distr(rangeMin, rangeMax);
		float random = distr(eng);
		return random;
	};

	// Generate random int between Min and Max values
	auto randRangeInt = [&eng](int rangeMin, int rangeMax)
	{
		std::uniform_real_distribution<> distr(rangeMin, rangeMax);
		int random = distr(eng);
		return random;
	};


	Vector3f tOrigin(400.0f, 200.0f, 300.0f);
	float tAngle = 10.0f;
	float tAngleDiff = 10.0f;
	//float tRadius = 1000.0f;

	std::vector<Vector3f> tPositions;
	float tRadius = 300.0f;
	float tSpeed = 0.007f;

	// For each Instanced Static Mesh, create 32 instances
	for (int i = 0; i < static_cast<int>(this->m_solarSystem->getISMs().size()); i++)
	{
		for (int j = 0; j < 32; j++)
		{
			Vector3f tPos(tOrigin.x + (tRadius * cos(tAngle)), tOrigin.y, tOrigin.z + (tRadius * sin(tAngle)));

			tAngle += tAngleDiff;

			// Generate Planet Info

			// GenerateRandomTexture
			EInstanceTexture tTexture = static_cast<EInstanceTexture>(randRangeInt(0, 3));

			tRadius += 37.0f;
			float tScale = randRangeFloat(1.0f, 3.0f);
			//tSpeed += randRangeFloat(0.0003f, 0.001f);
			tSpeed = randRangeFloat(0.0003f, 0.07f);

			this->m_solarSystem->addPlanet(i, tPos, tSpeed, tRadius, tScale, tTexture);
		}
	}


	/*tOrigin = Vector3f(400.0f, 200.0f, 300.0f);
	tRadius = 500.0f;
	tAngle = 10.0f;

	 Add inner part of the solar system
	this->m_solarSystem->addISM(tMesh, Vector3f(0.0f, 3000.0f, 0.0f), this->m_pRenderer11, this->m_pScene,
		L"rocks_ground_06_diff_2k.tiff",
		L"brown_mud_dry_diff_2k.tiff", L"mars.tif");

	for (int i = 0; i < 32; i++)
	{

		Vector3f tPos(tOrigin.x + (tRadius * cos(tAngle)), tOrigin.y, tOrigin.z + (tRadius * sin(tAngle)));

		tAngle += tAngleDiff;

		 GenerateRandomTexture
		EInstanceTexture tTexture = static_cast<EInstanceTexture>(randRangeInt(0, 3));

		this->m_solarSystem->addPlanet(1, tPos, tTexture);
	}*/
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
	MaterialPtr tTerrainMaterial2 = MaterialGenerator::createLitTerrainMultiTextureMaterial(*this->m_pRenderer11, L"snow_03_diff_1k.tiff", L"snow_field_aerial_col_1k.tiff", this->m_lights, tMatInfo);

	// Terrain from Heightmap
	Terrain* tTerrainHeightmap = new Terrain(254, 3, 12, this->m_pScene, 0.01f);
	tTerrainHeightmap->generateChunkedTerrainFromHeightmap(false, "heightmap.r16",1025, 1025, 512, 512, 10, 300);
	tTerrainHeightmap->setMaterial(tTerrainMaterial2);
	tTerrainHeightmap->GetNode()->Position() = Vector3f(-1000.0f, -110.0f, -1000.0f);

	// Terrain procedurally generated
	Terrain* tTerrainProcedural = new Terrain(254, 3, 12, this->m_pScene, 0.03f);
	tTerrainProcedural->generateChunkedTerrainFromNoise(true, 254, 10, 128);
	tTerrainProcedural->setMaterial(tTerrainMaterial);
	tTerrainProcedural->GetNode()->Position() = Vector3f(-100.0f, 0.0f, -100.0f);

	this->m_terrains.push_back(tTerrainHeightmap);
	this->m_terrains.push_back(tTerrainProcedural);

	this->m_currentTerrainIndex = 0;
	this->switchTerrainRendering();

	this->m_wireframeRendering = false;
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
	tMatInfo.SurfaceEmissiveColour = Vector4f(0.0f, 1.0f, 1.0f, 100.0f);
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

	Vector3f tOrigin(150.0f, -12.0f, 700.0f);
	float offset = 1.0f;

	float tRadius = 47.0f;

	for (int i = 0; i < this->m_dynamicLights.size(); i++)
	{
		LightBasePtr tDynamicLight = this->m_dynamicLights[i];

		static float tAngle = 0;
		tAngle += 0.01f * DT;

		Vector3f tNewPos = Vector3f(tOrigin.x + (tRadius * cos(tAngle + offset)), tOrigin.y, tOrigin.z + (tRadius * sin(tAngle + offset)));

		LightInfo tInfo;
		tInfo = tDynamicLight->getLightInfo();
		tInfo.LightPosition.x = tNewPos.x;
		tInfo.LightPosition.y = tNewPos.y;
		tInfo.LightPosition.z = tNewPos.z;

		tDynamicLight->setLightInfo(tInfo);

		offset++;

	}
	offset = 0;
}

void LJMUDX::LJMULevelDemo::setupBase()
{
	MaterialReflectanceInfo tMatInfo;
	tMatInfo.SurfaceEmissiveColour = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	tMatInfo.Ambient = 0.0f;
	tMatInfo.Diffuse = 0.0f;
	tMatInfo.Specular = 0.0f;
	tMatInfo.Shininess = 1.0f;

	//////////////////////////
	// Setup static meshes
	//////////////////////////

	Actor* tGenerator = new Actor();
	BasicMeshPtr tGeneratorMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"Emergency_Backup_Generator.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tGeneratorMat = MaterialGenerator::createLitBumpTexturedMaterial(*this->m_pRenderer11, std::wstring(L"Plastic010_1K_Color.png"), std::wstring(L"Plastic010_1K_Normal.png"), this->m_lights, tMatInfo);
	tGenerator->GetBody()->SetGeometry(tGeneratorMesh);
	tGenerator->GetBody()->SetMaterial(tGeneratorMat);
	tGenerator->GetBody()->Scale() = Vector3f(10.1f, 10.1f, 10.1f);
	tGenerator->GetBody()->Rotation().RotationEuler(Vector3f(0.0f, 1.0f, 0.0f), 90.0f);
	tGenerator->GetNode()->Position() = Vector3f(70.0f, 0.0f, 630.0f);

	// Add actor into a scene and into a vector of actors in order to update the material and lighting
	this->m_pScene->AddActor(tGenerator);
	this->m_actors.push_back(tGenerator);

	Actor* tLanding = new Actor();
	BasicMeshPtr tLandingMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"Landing.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tLandingMat = MaterialGenerator::createLitBumpTexturedMaterial(*this->m_pRenderer11, std::wstring(L"concrete_floor_01_diff_1k.png"), std::wstring(L"concrete_floor_01_nor_1k.png"), this->m_lights, tMatInfo);

	tLanding->GetBody()->SetGeometry(tLandingMesh);
	tLanding->GetBody()->SetMaterial(tLandingMat);
	tLanding->GetBody()->Scale() = Vector3f(10.0f, 10.0f, 10.0f);
	tLanding->GetNode()->Position() = Vector3f(250.0f, -10.0f, 600.0f);

	// Add actor into a scene and into a vector of actors in order to update the material and lighting
	this->m_pScene->AddActor(tLanding);
	this->m_actors.push_back(tLanding);


	Actor* tSpaceship = new Actor();
	BasicMeshPtr tSpaceshipMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"ares_I.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tSpaceshipMat = MaterialGenerator::createLitTexturedMaterial(*this->m_pRenderer11, std::wstring(L"Ares_I.png"), this->m_lights, tMatInfo);

	tSpaceship->GetBody()->SetGeometry(tSpaceshipMesh);
	tSpaceship->GetBody()->SetMaterial(tSpaceshipMat);
	tSpaceship->GetBody()->Scale() = Vector3f(3.0f, 3.0f, 3.0f);
	tSpaceship->GetNode()->Position() = Vector3f(150.0f, -10.0f, 700.0f);

	// Add actor into a scene and into a vector of actors in order to update the material and lighting
	this->m_pScene->AddActor(tSpaceship);
	this->m_actors.push_back(tSpaceship);

	Actor* tSpaceShipLanding = new Actor();
	BasicMeshPtr tSpaceShipLandingMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"StartShipLanding.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tSpaceShipLandingMat = MaterialGenerator::createLitBumpTexturedMaterial(*this->m_pRenderer11, std::wstring(L"Plastic010_1K_Color.png"), std::wstring(L"Plastic010_1K_Normal.png"), this->m_lights, tMatInfo);

	tSpaceShipLanding->GetBody()->SetGeometry(tSpaceShipLandingMesh);
	tSpaceShipLanding->GetBody()->SetMaterial(tSpaceShipLandingMat);
	tSpaceShipLanding->GetBody()->Scale() = Vector3f(3.0f, 3.0f, 3.0f);
	tSpaceShipLanding->GetNode()->Rotation().RotationEuler(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	tSpaceShipLanding->GetNode()->Position() = Vector3f(150.0f, -10.0f, 700.0f);

	// Add actor into a scene and into a vector of actors in order to update the material and lighting
	this->m_pScene->AddActor(tSpaceShipLanding);
	this->m_actors.push_back(tSpaceShipLanding);

	//////////////////////////
	// Setup static spaceships
	//////////////////////////

	MaterialReflectanceInfo tMatInfo2;
	tMatInfo2.SurfaceEmissiveColour = Vector4f(0.0f, 1.0f, 1.0f, 30.0f);
	tMatInfo2.Ambient = 0.0f;
	tMatInfo2.Diffuse = 0.0f;
	tMatInfo2.Specular = 0.0f;
	tMatInfo2.Shininess = 1.0f;

	Actor* tSpaceshipSmall = new Actor();
	BasicMeshPtr tSpaceshipSmallMesh = MeshImporter::generateMeshOBJWithSurfaceVectors(L"spaceship.obj", Vector4f(1, 1, 1, 1));
	MaterialPtr tSpaceshipSmallMat = MaterialGenerator::createLitTexturedMaterial(*this->m_pRenderer11, L"wedge_p1_diff_v1.png", this->m_lights, tMatInfo2);


	tSpaceshipSmall->GetBody()->SetGeometry(tSpaceshipSmallMesh);
	tSpaceshipSmall->GetBody()->SetMaterial(tSpaceshipSmallMat);
	tSpaceshipSmall->GetBody()->Scale() = Vector3f(0.1f, 0.1f, 0.1f);
	tSpaceshipSmall->GetNode()->Position() = Vector3f(230.0f, 20.0f, 630.0f);

	// Add actor into a scene and into a vector of actors in order to update the material and lighting
	this->m_pScene->AddActor(tSpaceshipSmall);
	this->m_actors.push_back(tSpaceshipSmall);


	Actor* tSpaceshipSmall2 = new Actor();
	tSpaceshipSmall2->GetBody()->SetGeometry(tSpaceshipSmallMesh);
	tSpaceshipSmall2->GetBody()->SetMaterial(tSpaceshipSmallMat);
	tSpaceshipSmall2->GetBody()->Scale() = Vector3f(0.1f, 0.1f, 0.1f);
	tSpaceshipSmall2->GetNode()->Position() = Vector3f(270.0f, 20.0f, 570.0f);

	// Add actor into a scene and into a vector of actors in order to update the material and lighting
	this->m_pScene->AddActor(tSpaceshipSmall2);
	this->m_actors.push_back(tSpaceshipSmall2);



}

void LJMUDX::LJMULevelDemo::terrainWireframeRendering(bool enabled)
{
	for (auto& TerrainObject : this->m_terrains)
	{
		MaterialPtr tMat  = TerrainObject->getMaterial();
		RenderEffectDX11* tEffect = tMat->Params[VT_PERSPECTIVE].pEffect;

		// Modify the rasterizer state
		RasterizerStateConfigDX11 rsConfig;
		RasterizerStateComPtr rstatePtr = this->m_pRenderer11->GetRasterizerState(tEffect->m_iRasterizerState);
		rstatePtr->GetDesc(&rsConfig);

		if (enabled)
		{
			// Set Wireframe mode
			rsConfig.FillMode= D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		}
		else
		{
			// Set Solid mode
			rsConfig.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		}

		int iRasterizerState = this->m_pRenderer11->CreateRasterizerState(&rsConfig);
		tEffect->m_iRasterizerState = iRasterizerState;
		tMat->Params[VT_PERSPECTIVE].bRender = true;
		tMat->Params[VT_PERSPECTIVE].pEffect = tEffect;
	}
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

	// Update materials for all actors
	for (auto& Actor : this->m_actors)
	{
		MaterialPtr tMat = Actor->GetBody()->GetMaterial();
		MaterialGenerator::updateMaterialLight(*this->m_pRenderer11, tMat, this->m_lights);
	}

	// Update materials for terrain meshes
	for (auto& Terrain : this->m_terrains)
	{
		Terrain->updateLighting(this->m_pRenderer11, this->m_lights);
	}

	this->m_planet->Update(tDT, this->m_pTimer);
	this->m_sun->Update(tDT, this->m_pTimer);
	this->m_solarSystem->Update(tDT);

	// Update pathFollowing actors
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

		if (tkeycode == VK_RIGHT)
		{
			if (this->m_wireframeRendering)
			{
				this->terrainWireframeRendering(false);
				this->m_wireframeRendering = false;
			}
			else
			{
				this->terrainWireframeRendering(true);
				this->m_wireframeRendering = true;
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