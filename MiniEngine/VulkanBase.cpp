#include "pch.h"

#include "VulkanBase.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////////
// Callback routine for API for debugging purposes
VKAPI_ATTR VkBool32 VKAPI_CALL CallbackDebugReport
(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t uObject,
	size_t sizeLocation,
	int32_t uMessageCode,
	const char* acLayerPrefix,
	const char* acMessage,
	void* pvUserData
)
{
	LogDebug("%s: %s", acLayerPrefix, acMessage);
	return VK_FALSE;
}

//////////////////////////////////////////////////////////////////////////
CVulkanBase::CVulkanBase()
{
}


CVulkanBase::~CVulkanBase()
{
}

#ifdef _WIN32
void CVulkanBase::Init(HWND hwnd, HINSTANCE hinstance)
{
	m_hwnd = hwnd;
	m_hinstance = hinstance;
#else
void CVulkanBase::Init()
{
#endif
	CreateLog();
	CreateInstance();
	CheckHardware();
	CreateExtensions();
	CreateSurface();
	CreateDevice();
	CreateSwapchain();
	CreateShaderModules();
	CreatePipleline();
	CreateFramebuffer();
	CreateCommandbuffer();
	CreateSemaphores();
}

void CVulkanBase::Tick()
{
	uint32_t uImage;
	
	// Get the current image from the swapchain
	vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_semaphoreReady, VK_NULL_HANDLE, &uImage);

	VkPipelineStageFlags aPipelineStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	m_submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_submitInfo.pNext					= nullptr;
	m_submitInfo.waitSemaphoreCount		= 1;
	m_submitInfo.pWaitSemaphores		= &m_semaphoreReady;
	m_submitInfo.pWaitDstStageMask		= aPipelineStageFlags;
	m_submitInfo.commandBufferCount		= 1;
	m_submitInfo.pCommandBuffers		= &(m_pCommandBuffer[uImage]);
	m_submitInfo.signalSemaphoreCount	= 1;
	m_submitInfo.pSignalSemaphores		= &m_semaphoreRenderingDone;

	VkResult result = vkQueueSubmit(m_queue, 1, &m_submitInfo, VK_NULL_HANDLE);
	LOG_CHECK_MSG_VK("vkQueueSubmit", result);

	VkPresentInfoKHR presentInfo;
	presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext				= nullptr;
	presentInfo.waitSemaphoreCount	= 1;
	presentInfo.pWaitSemaphores		= &m_semaphoreRenderingDone;
	presentInfo.swapchainCount		= 1;
	presentInfo.pSwapchains			= &m_swapChain;
	presentInfo.pImageIndices		= &uImage;
	presentInfo.pResults			= nullptr;

	result = vkQueuePresentKHR(m_queue, &presentInfo);
	LOG_CHECK_MSG_VK("vkQueuePresentKHR", result);
}

void CVulkanBase::Fini()
{

}

void CVulkanBase::CreateLog()
{
	LogStart("Vulkan MiniEngine Logfile", LogSource::Vulkan);
}

void CVulkanBase::CreateInstance()
{
	VkResult result;

	VkApplicationInfo applicationInfo;
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName	= "VulkanMiniGame";
	applicationInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName			= "VulkanMiniEngine";
	applicationInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion			= VK_API_VERSION_1_1;

	// Special feature in Vulkan: Extensions
	// => Extensions for Linux, MacOS, Windows, Debug-Modes, ...
	unsigned int uExtensionsUsed = 3;
	const char* pacExtensionUsed[] =
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
		"VK_EXT_debug_report"
	};

	// Another special feature: Layers
	// => Layers between different rendering steps for profiling, debugging, dumps, screenshots, ...
	unsigned int uLayersUsed = 1;
	const char* pacLayersUsed[] =
	{
		// WARNING: Only works with VK_EXT_debug_report extension
		"VK_LAYER_LUNARG_standard_validation",
	};

	VkInstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo			= &applicationInfo;
	instanceCreateInfo.enabledExtensionCount	= uExtensionsUsed; // if no extension used: nullptr
	instanceCreateInfo.ppEnabledExtensionNames	= pacExtensionUsed;
	instanceCreateInfo.enabledLayerCount		= uLayersUsed; // if no layer used: nullptr
	instanceCreateInfo.ppEnabledLayerNames		= pacLayersUsed;

	result = vkCreateInstance(
		&instanceCreateInfo,
		nullptr, // if nullptr: use internal Vulkan allocator
		&m_instance
	);
	LOG_CHECK_MSG_VK("vkCreateInstance: ", result);
}

