#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <d3d12.h>		// DirectX12 base functions
#include <dxgi1_2.h>	// Math functions of DX12 => dxgi.lib
#include <dxgi1_4.h>	// IDXGISwapChain3 =>dxgi.lib
#include <d3dcompiler.h> // Functions to compile HLSL shaders => d3dCompiler.lib
#include "Vertex.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dCompiler.lib")

#define FRAMES 3 // 2 back buffer and 1 front buffer

class CDirectX12Base
{
public:
	CDirectX12Base();
	~CDirectX12Base();

	void Init(HWND hwnd);
	void Tick();
	void Fini();

	/** Checks the hardware and selects the best adapter. */
	void CheckAdapter();

	CVertex m_avVertex[24]; // 8 * 3
	unsigned int m_auIndexBuffer[36];
	void CreateCube();

private:
	ID3D12Device* m_pDevice = nullptr; // Pointer to the entire dx system
	IDXGIFactory2* m_pFactory = nullptr; // A factory can produce other DX objects
	IDXGISwapChain3* m_pSwapChain = nullptr; // A swapchain consists of n backbuffers and 1 frontbuffer
	IDXGIAdapter1* m_pAdapterBest = nullptr; // Hardware, the graphics card
	ID3D12CommandQueue* m_pCommandListQueue = nullptr; // Command list for the instructions of the graphics card
	D3D12_VIEWPORT m_viewport = { 0 }; // Rendering area
	D3D12_RECT m_rectScissor = { 0 }; // The scissor can be used to mask parts of the viewport
	ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr; // Descriptor heap for intern buffers
	ID3D12Resource* m_paResourceRtv[FRAMES]; // Array of resources for the front- and back buffers
	D3D12_CPU_DESCRIPTOR_HANDLE m_aCpuDescriptorHandle[FRAMES]; // Handle for the CPU
	ID3D12CommandAllocator* m_paCommandAllocator[FRAMES]; // Allocation helper for the command queue
	ID3D12RootSignature* m_pRootSignature = nullptr; // Identifies the render pass
	ID3D12PipelineState* m_pPipelineState = nullptr; // Status of the render pipeline
};
