#pragma once

namespace vk
{
    // raii object
    template <typename T>
    class object
    {
    public:
        object()
            : object_(nullptr)
            , deleter_()
        {}

        template <typename F>
        object(T object, F&& deleter)
            : object_(object)
            , deleter_(std::forward<F>(deleter))
        {}

        ~object() noexcept
        {
            if(deleter_ && object_)
                deleter_(object_);
        }

        object(object const&) = delete;
        object(object&&) = default;
        object& operator= (object const&) = delete;
        object& operator= (object&&) = default;

        operator T() const noexcept
        {
            return get();
        }

        T get() const noexcept
        {
            return object_;
        }

        T reset(T object)
        {
            auto result = object_;
            object_ = object;
            return result;
        }

        T reset(T object, std::function<void(T)> deleter)
        {
            deleter_ = std::move(deleter);
            return reset(object);
        }

    private:
        T                         object_;
        std::function<void(T)>    deleter_;
    };
}