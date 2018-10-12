
#include "platform.hpp"

namespace vk
{
    class window_t : public wnd::native_interface<window_t>
    {
    protected:
        friend class application_t;
        friend wnd::native_interface<window_t>;

        GLFWwindow* window_;
        window_t(GLFWwindow* window)
            : window_(window) {
        }

        GLFWwindow* get_window() const noexcept
        {
            return window_;
        }

    public:
        using native_handle_t = wnd::native_interface<window_t>::wnd_handle_t;

        ~window_t() noexcept
        {
            destroy();
        }

        window_t(window_t const&) = delete;
        window_t& operator=(window_t const&) = delete;

        window_t(window_t&& other) noexcept
        {
            swap(other);
        }

        window_t& operator=(window_t&& other) noexcept
        {
            swap(other);
            other.destroy();
            return *this;
        }

        native_handle_t get_native_handle() const
        {
            return this->get_window_handle();
        }

    private:
        void swap(window_t& other) noexcept
        {
            std::swap(window_, other.window_);
        }

        void destroy() noexcept
        {
            if (window_)
            {
                glfwDestroyWindow(window_);
                window_ = nullptr;
            }
        }
    };

    class managed_window_t : public wnd::native_interface<managed_window_t>
    {
        friend class application_t;
        friend wnd::native_interface<managed_window_t>;

    protected:
        GLFWwindow* window_;
        managed_window_t(GLFWwindow* window)
            : window_(window)
        { }

        GLFWwindow* get_window() const noexcept
        {
            return window_;
        }

    public:
        using native_handle_t = wnd::native_interface<managed_window_t>::wnd_handle_t;

        managed_window_t(managed_window_t const&) = default;
        managed_window_t& operator=(managed_window_t const&) = default;
        managed_window_t(managed_window_t&&) = default;
        managed_window_t& operator=(managed_window_t&&) = default;

        native_handle_t get_native_handle() const
        {
            return this->get_window_handle();
        }
    };
}
