#include "pch.h"
#include "Vector4.h"


CVector4::CVector4()
	: _x(0.f)
	, _y(0.f)
	, _z(0.f)
	, _w(0.f)
{
}


CVector4::CVector4(float x, float y, float z, float w)
	: _x(x)
	, _y(y)
	, _z(x)
	, _w(w)
{
}

CVector4::~CVector4()
{
}

void CVector4::Init()
{
	_x = 0.f;
	_y = 0.f;
	_z = 0.f;
	_w = 0.f;
}
