#include "PlayerInfo.h"
#include <iostream>

#include "../Application.h"
#include "MouseController.h"
#include "KeyboardController.h"
#include "Mtx44.h"
#include "../Projectile/Projectile.h"
#include "../WeaponInfo/Pistol.h"
#include "../WeaponInfo/LaserBlaster.h"
#include "../WeaponInfo/GrenadeThrow.h"
#include "../SpatialPartition/SpatialPartition.h"
#include "../Wall/WallEntity.h"
#include "../TextEntity.h"

// Allocating and initializing CPlayerInfo's static data member.  
// The pointer is allocated but not the object's constructor.
CPlayerInfo *CPlayerInfo::s_instance = 0;

CPlayerInfo::CPlayerInfo(void)
	: m_dSpeed(40.0)
	, m_dRotationSpeed(160.0)
	, m_dAcceleration(10.0)
	, m_bJumpUpwards(false)
	, m_dJumpSpeed(25.0)
	, m_dJumpAcceleration(-5.0)
	, m_bFallDownwards(false)
	, m_dFallSpeed(0.0)
	, m_dFallAcceleration(-5.0)
	, attachedCamera(NULL)
	, m_pTerrain(NULL)
	, primaryWeapon(NULL)
	, secondaryWeapon(NULL)
	, hitmarker_type(NONE)
	, HitmarkerScale(100)
	, HitmarkerWaitTime(0.15f)
	, HitmarkerCurrentTime(0.f)
{
}

CPlayerInfo::~CPlayerInfo(void)
{
	if (secondaryWeapon)
	{
		delete secondaryWeapon;
		secondaryWeapon = NULL;
	}
	if (primaryWeapon)
	{
		delete primaryWeapon;
		primaryWeapon = NULL;
	}
	m_pTerrain = NULL;
}

// Initialise this class instance
void CPlayerInfo::Init(void)
{
	// Set the default values
	defaultPosition.Set(0 ,0, -58);
	defaultTarget.Set(0,0,0);
	defaultUp.Set(0,1,0);

	// Set the current values
	position.Set(0, 0, -58);
	target.Set(0, 0, 0);
	up.Set(0, 1, 0);

	// Set Boundary
	maxBoundary.Set(1,1,1);
	minBoundary.Set(-1, -1, -1);

	// Set the pistol as the primary weapon
	primaryWeapon = new CPistol();
	primaryWeapon->Init();
	// Set the laser blaster as the secondary weapon
	//secondaryWeapon = new CLaserBlaster();
	//secondaryWeapon->Init();
	secondaryWeapon = new CGrenadeThrow();
	secondaryWeapon->Init();
}

