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

	CheckAdapter();

	hresult = D3D12CreateDevice(m_pAdapterBest, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_pDevice));
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
	IDXGIAdapter1* pAdapter = nullptr;

	unsigned int uAdapter = 0;
	unsigned int uAdapterBest = 0;
	size_t sizeBestDedicatedVideoMemory = 0;

	while (m_pFactory->EnumAdapters1(uAdapter++, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);
		char ac[256];
		wcstombs(ac, adapterDesc.Description, 256);

		LogDebug("GPU Adapter %u: %s", uAdapter, ac);
		LogDebug("- Dedicated Video Memory: %u", adapterDesc.DedicatedVideoMemory);
		LogDebug("- Dedicated System Memory: %u", adapterDesc.DedicatedSystemMemory);
		LogDebug("- Shared System Memory: %u", adapterDesc.SharedSystemMemory);
		LogDebug("- Vendor ID: %u", adapterDesc.VendorId);
		LogDebug("- Device ID: %u", adapterDesc.DeviceId);
		LogDebug("- Subsystem ID: %u", adapterDesc.SubSysId);
		LogDebug("- Adapter LUID: %u", adapterDesc.AdapterLuid);
		LogDebug("- Revision: %u", adapterDesc.Revision);

		if (adapterDesc.DedicatedVideoMemory > sizeBestDedicatedVideoMemory)
		{
			m_pAdapterBest = pAdapter;
			sizeBestDedicatedVideoMemory = adapterDesc.DedicatedVideoMemory;
			uAdapterBest = uAdapter;
		}

		unsigned int uOutput = 0;
		IDXGIOutput* pOutput = nullptr;

		while (pAdapter->EnumOutputs(uOutput++, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC outputDesc;
			pOutput->GetDesc(&outputDesc);
			wcstombs(ac, outputDesc.DeviceName, 256);

			LogDebug("- - Output: %u: %s", uOutput, ac);

			unsigned int uDisplayModes = 0;
			unsigned int uFlags = 0;
			DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
			pOutput->GetDisplayModeList(format, uFlags, &uDisplayModes, nullptr);
			DXGI_MODE_DESC* pModeDesc = (DXGI_MODE_DESC *)malloc(sizeof(DXGI_MODE_DESC)*uDisplayModes);
			pOutput->GetDisplayModeList(format, uFlags, &uDisplayModes, pModeDesc);
			for (unsigned int u = 0; u < uDisplayModes; ++u)
			{
				unsigned int uRefreshRateNum = pModeDesc[u].RefreshRate.Numerator;
				unsigned int uRefreshRateDen = pModeDesc[u].RefreshRate.Denominator;
				unsigned int uX = pModeDesc[u].Width;
				unsigned int uY = pModeDesc[u].Height;
				float fRefreshRate;

				if (uRefreshRateDen)
				{
					fRefreshRate = (float)uRefreshRateNum / (float)uRefreshRateDen;
				}
				else
				{
					fRefreshRate = 0.f;
				}

				LogDebug("- - - Resolution: %u (%u * %u), Refresh Rate %u / %u = %5.2f Hz",
					u, uX, uY, uRefreshRateNum, uRefreshRateDen, fRefreshRate);
			}
			
			free(pModeDesc);
		}
	}
}
