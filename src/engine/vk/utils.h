#pragma once

#include <source_location>
#include <print>

#include <vulkan/vk_enum_string_helper.h>

inline void vk_check_impl(VkResult result, const char* expression, std::source_location loc = std::source_location::current())
{
    if (result != VK_SUCCESS)
    {
        std::println(stderr, "[Vulkan]");
        std::println(stderr, "  Code:     {}", expression);

        std::println(stderr, "  Result:   {} ({})", string_VkResult(result), (int)result);
        std::println(stderr, "  Location: {}:{}:{}", loc.file_name(), loc.line(), loc.function_name());

        if (result < 0)
        {
            std::println(stderr, "  FATAL ERROR, Aborting...");
            std::abort();
        }
    }
}

#define VK_CHECK(x) vk_check_impl(x, #x)