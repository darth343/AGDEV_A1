#ifndef SCENE_TEXT_H
#define SCENE_TEXT_H

#include "Scene.h"
#include "Mtx44.h"
#include "PlayerInfo/PlayerInfo.h"
#include "GroundEntity.h"
#include "FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "GenericEntity.h"
#include "SceneGraph/UpdateTransformation.h"
#include "Enemy\Enemy.h"
#include "Wall\WallEntity.h"

class ShaderProgram;
class SceneManager;
class TextEntity;
class Light;

struct EnemySpawner
{
	void Init(){ currentTime = 0.f; waitTime = 6.f; };
	float currentTime = 0.f;
	float waitTime = 60.f;
	Vector3 Position;
};

class SceneText : public Scene
{	
public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void SpawnEnemies(double dt);
	float DoorHealth;
private:
	SceneText(SceneManager* _sceneMgr); // This is used to register to SceneManager

	ShaderProgram* currProg;
	CPlayerInfo* playerInfo;
	GroundEntity* groundEntity;
	FPSCamera camera;
	TextEntity* textObj[3];
	Light* lights[2];
	EnemySpawner Spawner[3];
	GenericEntity* theCube;
	CWall* theWall;
	static SceneText* sInstance; // The pointer to the object that gets registered

	// Enemy Spawn Variables

};

#endif