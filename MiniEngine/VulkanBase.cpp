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
		LogDebug("DeviceName:   : %u", deviceProperties.deviceName);
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
}

void CVulkanBase::CreateDevice()
{

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
