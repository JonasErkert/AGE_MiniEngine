#include "pch.h"
#include "CDirectX12Base.h"
#include "LogFile.h"
#include "Vector4.h"
#include "Mat.h"


CDirectX12Base::CDirectX12Base()
{
}


CDirectX12Base::~CDirectX12Base()
{
}

// #IMPORTANT: DX vs. Vulkan
void CDirectX12Base::Init(HWND hwnd)
{
	LogStart("DirectX MiniEngine Logfile", LogSource::DirectX);
	
	HRESULT hresult;

	hresult = CreateDXGIFactory(IID_PPV_ARGS(&m_pFactory));
	LOG_CHECK_MSG("CreateDXGIFactory", hresult);

	CheckAdapter();

	hresult = D3D12CreateDevice(m_pAdapterBest, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice));
	LOG_CHECK_MSG("CreateDXGIFactory", hresult);

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
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

	//////////////////////////////////////////////////////////////////////////
	// Create Buffer

	unsigned int uRootParameter = 0; // Number of registered Root Parameter
	unsigned int uRegisterConstantBuffer = 0; // Number of used CBs

	m_bufferConstantWorldViewProj.Init(
		m_pDevice,
		m_aRootParamter,
		D3D12_SHADER_VISIBILITY_VERTEX,
		uRootParameter++,
		&m_worldViewProj,
		sizeof(m_worldViewProj),
		256,
		uRegisterConstantBuffer++
	);

	m_bufferConstantColorEffects.Init(
		m_pDevice,
		m_aRootParamter,
		D3D12_SHADER_VISIBILITY_PIXEL,
		uRootParameter++,
		&m_colorChanging,
		sizeof(m_colorChanging),
		256,
		uRegisterConstantBuffer++
	);

	//////////////////////////////////////////////////////////////////////////
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
	// Create shader vertex transfer structure
	// Information of vertices:
	// Position (must have)
	// Normal (necessary for shading)
	// Color (nice to have)
	// Tangent (normal mapping)
	// Bitangent (normal mapping)
	// UV coordinates (texturing)
	// (Weights for bone animation)
	
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{
			"POSITION",						// Semantic name
			0,								// Semantic index
			DXGI_FORMAT_R32G32B32_FLOAT,	// Transfer format
			0,								// Input slot, which transfers info to the vertex shader (0-15)
			0,								// Byte offset
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input for vertices
			0								// Data step size (Advanced offset, not needed necessarily)
		},
		{
			"NORMAL",						// Semantic name
			0,								// Semantic index
			DXGI_FORMAT_R32G32B32_FLOAT,	// Transfer format
			0,								// Input slot, which transfers info to the vertex shader (0-15)
			12,								// Byte offset, 3*4 byte of the position
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input for vertices
			0								// Data step size (Advanced offset, not needed necessarily)
		},
		{
			"TANGENT",						// Semantic name
			0,								// Semantic index
			DXGI_FORMAT_R32G32B32_FLOAT,	// Transfer format
			0,								// Input slot, which transfers info to the vertex shader (0-15)
			24,								// Byte offset, 12 Pos + 12 Normal
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input for vertices
			0								// Data step size (Advanced offset, not needed necessarily)
		},
		{
			"BITANGENT",					// Semantic name
			0,								// Semantic index
			DXGI_FORMAT_R32G32B32_FLOAT,	// Transfer format
			0,								// Input slot, which transfers info to the vertex shader (0-15)
			36,								// Byte offset, 12 Pos + 12 Normal + 12 Tangent
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input for vertices
			0								// Data step size (Advanced offset, not needed necessarily)
		},
		{
			"TEXCOORD",						// Semantic name
			0,								// Semantic index
			DXGI_FORMAT_R32G32_FLOAT,		// Transfer format (R = U, G = V coordinate)
			0,								// Input slot, which transfers info to the vertex shader (0-15)
			48,								// Byte offset, 12 Pos + 12 Normal + 12 Tangent + 12 Bitangent
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input for vertices
			0								// Data step size (Advanced offset, not needed necessarily)
		}
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	// Number of entries (Pos, Normal, Tangent, Bitangent, UV)
	inputLayoutDesc.NumElements = 5;


	//////////////////////////////////////////////////////////////////////////
	// Compile and bind shader
