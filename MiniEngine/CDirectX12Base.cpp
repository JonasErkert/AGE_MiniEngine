#include "pch.h"
#include "CDirectX12Base.h"
#include "LogFile.h"


CDirectX12Base::CDirectX12Base()
{
}


CDirectX12Base::~CDirectX12Base()
{
}

void CDirectX12Base::Init(HWND hwnd)
{
	LogStart("");
	
	HRESULT hresult;

	hresult = CreateDXGIFactory(IID_PPV_ARGS(&m_pFactory));
	LOG_CHECK_MSG("CreateDXGIFactory", hresult);

	CheckAdapter();

	hresult = D3D12CreateDevice(m_pAdapterBest, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice));
	LOG_CHECK_MSG("CreateDXGIFactory", hresult);

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hresult = m_pDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_pCommandListQueue));
	LOG_CHECK_MSG("Create Command Queue", hresult);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.BufferCount = FRAMES;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32 Bit 4 * 8
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// TODO: swapChainDesc.Height = 

	IDXGISwapChain1* pOldSwapChain = nullptr;
	hresult = m_pFactory->CreateSwapChainForHwnd(m_pCommandListQueue, hwnd, &swapChainDesc, 0, 0, &pOldSwapChain);
	LOG_CHECK_MSG("CreateSwapchain", hresult);

	hresult = pOldSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
	LOG_CHECK_MSG("pOldSwapChain->QueryInterface", hresult);

	m_pSwapChain->GetDesc1(&swapChainDesc);
	m_viewport.Width = (FLOAT)swapChainDesc.Width;
	m_viewport.Height = (FLOAT)swapChainDesc.Height;

	// Warning! The area defined as scissor area stays/ is rendered
	m_rectScissor.left = 0;
	m_rectScissor.top = 0;
	m_rectScissor.right = (LONG)swapChainDesc.Width;
	m_rectScissor.bottom = (LONG)swapChainDesc.Height;

	// Generate a descriptor heap for the render target view
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.NumDescriptors = FRAMES;
	// A render target view (RTV) is a BindSlot, which can be pinned to a OutputMerger
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	hresult = m_pDevice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_pDescriptorHeap));
	LOG_CHECK_MSG("CreateDescriptorHeap", hresult);

	// Create Swapchain

	// Size of the incrementSize varies depending on the vendor (AMD, NVdia, Intel) => Query it
	// Get number of bytes for a heap, which is suitable for RTVs
	size_t sizetDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle = m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (unsigned int uFrame = 0; uFrame < FRAMES; uFrame++)
	{
		ID3D12Resource* pResource = nullptr;
		hresult = m_pSwapChain->GetBuffer(uFrame, IID_PPV_ARGS(&pResource));
		m_paResourceRtv[uFrame] = pResource;

		m_pDevice->CreateRenderTargetView(pResource, nullptr, cpuDescriptorHandle);

		m_aCpuDescriptorHandle[uFrame] = cpuDescriptorHandle;
		cpuDescriptorHandle.ptr += sizetDescriptorSize;

		hresult = m_pDevice->CreateCommandAllocator(
			commandQueueDesc.Type, IID_PPV_ARGS(&m_paCommandAllocator[uFrame]));
		LOG_CHECK_MSG("CreateCommandAllocator", hresult);
	}

	//////////////////////////////////////////////////////////////////////////
	// Compile and bind shader
	// TODO
	ID3DBlob* pBlobVertexShader = nullptr;
	ID3DBlob* pBlobPixelShader = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// Configure render pipeline
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.NumParameters = 0;
	rootSignatureDesc.pParameters = 0;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// A blob is a undefined part buffer
	ID3DBlob* pBlobRootSignature = nullptr;
	ID3DBlob* pBlobRootSignatureError = nullptr;

	hresult = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pBlobRootSignatureError, &pBlobRootSignatureError);
	LOG_CHECK_MSG("D3D12SerializeRoorSignature", hresult);

	hresult = m_pDevice->CreateRootSignature(0, pBlobRootSignature->GetBufferPointer(),
		pBlobRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
	LOG_CHECK_MSG("CrateRootSignature", hresult);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = { 0 };
	pipelineStateDesc.VS.pShaderBytecode	= pBlobVertexShader->GetBufferPointer();
	pipelineStateDesc.VS.BytecodeLength		= pBlobVertexShader->GetBufferSize();
	pipelineStateDesc.PS.pShaderBytecode	= pBlobPixelShader->GetBufferPointer();
	pipelineStateDesc.PS.BytecodeLength		= pBlobPixelShader->GetBufferSize();
	pipelineStateDesc.pRootSignature		= m_pRootSignature;
	pipelineStateDesc.BlendState.AlphaToCoverageEnable = FALSE;
	pipelineStateDesc.BlendState.IndependentBlendEnable = FALSE;

	for (int i = 0; i < 8; i++)
	{
		pipelineStateDesc.BlendState.RenderTarget[i].BlendEnable	= FALSE; // No transparency
		pipelineStateDesc.BlendState.RenderTarget[i].LogicOpEnable	= FALSE;
		pipelineStateDesc.BlendState.RenderTarget[i].SrcBlend		= D3D12_BLEND_ONE;
		pipelineStateDesc.BlendState.RenderTarget[i].DestBlend		= D3D12_BLEND_ZERO;
		pipelineStateDesc.BlendState.RenderTarget[i].BlendOp		= D3D12_BLEND_OP_ADD;
		pipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha	= D3D12_BLEND_ONE;
		pipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha	= D3D12_BLEND_ZERO;
		pipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha	= D3D12_BLEND_OP_ADD;
		pipelineStateDesc.BlendState.RenderTarget[i].LogicOp		= D3D12_LOGIC_OP_NOOP;
		pipelineStateDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// Draw triangles solid. Alternative: D3D12_FILL_MODE_WIREFRAME
		pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		// Only for left handed systems: D3D12_CULL_MODE_FRONT
		pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // Later: D3D12_CULL_MODE_BACK
		// TRUE for right handed systems
		pipelineStateDesc.RasterizerState.FrontCounterClockwise = TRUE;
		// Shadow mapping
		pipelineStateDesc.RasterizerState.DepthBias = 0;
		pipelineStateDesc.RasterizerState.DepthBiasClamp = 0.f;
		pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = 0.f;
		// Occlude hidden objects, consider z-Buffer
		pipelineStateDesc.RasterizerState.DepthClipEnable = TRUE;
		pipelineStateDesc.RasterizerState.MultisampleEnable = FALSE;
		// Enable anti aliasing? No wire frame -> FALSE
		pipelineStateDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		pipelineStateDesc.RasterizerState.ForcedSampleCount = 0;
	}
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
