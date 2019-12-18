#pragma once
#include "Buffer.h"
/**
 * A constant buffer contains no constants, but variables which can change every tick.
 * It is only constant for the layout and during a shader call.
 */
class CBufferConstant : public CBuffer
{
public:
	CBufferConstant();
	~CBufferConstant();
	
	/**
	 * Init function.
	 * @param pDevice Device pointer to work with DX12.
	 * @param pRootParameter Array of the root parameters (Written in PSO).
	 * @param shaderVisibility Defines for which shader the buffer is visible.
	 * @param uRootParameter Index of the root parameter in the array.
	 * @param apvData Actual data.
	 * @param ubytesData Size of the data in bytes.
	 * @param uStrideData Stride in bytes (>= Size of a data package).
	 * @param uRegister Number of the register to transfer data (0-15).
	 */
	void Init (
		ID3D12Device* pDevice,
		D3D12_ROOT_PARAMETER* aRootParameters,
		D3D12_SHADER_VISIBILITY shaderVisibility,
		unsigned int uRootParameter,
		void* apvData,
		unsigned int uBytesData,
		unsigned int uStrideData,
		unsigned int uRegister
	);

	/**
	 * Tick function.
	 * @param pCommandList Command list to write command into.
	 * @param apvData Data which changes every tick.
	 */
	void Tick
	(
		ID3D12GraphicsCommandList* pCommandList,
		void* apvData
	);

	void Fini();

	unsigned int m_uBytesData = 0;
	unsigned int m_uRootParameter = 0;
	unsigned int m_uRegister = 0;

	void* m_pvMappedData = nullptr;
	ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddress;

private:
	/** No default buffer, only an upload buffer, because data is changed often. */
	ID3D12Resource* m_pResourceUploadBuffer = nullptr;

	/** Description of the constant buffer. */
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_constantBufferViewDesc;
	D3D12_DESCRIPTOR_HEAP_DESC m_DescriptorHeapDesc;
	D3D12_DESCRIPTOR_RANGE m_descriptorRange;

	/** Temporary command list for copying, don't need it anymore afterwards */
	D3D12_COMMAND_QUEUE_DESC m_commandQueueDesc = {};
	ID3D12CommandAllocator* m_pCommandAllocatorCopy = nullptr;
	ID3D12CommandQueue* m_pCommandQueue = nullptr;
	ID3D12GraphicsCommandList* m_pCommandListCopy = nullptr;
};

