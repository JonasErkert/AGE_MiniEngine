#include "pch.h"
#include "Color.h"

CColor::CColor()
	: m_fR(0.f)
	, m_fG(0.f)
	, m_fB(0.f)
	, m_fA(1.f)
{
}

CColor::CColor(float fR, float fG, float fB)
	: m_fR(fR)
	, m_fG(fG)
	, m_fB(fB)
	, m_fA(1.f)
{
}

CColor::CColor(float fR, float fG, float fB, float fA)
	: m_fR(fR)
	, m_fG(fG)
	, m_fB(fB)
	, m_fA(fA)
{
}

CColor::~CColor()
{
}
