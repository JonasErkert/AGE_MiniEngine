#include "pch.h"

#include "VulkanBase.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////////
// Callback routine for API for debugging purposes
VKAPI_ATTR VkBool32 VKAPI_CALL CallDebugReport
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

void CVulkanBase::Init()
{
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

}

void CVulkanBase::CreateSurface()
{

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
