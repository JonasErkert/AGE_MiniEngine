#pragma once

class CColor
{
public:
	CColor();
	CColor(float fR, float fG, float fB);
	CColor(float fR, float fG, float fB, float fA);
	~CColor();

	float m_fR;
	float m_fG;
	float m_fB;
	float m_fA;
};

