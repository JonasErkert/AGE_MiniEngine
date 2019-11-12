#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "VulkanBase.h"
#include "LogFile.h"


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
	char sz[MAX_PATH];
	sprintf(sz, ".\\logs\\LogVulkan.txt");
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
		"VK_KHR_WIN32_surface",
		"VK_EXT_DEBUG_report"
	};

	// Another special feature: Layers
	// => Layers between different rendering steps for profiling, debugging, dumps, screenshots, ...
	unsigned int uLayersUsed;
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
}

void CVulkanBase::CheckHardware()
{

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
