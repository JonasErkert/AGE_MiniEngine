#pragma once

#include <d3d12.h>		// DirectX12 base functions
#include <dxgi1_2.h>	// Math functions of DX12 => dxgi.lib
#include <dxgi1_4.h>	// IDXGISwapChain3 =>dxgi.lib
#include <d3dcompiler.h> // Functions to compile HLSL shaders => d3dCompiler.lib

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dCompiler.lib")

class CDirectX12Base
{
public:
	CDirectX12Base();
	~CDirectX12Base();

	void Init();
	void Tick();
	void Fini();

private:
	ID3D12Device* m_pDevice = nullptr; // Pointer to the entire dx system
	IDXGIFactory2* m_pFactory = nullptr;
	IDXGISwapChain3* m_pSwapChain = nullptr;
};

