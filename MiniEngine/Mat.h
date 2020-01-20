#pragma once
class CMat
{
public:
	CMat();
	~CMat();

	CMat operator=(const CMat& mat);
	CMat operator*(const CMat& mat);
	
	void Translate(float fX, float fY, float fZ);

	void RotateX(float fAngle);
	void RotateY(float fAngle);
	void RotateZ(float fAngle);

	// 4*4 tuple
	float m_f00 = 1.f;
	float m_f01 = 0.f;
	float m_f02 = 0.f;
	float m_f03 = 0.f;

	float m_f10 = 0.f;
	float m_f11 = 1.f;
	float m_f12 = 0.f;
	float m_f13 = 0.f;

	float m_f20 = 0.f;
	float m_f21 = 0.f;
	float m_f22 = 1.f;
	float m_f23 = 0.f;

	float m_f30 = 0.f;
	float m_f31 = 0.f;
	float m_f32 = 0.f;
	float m_f33 = 1.f;
};

