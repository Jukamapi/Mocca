#include "event_handler.h"
#include <engine/vk/context.h>
#include <engine/renderer/renderer.h>

EventHandler::EventHandler()
{
}

void EventHandler::handle(const Event& event)
{
    std::visit([this](const auto& e) {
        using T = std::decay_t<decltype(e)>;

        if constexpr (std::is_same_v<T, WindowResizeEvent>) {
            // Handle resize
        }
        else if constexpr (std::is_same_v<T, MouseMoveEvent>) {
            // Handle mouse
        }
    }, event);
}