// Returns true if the player is on ground
bool CPlayerInfo::isOnGround(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is jumping upwards
bool CPlayerInfo::isJumpUpwards(void)
{
	if (m_bJumpUpwards == true && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is on freefall
bool CPlayerInfo::isFreeFall(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == true)
		return true;

	return false;
}

// Set the player's status to free fall mode
void CPlayerInfo::SetOnFreeFall(bool isOnFreeFall)
{
	if (isOnFreeFall == true)
	{
		m_bJumpUpwards = false;
		m_bFallDownwards = true;
		m_dFallSpeed = 0.0;
	}
}

void CPlayerInfo::SetHitmarker(string hm_type, bool isKill)
{
	if (hm_type == "CRIT")
	{
		hitmarker_type = CRIT;
		Application::GetInstance().m_soundEngine->play2D("Music\\HeadShot.mp3");
	}
	else if (hm_type == "NON_CRIT")
	{
		hitmarker_type = NON_CRIT;
		Application::GetInstance().m_soundEngine->play2D("Music\\BodyShot.mp3");
	}

	if (isKill)
	{
		hitmarker_type = KILL;
		Application::GetInstance().m_soundEngine->play2D("Music\\KillSound.mp3");
		if (hm_type == "CRIT")
		primaryWeapon->SetMagRound(primaryWeapon->GetMaxMagRound());
	}
	HitmarkerCurrentTime = 0.f;
	HitmarkerScale = 200.f;
}

// Set the player to jumping upwards
void CPlayerInfo::SetToJumpUpwards(bool isOnJumpUpwards)
{
	if (isOnJumpUpwards == true)
	{
		m_bJumpUpwards = true;
		m_bFallDownwards = false;
		m_dJumpSpeed = 4.0;
	}
}

// Set position
void CPlayerInfo::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set target
void CPlayerInfo::SetTarget(const Vector3& target)
{
	this->target = target;
}

// Set position
void CPlayerInfo::SetUp(const Vector3& up)
{
	this->up = up;
}

// Set m_dJumpAcceleration of the player
void CPlayerInfo::SetJumpAcceleration(const double m_dJumpAcceleration)
{
	this->m_dJumpAcceleration = m_dJumpAcceleration;
}

// Set Fall Acceleration of the player
void CPlayerInfo::SetFallAcceleration(const double m_dFallAcceleration)
{
	this->m_dFallAcceleration = m_dFallAcceleration;
}

// Set the boundary for the player info
void CPlayerInfo::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}

// Set the terrain for the player info
void CPlayerInfo::SetTerrain(GroundEntity* m_pTerrain)
{
	if (m_pTerrain != NULL)
	{
		this->m_pTerrain = m_pTerrain;

		SetBoundary(this->m_pTerrain->GetMaxBoundary(), this->m_pTerrain->GetMinBoundary());
	}
}

// Stop the player's movement
void CPlayerInfo::StopVerticalMovement(void)
{
	m_bJumpUpwards = false;
	m_bFallDownwards = false;
}

// Reset this player instance to default
void CPlayerInfo::Reset(void)
{
	// Set the current values to default values
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;

	// Stop vertical movement too
	StopVerticalMovement();
}

// Get position x of the player
Vector3 CPlayerInfo::GetPos(void) const
{
	return position;
}

// Get target
Vector3 CPlayerInfo::GetTarget(void) const
{
	return target;
}
// Get Up
Vector3 CPlayerInfo::GetUp(void) const
{
	return up;
}

// Get m_dJumpAcceleration of the player
double CPlayerInfo::GetJumpAcceleration(void) const
{
	return m_dJumpAcceleration;
}

// Get the terrain for the player info
GroundEntity* CPlayerInfo::GetTerrain(void)
{
	return m_pTerrain;
}

// Update Jump Upwards
void CPlayerInfo::UpdateJumpUpwards(double dt)
{
	if (m_bJumpUpwards == false)
		return;

	// Update position and target y values
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dJumpSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	target.y += (float)(m_dJumpSpeed*dt + 0.5 * m_dJumpAcceleration * dt * dt);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t;
	// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	m_dJumpSpeed = m_dJumpSpeed + m_dJumpAcceleration * dt;
	// Check if the jump speed is less than zero, then it should be falling
	if (m_dJumpSpeed < 0.0)
	{
		m_dJumpSpeed = 0.0;
		m_bJumpUpwards = false;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = true;
	}
}

// Update FreeFall
void CPlayerInfo::UpdateFreeFall(double dt)
{
	if (m_bFallDownwards == false)
		return;

	// Update position and target y values
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dFallSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	target.y += (float)(m_dFallSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t;
	// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	m_dFallSpeed = m_dFallSpeed + m_dFallAcceleration * dt;
	// Check if the jump speed is below terrain, then it should be reset to terrain height
	if (position.y < m_pTerrain->GetTerrainHeight(position))
	{
		Vector3 viewDirection = target - position;
		position.y = m_pTerrain->GetTerrainHeight(position);
		target = position + viewDirection;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = false;
	}
}

void CPlayerInfo::CheckWallCollision(Vector3& nextPos)
{
	vector<EntityBase*> ObjectList = CSpatialPartition::GetInstance()->GetObjects(nextPos);
	bool moveX = true;
	bool moveZ = true;

	float boxSize = 4;

	for (int i = 0; i < ObjectList.size(); i++)
	{
		if (ObjectList[i]->HasCollider())
		{
			GenericEntity* Wall = dynamic_cast<GenericEntity*>(ObjectList[i]);
			if (moveX)
			{
				if (nextPos.x > Wall->GetMin().x - boxSize && nextPos.x < Wall->GetMax().x + boxSize)
				{
					if ((position.z > Wall->GetMin().z - boxSize && position.z < Wall->GetMax().z + boxSize))
					{
						moveX = false;
					}
				}
			}
			if (moveZ)
			{
				if (nextPos.z > Wall->GetMin().z - boxSize && nextPos.z < Wall->GetMax().z + boxSize)
				{
					if ((position.x > Wall->GetMin().x - boxSize && position.x < Wall->GetMax().x + boxSize))
					{
						moveZ = false;
					}
				}
			}
		}
	}
	if (!moveX)
	{
		nextPos.x = position.x;
	}
	if (!moveZ)
	{
		nextPos.z = position.z;
	}
}

void CPlayerInfo::UpdateCamera(double dt)
{
	double mouse_diff_x, mouse_diff_y;
	MouseController::GetInstance()->GetMouseDelta(mouse_diff_x, mouse_diff_y);

	double camera_yaw = mouse_diff_x * 0.0174555555555556;		// 3.142 / 180.0
	double camera_pitch = mouse_diff_y * 0.0174555555555556;	// 3.142 / 180.0

	// Update the position if the WASD buttons were activated
	if (KeyboardController::GetInstance()->IsKeyDown('W') ||
		KeyboardController::GetInstance()->IsKeyDown('A') ||
		KeyboardController::GetInstance()->IsKeyDown('S') ||
		KeyboardController::GetInstance()->IsKeyDown('D'))
	{
		Vector3 viewVector = target - position;
		Vector3 rightUV;
		Vector3 nextPosition;
		if (KeyboardController::GetInstance()->IsKeyDown('W'))
		{
			nextPosition = position + viewVector.Normalized() * (float)m_dSpeed * (float)dt;
		}
		else if (KeyboardController::GetInstance()->IsKeyDown('S'))
		{
			nextPosition = position - viewVector.Normalized() * (float)m_dSpeed * (float)dt;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('A'))
		{
			rightUV = (viewVector.Normalized()).Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			nextPosition = position - rightUV * (float)m_dSpeed * (float)dt;
		}
		else if (KeyboardController::GetInstance()->IsKeyDown('D'))
		{
			rightUV = (viewVector.Normalized()).Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			nextPosition = position + rightUV * (float)m_dSpeed * (float)dt;
		}
		CheckWallCollision(nextPosition);
		position = nextPosition;
		// Constrain the position
		Constrain();
		// Update the target
		target = position + viewVector;
	}

	// Rotate the view direction
	if (KeyboardController::GetInstance()->IsKeyDown(VK_LEFT) ||
		KeyboardController::GetInstance()->IsKeyDown(VK_RIGHT) ||
		KeyboardController::GetInstance()->IsKeyDown(VK_UP) ||
		KeyboardController::GetInstance()->IsKeyDown(VK_DOWN))
	{
		Vector3 viewUV = (target - position).Normalized();
		Vector3 rightUV;
		if (KeyboardController::GetInstance()->IsKeyDown(VK_LEFT))
		{
			float yaw = (float)m_dRotationSpeed * (float)dt;
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			viewUV = rotation * viewUV;
			target = position + viewUV;
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
		}
		else if (KeyboardController::GetInstance()->IsKeyDown(VK_RIGHT))
		{
			float yaw = (float)(-m_dRotationSpeed * (float)dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			viewUV = rotation * viewUV;
			target = position + viewUV;
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
		}
		if (KeyboardController::GetInstance()->IsKeyDown(VK_UP))
		{
			float pitch = (float)(m_dRotationSpeed * (float)dt);
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
			Mtx44 rotation;
			rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
			viewUV = rotation * viewUV;
			target = position + viewUV;
		}
		else if (KeyboardController::GetInstance()->IsKeyDown(VK_DOWN))
		{
			float pitch = (float)(-m_dRotationSpeed * (float)dt);
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
			Mtx44 rotation;
			rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
			viewUV = rotation * viewUV;
			target = position + viewUV;
		}
	}

	//Update the camera direction based on mouse move
	{
		Vector3 viewUV = (target - position).Normalized();
		Vector3 rightUV;

		{
			float yaw = (float)(-m_dRotationSpeed * camera_yaw * (float)dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			viewUV = rotation * viewUV;
			target = position + viewUV;
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
		}
		{
			float pitch = (float)(-m_dRotationSpeed * camera_pitch * (float)dt);
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
			Mtx44 rotation;
			rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
			viewUV = rotation * viewUV;
			target = position + viewUV;
		}
	}

	// If the user presses SPACEBAR, then make him jump
	if (KeyboardController::GetInstance()->IsKeyDown(VK_SPACE) &&
		position.y == m_pTerrain->GetTerrainHeight(position))
	{
		SetToJumpUpwards(true);
	}
}

void CPlayerInfo::UpdateHitmarker(double dt)
{
	if (HitmarkerCurrentTime > HitmarkerWaitTime)
	{
		hitmarker_type = NONE;
		HitmarkerScale = 100.f;
	}
	else
	{
		HitmarkerCurrentTime += dt;
		if (hitmarker_type != NONE)
		{
			HitmarkerScale += dt * 1600;
			if (HitmarkerScale > 400)
			{
				HitmarkerScale = 400.f;
			}
		}
	}
}

/********************************************************************************
 Hero Update
 ********************************************************************************/
void CPlayerInfo::Update(double dt)
{
	UpdateCamera(dt);
	UpdateHitmarker(dt);
	//CGrid* temp = CSpatialPartition::GetInstance()->GetGrid(position);
	// Update the weapons
	if (KeyboardController::GetInstance()->IsKeyReleased('R'))
	{
		if (primaryWeapon)
		{
			primaryWeapon->Reload();
			//primaryWeapon->PrintSelf();
		}
		if (secondaryWeapon)
		{
			secondaryWeapon->Reload();
			//secondaryWeapon->PrintSelf();
		}
	}
	if (primaryWeapon)
		primaryWeapon->Update(dt);
	if (secondaryWeapon)
		secondaryWeapon->Update(dt);

	// if Mouse Buttons were activated, then act on them
	if (MouseController::GetInstance()->IsButtonPressed(MouseController::LMB))
	{
		if (primaryWeapon)
			primaryWeapon->Discharge(position, target, this);

	}
	else if (MouseController::GetInstance()->IsButtonPressed(MouseController::RMB))
	{
		if (secondaryWeapon)
			secondaryWeapon->Discharge(position, target, this);
	}

	// If the user presses R key, then reset the view to default values
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
	{
		Reset();
	}
	else
	{
		UpdateJumpUpwards(dt);
		UpdateFreeFall(dt);
	}

	// If a camera is attached to this playerInfo class, then update it
	if (attachedCamera)
	{
		attachedCamera->SetCameraPos(position);
		attachedCamera->SetCameraTarget(target);
		attachedCamera->SetCameraUp(up);
	}
}

void CPlayerInfo::Render()
{
	Vector3 view = (target - position).Normalized();

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	modelStack.Translate(position.x + view.x, position.y + view.y, position.z + view.z);
	modelStack.Rotate(- 85 + Math::RadianToDegree(atan2(view.x, view.z)), 0, 1, 0);
	modelStack.Rotate(90.f - Math::RadianToDegree(acos(view.Dot(Vector3(0, 1, 0)))), 0, 0, 1);
	modelStack.Translate(1.6f, -0.6f, 0.6f);

	RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("gun"));
	modelStack.PopMatrix();
}

void CPlayerInfo::RenderUI()
{

	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	modelStack.Translate(0, 0, 0);
	switch (hitmarker_type)
	{
	case KILL:
		modelStack.Scale(HitmarkerScale, HitmarkerScale, HitmarkerScale);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("KILL_HITMARKER"));
		break;
	case CRIT:
		modelStack.Scale(HitmarkerScale, HitmarkerScale, HitmarkerScale);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("CRIT_HITMARKER"));
		break;
	case NON_CRIT:
		modelStack.Scale(HitmarkerScale, HitmarkerScale, HitmarkerScale);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("NONCRIT_HITMARKER"));
		break;
	case NONE:
		modelStack.Scale(50, 50, 50);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("crosshair"));
		break;
	}
	modelStack.PopMatrix();
}

