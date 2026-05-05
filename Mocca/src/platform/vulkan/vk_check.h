#pragma once

#include <volk.h>
#include <source_location>



void vk_check_impl(VkResult result, const char* expression, std::source_location loc = std::source_location::current());

#define VK_CHECK(x) vk_check_impl(x, #x)