void CVulkanBase::CheckHardware()
{
	VkResult result;
	uint32_t uPhysicalDevices = 0;
	// First call: Check how many physical devices are on the computer:
	// nullptr: 2nd parameter is output
	// Number of graphics card is determined and is written into uPhysicalDevices
	result = vkEnumeratePhysicalDevices(m_instance, &uPhysicalDevices, nullptr);
	LOG_CHECK_MSG_VK("vkEnumeratePhysicalDevices", result);
	LogDebug("Your computer has %u graphics cards", uPhysicalDevices);

	// Allocate memory for the array, depending on the number of physical devices
	m_pPhysicalDevices = new VkPhysicalDevice[uPhysicalDevices];

	// Second call: 
	// 2nd parameter is input (number of graphics cards)
	result = vkEnumeratePhysicalDevices(m_instance, &uPhysicalDevices, m_pPhysicalDevices);

	for (unsigned int u = 0; u < uPhysicalDevices; u++)
	{
		LogDebug("");
		LogDebug("Here are the properties of the %u. device: ", u + 1);
		
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_pPhysicalDevices[u], &deviceProperties);

		uint32_t uVersionMajor = VK_VERSION_MAJOR(deviceProperties.apiVersion);
		uint32_t uVersionMinor = VK_VERSION_MINOR(deviceProperties.apiVersion);
		uint32_t uVersionPatch = VK_VERSION_PATCH(deviceProperties.apiVersion);
		LogDebug("API Version: %u.%u. Patch %u", uVersionMajor, uVersionMinor, uVersionPatch);
		LogDebug("Driver Version: %u", deviceProperties.driverVersion);
		LogDebug("DeviceName:   : %s", deviceProperties.deviceName);
		LogDebug("DeviceID      : %u", deviceProperties.deviceID);
		LogDebug("Vendor ID     : %u", deviceProperties.vendorID);

		switch (deviceProperties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			LogDebug("Device Type: Integrated GPU");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			LogDebug("Device Type: Discrete GPU");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			LogDebug("Device Type: Virtual GPU");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			LogDebug("Device Type: CPU");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			LogDebug("Device Type: Other");
			break;
		}

		LogDebug("No. of priorities: %u", deviceProperties.limits.discreteQueuePriorities);

		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(m_pPhysicalDevices[u], &physicalDeviceMemoryProperties);
		LogDebug("No. of memory heaps: %u", physicalDeviceMemoryProperties.memoryHeapCount);

		for (unsigned int v = 0; v < physicalDeviceMemoryProperties.memoryHeapCount; v++)
		{
			LogDebug("Memory Heap Size %u:                        %u",
				v, physicalDeviceMemoryProperties.memoryHeaps[v].size);
			// If flag = 0: CPU memory, if flag = 1: GPU memory
			LogDebug("Memory Heap Flag %u (local bit):            %u",
				v, physicalDeviceMemoryProperties.memoryHeaps[v].flags &
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			// If flag = 0: memory for CPU invisible, if flag = 1: memory for CPU visible
			LogDebug("Memory Heap Flag %u (host visible bit):     %u",
				v, physicalDeviceMemoryProperties.memoryHeaps[v].flags &
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			// If flag = 1: changes for the other side (CPU vs. GPU) visible
			LogDebug("Memory Heap Flag %u (host coherent bit):    %u",
				v, physicalDeviceMemoryProperties.memoryHeaps[v].flags &
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			// If flag = 1: memory isn't allocated directly, instead only when necessary
			LogDebug("Memory Heap Flag %u (lazily allocated bit): %u",
				v, physicalDeviceMemoryProperties.memoryHeaps[v].flags &
				VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
		}

		uint32_t uQueueFamilies;
		// Call 2 times, first time with nullptr to calculate number of queue families
		vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevices[u], &uQueueFamilies, nullptr);
		LogDebug("No. of queue families: %u", uQueueFamilies);
		m_pQueueFamilyProperties = new VkQueueFamilyProperties[uQueueFamilies];

		// Second call to fill the array
		vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevices[u], &uQueueFamilies, m_pQueueFamilyProperties);
	}
}

void CVulkanBase::CreateExtensions()
{
	VkResult result;
	*(void**)&m_vkCreateDebugReportCallbackEXT = vkGetInstanceProcAddr(
		m_instance, "vkCreateDebugReportCallbackEXT");

	*(void**)&m_vkDestroyDebugReportCallbackEXT = vkGetInstanceProcAddr(
		m_instance, "vkDestroyDebugReportCallbackEXT");

	*(void**)&m_vkDebugReportCallbackEXT = vkGetInstanceProcAddr(
		m_instance, "vkDebugReportMessageEXT");

	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	callbackCreateInfo.pNext = nullptr;
	callbackCreateInfo.flags =	VK_DEBUG_REPORT_ERROR_BIT_EXT |
								VK_DEBUG_REPORT_WARNING_BIT_EXT |
								VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	callbackCreateInfo.pfnCallback = &CallbackDebugReport;
	callbackCreateInfo.pUserData = nullptr;

	result = m_vkCreateDebugReportCallbackEXT(m_instance, &callbackCreateInfo, 
		nullptr, &m_debugReportCallbackEXT);
	LOG_CHECK_MSG_VK("vkCreateDebugReportCallbackEXT", result);
}

void CVulkanBase::CreateSurface()
{
#ifdef _WIN32
	VkResult result;
	VkWin32SurfaceCreateInfoKHR win32CreateSurfaceInfo = {};
	win32CreateSurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	win32CreateSurfaceInfo.hwnd = m_hwnd;
	win32CreateSurfaceInfo.hinstance = m_hinstance;

	result = vkCreateWin32SurfaceKHR(m_instance, &win32CreateSurfaceInfo,
		nullptr, &m_surface);
	LOG_CHECK_MSG_VK("vkCreateWin32SurfaceKHR", result);
#endif
#ifdef LINUX
	// TODO
#endif

	// Define what the surface can do
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	// TODO: Select correct physical device
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevices[0], m_surface,
		&surfaceCapabilities);
	LOG_CHECK_MSG_VK("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", result);

	LogDebug("Surface Capabilities:");
	LogDebug(" - minImageCount:            %u", surfaceCapabilities.minImageCount);
	LogDebug(" - maxImageCount:            %u", surfaceCapabilities.maxImageCount);
	LogDebug(" - current extend, width:    %u", surfaceCapabilities.currentExtent.width);
	LogDebug(" - current extend, height:   %u", surfaceCapabilities.currentExtent.height);
	LogDebug(" - min image extend, width:  %u", surfaceCapabilities.minImageExtent.width);
	LogDebug(" - min image extend, height: %u", surfaceCapabilities.minImageExtent.height);
	LogDebug(" - max image extend, width:  %u", surfaceCapabilities.maxImageExtent.width);
	LogDebug(" - max image extend, height: %u", surfaceCapabilities.maxImageExtent.height);
	LogDebug(" - max image array layers:   %u", surfaceCapabilities.maxImageArrayLayers);
	LogDebug(" - supported transforms:     %u", surfaceCapabilities.supportedTransforms);
	LogDebug(" - current transforms:       %u", surfaceCapabilities.currentTransform);
	LogDebug(" - current composite alpha:  %u", surfaceCapabilities.supportedCompositeAlpha);
	LogDebug("Supported Usage Flags:");
	LogDebug(" - transfer src bit:             %08x", surfaceCapabilities.supportedUsageFlags &
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	LogDebug(" - color attachment bit:         %08x", surfaceCapabilities.supportedUsageFlags &
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	LogDebug(" - depth stencil attachment bit: %08x", surfaceCapabilities.supportedUsageFlags &
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	LogDebug(" - input attachment bit:         %08x", surfaceCapabilities.supportedUsageFlags &
		VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	LogDebug(" - transient attachment bit:     %08x", surfaceCapabilities.supportedUsageFlags &
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT);
	LogDebug(" - storage bit:                  %08x", surfaceCapabilities.supportedUsageFlags &
		VK_IMAGE_USAGE_STORAGE_BIT);
	LogDebug(" - sampled bit:                  %08x", surfaceCapabilities.supportedUsageFlags &
		VK_IMAGE_USAGE_SAMPLED_BIT);

	// Calculate the current resolution of the rendering area
	m_uWidth = surfaceCapabilities.currentExtent.width;
	m_uHeight = surfaceCapabilities.currentExtent.height;

	uint32_t uFormats = 0;
	// TODO: Select correct physical device, not the zero index1

	// The mehod vkGetPhysicalDeviceSurfaceFormatsKHR has to be called twice:
	// The first time with nullptr, to calculate the number of formats
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pPhysicalDevices[0], m_surface, &uFormats, nullptr);

	VkSurfaceFormatKHR* pSurfaceFormats = new VkSurfaceFormatKHR[uFormats];

	// the second time to calculate the format array depending of the number of formats
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pPhysicalDevices[0], m_surface, &uFormats, pSurfaceFormats);

	for (unsigned int u = 0; u < uFormats; u++)
	{
		LogDebug("Format %u: Color Space %u", u, pSurfaceFormats[u].colorSpace);
		LogDebug("Format %u: Color Space %u", u, pSurfaceFormats[u].colorSpace);
	}

	delete[] pSurfaceFormats;
}

void CVulkanBase::CreateDevice()
{
	// From the physical device which was selected the logic device is created now
	VkResult result;
	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

	// The priorities range from 0 to 1
	// Warning: Internally the priorities are mapped to integer-values (min. 2)
	// Depending on the graphics card there are many different priorities
	float afQueuePriorities[NR_OF_QUEUES_USED] = { 0.5f, 0.5f, 0.5f, 0.5f };

	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = nullptr;	// for future
	deviceQueueCreateInfo.flags = 0;		// for future
	// A graphics card can have different index families, each family is defined with a number
	// Only one family is needed here
	deviceQueueCreateInfo.queueFamilyIndex = 0;

	if (m_pQueueFamilyProperties[deviceQueueCreateInfo.queueFamilyIndex].queueCount <
		NR_OF_QUEUES_USED)
	{
		LogDebug("ERROR! Your graphics card ha not enough queues!");
	}

	deviceQueueCreateInfo.queueCount = NR_OF_QUEUES_USED;
	deviceQueueCreateInfo.pQueuePriorities = afQueuePriorities;

	// Define the extensions on device layer
	unsigned int uDeviceExtensionsUsed = 1;
	const char* m_aacDeviceExtensionsUsed[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkBool32 bSurfaceSupport = false;
	result = vkGetPhysicalDeviceSurfaceSupportKHR(m_pPhysicalDevices[0], 0, m_surface, &bSurfaceSupport);
	LOG_CHECK_MSG_VK("vkGetPhysicalDeviceSurfaceSupportKHR", result);

	if (bSurfaceSupport)
	{
		LogDebug("ERROR! Surface is not supported");
	}

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = uDeviceExtensionsUsed;
	deviceCreateInfo.ppEnabledExtensionNames = m_aacDeviceExtensionsUsed;
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

	// TODO: Select best device
	result = vkCreateDevice(m_pPhysicalDevices[0], &deviceCreateInfo, nullptr, &m_device);
	LOG_CHECK_MSG_VK("vkCreateDevice", result);

	vkGetDeviceQueue(m_device, 0, 0, &m_queue);
}

void CVulkanBase::CreateSwapchain()
{
	VkResult result;
	VkSwapchainCreateInfoKHR swapChainCreateInfo;
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.pNext = nullptr;
	swapChainCreateInfo.flags = 0;
	swapChainCreateInfo.surface = m_surface;
	swapChainCreateInfo.minImageCount = 3;
	swapChainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	swapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapChainCreateInfo.imageExtent = VkExtent2D{ m_uWidth, m_uHeight };
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.queueFamilyIndexCount = 0;
	swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	// Don't translate, rotate, scale image: IDENTITY_BIT_KHR
	swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	// Don't do transparency calculations on the screen: ALPHA_OPAQUE_BIT_KHR
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// First picture rendered is first picture shown
	swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = m_swapChain;

	result = vkCreateSwapchainKHR(m_device, &swapChainCreateInfo, nullptr, &m_swapChain);
	LOG_CHECK_MSG_VK("vkCreateSwapchainKHR", result);

	result = vkGetSwapchainImagesKHR(m_device, m_swapChain, &m_uImagesInSwapChain, nullptr);
	LOG_CHECK_MSG_VK("vkGetSwapchainImagesKHR", result);
	VkImage* pImageSpawchains = new VkImage[m_uImagesInSwapChain];
	LogDebug("Images in Swapchain: %u", m_uImagesInSwapChain);
	result = vkGetSwapchainImagesKHR(m_device, m_swapChain, &m_uImagesInSwapChain, pImageSpawchains);
	LOG_CHECK_MSG_VK("vkGetSwapchainImagesKHR", result);

	m_pImageView = new VkImageView[m_uImagesInSwapChain];

	for (unsigned int u = 0; u < m_uImagesInSwapChain; u++)
	{
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = pImageSpawchains[u];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		result = vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &m_pImageView[u]);
		LOG_CHECK_MSG_VK("vkCreateImageView", result);
	}

	delete[] pImageSpawchains;
	delete[] m_pImageView;
}

void CVulkanBase::CreateShaderModules()
{
	VkResult result;
	unsigned int uBufferSizeAllocated = 100000;
	HANDLE hFile;

	//////////////////////////////////////////////////////////////////////////
	// Read the vertex shader
	uint32_t uSizeCodeVert = 0;
	// HACK
	// TODO: dynamically allocate
	char* acShaderCodeVert = (char*)malloc(uBufferSizeAllocated);
	hFile = CreateFile("shaders\\vert.spv", GENERIC_READ, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		LogDebug("Couldn't read vertex shader file!");
	}
	else
	{
		ReadFile((HANDLE)hFile, acShaderCodeVert, 10000, (LPDWORD)&uSizeCodeVert, 0);
		CloseHandle(hFile);
		hFile = nullptr;
	}

	VkShaderModuleCreateInfo shaderModuleCreateInfoVert;
	shaderModuleCreateInfoVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfoVert.pNext = nullptr;
	shaderModuleCreateInfoVert.flags = 0;
	shaderModuleCreateInfoVert.codeSize = uSizeCodeVert;
	shaderModuleCreateInfoVert.pCode = (uint32_t*)acShaderCodeVert;

	result = vkCreateShaderModule(m_device, &shaderModuleCreateInfoVert, nullptr, &m_shaderModuleVert);
	LOG_CHECK_MSG_VK("vkCreateShaderModule Vertex:   ", result);

	//////////////////////////////////////////////////////////////////////////
	// Read the frag shader
	uint32_t uSizeCodeFrag = 0;
	// HACK
	// TODO: dynamically allocate
	char* acShaderCodeFrag = (char*)malloc(uBufferSizeAllocated);
	hFile = CreateFile("shaders\\frag.spv", GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		LogDebug("Couldn't read fragment shader file!");
	}
	else
	{
		ReadFile((HANDLE)hFile, acShaderCodeFrag, 10000, (LPDWORD)&uSizeCodeFrag, 0);
		CloseHandle(hFile);
		hFile = nullptr;
	}

	VkShaderModuleCreateInfo shaderModuleCreateInfoFrag;
	shaderModuleCreateInfoFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfoFrag.pNext = nullptr;
	shaderModuleCreateInfoFrag.flags = 0;
	shaderModuleCreateInfoFrag.codeSize = uSizeCodeFrag;
	shaderModuleCreateInfoFrag.pCode = (uint32_t*)acShaderCodeFrag;

	result = vkCreateShaderModule(m_device, &shaderModuleCreateInfoFrag, nullptr, &m_shaderModuleFrag);
	LOG_CHECK_MSG_VK("vkCreateShaderModule Fragment: ", result);
}

void CVulkanBase::CreatePipleline()
{
	VkResult result;

	VkPipelineShaderStageCreateInfo piplelineShaderStageCreateInfoVert;
	piplelineShaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	piplelineShaderStageCreateInfoVert.pNext = nullptr;
	piplelineShaderStageCreateInfoVert.flags = 0;
	piplelineShaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
	piplelineShaderStageCreateInfoVert.module = m_shaderModuleVert;
	piplelineShaderStageCreateInfoVert.pName = "main";
	piplelineShaderStageCreateInfoVert.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo piplelineShaderStageCreateInfoFrag;
	piplelineShaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	piplelineShaderStageCreateInfoFrag.pNext = nullptr;
	piplelineShaderStageCreateInfoFrag.flags = 0;
	piplelineShaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	piplelineShaderStageCreateInfoFrag.module = m_shaderModuleFrag;
	piplelineShaderStageCreateInfoFrag.pName = "main";
	piplelineShaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[] =
	{
		piplelineShaderStageCreateInfoVert,
		piplelineShaderStageCreateInfoFrag
	};

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.pNext = nullptr;
	pipelineVertexInputStateCreateInfo.flags = 0;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

	VkViewport viewport;
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = m_uWidth;
	viewport.width = m_uHeight;

	VkRect2D rect2D;
	rect2D.offset.x = 0;
	rect2D.offset.y = 0;
	rect2D.extent.width = m_uWidth;
	rect2D.extent.height = m_uHeight;

	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.pNext = nullptr;
	pipelineViewportStateCreateInfo.flags = 0;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports = &viewport;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	pipelineViewportStateCreateInfo.pScissors = &rect2D;

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.pNext = nullptr;
	pipelineRasterizationStateCreateInfo.flags = 0;
	// If false: in front of the NearClipping plane and behind the FarClipping plane values are discarded (default)
	// If true: values are kept (important for the shadow map render pass)
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	// If true the rasterizer is disabled, only used under special circumstances
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	// No wireframe and no vertex point clouds, shade and fill polygons
	pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	// Enable backface culling
	pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	// Right handed coordinate system
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
	pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.f;
	pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 1.f;
	// Width of lines in wireframe mode: 1px
	pipelineRasterizationStateCreateInfo.lineWidth = 1.f;

	// Multisampling isn't used, should't be used typically
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.pNext = nullptr;
	pipelineMultisampleStateCreateInfo.flags = 0;
	pipelineMultisampleStateCreateInfo.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
	pipelineMultisampleStateCreateInfo.sampleShadingEnable	= VK_FALSE;
	pipelineMultisampleStateCreateInfo.minSampleShading		= 1.f;
	// Certain regions can be excluded of multisampling
	pipelineMultisampleStateCreateInfo.pSampleMask			= nullptr;
	pipelineMultisampleStateCreateInfo.alphaToCoverageEnable= VK_FALSE;
	pipelineMultisampleStateCreateInfo.alphaToOneEnable		= VK_FALSE;

	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentstate;
	// The underlying pixel is used as well
	pipelineColorBlendAttachmentstate.blendEnable			= VK_TRUE;
	// Use the alpha channel
	pipelineColorBlendAttachmentstate.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;
	// 1-alpha for the underlying pixel
	pipelineColorBlendAttachmentstate.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	// Addition is used for the calculations
	pipelineColorBlendAttachmentstate.colorBlendOp			= VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentstate.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;
	pipelineColorBlendAttachmentstate.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentstate.alphaBlendOp			= VK_BLEND_OP_ADD;
	// Take all color channels into consideration, including alpha
	pipelineColorBlendAttachmentstate.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	VkDynamicState aDynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.pNext = nullptr;
	pipelineColorBlendStateCreateInfo.flags = 0;
	pipelineColorBlendStateCreateInfo.logicOpEnable		= VK_FALSE;
	pipelineColorBlendStateCreateInfo.logicOp			= VK_LOGIC_OP_NO_OP;
	pipelineColorBlendStateCreateInfo.attachmentCount	= 1;
	pipelineColorBlendStateCreateInfo.pAttachments		= &pipelineColorBlendAttachmentstate;
	pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.f;
	pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.f;
	pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.f;
	pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.f;

	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
	pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineDynamicStateCreateInfo.pNext = nullptr;
	pipelineDynamicStateCreateInfo.flags = 0;
	pipelineDynamicStateCreateInfo.dynamicStateCount	= 2;
	pipelineDynamicStateCreateInfo.pDynamicStates		= aDynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext						= nullptr;
	pipelineLayoutCreateInfo.flags						= 0;
	pipelineLayoutCreateInfo.setLayoutCount				= 0;
	pipelineLayoutCreateInfo.pSetLayouts				= nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount		= 0;
	pipelineLayoutCreateInfo.pPushConstantRanges		= nullptr;

	result = vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
	LOG_CHECK_MSG_VK("vkCreatePipelineLayout", result);

	VkAttachmentDescription attachmentDescription;
	attachmentDescription.flags = 0;
	attachmentDescription.format = VK_FORMAT_B8G8R8A8_UNORM;
	// Only important when using anti-aliasing
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	// Define how to use the data, which were previously written here:
	// Delete data before loading
	attachmentDescription.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentReference;
	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription;
	// Use the graphics pipeline instead of the compute pipeline
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount		= 0;
	subpassDescription.pInputAttachments		= nullptr;
	subpassDescription.colorAttachmentCount		= 1;
	subpassDescription.pColorAttachments		= &attachmentReference;
	subpassDescription.pResolveAttachments		= nullptr;
	subpassDescription.pDepthStencilAttachment	= nullptr;
	subpassDescription.preserveAttachmentCount	= 0;
	subpassDescription.pPreserveAttachments		= nullptr;

	VkSubpassDependency subpassDependency;
	subpassDependency.srcSubpass		= VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass		= 0;
	subpassDependency.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask		= 0;
	subpassDependency.dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags	= 0;

	VkRenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext				= nullptr;
	renderPassCreateInfo.flags				= 0;
	renderPassCreateInfo.attachmentCount	= 1;
	renderPassCreateInfo.pAttachments		= &attachmentDescription;
	renderPassCreateInfo.subpassCount		= 1;
	renderPassCreateInfo.pSubpasses			= &subpassDescription;
	renderPassCreateInfo.dependencyCount	= 1;
	renderPassCreateInfo.pDependencies		= &subpassDependency;

	result = vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &m_renderpass);
	LOG_CHECK_MSG_VK("vkCreateRenderPass", result);

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
	pipelineInputAssemblyStateCreateInfo.sType		= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.pNext		= nullptr;
	pipelineInputAssemblyStateCreateInfo.flags		= 0;
	pipelineInputAssemblyStateCreateInfo.topology	= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	// Can triangle strips be broken with the special command 0xFFFFFF?
	// If yes: VK_TRUE
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = nullptr;
	graphicsPipelineCreateInfo.flags = 0;
	graphicsPipelineCreateInfo.stageCount			= 2; // Two shader stages: vert/ frag
	graphicsPipelineCreateInfo.pStages				= pipelineShaderStageCreateInfo;
	graphicsPipelineCreateInfo.pVertexInputState	= &pipelineVertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState	= &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pTessellationState	= nullptr;
	graphicsPipelineCreateInfo.pViewportState		= &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState	= &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState	= &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState	= nullptr;
	graphicsPipelineCreateInfo.pColorBlendState		= &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState		= &pipelineDynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout				= m_pipelineLayout;
	graphicsPipelineCreateInfo.renderPass			= m_renderpass;
	graphicsPipelineCreateInfo.subpass				= 0;
	graphicsPipelineCreateInfo.basePipelineHandle	= VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex	= -1;

	result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_pipeline);
	LOG_CHECK_MSG_VK("vkCreateGraphicsPipelines", result);
}

