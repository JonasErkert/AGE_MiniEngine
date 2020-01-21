#pragma once

#include <DirectXMath.h>

using namespace DirectX;

enum EMatConstruction
{
	MatConstruction_Unit,
	MatConstruction_RotateX,
	MatConstruction_RotateY,
	MatConstruction_RotateZ,
	MatConstruction_TranslateX,
	MatConstruction_TranslateY,
	MatConstruction_TranslateZ
};

enum EMatConstructionFloat3
{
	MatConstructionFloat3_TranslateVersatile,
	MatConstructionFloat3_ScaleVersatile
};

enum EMatConstructionFloat4
{
	MatConstructionFloat4_RotateVersatile
};

class CMat
{
public:
	CMat();
	CMat(float f00, float f01, float f02, float f03,
		float f10, float f11, float f12, float f13,
		float f20, float f21, float f22, float f23,
		float f30, float f31, float f32, float f33);
	CMat(float f, EMatConstruction eMatConstruction);
	CMat(float fx, float fy, float fz, EMatConstructionFloat3 eMatConstructionFloat3);
	CMat(float fx, float fy, float fz, float fa, EMatConstructionFloat4 eMatConstructionFloat4);

	~CMat();

	CMat operator=(const CMat & hmat);
	CMat operator*(const CMat & hmat);

	CMat RotateX(float fa);
	/*
	CMat RotateY(float fa);
	CMat RotateZ(float fa);
	CMat RotateXDelta(float fa);
	CMat RotateYDelta(float fa);
	CMat RotateZDelta(float fa);
	*/
	void ProjFov(float faHorizontal, float fRatio, float fNear, float fFar);

	/** Calculate Determinante */
	float Det();
	void Inverse();

	float m_f00;
	float m_f01;
	float m_f02;
	float m_f03;
	float m_f10;
	float m_f11;
	float m_f12;
	float m_f13;
	float m_f20;
	float m_f21;
	float m_f22;
	float m_f23;
	float m_f30;
	float m_f31;
	float m_f32;
	float m_f33;
};

inline void CMat_2_XMFLOAT4X4(CMat mat, XMFLOAT4X4& xm)
{
	xm._11 = mat.m_f00;
	xm._12 = mat.m_f01;
	xm._13 = mat.m_f02;
	xm._14 = mat.m_f03;

	xm._21 = mat.m_f10;
	xm._22 = mat.m_f11;
	xm._23 = mat.m_f12;
	xm._24 = mat.m_f13;

	xm._31 = mat.m_f20;
	xm._32 = mat.m_f21;
	xm._33 = mat.m_f22;
	xm._34 = mat.m_f23;

	xm._41 = mat.m_f30;
	xm._42 = mat.m_f31;
	xm._43 = mat.m_f32;
	xm._44 = mat.m_f33;
}
