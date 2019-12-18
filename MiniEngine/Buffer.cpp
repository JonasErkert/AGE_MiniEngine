#include "pch.h"
#include "Buffer.h"


CBuffer::CBuffer()
{
}


CBuffer::~CBuffer()
{
}

void CBuffer::MemcpySubresource(const D3D12_MEMCPY_DEST* pMemcpyDest, const D3D12_SUBRESOURCE_DATA* pSubresourceData,
	SIZE_T sizetRowSizeInBytes, UINT uRows, UINT uSlices)
{
	for (UINT u = 0; u < uSlices; u++)
	{
		BYTE* pDestSlice = reinterpret_cast<BYTE*>(pMemcpyDest->pData)
			+ pMemcpyDest->SlicePitch * u;
		const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pSubresourceData->pData)
			+ pSubresourceData->SlicePitch * u;

		for (UINT uRow = 0; uRow < uRows; uRow++)
		{
			memcpy(pDestSlice + pMemcpyDest->RowPitch * uRow,
				pSrcSlice + pSubresourceData->RowPitch * uRow,
				sizetRowSizeInBytes);
		}
	}
}

UINT64 CBuffer::UpdateSubresources
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
)
{
	D3D12_RESOURCE_DESC resourceDescUpload = pResourceUpload->GetDesc();
	D3D12_RESOURCE_DESC resourceDescDestination = pResourceDestination->GetDesc();

	if (resourceDescUpload.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
		resourceDescUpload.Width < uRequiredSize + pLayouts[0].Offset ||
		uRequiredSize > (SIZE_T)-1 ||
		(resourceDescDestination.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
			(uSubresourceFirst != 0 || uSubresources != 1)))
	{
		return 0;
	}

	BYTE* pData;
	HRESULT hresult = pResourceUpload->Map(0, NULL, reinterpret_cast<void**>(&pData));

	if (FAILED(hresult))
	{
		return 0;
	}

	for (UINT u = 0; u < uSubresources; ++u)
	{
		if (pullRowSizeInBytes[u] > (SIZE_T)-1)
		{
			return 0;
		}

		D3D12_MEMCPY_DEST DestData =
		{
			pData + pLayouts[u].Offset,
			pLayouts[u].Footprint.RowPitch,
			SIZE_T(pLayouts[u].Footprint.RowPitch)* SIZE_T(puRows[u])
		};

		MemcpySubresource(&DestData, &pSubresourceData[u],
			(SIZE_T)pullRowSizeInBytes[u],
			puRows[u], pLayouts[u].Footprint.Depth);
	}

	pResourceUpload->Unmap(0, NULL);

	if (resourceDescDestination.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
	{
		pGraphicsCommandList->CopyBufferRegion(
			pResourceDestination,
			0,
			pResourceUpload,
			pLayouts[0].Offset,
			pLayouts[0].Footprint.Width);
	}
	else // if it is not an actual buffer
	{
		for (UINT u = 0; u < uSubresources; ++u)
		{
			D3D12_TEXTURE_COPY_LOCATION Dst;
			Dst.pResource = pResourceDestination;
			Dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			Dst.SubresourceIndex = u + uSubresourceFirst;

			D3D12_TEXTURE_COPY_LOCATION Src;
			Src.pResource = pResourceUpload;
			Src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			Src.PlacedFootprint = pLayouts[u];
		
			pGraphicsCommandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
		}
	}

	return uRequiredSize;
}

UINT64 CBuffer::UpdateSubresources
(
	ID3D12GraphicsCommandList* pGraphicsCommandList,
	ID3D12Resource* pResourceDestination,
	ID3D12Resource* pResourceUpload,
	UINT64 uIntermediateOffset,
	UINT uSubresourceFirst,
	UINT uSubresources,
	const D3D12_SUBRESOURCE_DATA* pSubresourceData
)
{
	UINT64 ullRequiredSize = 0;
	UINT64 ullMemToAlloc = static_cast<UINT64>(
		sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) +
		sizeof(UINT) + sizeof(UINT64)) * uSubresources;

	if (ullMemToAlloc > SIZE_MAX) // Is memory size larger than available
	{
		return 0;
	}

	void* pvMemory = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(ullMemToAlloc));

	if (pvMemory == NULL)
	{
		return 0;
	}

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts =
		reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pvMemory);
	UINT64* pullRowSizeInBytes = reinterpret_cast<UINT64*>(pLayouts + uSubresources);
	UINT* puRows = reinterpret_cast<UINT*>(pullRowSizeInBytes + uSubresources);

	D3D12_RESOURCE_DESC resourceDesc = pResourceDestination->GetDesc();
	ID3D12Device* pDevice;
	pResourceDestination->GetDevice(__uuidof(pDevice), reinterpret_cast<void**>(&pDevice));
	pDevice->GetCopyableFootprints(&resourceDesc, uSubresourceFirst, uSubresources,
		uIntermediateOffset, pLayouts, puRows, pullRowSizeInBytes, &ullRequiredSize);
	pDevice->Release();
	UINT64 ullResult = UpdateSubresources(
		pGraphicsCommandList,
		pResourceDestination,
		pResourceUpload,
		uSubresourceFirst,
		uSubresources,
		ullRequiredSize,
		pLayouts,
		puRows,
		pullRowSizeInBytes,
		pSubresourceData);

	HeapFree(GetProcessHeap(), 0, pvMemory);
	HRESULT hresult = ullResult;

	return ullResult;
}
