#include "Enemy.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../PlayerInfo/PlayerInfo.h"
#include "../../Common/Source/SceneManager.h"
#include "../SceneText.h"
#define Vector3ToTripleFloat(var)var.x, var.y, var.z

CEnemy::CEnemy()
: GenericEntity(NULL)
, defaultPosition(Vector3(0.f, 0.f, 0.f))
, defaultTarget(Vector3(0.f, 0.f, 0.f))
, defaultUp(Vector3(0.f, 0.f, 0.f))
, target(Vector3(0.f, 0.f, 0.f))
, up(Vector3(0.f, 0.f, 0.f))
, maxBoundary(Vector3(0.f, 0.f, 0.f))
, minBoundary(Vector3(0.f, 0.f, 0.f))
, m_pTerrain(NULL)
, prevPosition(Vector3(0.f, 0.f, 0.f))
{
	SetShouldRender(false);
}


CEnemy::~CEnemy()
{
}

void CEnemy::Init()
{
	// Set the default values
	defaultPosition.Set(0.f,0.f,10.f);
	defaultTarget.Set(0.f, 0.f, 0.f);
	defaultUp.Set(0.f, 1.f, 0.f);

	// Set the current values
	position.Set(10.0f, 0.f, 0.f);
	target.Set(10.0f, 0.f, 450.f);
	up.Set(0.0f, 1.f, 0.f);

	// Set Boundary
	maxBoundary.Set(1, 1, 1);
	minBoundary.Set(1, 1, 1);

	// Set Speed
	m_dSpeed = 55.0;

	// Initialize the LOD meshes
	InitLOD("Chair", "sphere", "cubeSG");

	// Initialize the Collider
	this->SetCollider(true);
	this->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));
	this->SetScale(Vector3(3, 3, 3));
	this->SetIsEnemy(true);
	health = 150;
	GenericEntity* BodyEntity = Create::Asset("RobotBody_high", Vector3(0.0f, 0.0f, 0.0f));
	BodyNode = CSceneGraph::GetInstance()->AddNode(BodyEntity);
	BodyEntity->SetAABB(MeshBuilder::GetInstance()->GetMesh("RobotBody_low")->Max, MeshBuilder::GetInstance()->GetMesh("RobotBody_low")->Min);
	BodyEntity->InitLOD("RobotBody_high", "RobotBody_med", "RobotBody_low");

	GenericEntity* HeadEntity = Create::Asset("RobotHead_high", Vector3(0.0f, 0.0f, 0.0f));
	HeadNode = BodyNode->AddChild(HeadEntity);
	HeadOffset = Vector3(0.0f, 0.7f, 0.0f);
	HeadEntity->SetAABB(MeshBuilder::GetInstance()->GetMesh("RobotHead_low")->Max, MeshBuilder::GetInstance()->GetMesh("RobotHead_low")->Min);
	HeadEntity->InitLOD("RobotHead_high", "RobotHead_med", "RobotHead_low");
	HeadNode->offset = HeadOffset;
	//HeadNode->ApplyTranslate(0.f, 0.7f, 0.f);

	GenericEntity* RHandEntity = Create::Asset("RobotHand_high", Vector3(0.0f, 0.0f, 0.0f));
	RHandNode = BodyNode->AddChild(RHandEntity);
	RHandOffset = Vector3(-0.6f, 0.0f, 0.0f);
	RHandEntity->SetAABB(MeshBuilder::GetInstance()->GetMesh("RobotHand_low")->Max, MeshBuilder::GetInstance()->GetMesh("RobotHand_low")->Min);
	RHandEntity->InitLOD("RobotHand_high", "RobotHand_med", "RobotHand_low");
	RHandNode->offset = RHandOffset;
	//RHandNode->ApplyTranslate(-0.6, 0.f, 0.f);

	GenericEntity* LHandEntity = Create::Asset("RobotHand_high", Vector3(0.0f, 0.0f, 0.0f));
	LHandNode = BodyNode->AddChild(LHandEntity);
	LHandOffset = Vector3(0.6f, 0.0f, 0.0f);
	LHandEntity->SetAABB(MeshBuilder::GetInstance()->GetMesh("RobotHand_low")->Max, MeshBuilder::GetInstance()->GetMesh("RobotHand_low")->Min);
	LHandEntity->InitLOD("RobotHand_high", "RobotHand_med", "RobotHand_low");
	LHandNode->offset = LHandOffset;
	//LHandNode->ApplyTranslate(0.6, 0.f, 0.f);

	// Add the EntityManager
	EntityManager::GetInstance()->AddEntity(this, true);
}

// Reset this player instance to default
void CEnemy::Reset()
{
	// Set the current values to default values
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

// Set Position
void CEnemy::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set Target
void CEnemy::SetTarget(const Vector3& target)
{
	this->target = target;
}

// Set Up
void CEnemy::SetUp(const Vector3& up)
{
	this->up = up;
}

// Set the boundary for the player info
void CEnemy::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}

