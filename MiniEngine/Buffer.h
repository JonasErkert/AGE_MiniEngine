#pragma once
#include <d3d12.h>			// DirectX12 base functions
#include <dxgi1_2.h>		// Math functions of DX12 => dxgi.lib
#include <dxgi1_4.h>		// IDXGISwapChain3 =>dxgi.lib
#include <d3dcompiler.h>	// Functions to compile HLSL shaders => d3dCompiler.lib

#include "LogFile.h"

enum class EResourceType
{
	Vertex,
	Index,
	Texture2D,
	Constant,
	None
};

class CBuffer
{
public:
	CBuffer();
	~CBuffer();

	EResourceType m_ResourceType = EResourceType::None;

protected:
	ID3D12Device* m_pDevice = nullptr;

	void MemcpySubresource
	(
		const D3D12_MEMCPY_DEST* pMemcpyDest,
		const D3D12_SUBRESOURCE_DATA* pSubresourceData,
		SIZE_T sizetRowSizeInBytes,
		UINT uRows,
		UINT uSlices
	);

	UINT64 UpdateSubresources
	(
		ID3D12GraphicsCommandList* pGraphicsCommandList,
		ID3D12Resource* pResourceDestination,
		ID3D12Resource* pResourceUpload,
		UINT uSubresourceFirst,
		UINT uSubresources,
		UINT64 uRequiredSize,
		const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
		const UINT* puRows,
		const UINT64* pullRowSizeInBytes,
		const D3D12_SUBRESOURCE_DATA* pSubresourceData
	);

	UINT64 UpdateSubresources
	(
		ID3D12GraphicsCommandList* pGraphicsCommandList,
		ID3D12Resource* pResourceDestination,
		ID3D12Resource* pResourceUpload,
		UINT64 uIntermediateOffset,
		UINT uSubresourceFirst,
		UINT uSubresources, 
		const D3D12_SUBRESOURCE_DATA* pSubresourceData
	);
};

