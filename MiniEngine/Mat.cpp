#include "pch.h"
#include "Mat.h"

void CMat::Translate(float fX, float fY, float fZ)
{
	m_f30 = fX;
	m_f31 = fY;
	m_f32 = fZ;
}
