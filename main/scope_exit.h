#pragma once

template <typename T>
struct scope_exit
{
    inline scope_exit(T&& t) :
        t_{ static_cast<T&&>(t) }
    {}

    inline ~scope_exit()
    {
        t_();
    }
    T t_;
};

template <typename T>
inline scope_exit<T> make_scope_exit(T&& t)
{
    return scope_exit<T>{static_cast<T&&>(t)};
}

#define EXIT_SCOPE_CREATE_UNIQ_NAME2(line) exit_scope_guard_##line
#define EXIT_SCOPE_CREATE_UNIQ_NAME(line) EXIT_SCOPE_CREATE_UNIQ_NAME2(line)
#define EXIT_SCOPE_NAME() auto EXIT_SCOPE_CREATE_UNIQ_NAME(__LINE__)
#define EXIT_SCOPE_SIMPLE(var) auto EXIT_SCOPE_CREATE_UNIQ_NAME(__LINE__) = make_scope_exit([](){ var })
#define EXIT_SCOPE_FULL(var) auto EXIT_SCOPE_CREATE_UNIQ_NAME(__LINE__) = make_scope_exit([&](){ var })
#define EXIT_SCOPE(var) auto EXIT_SCOPE_CREATE_UNIQ_NAME(__LINE__) = make_scope_exit( var )