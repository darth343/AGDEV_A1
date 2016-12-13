#include "Enemy.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

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
{
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
	m_dSpeed = 1.0;

	// Initialize the LOD meshes
	InitLOD("Chair", "sphere", "cubeSG");

	// Initialize the Collider
	this->SetCollider(true);
	this->SetAABB(Vector3(1,1,1), Vector3(-1, -1, -1));

	// Add the EntityManager
	EntityManager::GetInstance()->AddEntity(this, true);
	SetType(T_ENEMY);
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

// Update
void CEnemy::Update(double dt)
{
	Vector3 viewVector = (target - position).Normalized();
	position += viewVector * (float)m_dSpeed * (float)dt;

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
	if (position.y != m_pTerrain->GetTerrainHeight(position))
	{
		position.y = m_pTerrain->GetTerrainHeight(position);
	}
}

void CEnemy::Render()
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();

	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(scale.x, scale.y, scale.z);
	if (GetLODStatus() == true)
	{
		if (theDetailLevel != NO_DETAILS)
			RenderHelper::RenderMesh(GetLODMesh());
	}
	modelStack.PopMatrix();
}