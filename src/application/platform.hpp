#pragma once
#if defined(__MINGW32__)
#ifdef __clang__
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(_MSC_VER)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif		// end of mingw32
#elif defined(WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3native.h>

namespace vk { namespace wnd 
{

#ifdef GLFW_EXPOSE_NATIVE_WIN32
    template <typename Window>
    struct native_interface
    {
        using const_reference = Window const&;
        using wnd_handle_t = HWND;
        using app_handle_t = HINSTANCE;
        
        wnd_handle_t get_window_handle() const
        {
            auto window = static_cast<const_reference>(*this).get_window();
            return glfwGetWin32Window(window);
        }

        app_handle_t get_app_handle() const
        {
            return get_window_instance(get_window_handle());
        }

        static app_handle_t get_window_instance(wnd_handle_t wnd)
        {
            return reinterpret_cast<app_handle_t>(GetWindowLongPtr(wnd, GWLP_HINSTANCE));
        }
    };

#elif defined(GLFW_EXPOSE_NATIVE_X11)
    struct native_interface
    {
        using wnd_handle_t = Window;

        wnd_handle_t get_window_handle(GLFWwindow* window) const
        {
            return glfwGetX11Window(window);
        }
    };
#endif

} }
