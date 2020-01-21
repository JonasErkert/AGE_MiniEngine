#include "pch.h"
#include "Mat.h"
#include "math.h"

CMat::CMat()
{
	m_f00 = 1.0F; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = 0.0f;
	m_f10 = 0.0F; m_f11 = 1.0f; m_f12 = 0.0f; m_f13 = 0.0f;
	m_f20 = 0.0F; m_f21 = 0.0f; m_f22 = 1.0f; m_f23 = 0.0f;
	m_f30 = 0.0F; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
}

CMat::CMat(float f00, float f01, float f02, float f03, float f10, float f11, float f12, float f13, float f20, float f21, float f22, float f23, float f30, float f31, float f32, float f33)
{
	m_f00 = f00;
	m_f01 = f01;
	m_f02 = f02;
	m_f03 = f03;
	m_f10 = f10;
	m_f11 = f11;
	m_f12 = f12;
	m_f13 = f13;
	m_f20 = f20;
	m_f21 = f21;
	m_f22 = f22;
	m_f23 = f23;
	m_f30 = f30;
	m_f31 = f31;
	m_f32 = f32;
	m_f33 = f33;
}

CMat::CMat(float f, EMatConstruction eMatConstruction)
{
	float fSin; 
	float fCos; 
	switch (eMatConstruction)
	{
	case MatConstruction_RotateX:
		fSin = sinf(f);
		fCos = cosf(f);
		m_f00 = 1.0f; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = 0.0f;
		m_f10 = 0.0f; m_f11 = fCos; m_f12 = -fSin; m_f13 = 0.0f;
		m_f20 = 0.0f; m_f21 = fSin; m_f22 = fCos; m_f23 = 0.0f;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	case MatConstruction_RotateY:
		fSin = sinf(f);
		fCos = cosf(f);

		m_f00 = fCos; m_f01 = 0.0f; m_f02 = fSin; m_f03 = 0.0f;
		m_f10 = 0.0f; m_f11 = 1.0f; m_f12 = 0.0f; m_f13 = 0.0f;
		m_f20 =-fSin; m_f21 = 0.0f; m_f22 = fCos; m_f23 = 0.0f;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	case MatConstruction_RotateZ:
		fSin = sinf(f);
		fCos = cosf(f);

		m_f00 = fCos; m_f01 = fSin; m_f02 = 0.0f; m_f03 = 0.0f;
		m_f10 =-fSin; m_f11 = fCos; m_f12 = 0.0f; m_f13 = 0.0f;
		m_f20 = 0.0f; m_f21 = 0.0f; m_f22 = 1.0f; m_f23 = 0.0f;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	case MatConstruction_TranslateX:
		m_f00 = 1.0f; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = f;
		m_f10 = 0.0f; m_f11 = 1.0f; m_f12 = 0.0f; m_f13 = 0.0f;
		m_f20 = 0.0f; m_f21 = 0.0f; m_f22 = 1.0f; m_f23 = 0.0f;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	case MatConstruction_TranslateY:
		m_f00 = 1.0f; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = 0.0f;
		m_f10 = 0.0f; m_f11 = 1.0f; m_f12 = 0.0f; m_f13 = f;
		m_f20 = 0.0f; m_f21 = 0.0f; m_f22 = 1.0f; m_f23 = 0.0f;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	case MatConstruction_TranslateZ:
		m_f00 = 1.0f; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = 0.0f;
		m_f10 = 0.0f; m_f11 = 1.0f; m_f12 = 0.0f; m_f13 = 0.0f;
		m_f20 = 0.0f; m_f21 = 0.0f; m_f22 = 1.0f; m_f23 = f;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	}
}

