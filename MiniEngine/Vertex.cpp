#include "pch.h"
#include "Vertex.h"


CVertex::CVertex()
{
}


CVertex::~CVertex()
{
}

void CVertex::Init(CVector4 vPos, CVector4 vNormal, CVector4 vTangent, CVector4 vBitangent, float fU, float fV)

{
	m_vPos = vPos;
	m_vNormal = vNormal;
	m_vTangent = vTangent;
	m_vBitangent = vBitangent;
	m_fU = fU;
	m_fV= fV;
}
