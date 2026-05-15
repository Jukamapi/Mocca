// translation unit that provides implementations
// for header libraries
// volk should be included first as vma depends on it

#include <volk.h>

#define VMA_IMPLEMENTATION
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#include <vma/vk_mem_alloc.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
