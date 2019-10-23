#include "pch.h"
#include "CDirectX12Base.h"
#include "LogFile.h"


CDirectX12Base::CDirectX12Base()
{
}


CDirectX12Base::~CDirectX12Base()
{
}

void CDirectX12Base::Init()
{
	LogStart("");
	
	HRESULT hresult;

	hresult = CreateDXGIFactory(IID_PPV_ARGS(&m_pFactory));
	LOG_CHECK_MSG("CreateDXGIFactory", hresult);

	int iBestAdapter = CheckAdapter();

	hresult = D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_pDevice));
	LOG_CHECK_MSG("CreateDXGIFactory", hresult);
}

void CDirectX12Base::Tick()
{

}

void CDirectX12Base::Fini()
{

}

void CDirectX12Base::CheckAdapter()
{
	unsigned int uAdapter = 0;
	unsigned int uAdapterBest = 0;
}