// Constrain the position within the borders
void CPlayerInfo::Constrain(void)
{
	// Constrain player within the boundary
	if (position.x > maxBoundary.x - 1.0f)
		position.x = maxBoundary.x - 1.0f;
	//if (position.y > maxBoundary.y - 1.0f)
	//	position.y = maxBoundary.y - 1.0f;
	if (position.z > maxBoundary.z - 1.0f)
		position.z = maxBoundary.z - 1.0f;
	if (position.x < minBoundary.x + 1.0f)
		position.x = minBoundary.x + 1.0f;
	//if (position.y < minBoundary.y + 1.0f)
	//	position.y = minBoundary.y + 1.0f;
	if (position.z < minBoundary.z + 1.0f)
		position.z = minBoundary.z + 1.0f;

	// if the player is not jumping nor falling, then adjust his y position
	if ((m_bJumpUpwards == false) && (m_bFallDownwards == false))
	{
		// if the y position is not equal to terrain height at that position, 
		// then update y position to the terrain height
		if (position.y != m_pTerrain->GetTerrainHeight(position))
			position.y = m_pTerrain->GetTerrainHeight(position);
	}
}

void CPlayerInfo::AttachCamera(FPSCamera* _cameraPtr)
{
	attachedCamera = _cameraPtr;
}

void CPlayerInfo::DetachCamera()
{
	attachedCamera = nullptr;
}
