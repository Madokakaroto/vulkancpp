#pragma once

namespace vk
{
    struct null_type {};

    struct swallow_t
    {
        template <typename ... T>
        constexpr swallow_t(T&& ... t) noexcept {}
    };

    template <bool Test, typename T = void>
    using disable_if = std::enable_if<std::negation_v<std::bool_constant<Test>>, T>;

    template <bool Test, typename T = void>
    using disable_if_t = typename disable_if<Test, T>::type;

    template <typename T, typename = void>
    struct is_functor : std::false_type {};
    template <typename T>
    struct is_functor<T, std::enable_if_t<std::is_member_pointer_v<decltype(&T::operator())>>>
        : std::true_type {};
    template <typename T>
    inline constexpr bool is_functor_v = is_functor<T>::value;

    template <typename T>
    using is_callable = std::conjunction<std::is_function<T>, is_functor<T>>;
    template <typename T>
    inline constexpr bool is_callable_v = is_callable<T>::value;
}

// callable traits
namespace vk
{
    namespace detail
    {
        // generics 
        template <typename Callable, typename = void>
        struct callable_traits_impl;

        template <typename Callable>
        struct callable_traits_impl<Callable, std::enable_if_t<is_functor_v<Callable>>>
            : callable_traits_impl<decltype(&Callable::operator())>
        {
            inline static constexpr bool is_pmf = false;
        };

        // specialization for function
        template <typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(Args...), void>
        {
            using result_type = Ret;
            using caller_type = void;
            using args_pack = std::tuple<Args...>;
            using signature_type = Ret(*)(Args...);

            inline static constexpr size_t arity = sizeof...(Args);
            inline static constexpr bool is_pmf = false;
            inline static constexpr bool has_const_qualifier = false;
            inline static constexpr bool has_volatile_qualifier = false;
            inline static constexpr bool has_lvalue_ref_qualifier = false;
            inline static constexpr bool has_rvalue_ref_qualifier = false;
        };

        template <typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(*)(Args...), void> : callable_traits_impl<Ret(Args...)> {};

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...), void> : callable_traits_impl<Ret(Args...)>
        {
            using caller_type = T;
            using args_pack = std::tuple<Args...>;

            inline static constexpr bool is_pmf = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) &, void> : callable_traits_impl<Ret(T::*)(Args...)>
        {
            inline static constexpr bool has_lvalue_ref_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) && , void> : callable_traits_impl<Ret(T::*)(Args...)>
        {
            inline static constexpr bool has_rvalue_ref_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) const, void> : callable_traits_impl<Ret(T::*)(Args...)>
        {
            inline static constexpr bool has_const_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) const &, void> : callable_traits_impl<Ret(T::*)(Args...) &>
        {
            inline static constexpr bool has_const_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) const &&, void> : callable_traits_impl<Ret(T::*)(Args...) && >
        {
            inline static constexpr bool has_const_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) volatile, void> : callable_traits_impl<Ret(T::*)(Args...)>
        {
            inline static constexpr bool has_volatile_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) volatile &, void> : callable_traits_impl<Ret(T::*)(Args...) &>
        {
            inline static constexpr bool has_volatile_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) volatile &&, void> : callable_traits_impl<Ret(T::*)(Args...) && >
        {
            inline static constexpr bool has_volatile_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) const volatile, void> : callable_traits_impl<Ret(T::*)(Args...) const>
        {
            inline static constexpr bool has_volatile_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) const volatile &, void> : callable_traits_impl<Ret(T::*)(Args...) const&>
        {
            inline static constexpr bool has_volatile_qualifier = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct callable_traits_impl<Ret(T::*)(Args...) const volatile &&, void> : callable_traits_impl<Ret(T::*)(Args...) const &&>
        {
            inline static constexpr bool has_volatile_qualifier = true;
        };
    }

    template <typename Callable>
    struct callable_traits : detail::callable_traits_impl<std::remove_const_t<Callable>> {};
}

// interface generate
namespace vk
{
    // class hierarchy generation
    template
    <
        typename TT,
        template <typename, typename, typename> class TE,
        typename ... Exts
    >
    struct generate_extensions_hierarchy;

    template
    <
        typename TT,
        template <typename, typename, typename> class TE,
        typename T, typename ... Rests
    >
    struct generate_extensions_hierarchy<TT, TE, T, Rests...>
    {
        using type = TE<T, TT, typename generate_extensions_hierarchy<TT, TE, Rests...>::type>;
    };

    template
    <
        typename TT,
        template <typename, typename, typename> class TE,
        typename T
    >
    struct generate_extensions_hierarchy<TT, TE, T>
    {
        using type = TE<T, TT, null_type>;
    };

    template
    <
        typename TT,
        template <typename, typename, typename> class TE,
        typename ... Exts
    >
    using generate_extensions_hierarchy_t = typename generate_extensions_hierarchy<TT, TE, Exts...>::type;
}