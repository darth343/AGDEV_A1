#pragma once
#include "../GenericEntity.h"

class Mesh;

class CWall :
	public GenericEntity
{
protected:
	Vector3 position;
	Vector3 rotationAxis;
	float rotation;
public:
	CWall();
	virtual ~CWall();

	// Set Position
	void SetPos(const Vector3& pos);
	// Set Rotation Axis
	void SetRotationAxis(Vector3 Axis);
	// Set Rotation
	void SetRotation(float angle);
	// Get Position
	Vector3 GetPos() const;
	// Get Rotation Axis
	Vector3 GetRotationAxis();
	// Get Rotation
	float GetRotation();
	// Get Min vector
	Vector3 GetMin();
	// Get Max vector
	Vector3 GetMax();
	// Update
	void Update(double dt = 0.0333f);

	// Render
	void Render();
};

