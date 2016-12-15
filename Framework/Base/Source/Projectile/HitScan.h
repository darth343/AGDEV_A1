#pragma once
#include "Projectile.h"
#include "EntityBase.h"
#include "Vector3.h"
#include "Collider/Collider.h"

class Mesh;
class CPlayerInfo;

class CHitScan : public CProjectile
{
public:
	CHitScan(void);
	CHitScan(Mesh* _modelMesh);
	~CHitScan(void);
public:
	void SetLength(const float m_fLength);
	float GetLength(void) const;
	void CalculateAngles(void);

	// Update the status of this projectile
	void Update(double dt = 0.0333f);
	// Render this projectile
	virtual void Render(void);
private:
	Vector3 HitPosition;
	float m_fLength;
	float angle_x, angle_y, angle_z;
};

namespace Create
{
	CHitScan* HitScan(const std::string& _meshName,
		const Vector3& _position,
		const Vector3& _direction,
		const float m_fLength = 9999.f,
		const float m_fLifetime = 1.0,
		CPlayerInfo* _source = NULL);
};

