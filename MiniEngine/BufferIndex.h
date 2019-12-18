#pragma once
#include "Buffer.h"

class CBufferIndex : public CBuffer
{
public:
	CBufferIndex();
	~CBufferIndex();

	void Init(ID3D12Device* pDevice, void* apvData, unsigned int uBytesData);
	void Draw(ID3D12GraphicsCommandList* pCommandList); // Draws with the help of the indices
	void Fini();

	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

private:
	/** Temporary memory with access rights for the CPU. */
	ID3D12Resource* m_pResourceDefaultBuffer = nullptr;
	/** Main memory from which the GPU reads. */
	ID3D12Resource* m_pResourceUploadBuffer = nullptr;

	/** Temporary command list for copying, don't need it anymore afterwards */
	D3D12_COMMAND_QUEUE_DESC m_commandQueueDesc = {};
	ID3D12CommandAllocator* m_pCommandAllocatorCopy = nullptr;
	ID3D12CommandQueue* m_pCommandQueue = nullptr;
	ID3D12GraphicsCommandList* m_pCommandListCopy = nullptr;
};