#ifdef _DEBUG
	UINT compilerFlags = D3DCOMPILE_DEBUG;
#else
	UINT compilerFlags = D3DCOMPILE_OPTIMIZATION_LEVEL0;
#endif

	ID3DBlob* pBlobVertexShader = nullptr;
	hresult = D3DCompileFromFile(L"shaders//vertexshader.hlsl", 0, 0,
		"main", "vs_5_0", compilerFlags, 0, &pBlobVertexShader, 0);
	LOG_CHECK_MSG("D3DCompileFromFile(VS)", hresult);

	ID3DBlob* pBlobPixelShader = nullptr;
	hresult = D3DCompileFromFile(L"shaders//pixelshader.hlsl", 0, 0,
		"main", "ps_5_0", compilerFlags, 0, &pBlobPixelShader, 0);
	LOG_CHECK_MSG("D3DCompileFromFile(PS)", hresult);

	//////////////////////////////////////////////////////////////////////////
	// Configure render pipeline
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ZeroMemory(&rootSignatureDesc, sizeof(rootSignatureDesc));
	rootSignatureDesc.NumParameters = uRootParameter;
	rootSignatureDesc.pParameters = &m_aRootParamter[0];
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = 0;
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// A blob is an undefined part buffer
	ID3DBlob* pBlobRootSignature = nullptr;
	ID3DBlob* pBlobRootSignatureError = nullptr;

	hresult = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pBlobRootSignature, &pBlobRootSignatureError);
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
	}

	// Draw triangles solid. Alternative: D3D12_FILL_MODE_WIREFRAME
	pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// Only for left handed systems: D3D12_CULL_MODE_FRONT
	pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // Later: D3D12_CULL_MODE_BACK
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
	pipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = swapChainDesc.Format;

	pipelineStateDesc.InputLayout = inputLayoutDesc;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0; // Selects best quality

	hresult = m_pDevice->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_pPipelineState));
	LOG_CHECK_MSG("CreateGraphicsPipelineState", hresult);

	// Create vertex data of the cube and pass them to the vertex buffer
	m_fRatio = (float)swapChainDesc.Width / (float)swapChainDesc.Height;

	m_pGeoCube = new CGeoCube();
	m_pGeoCube->Init(1, 2, 3, CColor(1, 1, 0));
	m_bufferVertex.Init(
		m_pDevice,
		m_pGeoCube->m_aVertex,
		m_pGeoCube->m_iVertices * sizeof(SVertex),
		sizeof(SVertex));
	m_bufferIndex.Init(
		m_pDevice,
		m_pGeoCube->m_aiIndex,
		m_pGeoCube->m_iIndices * sizeof(int));
}