void CVulkanBase::CreateFramebuffer()
{
	VkResult result;

	m_pFramebuffer = new VkFramebuffer[m_uImagesInSwapChain];

	for (unsigned int i = 0; i < m_uImagesInSwapChain; i++)
	{
		VkFramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass		= m_renderpass;
		framebufferCreateInfo.attachmentCount	= 1;
		framebufferCreateInfo.pAttachments		= &(m_pImageView[i]);
		framebufferCreateInfo.width				= m_uWidth;
		framebufferCreateInfo.height			= m_uHeight;
		framebufferCreateInfo.layers			= 1;

		result = vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &(m_pFramebuffer[i]));
		LOG_CHECK_MSG_VK("vkCreateFramebuffer", result);
	}
}

void CVulkanBase::CreateCommandbuffer()
{
	VkResult result;
	VkCommandPoolCreateInfo commandPoolCreateInfo;
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.flags = 0;
	// TODO: Can be not equal to null on exotic graphic cards: check it
	// Grab the first one
	commandPoolCreateInfo.queueFamilyIndex = 0;

	result = vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool);
	LOG_CHECK_MSG_VK("vkCreateCommandPool", result);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext					= nullptr;
	commandBufferAllocateInfo.commandPool			= m_commandPool;
	// Primary buffers can call secondary buffers, not possible in reverse order
	commandBufferAllocateInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount	= m_uImagesInSwapChain;

	m_pCommandBuffer = new VkCommandBuffer[m_uImagesInSwapChain];
	result = vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, m_pCommandBuffer);
	LOG_CHECK_MSG_VK("vkAllocateCommandBuffers", result);

	// Begin recording of commands
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	// SUBMIT_BIT: record new directly after committing
	// RENDERPASS_CONTINUE_BIT: Only one renderpass is committed
	// SIMULTANEOUS_USE_BIT: Even if the command isn't fully processed by the GPU new commands can be issued nevertheless
	commandBufferBeginInfo.flags			= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferBeginInfo.pInheritanceInfo	= nullptr; // Only important on secondary buffers

	VkViewport viewport;
	// Extends of the viewport
	viewport.x		= 0.f;
	viewport.y		= 0.f;
	viewport.width	= m_uWidth;
	viewport.height	= m_uHeight;
	// Depthbuffer distance
	viewport.minDepth = 1.f;
	viewport.maxDepth = 0.f;

	VkRect2D rect2D;
	rect2D.offset.x			= 0;
	rect2D.offset.y			= 0;
	rect2D.extent.width		= m_uWidth;
	rect2D.extent.height	= m_uHeight;

	for (unsigned int i = 0; i < m_uImagesInSwapChain; i++)
	{
		result = vkBeginCommandBuffer(m_pCommandBuffer[i], &commandBufferBeginInfo);
		LOG_CHECK_MSG_VK("vkBeginCommandBuffer", result);

		m_clearValue.color = { 0.f, 0.f, 1.f, 1.f }; // Blue
		m_clearValue.depthStencil.depth = 0.f;
		m_clearValue.depthStencil.stencil = 0;

		VkRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext				= nullptr;
		renderPassBeginInfo.renderPass			= m_renderpass;
		renderPassBeginInfo.framebuffer			= m_pFramebuffer[i];
		renderPassBeginInfo.renderArea.offset	= { 0, 0 };
		renderPassBeginInfo.renderArea.extent	= { m_uWidth, m_uHeight };
		renderPassBeginInfo.clearValueCount		= 1;
		renderPassBeginInfo.pClearValues		= &m_clearValue;
		
		// All command procedures in Vulkan begin with a VkCmd
		vkCmdBeginRenderPass(m_pCommandBuffer[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_pCommandBuffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

		vkCmdSetViewport(m_pCommandBuffer[i], 0, 1, &viewport);
		vkCmdSetScissor(m_pCommandBuffer[i], 0, 1, &rect2D);
		vkCmdDraw(m_pCommandBuffer[i], 3, 1, 0, 0); // 3 vertices, 1 instance

		vkCmdEndRenderPass(m_pCommandBuffer[i]);

		result = vkEndCommandBuffer(m_pCommandBuffer[i]);
		LOG_CHECK_MSG_VK("vkEndCommandBuffer", result);
	}
}

void CVulkanBase::CreateSemaphores()
{
	VkResult result;
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	// Semaphore at the beginning of the access of a resource
	result = vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_semaphoreReady);
	LOG_CHECK_MSG_VK("vkCreateSemaphore: Ready", result);

	// Semaphore at the end of the access of a resource
	result = vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_semaphoreRenderingDone);
	LOG_CHECK_MSG_VK("vkCreateSemaphore: Rendering Done", result);
}

void CVulkanBase::ReSize(unsigned int uX, unsigned int uY)
{

}
