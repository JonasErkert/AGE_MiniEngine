#pragma once

struct SVertex
{
	float fX, fY, fZ;
	float fxNormal, fyNormal, fzNormal;
	float fxTangent, fyTangent, fzTangent;
	float fxBitangent, fyBitangent, fzBitangent;
	float fU, fV;
};

class CGeo
{
public:
	CGeo();
	~CGeo();

	// Vertices
	SVertex* m_aVertex;
	int m_iVertices;

	// Indices
	int* m_aiIndex;
	int m_iIndices;
};

