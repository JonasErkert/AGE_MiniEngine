#pragma once
#define _CRT_SECURE_NO_WARNINGS 1

#include "C:\\VulkanSDK\\1.1.121.2\Include\vulkan\vulkan.h"

#define VK_NO_PROPERTIES
#define NR_OF_QUEUES_USED 4

#ifdef _WIN32
#include "windows.h"
#include "C:\\VulkanSDK\\1.1.121.2\Include\vulkan\vulkan_win32.h"
#define VK_USE_PLATFORM_WIN32_KHR
#endif

class CVulkanBase
{
public:
	CVulkanBase();
	~CVulkanBase();
	

#ifdef _WIN32
	void Init(HWND hwnd, HINSTANCE hinstance);
#else
	void Init();
#endif
	void Tick();
	void Fini();

private:
	void CreateLog();
	void CreateInstance();
	void CheckHardware();
	/** Load the functions of the extensions, so we can use them. */
	void CreateExtensions();
	/** Create an area to paint on. */
	void CreateSurface();
	void CreateDevice();
	void CreateSwapchain();
	void CreateShaderModules();
	void CreatePipleline();
	void CreateFramebuffer();
	void CreateCommandbuffer();
	void CreateSemaphores();

	void ReSize(unsigned int uX, unsigned int uY);

#ifdef _WIN32
	HWND m_hwnd = nullptr;
	HINSTANCE m_hinstance = nullptr;
#endif

	VkInstance m_instance;
	uint32_t m_uWidth;
	uint32_t m_uHeight;

	// Logic device, an abstraction of the hardware
	// (Contrary to the physical device)
	VkDevice m_device;
	// Physical Device (graphics card)
	VkPhysicalDevice* m_pPhysicalDevices = nullptr;

	// Structs for the command lists
	VkQueue m_queue;
	VkQueueFamilyProperties* m_pQueueFamilyProperties = nullptr;
	VkFramebuffer* m_pFramebuffer = nullptr;
	VkCommandPool* m_pCommandPool = nullptr;
	VkCommandBuffer* m_pCommandBuffer = nullptr;

	// Background color and Deletion values of the depth stencil buffer
	VkClearValue m_clearValue;

	// Semaphores are equal to the resource barriers in dx12
	VkSemaphore m_semaphoreReady;
	VkSemaphore m_semaphoreRenderingDone;
	VkSubmitInfo m_submitInfo;

	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface;
	VkImageView* m_pImageView = nullptr;

	uint32_t m_uImagesInSwapChain = 0;

	// Shader data buffer (equal to shader blobs in dx12)
	VkShaderModule m_shaderModuleVert; // vertex shader blob in dx12
	VkShaderModule m_shaderModuleFrag; // pixel shader blob in dx12

	// Equal to the PipelineStateObject in dx12
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
	VkRenderPass m_renderpass;

	// Callback routines for debugging
	PFN_vkCreateDebugReportCallbackEXT m_vkCreateDebugReportCallbackEXT = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT m_vkDestroyDebugReportCallbackEXT = nullptr;
	PFN_vkDebugReportCallbackEXT m_vkDebugReportCallbackEXT = nullptr;
	VkDebugReportCallbackEXT m_debugReportCallbackEXT;
};
