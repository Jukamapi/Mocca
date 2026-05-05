#include "Mocca/vulkan/vk_check.h"

#include <vulkan/vk_enum_string_helper.h>
#include <print>
#include <stdexcept>


void vk_check_impl(VkResult result, const char* expression, std::source_location loc)
{
    if(result != VK_SUCCESS)
    {
        std::println(stderr, "[Vulkan]");
        std::println(stderr, "  Code:     {}", expression);

        std::println(stderr, "  Result:   {} ({})", string_VkResult(result), (int)result);
        std::println(stderr, "  Location: {}:{}:{}", loc.file_name(), loc.line(), loc.function_name());

        if(result < 0)
        {
            throw std::runtime_error(std::string("Fatal Vulkan Error: ") + string_VkResult(result));
        }
    }
}