CMat::CMat(float fx, float fy, float fz, EMatConstructionFloat3 eMatConstructionFloat3)
{
	switch (eMatConstructionFloat3)
	{
	case MatConstructionFloat3_TranslateVersatile:
		m_f00 = 1.0f; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = fx;
		m_f10 = 0.0f; m_f11 = 1.0f; m_f12 = 0.0f; m_f13 = fy;
		m_f20 = 0.0f; m_f21 = 0.0f; m_f22 = 1.0f; m_f23 = fz;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	case MatConstructionFloat3_ScaleVersatile:
		m_f00 =   fx; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = 0.0f;
		m_f10 = 0.0f; m_f11 =   fy; m_f12 = 0.0f; m_f13 = 0.0f;
		m_f20 = 0.0f; m_f21 = 0.0f; m_f22 =   fz; m_f23 = 0.0f;
		m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;
		break;
	}
}

CMat::CMat(float fx, float fy, float fz, float fa, EMatConstructionFloat4 eMatConstructionFloat4)
{
	switch (eMatConstructionFloat4)
	{
	case MatConstructionFloat4_RotateVersatile:
		float fSin = sinf(fa);
		float fCos = cosf(fa);
		float fInvCos = 1.0f - fCos;

		m_f00 = fx * fx * fInvCos + fCos;
		m_f01 = fx * fy * fInvCos - fz * fSin;
		m_f02 = fx * fz * fInvCos + fy * fSin;
		m_f03 = 0.0f;

		m_f10 = fy * fx * fInvCos + fz * fSin;
		m_f11 = fy * fy * fInvCos + fCos;
		m_f12 = fy * fz * fInvCos - fx * fSin;
		m_f13 = 0.0f;

		m_f20 = fz * fx * fInvCos - fy * fSin;
		m_f21 = fz * fy * fInvCos + fx * fSin;
		m_f22 = fz * fz * fInvCos + fCos;
		m_f23 = 0.0f;

		m_f30 = 0.0f;
		m_f31 = 0.0f;
		m_f32 = 0.0f;
		m_f33 = 1.0f;
		break;
	}
}

CMat::~CMat()
{
}

CMat CMat::operator=(const CMat & hmat)
{
	m_f00 = hmat.m_f00;
	m_f01 = hmat.m_f01;
	m_f02 = hmat.m_f02;
	m_f03 = hmat.m_f03;
	m_f10 = hmat.m_f10;
	m_f11 = hmat.m_f11;
	m_f12 = hmat.m_f12;
	m_f13 = hmat.m_f13;
	m_f20 = hmat.m_f20;
	m_f21 = hmat.m_f21;
	m_f22 = hmat.m_f22;
	m_f23 = hmat.m_f23;
	m_f30 = hmat.m_f30;
	m_f31 = hmat.m_f31;
	m_f32 = hmat.m_f32;
	m_f33 = hmat.m_f33;
	
	return *this;
}

CMat CMat::operator*(const CMat & m) // Matrix-Matrix-Multiplikation
{
	return CMat(
		m_f00*m.m_f00 + m_f01 * m.m_f10 + m_f02 * m.m_f20 + m_f03 * m.m_f30,
		m_f00*m.m_f01 + m_f01 * m.m_f11 + m_f02 * m.m_f21 + m_f03 * m.m_f31,
		m_f00*m.m_f02 + m_f01 * m.m_f12 + m_f02 * m.m_f22 + m_f03 * m.m_f32,
		m_f00*m.m_f03 + m_f01 * m.m_f13 + m_f02 * m.m_f23 + m_f03 * m.m_f33,

		m_f10*m.m_f00 + m_f11 * m.m_f10 + m_f12 * m.m_f20 + m_f13 * m.m_f30,
		m_f10*m.m_f01 + m_f11 * m.m_f11 + m_f12 * m.m_f21 + m_f13 * m.m_f31,
		m_f10*m.m_f02 + m_f11 * m.m_f12 + m_f12 * m.m_f22 + m_f13 * m.m_f32,
		m_f10*m.m_f03 + m_f11 * m.m_f13 + m_f12 * m.m_f23 + m_f13 * m.m_f33,

		m_f20*m.m_f00 + m_f21 * m.m_f10 + m_f22 * m.m_f20 + m_f23 * m.m_f30,
		m_f20*m.m_f01 + m_f21 * m.m_f11 + m_f22 * m.m_f21 + m_f23 * m.m_f31,
		m_f20*m.m_f02 + m_f21 * m.m_f12 + m_f22 * m.m_f22 + m_f23 * m.m_f32,
		m_f20*m.m_f03 + m_f21 * m.m_f13 + m_f22 * m.m_f23 + m_f23 * m.m_f33,

		m_f30*m.m_f00 + m_f31 * m.m_f10 + m_f32 * m.m_f20 + m_f33 * m.m_f30,
		m_f30*m.m_f01 + m_f31 * m.m_f11 + m_f32 * m.m_f21 + m_f33 * m.m_f31,
		m_f30*m.m_f02 + m_f31 * m.m_f12 + m_f32 * m.m_f22 + m_f33 * m.m_f32,
		m_f30*m.m_f03 + m_f31 * m.m_f13 + m_f32 * m.m_f23 + m_f33 * m.m_f33);
};