void CDirectX12Base::Tick()
{
	UINT uFrame = m_pSwapChain->GetCurrentBackBufferIndex();
	m_paCommandAllocator[uFrame]->Reset();
	ID3D12GraphicsCommandList* pCommandList;

	HRESULT hresult;
	hresult = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_paCommandAllocator[uFrame], 0, IID_PPV_ARGS(&pCommandList));
	LOG_CHECK_MSG("CreateCommandList", hresult);

	// Thread synchronization in dx12:
	// 1. Resource barriers (Limit access to resources between gpu threads)
	// 2. Fences

	// Resource semaphores for render threads, especially meant for resources
	D3D12_RESOURCE_BARRIER resourceBarrierOne = {};
	resourceBarrierOne.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrierOne.Transition.pResource = m_paResourceRtv[uFrame];
	resourceBarrierOne.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrierOne.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrierOne.Transition.Subresource = 0;

	D3D12_RESOURCE_BARRIER resourceBarrierTwo = {};
	resourceBarrierTwo.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrierTwo.Transition.pResource = m_paResourceRtv[uFrame];
	resourceBarrierTwo.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrierTwo.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrierOne.Transition.Subresource = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescrHandle = m_aCpuDescriptorHandle[uFrame];

	// Place the command to color the background into the command list
	float afClearColor[4] = { 1.0f, 0.f, 0.f, 1.f }; // Red
	pCommandList->ClearRenderTargetView(cpuDescrHandle, afClearColor, 0, 0);

	// Viewport and scissor size
	pCommandList->RSSetViewports(1, &m_viewport);
	pCommandList->RSSetScissorRects(1, &m_rectScissor);

	// Render targets are now writable
	pCommandList->ResourceBarrier(1, &resourceBarrierOne);
	
	// Enter root signature
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);
	pCommandList->SetPipelineState(m_pPipelineState);
	// Type of the topology (Trianglelist, Trianglestrip, Triangletable, Bezierpatches)
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Set the render targets
	// Param 1: Number of instructions
	// Param 2: Handle for the cpu to access the render target
	// Param 3: Descriptor range
	// Param 4: Depth stencil descriptor
	pCommandList->OMSetRenderTargets(1, &cpuDescrHandle, 0, 0);

	CMat mCam(10.f, MatConstruction_TranslateZ);
	CMat mView = mCam;

	// The View Component in model view projection system is the inverse of the camera matrix
	mView.Inverse();
	CMat mProj;

	mProj.ProjFov(3.14159f / 5.5, m_fRatio, 0.1f, 1000.f);

	CMat mModel = CMat(0.f, MatConstruction_TranslateZ);
	
	// Increments frames
	static int S_TICK = 0;
	S_TICK++;

	// Rotation angle in radians
	float fRotX = (float)S_TICK / 50.f;
	float fRotY = (float)S_TICK / 231.3378f;
	float fRotZ = (float)S_TICK / 113.27678f;

	// Rotate cube
	mModel.RotateX(fRotX);
	CMat mRotY = CMat(fRotY, MatConstruction_RotateY);
	CMat mRotZ = CMat(fRotZ, MatConstruction_RotateZ);

	CMat_2_XMFLOAT4X4(mModel,	m_worldViewProj.mWorld);
	CMat_2_XMFLOAT4X4(mView,	m_worldViewProj.mView);
	CMat_2_XMFLOAT4X4(mProj,	m_worldViewProj.mProj);

	XMMATRIX a = XMLoadFloat4x4(&m_worldViewProj.mProj);
	a = XMMatrixTranspose(a);
	XMStoreFloat4x4(&m_worldViewProj.mProj, a);

	m_worldViewProj.f4LightDirection.x =  0.817f;
	m_worldViewProj.f4LightDirection.y = -0.517f;
	m_worldViewProj.f4LightDirection.z = -0.817f;
	m_worldViewProj.f4LightDirection.w =  0.f;

	CMat mYellow(1.f, 1.f, 0.f, MatConstructionFloat3_TranslateVersatile);
	CMat_2_XMFLOAT4X4(mYellow, m_colorChanging.mColorChange);

	m_bufferConstantColorEffects.Tick(pCommandList, (void*)&m_colorChanging);
	m_bufferConstantWorldViewProj.Tick(pCommandList, (void*)&m_worldViewProj);
	m_bufferVertex.Draw(pCommandList);
	m_bufferIndex.Draw(pCommandList);

	pCommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

	// Draw a triangle
	// Param 1: Number of vertices
	// Param 2: Instance counter (how many instances of it to draw)
	// Param 3: Seldom used, only for pros
	// pCommandList->DrawInstanced(9, 1, 0, 0);

	// Free the resource of the render target for other threads
	pCommandList->ResourceBarrier(1, &resourceBarrierOne);
	pCommandList->Close();

	//////////////////////////////////////////////////////////////////////////
	// List is now filled and closed -> Let it do it's job
	// There are probably multiple lists, therefore enter the list into an array
	ID3D12CommandList* aCommandList[] = { pCommandList };

	// Execute command list
	m_pCommandListQueue->ExecuteCommandLists(1, aCommandList);

	//////////////////////////////////////////////////////////////////////////
	// Swap back buffer and front buffer according to the rotation principle
	m_pSwapChain->Present(1, 0);

	// Free command list again or garbage will be collected
	pCommandList->Release();
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