// Set the terrain for the player infp
void CEnemy::SetTerrain(GroundEntity* m_pTerrain)
{
	if (m_pTerrain != NULL)
	{
		this->m_pTerrain = m_pTerrain;
		SetBoundary(this->m_pTerrain->GetMaxBoundary(), this->m_pTerrain->GetMinBoundary());
	}
}

// Get position
Vector3 CEnemy::GetPos() const
{
	return position;
}

// Get target
Vector3 CEnemy::GetTarget() const
{
	return target;
}

// Get Up
Vector3 CEnemy::GetUp() const
{
	return up;
}

// Get the terrain for the player info
GroundEntity* CEnemy::GetTerrain()
{
	return m_pTerrain;
}

void CEnemy::Explode()
{
	SceneText* scene = dynamic_cast<SceneText*>(SceneManager::GetInstance()->getActiveScene());
	scene->DoorHealth -= 200;
	this->SetIsDone(true);
	this->BodyNode->GetEntity()->SetIsDone(true);
}

void CEnemy::UpdateMatrices()
{
	Vector3 MainDirection = (target - position).Normalized();
	Vector3 right = MainDirection.Cross(up);
	right.y = 0;
	right.Normalize();

	float MainAngle = Math::RadianToDegree(atan2(0, 1) - atan2(MainDirection.z, MainDirection.x)) + 90;
	BodyNode->Reset();
	BodyNode->SetScale(scale.x, scale.y, scale.z);
	BodyNode->ApplyTranslate(position.x / scale.x, position.y / scale.y, position.z / scale.z);
	BodyNode->ApplyRotate(MainAngle, 0, 1, 0);
	
	Vector3 direction = (CPlayerInfo::GetInstance()->GetPos() - (position + (right * LHandOffset.Length() * scale.x) )).Normalized();
	if (direction.Dot(MainDirection) > 0)
		direction = (CPlayerInfo::GetInstance()->GetPos() - (position - (right * RHandOffset.Length() * scale.x))).Normalized();
	float angle = Math::RadianToDegree(atan2(0, 1) - atan2(direction.z, direction.x)) + 90;
	LHandNode->Reset();
	//if (direction.Dot(MainDirection) > 0)
	//	angle + 180;
	LHandNode->ApplyRotate(angle, 0, 1, 0);
	LHandNode->ApplyTranslate(Vector3ToTripleFloat(LHandOffset));

	direction = (CPlayerInfo::GetInstance()->GetPos() - (position - (right * RHandOffset.Length() * scale.x))).Normalized();
	if (direction.Dot(MainDirection) > 0)
		direction = (CPlayerInfo::GetInstance()->GetPos() - (position + (right * RHandOffset.Length() * scale.x))).Normalized();
	angle = Math::RadianToDegree(atan2(0, 1) - atan2(direction.z, direction.x)) + 90;
	RHandNode->Reset();
	RHandNode->ApplyRotate(angle, 0, 1, 0);
	RHandNode->ApplyTranslate(Vector3ToTripleFloat(RHandOffset));

	HeadNode->Reset();
	HeadNode->ApplyTranslate(Vector3ToTripleFloat(HeadOffset));
	//target = CPlayerInfo::GetInstance()->GetPos();
}

// Update
void CEnemy::Update(double dt)
{
	prevPosition = position;
	Vector3 direction = (target - position);
	if (((position + direction.Normalized() * (float)m_dSpeed * (float)dt) - target).Length() > 5)
		position += direction.Normalized() * (float)m_dSpeed * (float)dt;
	else
		Explode();
	Constrain();
	// Update the target
	if (position.z > 400.f)
	{
		target.z = position.z * -1;
	}
	else if (position.z < -400.f)
	{
		target.z = position.z * -1;
	}
}

void CEnemy::Constrain()
{
	if (position.x > maxBoundary.x - 1.0f)
		position.x = maxBoundary.x - 1.0f;
	if (position.z > maxBoundary.z - 1.0f)
		position.z = maxBoundary.z - 1.0f;
	if (position.x < minBoundary.x + 1.0f)
		position.x = minBoundary.x + 1.0f;
	if (position.z < minBoundary.z + 1.0f)
		position.z = minBoundary.z + 1.0f;

	// if the y position is not equal to terrain height at that position
	// then update y position to the terrain height
	if (position.y - 2 != m_pTerrain->GetTerrainHeight(position))
	{
		position.y = m_pTerrain->GetTerrainHeight(position) - 2;
	}
	UpdateMatrices();
}

void CEnemy::Render()
{
	cout << "This should not be called" << endl;
	//MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();

	//modelStack.PushMatrix();
	//modelStack.Translate(position.x, position.y, position.z);
	//modelStack.Scale(scale.x, scale.y, scale.z);
	//if (GetLODStatus() == true)
	//{
	//	//if (theDetailLevel != NO_DETAILS)
	//	//	RenderHelper::RenderMesh(GetLODMesh());
	//	//BodyNode->Render();
	//}
	//modelStack.PopMatrix();
}