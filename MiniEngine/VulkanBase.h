#pragma once
#include "C:\\VulkanSDK\\1.1.121.2\Include\vulkan\vulkan.h"

class CVulkanBase
{
public:
	CVulkanBase();
	~CVulkanBase();

	void Init();
	void Tick();
	void Fini();
};
