#include "pch.h"
#include "GeoCube.h"
#include "Windows.h"


CGeoCube::CGeoCube()
{
	m_iVertices = 24;
	m_aVertex = (SVertex*)malloc(sizeof(SVertex)*m_iVertices);

	// 6 sides * 2 triangles * 3 vertices
	m_iIndices = 36;
	m_aiIndex = (int*)malloc(sizeof(int)*m_iIndices);
}

CGeoCube::~CGeoCube()
{
	if (m_aVertex)
		free(m_aVertex);
	
	if (m_aiIndex)
		free(m_aiIndex);
}

void CGeoCube::Init(float fxSize, float fySize, float fzSize, CColor Color)
{
	float fxSizeHalf = fxSize * 0.5f;
	float fySizeHalf = fySize * 0.5f;
	float fzSizeHalf = fzSize * 0.5f;

	// Front:
	m_aVertex[0].fX = -fxSizeHalf;
	m_aVertex[0].fY = -fySizeHalf;
	m_aVertex[0].fZ = fzSizeHalf;
	m_aVertex[0].fxNormal = 0.0f;
	m_aVertex[0].fyNormal = 0.0f;
	m_aVertex[0].fzNormal = 1.0f;

	m_aVertex[1].fX = fxSizeHalf;
	m_aVertex[1].fY = -fySizeHalf;
	m_aVertex[1].fZ = fzSizeHalf;
	m_aVertex[1].fxNormal = 0.0f;
	m_aVertex[1].fyNormal = 0.0f;
	m_aVertex[1].fzNormal = 1.0f;

	m_aVertex[2].fX = fxSizeHalf;
	m_aVertex[2].fY = fySizeHalf;
	m_aVertex[2].fZ = fzSizeHalf;
	m_aVertex[2].fxNormal = 0.0f;
	m_aVertex[2].fyNormal = 0.0f;
	m_aVertex[2].fzNormal = 1.0f;

	m_aVertex[3].fX = -fxSizeHalf;
	m_aVertex[3].fY = fySizeHalf;
	m_aVertex[3].fZ = fzSizeHalf;
	m_aVertex[3].fxNormal = 0.0f;
	m_aVertex[3].fyNormal = 0.0f;
	m_aVertex[3].fzNormal = 1.0f;

	m_aiIndex[0] = 0;
	m_aiIndex[1] = 1;
	m_aiIndex[2] = 2;

	m_aiIndex[3] = 0;
	m_aiIndex[4] = 2;
	m_aiIndex[5] = 3;

	// Back:
	m_aVertex[4].fX = -fxSizeHalf;
	m_aVertex[4].fY = fySizeHalf;
	m_aVertex[4].fZ = -fzSizeHalf;
	m_aVertex[4].fxNormal = 0.0f;
	m_aVertex[4].fyNormal = 0.0f;
	m_aVertex[4].fzNormal = -1.0f;

	m_aVertex[5].fX = fxSizeHalf;
	m_aVertex[5].fY = fySizeHalf;
	m_aVertex[5].fZ = -fzSizeHalf;
	m_aVertex[5].fxNormal = 0.0f;
	m_aVertex[5].fyNormal = 0.0f;
	m_aVertex[5].fzNormal = -1.0f;

	m_aVertex[6].fX = fxSizeHalf;
	m_aVertex[6].fY = -fySizeHalf;
	m_aVertex[6].fZ = -fzSizeHalf;
	m_aVertex[6].fxNormal = 0.0f;
	m_aVertex[6].fyNormal = 0.0f;
	m_aVertex[6].fzNormal = -1.0f;

	m_aVertex[7].fX = -fxSizeHalf;
	m_aVertex[7].fY = -fySizeHalf;
	m_aVertex[7].fZ = -fzSizeHalf;
	m_aVertex[7].fxNormal = 0.0f;
	m_aVertex[7].fyNormal = 0.0f;
	m_aVertex[7].fzNormal = -1.0f;

	m_aiIndex[6] = 7;
	m_aiIndex[7] = 5;
	m_aiIndex[8] = 6;

	m_aiIndex[9] = 7;
	m_aiIndex[10] = 4;
	m_aiIndex[11] = 5;

	// Right:
	m_aVertex[8].fX = fxSizeHalf;
	m_aVertex[8].fY = -fySizeHalf;
	m_aVertex[8].fZ = fzSizeHalf;
	m_aVertex[8].fxNormal = 1.0f;
	m_aVertex[8].fyNormal = 0.0f;
	m_aVertex[8].fzNormal = 0.0f;

	m_aVertex[9].fX = fxSizeHalf;
	m_aVertex[9].fY = -fySizeHalf;
	m_aVertex[9].fZ = -fzSizeHalf;
	m_aVertex[9].fxNormal = 1.0f;
	m_aVertex[9].fyNormal = 0.0f;
	m_aVertex[9].fzNormal = 0.0f;

	m_aVertex[10].fX = fxSizeHalf;
	m_aVertex[10].fY = fySizeHalf;
	m_aVertex[10].fZ = -fzSizeHalf;
	m_aVertex[10].fxNormal = 1.0f;
	m_aVertex[10].fyNormal = 0.0f;
	m_aVertex[10].fzNormal = 0.0f;

	m_aVertex[11].fX = fxSizeHalf;
	m_aVertex[11].fY = fySizeHalf;
	m_aVertex[11].fZ = fzSizeHalf;
	m_aVertex[11].fxNormal = 1.0f;
	m_aVertex[11].fyNormal = 0.0f;
	m_aVertex[11].fzNormal = 0.0f;

	m_aiIndex[12] = 8;
	m_aiIndex[13] = 9;
	m_aiIndex[14] = 10;

	m_aiIndex[15] = 8;
	m_aiIndex[16] = 10;
	m_aiIndex[17] = 11;

	// Left:
	m_aVertex[12].fX = -fxSizeHalf;
	m_aVertex[12].fY = fySizeHalf;
	m_aVertex[12].fZ = fzSizeHalf;
	m_aVertex[12].fxNormal = -1.0f;
	m_aVertex[12].fyNormal = 0.0f;
	m_aVertex[12].fzNormal = 0.0f;

	m_aVertex[13].fX = -fxSizeHalf;
	m_aVertex[13].fY = fySizeHalf;
	m_aVertex[13].fZ = -fzSizeHalf;
	m_aVertex[13].fxNormal = -1.0f;
	m_aVertex[13].fyNormal = 0.0f;
	m_aVertex[13].fzNormal = 0.0f;

	m_aVertex[14].fX = -fxSizeHalf;
	m_aVertex[14].fY = -fySizeHalf;
	m_aVertex[14].fZ = -fzSizeHalf;
	m_aVertex[14].fxNormal = -1.0f;
	m_aVertex[14].fyNormal = 0.0f;
	m_aVertex[14].fzNormal = 0.0f;

	m_aVertex[15].fX = -fxSizeHalf;
	m_aVertex[15].fY = -fySizeHalf;
	m_aVertex[15].fZ = fzSizeHalf;
	m_aVertex[15].fxNormal = -1.0f;
	m_aVertex[15].fyNormal = 0.0f;
	m_aVertex[15].fzNormal = 0.0f;

	m_aiIndex[18] = 15;
	m_aiIndex[19] = 12;
	m_aiIndex[20] = 13;

	m_aiIndex[21] = 15;
	m_aiIndex[22] = 13;
	m_aiIndex[23] = 14;

	// Top:
	m_aVertex[16].fX = -fxSizeHalf;
	m_aVertex[16].fY = fySizeHalf;
	m_aVertex[16].fZ = fzSizeHalf;
	m_aVertex[16].fxNormal = 0.0f;
	m_aVertex[16].fyNormal = 1.0f;
	m_aVertex[16].fzNormal = 0.0f;

	m_aVertex[17].fX = fxSizeHalf;
	m_aVertex[17].fY = fySizeHalf;
	m_aVertex[17].fZ = fzSizeHalf;
	m_aVertex[17].fxNormal = 0.0f;
	m_aVertex[17].fyNormal = 1.0f;
	m_aVertex[17].fzNormal = 0.0f;

	m_aVertex[18].fX = fxSizeHalf;
	m_aVertex[18].fY = fySizeHalf;
	m_aVertex[18].fZ = -fzSizeHalf;
	m_aVertex[18].fxNormal = 0.0f;
	m_aVertex[18].fyNormal = 1.0f;
	m_aVertex[18].fzNormal = 0.0f;

	m_aVertex[19].fX = -fxSizeHalf;
	m_aVertex[19].fY = fySizeHalf;
	m_aVertex[19].fZ = -fzSizeHalf;
	m_aVertex[19].fxNormal = 0.0f;
	m_aVertex[19].fyNormal = 1.0f;
	m_aVertex[19].fzNormal = 0.0f;

	m_aiIndex[24] = 16;
	m_aiIndex[25] = 17;
	m_aiIndex[26] = 18;

	m_aiIndex[27] = 16;
	m_aiIndex[28] = 18;
	m_aiIndex[29] = 19;

	// Bottom:
	m_aVertex[20].fX = -fxSizeHalf;
	m_aVertex[20].fY = -fySizeHalf;
	m_aVertex[20].fZ = -fzSizeHalf;
	m_aVertex[20].fxNormal = 0.0f;
	m_aVertex[20].fyNormal = -1.0f;
	m_aVertex[20].fzNormal = 0.0f;

	m_aVertex[21].fX = fxSizeHalf;
	m_aVertex[21].fY = -fySizeHalf;
	m_aVertex[21].fZ = -fzSizeHalf;
	m_aVertex[21].fxNormal = 0.0f;
	m_aVertex[21].fyNormal = -1.0f;
	m_aVertex[21].fzNormal = 0.0f;

	m_aVertex[22].fX = fxSizeHalf;
	m_aVertex[22].fY = -fySizeHalf;
	m_aVertex[22].fZ = fzSizeHalf;
	m_aVertex[22].fxNormal = 0.0f;
	m_aVertex[22].fyNormal = -1.0f;
	m_aVertex[22].fzNormal = 0.0f;

	m_aVertex[23].fX = -fxSizeHalf;
	m_aVertex[23].fY = -fySizeHalf;
	m_aVertex[23].fZ = fzSizeHalf;
	m_aVertex[23].fxNormal = 0.0f;
	m_aVertex[23].fyNormal = -1.0f;
	m_aVertex[23].fzNormal = 0.0f;

	m_aiIndex[30] = 23;
	m_aiIndex[31] = 21;
	m_aiIndex[32] = 22;

	m_aiIndex[33] = 23;
	m_aiIndex[34] = 20;
	m_aiIndex[35] = 21;
}
