#pragma once
#include "Vector4.h"
class CVertex
{
public:
	CVertex();
	~CVertex();

	void Init(CVector4 vPos, CVector4 vNormal, CVector4 vTangent, CVector4 vBitangent, float fU, float fV);
	
	CVector4 m_vPos;
	CVector4 m_vNormal;
	CVector4 m_vTangent;
	CVector4 m_vBitangent;

	float m_fU;
	float m_fV;
};

