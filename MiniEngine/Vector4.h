#pragma once
class CVector4
{
public:
	CVector4();
	CVector4(float x, float y, float z, float w);
	~CVector4();

	void Init();

	float _x;
	float _y;
	float _z;
	float _w;
};

