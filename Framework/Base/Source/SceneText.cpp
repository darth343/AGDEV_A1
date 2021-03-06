#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox\SkyBoxEntity.h"
#include "SceneGraph\SceneGraph.h"
#include "SpatialPartition\SpatialPartition.h"

#include <iostream>
using namespace std;

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());
SceneText::SceneText()
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
	CSpatialPartition::GetInstance()->RemoveCamera();
	CSceneGraph::GetInstance()->Destroy();
}

void SceneText::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);
	
	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");

	MeshBuilder::GetInstance()->GenerateQuad("CRIT_HITMARKER", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("CRIT_HITMARKER")->textureID = LoadTGA("Image//Hitmarker_CRIT.tga");

	MeshBuilder::GetInstance()->GenerateQuad("NONCRIT_HITMARKER", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("NONCRIT_HITMARKER")->textureID = LoadTGA("Image//Hitmarker_NONCRIT.tga");
	MeshBuilder::GetInstance()->GenerateQuad("KILL_HITMARKER", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("KILL_HITMARKER")->textureID = LoadTGA("Image//Hitmarker_KILL.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	//MeshBuilder::GetInstance()->GenerateOBJ("Chair", "OBJ//chair.obj");
	//MeshBuilder::GetInstance()->GetMesh("Chair")->textureID = LoadTGA("Image//chair.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("RobotHead_high", "OBJ//robotHead_high.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotHead_high")->textureID = LoadTGA("Image//robotHead.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("RobotBody_high", "OBJ//robotBody_high.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotBody_high")->textureID = LoadTGA("Image//robotBody.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("RobotHand_high", "OBJ//robotHand_high.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotHand_high")->textureID = LoadTGA("Image//robotHand.tga");


	MeshBuilder::GetInstance()->GenerateOBJ("RobotHead_med", "OBJ//robotHead_med.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotHead_high")->textureID = LoadTGA("Image//robotHead.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("RobotBody_med", "OBJ//robotBody_med.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotBody_med")->textureID = LoadTGA("Image//robotBody.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("RobotHand_med", "OBJ//robotHand_med.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotHand_med")->textureID = LoadTGA("Image//robotHand.tga");


	MeshBuilder::GetInstance()->GenerateOBJ("RobotHead_low", "OBJ//robotHead.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotHead_low")->textureID = LoadTGA("Image//robotHead.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("RobotBody_low", "OBJ//robotBody.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotBody_low")->textureID = LoadTGA("Image//robotBody.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("RobotHand_low", "OBJ//robotHand.obj");
	MeshBuilder::GetInstance()->GetMesh("RobotHand_low")->textureID = LoadTGA("Image//robotHand.tga");

	// Gun
	MeshBuilder::GetInstance()->GenerateOBJ("gun", "OBJ//gun.obj");
	MeshBuilder::GetInstance()->GetMesh("gun")->textureID = LoadTGA("Image//gun.tga");

	// Bullet
	//MeshBuilder::GetInstance()->GenerateOBJ("bullet", "OBJ//bullet.obj");
	//MeshBuilder::GetInstance()->GetMesh("bullet")->textureID = LoadTGA("Image//bullet.tga");
	MeshBuilder::GetInstance()->GenerateSphere("bullet", Color(0.3, 0.3, 0.3), 18, 36, 0.1f);

	// Enemy house
	MeshBuilder::GetInstance()->GenerateOBJ("enemyhouse", "OBJ//enemy_house.obj");
	MeshBuilder::GetInstance()->GetMesh("enemyhouse")->textureID = LoadTGA("Image//enemyhouse.tga");

	// Fortress
	MeshBuilder::GetInstance()->GenerateOBJ("fortress", "OBJ//fortress.obj");
	MeshBuilder::GetInstance()->GetMesh("fortress")->textureID = LoadTGA("Image//fortress.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("fortress2", "OBJ//fortress2.obj");
	MeshBuilder::GetInstance()->GetMesh("fortress2")->textureID = LoadTGA("Image//fortress.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("fortress3", "OBJ//fortress3.obj");
	MeshBuilder::GetInstance()->GetMesh("fortress3")->textureID = LoadTGA("Image//fortress.tga");

	// Ground 1
	MeshBuilder::GetInstance()->GenerateOBJ("ground1", "OBJ//ground1.obj");
	MeshBuilder::GetInstance()->GetMesh("ground1")->textureID = LoadTGA("Image//ground1.tga");

	// Ground 2
	MeshBuilder::GetInstance()->GenerateOBJ("ground2", "OBJ//ground2.obj");
	MeshBuilder::GetInstance()->GetMesh("ground2")->textureID = LoadTGA("Image//chair.tga");

	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 0.5f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID = LoadTGA("Image//grass_darkgreen.tga");
	MeshBuilder::GetInstance()->GenerateQuad("GEO_GRASS_LIGHTGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GEO_GRASS_LIGHTGREEN")->textureID = LoadTGA("Image//grass_lightgreen.tga");
	MeshBuilder::GetInstance()->GenerateCube("cubeSG", Color(1.0f, 0.64f, 0.0f), 1.0f);

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//SkyBox//skybox_front.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//SkyBox//skybox_back.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//SkyBox//skybox_left.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//SkyBox//skybox_right.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//SkyBox//skybox_top.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//SkyBox//skybox_bottom.tga");
	MeshBuilder::GetInstance()->GenerateRay("laser", 10.0f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_WHITE", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_RED", Color(1, 0, 0), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_BLUE", Color(0, 0, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_GREEN", Color(0, 1, 0), 1.f);

	// Set up the Spatial Partition and pass it to the EntityManager to manage
	CSpatialPartition::GetInstance()->Init(100, 100, 10, 10);
	CSpatialPartition::GetInstance()->SetMesh("GRIDMESH_WHITE");
	CSpatialPartition::GetInstance()->SetCamera(&camera);
	CSpatialPartition::GetInstance()->SetLevelOfDetails(40000.0f, 160000.0f);
	EntityManager::GetInstance()->SetSpatialPartition(CSpatialPartition::GetInstance());

	// Create entities into the scene
	Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball



	// Test
	//GenericEntity* Test = Create::Entity("test", Vector3(0.f, 0.f, 0.f));
	//Test->SetCollider(true);
	//Test->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	//Test->InitLOD("test", "sphere", "cubeSG");


	//GenericEntity* aCube = Create::Entity("cube", Vector3(-20.0f, 0.0f, -20.0f));
	//aCube->SetCollider(true);
	////aCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	//aCube->InitLOD("cube", "sphere", "cubeSG");

	//// Add the pointer to this new entity to the Scene Graph
	//CSceneNode* theNode = CSceneGraph::GetInstance()->AddNode(aCube);
	//if (theNode == NULL)
	//{
	//	cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	//}

	//GenericEntity* anotherCube = Create::Entity("cube", Vector3(-20.0f, 1.1f, -20.0f));
	//anotherCube->SetCollider(true);
	////anotherCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	//CSceneNode* anotherNode = theNode->AddChild(anotherCube);
	//if (anotherNode == NULL)
	//{
	//	cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	//}
	//
	//GenericEntity* baseCube = Create::Asset("cube", Vector3(0.0f, 0.0f, 0.0f));

	//CSceneNode* baseNode = CSceneGraph::GetInstance()->AddNode(baseCube);

	//CUpdateTransformation* baseMtx = new CUpdateTransformation();
	//baseMtx->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
	//baseMtx->SetSteps(-60, 60);
	//baseNode->SetUpdateTransformation(baseMtx);

	//GenericEntity* childCube = Create::Asset("cubeSG", Vector3(0.0f, 0.0f, 0.0f));
	//CSceneNode* childNode = baseNode->AddChild(childCube);
	//childNode->ApplyTranslate(0.0f, 1.0f, 0.0f);

	//GenericEntity* grandchildCube = Create::Asset("cubeSG", Vector3(0.0f, 0.0f, 0.0f));
	//CSceneNode* grandchildNode = childNode->AddChild(grandchildCube);
	//grandchildNode->ApplyTranslate(0.0f, 0.0f, 1.0f);

	//CUpdateTransformation* aRotateMtx = new CUpdateTransformation();
	//aRotateMtx->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
	//aRotateMtx->SetSteps(-120, 60);
	//grandchildNode->SetUpdateTransformation(aRotateMtx);
	
	groundEntity = Create::Ground("GRASS_DARKGREEN", "GRASS_DARKGREEN");
	//Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	//Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	groundEntity->SetPosition(Vector3(0, -10, 0));
	groundEntity->SetScale(Vector3(100.0f, 100.0f, 100.0f));
	groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	playerInfo->SetTerrain(groundEntity);

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 3; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}

	//CEnemy* theEnemy = new CEnemy();
	//theEnemy->Init();
	//theEnemy->SetTerrain(groundEntity);


	//theWall = Create::Wall("cube", "cubeSG", "sphere",
	//	Vector3(0, 0, 0),
	//	Vector3(100, 10, 10));

	theWall = Create::Wall("enemyhouse", "enemyhouse", "enemyhouse",
		Vector3(0, -11, 280),
		Vector3(2, 1.5, 1.5)
		);

	theWall->SetCollider(false);

	//enemyHouse->SetScale(Vector3(2, 1.5, 1.5));
	//enemyHouse->SetPosition(Vector3(0, -11, 70));
	//enemyHouse->InitLOD("enemyhouse", "enemyhouse", "enemyhouse");
	//enemyHouse->SetAABB(Vector3(14, 0.5, 9), Vector3(-23, -0.5, -14));

	CWall* fortress = Create::Wall("fortress", "fortress2", "fortress3",
		Vector3(0, -11, -80),
		Vector3(1.5, 1.3, 1)
		);
	//fortress->SetScale(Vector3(1.5, 1.3, 1));
	//fortress->SetPosition(Vector3(0, -11, -80));
	//fortress->InitLOD("fortress", "fortress2", "fortress3");
	//fortress->SetAABB(Vector3(33, 0.5, 9), Vector3(-33, -0.5, -9));

	CWall* ground1 = Create::Wall("ground1", "ground1", "ground1",
		Vector3(0, -11, -2.9),
		Vector3(2.2, 1, 2.5)
		);
	ground1->SetCollider(false);

	//ground1->SetScale(Vector3(2.2, 1, 2.5));
	//ground1->SetPosition(Vector3(0, -11, -2.9));
	//ground1->InitLOD("ground1", "ground1", "ground1");
	//ground1->SetAABB(Vector3(33, 0.5, 9), Vector3(-33, -0.5, -9));

	//CWall* ground2 = new CWall();
	//ground2->SetScale(Vector3(1.5, 1, 2));
	//ground2->SetPosition(Vector3(0, -11, 0));
	//ground2->InitLOD("ground2", "ground2", "ground2");
	//ground1->SetAABB(Vector3(33, 0.5, 9), Vector3(-33, -0.5, -9));
	Spawner[0].Init();
	Spawner[0].Position.Set(-17, 0, 260);
	Spawner[0].currentTime = Math::RandFloatMinMax(-2, 2);
	Spawner[1].Init();
	Spawner[1].Position.Set(-0.6, 0, 273);
	Spawner[1].currentTime = Math::RandFloatMinMax(-2, 2);
	Spawner[2].Init();
	Spawner[2].Position.Set(23, 0, 277);
	Spawner[2].currentTime = Math::RandFloatMinMax(-2, 2);
	DoorHealth = 2000;
}

void SceneText::SpawnEnemies(double dt)
{
	for (int i = 0; i < 3; ++i)
	{
		Spawner[i].currentTime += dt;
		if (Spawner[i].currentTime > Spawner[i].waitTime)
		{
			Spawner[i].currentTime -= Spawner[i].waitTime;
			CEnemy* theEnemy = new CEnemy();
			theEnemy->Init();
			theEnemy->SetTerrain(groundEntity);
			theEnemy->SetPosition(Spawner[i].Position);
			theEnemy->SetTarget(Vector3(0, 0, -67));
			Spawner[i].waitTime = Math::RandFloatMinMax(Spawner[i].waitTime - 2, Spawner[i].waitTime + 2);
		}
	}
}

void SceneText::Update(double dt)
{
		SpawnEnemies(dt);
	// Update our entities
	EntityManager::GetInstance()->Update(dt);
	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	if(KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if(KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if(KeyboardController::GetInstance()->IsKeyDown('I'))
		lights[0]->position.z -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('K'))
		lights[0]->position.z += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('J'))
		lights[0]->position.x -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('L'))
		lights[0]->position.x += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);

	if (KeyboardController::GetInstance()->IsKeyReleased('M'))
	{
		CSceneNode* theNode = CSceneGraph::GetInstance()->GetNode(1);
		Vector3 pos = theNode->GetEntity()->GetPosition();
		theNode->GetEntity()->SetPosition(Vector3(pos.x + 50.0f, pos.y, pos.z + 50.0f));
	}
	if (KeyboardController::GetInstance()->IsKeyReleased('N'))
	{
		CSpatialPartition::GetInstance()->PrintSelf();
	}

	float x = theWall->GetPosition().x;
	float y = theWall->GetPosition().y;
	float z = theWall->GetPosition().z;
	if (KeyboardController::GetInstance()->IsKeyDown('G'))
		z -= (float)(100.f * dt);
	if (KeyboardController::GetInstance()->IsKeyDown('H'))
		z += (float)(100.f * dt);
	if (KeyboardController::GetInstance()->IsKeyReleased('B'))
		CSpatialPartition::GetInstance()->ShouldRender = !CSpatialPartition::GetInstance()->ShouldRender;

	theWall->SetPosition(Vector3(x, y, z));

	//cout << theWall->GetPosition() << endl;
	// Update the player position and other details based on keyboard and mouse inputs
	playerInfo->Update(dt);

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	std::ostringstream ss2;
	ss2 << "Ammo: " << CPlayerInfo::GetInstance()->GetPrimaryWeapon()->GetMagRound() << " / " << CPlayerInfo::GetInstance()->GetPrimaryWeapon()->GetTotalRound();
	textObj[0]->SetText(ss2.str());

	std::ostringstream ss;
	ss.precision(5);
	float fps = (float)(1.f / dt);
	ss << "FPS: " << fps;
	textObj[1]->SetText(ss.str());

	std::ostringstream ss1;
	ss1.str("");
	ss1.precision(4);
	ss1 << "Door HP:" << DoorHealth << " / 2000";
	textObj[2]->SetText(ss1.str());
}

void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();
	playerInfo->Render();

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();
	playerInfo->RenderUI();
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}

	// Delete the lights
	delete lights[0];
	delete lights[1];
}
