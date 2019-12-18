#include "pch.h"
#include "BufferVertex.h"


CBufferVertex::CBufferVertex()
{
}


CBufferVertex::~CBufferVertex()
{
}

void CBufferVertex::Init(ID3D12Device* pDevice, void* apvData, unsigned int uBytesData, unsigned int uStrideData)
{
	m_pDevice = pDevice;
	m_ResourceType = EResourceType::Vertex;

	HRESULT hresult;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	// 3D array, originally meant for 3D/ 2D Textures, misused here as 1D data array
	resourceDesc.Width = uBytesData;	// x dim
	resourceDesc.Height = 1;			// y dim
	resourceDesc.DepthOrArraySize = 1;	// z dim

	resourceDesc.MipLevels	= 1;
	resourceDesc.Format		= DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc	= sampleDesc;
	resourceDesc.Layout		= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Alignment	= D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Flags		= D3D12_RESOURCE_FLAG_NONE;

	//////////////////////////////////////////////////////////////////////////
	// Creation of the default buffer
	D3D12_HEAP_PROPERTIES heapPropertiesDefaultBuffer;
	heapPropertiesDefaultBuffer.CPUPageProperty			= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDefaultBuffer.CreationNodeMask		= 1;
	heapPropertiesDefaultBuffer.VisibleNodeMask			= 1;
	heapPropertiesDefaultBuffer.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDefaultBuffer.Type					= D3D12_HEAP_TYPE_DEFAULT; // Default heap for default buffer

	hresult = m_pDevice->CreateCommittedResource
	(
		&heapPropertiesDefaultBuffer,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_pResourceDefaultBuffer)
	);
	LOG_CHECK_MSG("CreateCommittedResource: Vertex Default Buffer", hresult);

	//////////////////////////////////////////////////////////////////////////
	// Create the upload buffer
	D3D12_HEAP_PROPERTIES heapPropertiesUploadBuffer;
	heapPropertiesUploadBuffer.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesUploadBuffer.CreationNodeMask		= 1;
	heapPropertiesUploadBuffer.VisibleNodeMask		= 1;
	heapPropertiesUploadBuffer.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesUploadBuffer.Type					= D3D12_HEAP_TYPE_UPLOAD; // Upload heap for upload buffer

	hresult = m_pDevice->CreateCommittedResource
	(
		&heapPropertiesUploadBuffer,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_pResourceUploadBuffer)
	);
	LOG_CHECK_MSG("CreateCommittedResource: Vertex Upload Buffer", hresult);

	//////////////////////////////////////////////////////////////////////////
	// Thread synchronization
	D3D12_RESOURCE_BARRIER resourceBarrierCopyDataStart = {};
	resourceBarrierCopyDataStart.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrierCopyDataStart.Transition.StateBefore	= D3D12_RESOURCE_STATE_COMMON;
	resourceBarrierCopyDataStart.Transition.StateAfter	= D3D12_RESOURCE_STATE_COPY_DEST;
	resourceBarrierCopyDataStart.Transition.pResource	= m_pResourceDefaultBuffer;
	resourceBarrierCopyDataStart.Transition.Subresource	= 0;

	m_commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hresult = m_pDevice->CreateCommandQueue
	(
		&m_commandQueueDesc,
		IID_PPV_ARGS(&m_pCommandQueue)
	);
	LOG_CHECK_MSG("CreateCommandQueue: Vertex Buffer", hresult);

	hresult = m_pDevice->CreateCommandAllocator
	(
		m_commandQueueDesc.Type,
		IID_PPV_ARGS(&m_pCommandAllocatorCopy)
	);
	LOG_CHECK_MSG("CreateCommandAllocator: Vertex Buffer", hresult);

	hresult = m_pCommandAllocatorCopy->Reset();
	LOG_CHECK_MSG("Reset the Vertex Buffer", hresult);

	hresult = m_pDevice->CreateCommandList
	(
		0,
		m_commandQueueDesc.Type,
		m_pCommandAllocatorCopy,
		0,
		IID_PPV_ARGS(&m_pCommandListCopy)
	);
	LOG_CHECK_MSG("CreateCommandList", hresult);

	// Start copying
	m_pCommandListCopy->ResourceBarrier(1, &resourceBarrierCopyDataStart); // Number of barriers is 1

	D3D12_SUBRESOURCE_DATA subresourceData;
	subresourceData.pData = apvData;
	subresourceData.RowPitch = uBytesData;
	subresourceData.SlicePitch = subresourceData.RowPitch;

	UpdateSubresources
	(
		m_pCommandListCopy,			// Special command list for copying
		m_pResourceDefaultBuffer,	// Target buffer on the GPU
		m_pResourceUploadBuffer,	// Temporary buffer for uploading with visibility for the CPU
		0,							// Internal offset for copying
		0,							// Subresource to copy first
		1,							// Amount of subresources
		&subresourceData			// Struct with data to copy
	);

	D3D12_RESOURCE_BARRIER resourceBarrierCopyDataFinished	= {};
	resourceBarrierCopyDataFinished.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrierCopyDataFinished.Transition.StateBefore	= D3D12_RESOURCE_STATE_COPY_DEST;
	resourceBarrierCopyDataFinished.Transition.StateAfter	= D3D12_RESOURCE_STATE_GENERIC_READ;
	resourceBarrierCopyDataFinished.Transition.pResource	= m_pResourceDefaultBuffer;
	resourceBarrierCopyDataFinished.Transition.Subresource	= 0;

	m_pCommandListCopy->ResourceBarrier(1, &resourceBarrierCopyDataFinished);
	hresult = m_pCommandListCopy->Close();
	LOG_CHECK_MSG("Closing the command list of the Vertex Buffer", hresult);

	ID3D12CommandList* pCommandListsCopy[] = { m_pCommandListCopy };
	m_pCommandQueue->ExecuteCommandLists(1, pCommandListsCopy); // Command lists array to process

	m_pCommandListCopy->Release(); // Release memory
	// UINT64 address, which is different for the GPU than the CPU
	m_vertexBufferView.BufferLocation	= m_pResourceDefaultBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes		= uBytesData;
	m_vertexBufferView.StrideInBytes	= uStrideData;
}

void CBufferVertex::Draw(ID3D12GraphicsCommandList* pCommandList)
{
	// Tell the input assembler about the vertex buffer view,
	// where to find the vertex data and how they are structured
	pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
}

void CBufferVertex::Fini()
{

}
