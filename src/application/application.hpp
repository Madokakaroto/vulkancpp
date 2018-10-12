#pragma once

#include <GLFW/glfw3.h>
#include "window.hpp"

namespace vk
{
    class application_t
    {
    protected:
        application_t()
        {
            glfwInit();

            // we create vulkan API ourselves
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

    public:
        ~application_t()
        {
            glfwTerminate();
        }

        application_t(application_t const&) = delete;
        application_t& operator=(application_t const&) = delete;
        application_t(application_t&&) = delete;
        application_t& operator=(application_t&&) = delete;

        static application_t& get() noexcept
        {
            static application_t app{};
            return app;
        }

        window_t create_window(size_t w, size_t h, std::string name) const
        {
            return { create_window_impl(w, h, name) };
        }

        managed_window_t create_managed_window(size_t w, size_t h, std::string name) const
        {
            return { create_window_impl(w, h, name) };
        }

        template <typename Window>
        void run(Window const& window)
        {
            while (!glfwWindowShouldClose(window.get_window()))
            {
                glfwPollEvents();
            }
        }

    private:
        GLFWwindow* create_window_impl(size_t width, size_t height, std::string const& name) const
        {
            auto window = glfwCreateWindow(
                static_cast<int>(width),
                static_cast<int>(height),
                name.c_str(), nullptr, nullptr);

            if (nullptr == window)
                throw std::runtime_error{ "Failed to create window_t" };

            return window;
        }
    };
}