CMat CMat::RotateX(float fa)
{
	float fSin = sinf(fa);
	float fCos = cosf(fa);

	m_f00 = 1.0f; m_f01 = 0.0f; m_f02 = 0.0f; m_f03 = 0.0f;
	m_f10 = 0.0f; m_f11 = fCos; m_f12 =-fSin; m_f13 = 0.0f;
	m_f20 = 0.0f; m_f21 = fSin; m_f22 = fCos; m_f23 = 0.0f;
	m_f30 = 0.0f; m_f31 = 0.0f; m_f32 = 0.0f; m_f33 = 1.0f;

	return *this;
}

void CMat::ProjFov(float faHorizontal, float fRatio, 
	float fNear, float fFar)
{
	float fDist = fNear - fFar;
	if (faHorizontal == 0 || fDist == 0)
		return;
	// fx soll horizontaler Öffnungswinkel sein
	float faxFovHalf = faHorizontal * 0.5f;
	float fxScale = 1.0f / tanf(faxFovHalf);
	float fyScale = fxScale * fRatio; // fRatio soll Ratio zwischen der Höhe und der Breite des Vieports sein.

	m_f00 = fxScale; m_f01 = 0.0f;		m_f02 = 0.0f;				m_f03 = 0.0f;
	m_f10 = 0.0f;	 m_f11 = fyScale;	m_f12 = 0.0f;				m_f13 = 0.0f;
	m_f20 = 0.0f;	 m_f21 = 0.0f;		m_f22 = fFar/fDist;			m_f23 = -1.0f;
	m_f30 = 0.0f;	 m_f31 = 0.0f;		m_f32 = fFar*fNear/fDist;	m_f33 = 0.0f;
}

float CMat::Det()
{
	/*
	m_f00 	m_f01 	m_f02 	m_f03
	m_f10 	m_f11 	m_f12 	m_f13
	m_f20 	m_f21 	m_f22 	m_f23
	m_f30 	m_f31 	m_f32 	m_f33
	*/
	float f = 0;
	// Entwickle nach letzter Matrxreihe, 
	// da hier die hoechste Wahrscheinlichkeit auf Nullelemente besteht!
	if (m_f30 != 0)
	{
		float fDet30 = 0;
		fDet30 -= m_f01 * m_f12*m_f23 +
			m_f11 * m_f22*m_f03 +
			m_f02 * m_f13*m_f21;
		fDet30 += m_f21 * m_f12*m_f03 +
			m_f11 * m_f02*m_f23 +
			m_f22 * m_f13*m_f01;
		f += fDet30 * m_f30;
	}
	if (m_f31 != 0)
	{
		float fDet31 = 0;
		fDet31 += m_f00 * m_f12*m_f23 +
			m_f10 * m_f22*m_f03 +
			m_f02 * m_f13*m_f20;
		fDet31 -= m_f20 * m_f12*m_f03 +
			m_f10 * m_f02*m_f23 +
			m_f22 * m_f13*m_f00;
		f += fDet31 * m_f31;
	}
	if (m_f32 != 0)
	{
		float fDet32 = 0;
		fDet32 -= m_f00 * m_f11*m_f23 +
			m_f10 * m_f21*m_f03 +
			m_f01 * m_f13*m_f20;
		fDet32 += m_f20 * m_f11*m_f03 +
			m_f10 * m_f01*m_f23 +
			m_f21 * m_f13*m_f00;
		f += fDet32 * m_f32;
	}
	if (m_f33 != 0)
	{
		float fDet33 = 0;
		fDet33 += m_f00 * m_f11*m_f22 +
			m_f10 * m_f21*m_f02 +
			m_f01 * m_f12*m_f20;
		fDet33 -= m_f20 * m_f11*m_f02 +
			m_f10 * m_f01*m_f22 +
			m_f21 * m_f12*m_f00;
		f += fDet33 * m_f33;
	}

	return f;
}

