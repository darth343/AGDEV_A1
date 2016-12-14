#include "WallEntity.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

CWall::CWall()
: GenericEntity(NULL)
, position(Vector3(0.f, 0.f, 0.f))
, rotationAxis(0, 1, 0)
, rotation(0.f)
{
	InitLOD("cube", "cubeSG", "sphere");
	SetType(T_WALL);
	// Initialize the Collider
	this->SetCollider(true);
	this->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));
	// Add the EntityManager
	EntityManager::GetInstance()->AddEntity(this, true);
}

CWall::~CWall()
{
}

void CWall::SetPos(const Vector3& pos)
{
	position = pos;
}

void CWall::SetRotationAxis(Vector3 Axis)
{
	rotationAxis = Axis;
}

void CWall::SetRotation(float angle)
{
	rotation = angle;
}


Vector3 CWall::GetPos() const
{
	return position;
}

Vector3 CWall::GetRotationAxis()
{
	return rotationAxis;
}

float CWall::GetRotation()
{
	return rotation;
}

Vector3 CWall::GetMin()
{
	Vector3 min;
	min.x = GetMinAABB().x * scale.x;
	min.y = GetMinAABB().y * scale.y;
	min.z = GetMinAABB().z * scale.z;

	return min;
}

Vector3 CWall::GetMax()
{
	Vector3 max;
	max.x = GetMaxAABB().x * scale.x;
	max.y = GetMaxAABB().y * scale.y;
	max.z = GetMaxAABB().z * scale.z;

	return max;
}

void CWall::Update(double dt)
{
	// Empty	
}

void CWall::Render()
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();

	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(scale.x, scale.y, scale.z);
	modelStack.Rotate(rotation, rotationAxis.x, rotationAxis.y, rotationAxis.z);
	if (GetLODStatus() == true)
	{
		if (theDetailLevel != NO_DETAILS)
			RenderHelper::RenderMesh(GetLODMesh());
	}
	modelStack.PopMatrix();
}