#pragma once

namespace vk
{
    // raii object
    template <typename T>
    class object
    {
    public:
        template <typename F>
        object(T object, F&& deleter)
            : object_(object)
            , deleter_(std::forward<F>(deleter))
        {
        }

        ~object()
        {
            deleter_(object_);
        }

        object(object const&) = delete;
        object(object&&) = default;
        object& operator= (object const&) = delete;
        object& operator= (object&&) = default;

        T get_object() const noexcept
        {
            return object_;
        }

    private:
        T                         object_;
        std::function<void(T)>    deleter_;
    };
}