#pragma once
#include "../GenericEntity.h"
#include "../GroundEntity.h"

class Mesh;

class CEnemy :
	public GenericEntity
{
protected:
	Vector3 defaultPosition, defaultTarget, defaultUp;
	Vector3 target, up;
	Vector3 maxBoundary, minBoundary;
	GroundEntity* m_pTerrain;

	double m_dSpeed;
	double m_dAcceleration;
public:
	CEnemy();
	virtual ~CEnemy();

	void Init();
	// Reset this player instance to default
	void Reset();
	// Set Position
	void SetPos(const Vector3& pos);
	// Set Target
	void SetTarget(const Vector3& pos);
	// Set Up
	void SetUp(const Vector3& pos);
	// Set the boundary for the player info
	void SetBoundary(Vector3 max, Vector3 min);
	// Set the terinra for the player info
	void SetTerrain(GroundEntity* m_pTerrain);

	// Get Position
	Vector3 GetPos() const;
	// Get Target
	Vector3 GetTarget() const;
	// Get Uo
	Vector3 GetUp() const;
	// Get the terrain for the player info
	GroundEntity* GetTerrain();

	// Update
	void Update(double dt = 0.0333f);

	// Constrain the position within the borders
	void Constrain();

	// Render
	void Render();
};
