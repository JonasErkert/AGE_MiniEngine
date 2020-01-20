#pragma once
#include "Geo.h"
#include "Color.h"

class CGeoCube : public CGeo
{
public:
	CGeoCube();
	~CGeoCube();

	void Init(float fxSize, float fySize, float fzSize, CColor Color);
};

