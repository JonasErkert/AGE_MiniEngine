#include "pch.h"
#include "BufferConstant.h"


CBufferConstant::CBufferConstant()
{
}


CBufferConstant::~CBufferConstant()
{
}

void CBufferConstant::Init(
	ID3D12Device* pDevice,
	D3D12_ROOT_PARAMETER* aRootParameters,
	D3D12_SHADER_VISIBILITY shaderVisibility,
	unsigned int uRootParameter,
	void* apvData, 
	unsigned int uBytesData,
	unsigned int uStrideData,
	unsigned int uRegister
)
{
	m_pDevice = pDevice;
	m_ResourceType = EResourceType::Constant;

	HRESULT hresult;

	m_uRootParameter = uRootParameter;
	m_uRegister = uRegister;
	m_uBytesData = uBytesData;

	// Write the constant buffer into the root parameter list
	aRootParameters[m_uRootParameter].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // ConstantBufferView
	aRootParameters[m_uRootParameter].ShaderVisibility = shaderVisibility;
	aRootParameters[m_uRootParameter].Descriptor.ShaderRegister = uRegister;
	aRootParameters[m_uRootParameter].Descriptor.RegisterSpace = 0;

	//////////////////////////////////////////////////////////////////////////
	// Create buffer
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
	LOG_CHECK_MSG("CreateCommittedResource: Constant Upload Buffer", hresult);

	//////////////////////////////////////////////////////////////////////////
	// Create descriptor heap
	m_DescriptorHeapDesc.NumDescriptors = 1;
	// Works with 3 types:
	// ConstantBufferView
	// ShaderResourceView
	// UnorderedAccessView
	m_DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	m_DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_DescriptorHeapDesc.NodeMask = 0;

	hresult = m_pDevice->CreateDescriptorHeap(&m_DescriptorHeapDesc, IID_PPV_ARGS(&m_pDescriptorHeap));
	LOG_CHECK_MSG("CreateDescriptorHeap: Constant Buffer", hresult);

	//////////////////////////////////////////////////////////////////////////
	// Description of the structure of the constant buffer view
	m_constantBufferViewDesc.BufferLocation = m_pResourceUploadBuffer->GetGPUVirtualAddress();
	// Size of a constant buffer must be a multiple of 256 bytes
	m_constantBufferViewDesc.SizeInBytes = (uBytesData + 255) & ~255;

	m_pDevice->CreateConstantBufferView
	(
		&m_constantBufferViewDesc,
		m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
	);

	m_gpuVirtualAddress = m_pResourceUploadBuffer->GetGPUVirtualAddress();

	m_pResourceUploadBuffer->Map
	(
		0,				// Subresource number
		nullptr,		// Range
		&m_pvMappedData	// Pointer to data section of the mapped data buffer
	);
}

void CBufferConstant::Tick(ID3D12GraphicsCommandList* pCommandList, void* apvData)
{
	pCommandList->SetGraphicsRootConstantBufferView(m_uRootParameter, m_gpuVirtualAddress);
	memcpy(m_pvMappedData, apvData, m_uBytesData); // Actual copying
}

void CBufferConstant::Fini()
{
	m_pResourceUploadBuffer->Unmap
	(
		0,		// Subresource
		nullptr	// Range (nullptr = all)
	);
}