void CMat::Inverse()
{
	CMat m = *this;
	m_f00 = m.m_f11 * m.m_f22 * m.m_f33 +
		m.m_f21 * m.m_f32 * m.m_f13 +
		m.m_f12 * m.m_f23 * m.m_f31 -
		m.m_f31 * m.m_f22 * m.m_f13 -
		m.m_f21 * m.m_f12 * m.m_f33 -
		m.m_f32 * m.m_f23 * m.m_f11;
	m_f10 = -m.m_f10 * m.m_f22 * m.m_f33 -
		m.m_f20 * m.m_f32 * m.m_f13 -
		m.m_f12 * m.m_f23 * m.m_f30 +
		m.m_f30 * m.m_f22 * m.m_f13 +
		m.m_f20 * m.m_f12 * m.m_f33 +
		m.m_f32 * m.m_f23 * m.m_f10;
	m_f20 = m.m_f10 * m.m_f21 * m.m_f33 +
		m.m_f20 * m.m_f31 * m.m_f13 +
		m.m_f11 * m.m_f23 * m.m_f30 -
		m.m_f30 * m.m_f21 * m.m_f13 -
		m.m_f20 * m.m_f11 * m.m_f33 -
		m.m_f31 * m.m_f23 * m.m_f10;
	m_f30 = -m.m_f10 * m.m_f21 * m.m_f32 -
		m.m_f20 * m.m_f31 * m.m_f12 -
		m.m_f11 * m.m_f22 * m.m_f30 +
		m.m_f30 * m.m_f21 * m.m_f12 +
		m.m_f20 * m.m_f11 * m.m_f32 +
		m.m_f31 * m.m_f22 * m.m_f10;
	m_f01 = -m.m_f01 * m.m_f22 * m.m_f33 -
		m.m_f21 * m.m_f32 * m.m_f03 -
		m.m_f02 * m.m_f23 * m.m_f31 +
		m.m_f31 * m.m_f22 * m.m_f03 +
		m.m_f21 * m.m_f02 * m.m_f33 +
		m.m_f32 * m.m_f23 * m.m_f01;
	m_f11 = m.m_f00 * m.m_f22 * m.m_f33 +
		m.m_f20 * m.m_f32 * m.m_f03 +
		m.m_f02 * m.m_f23 * m.m_f30 -
		m.m_f30 * m.m_f22 * m.m_f03 -
		m.m_f20 * m.m_f02 * m.m_f33 -
		m.m_f32 * m.m_f23 * m.m_f00;
	m_f21 = -m.m_f00 * m.m_f21 * m.m_f33 -
		m.m_f20 * m.m_f31 * m.m_f03 -
		m.m_f01 * m.m_f23 * m.m_f30 +
		m.m_f30 * m.m_f21 * m.m_f03 +
		m.m_f20 * m.m_f01 * m.m_f33 +
		m.m_f31 * m.m_f23 * m.m_f00;
	m_f31 = m.m_f00 * m.m_f21 * m.m_f32 +
		m.m_f20 * m.m_f31 * m.m_f02 +
		m.m_f01 * m.m_f22 * m.m_f30 -
		m.m_f30 * m.m_f21 * m.m_f02 -
		m.m_f20 * m.m_f01 * m.m_f32 -
		m.m_f31 * m.m_f22 * m.m_f00;

	m_f02 = m.m_f01 * m.m_f12 * m.m_f33 +
		m.m_f11 * m.m_f32 * m.m_f03 +
		m.m_f02 * m.m_f13 * m.m_f31 -
		m.m_f31 * m.m_f12 * m.m_f03 -
		m.m_f11 * m.m_f02 * m.m_f33 -
		m.m_f32 * m.m_f13 * m.m_f01;
	m_f12 = -m.m_f00 * m.m_f12 * m.m_f33 -
		m.m_f10 * m.m_f32 * m.m_f03 -
		m.m_f02 * m.m_f13 * m.m_f30 +
		m.m_f30 * m.m_f12 * m.m_f03 +
		m.m_f10 * m.m_f02 * m.m_f33 +
		m.m_f32 * m.m_f13 * m.m_f00;
	m_f22 = m.m_f00 * m.m_f11 * m.m_f33 +
		m.m_f10 * m.m_f31 * m.m_f03 +
		m.m_f01 * m.m_f13 * m.m_f30 -
		m.m_f30 * m.m_f11 * m.m_f03 -
		m.m_f10 * m.m_f01 * m.m_f33 -
		m.m_f31 * m.m_f13 * m.m_f00;
	m_f32 = -m.m_f00 * m.m_f11 * m.m_f32 -
		m.m_f10 * m.m_f31 * m.m_f02 -
		m.m_f01 * m.m_f12 * m.m_f30 +
		m.m_f30 * m.m_f11 * m.m_f02 +
		m.m_f10 * m.m_f01 * m.m_f32 +
		m.m_f31 * m.m_f12 * m.m_f00;
	m_f03 = -m.m_f01 * m.m_f12 * m.m_f23 -
		m.m_f11 * m.m_f22 * m.m_f03 -
		m.m_f02 * m.m_f13 * m.m_f21 +
		m.m_f21 * m.m_f12 * m.m_f03 +
		m.m_f11 * m.m_f02 * m.m_f23 +
		m.m_f22 * m.m_f13 * m.m_f01;
	m_f13 = m.m_f00 * m.m_f12 * m.m_f23 +
		m.m_f10 * m.m_f22 * m.m_f03 +
		m.m_f02 * m.m_f13 * m.m_f20 -
		m.m_f20 * m.m_f12 * m.m_f03 -
		m.m_f10 * m.m_f02 * m.m_f23 -
		m.m_f22 * m.m_f13 * m.m_f00;
	m_f23 = -m.m_f00 * m.m_f11 * m.m_f23 -
		m.m_f10 * m.m_f21 * m.m_f03 -
		m.m_f01 * m.m_f13 * m.m_f20 +
		m.m_f20 * m.m_f11 * m.m_f03 +
		m.m_f10 * m.m_f01 * m.m_f23 +
		m.m_f21 * m.m_f13 * m.m_f00;
	m_f33 = m.m_f00 * m.m_f11 * m.m_f22 +
		m.m_f10 * m.m_f21 * m.m_f02 +
		m.m_f01 * m.m_f12 * m.m_f20 -
		m.m_f20 * m.m_f11 * m.m_f02 -
		m.m_f10 * m.m_f01 * m.m_f22 -
		m.m_f21 * m.m_f12 * m.m_f00;

	float fDet = m.Det();
	m_f00 /= fDet;	m_f01 /= fDet;	m_f02 /= fDet;	m_f03 /= fDet;
	m_f10 /= fDet;	m_f11 /= fDet;	m_f12 /= fDet;	m_f13 /= fDet;
	m_f20 /= fDet;	m_f21 /= fDet;	m_f22 /= fDet;	m_f23 /= fDet;
	m_f30 /= fDet;	m_f31 /= fDet;	m_f32 /= fDet;	m_f33 /= fDet;
}
