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

		VkPhysicalDeviceType deviceType;
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
}

void CVulkanBase::CreateSwapchain()
{

}

void CVulkanBase::CreateShaderModules()
{

}

void CVulkanBase::CreatePipleline()
{

}

void CVulkanBase::CreateFramebuffer()
{

}

void CVulkanBase::CreateCommandbuffer()
{

}

void CVulkanBase::CreateSemaphores()
{

}

void CVulkanBase::ReSize(unsigned int uX, unsigned int uY)
{

}
