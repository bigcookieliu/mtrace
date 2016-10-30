#pragma once

extern "C"
{
#include <malloc.h>
}

#include <iostream>

namespace
{
    using malloc_hook = void*(*)(size_t, const void*);
    using free_hook = void(*)(void*, const void*);
    using realloc_hook = void*(*)(void*, size_t, const void*);
}

struct mtrace
{
    mtrace()
    {
        save_hooks();
        load_custom_hooks();
    }

    ~mtrace()
    {
        restore_hooks();
    }

    static void* malloc(size_t size, const void* caller)
    {
        restore_hooks();

        void* p = ::malloc(size);

        save_hooks();
        std::cout << "malloc " << size << " bytes: " << p << std::endl;
        load_custom_hooks();
        return p;
    }

    static void free(void* mem, const void* caller)
    {
        restore_hooks();

        ::free(mem);

        save_hooks();
        std::cout << "free " << mem << std::endl;
        load_custom_hooks();
    }

    static void* realloc(void* mem, size_t size, const void* caller)
    {
        restore_hooks();

        void* p = ::realloc(mem, size);

        save_hooks();
        std::cout << "realloc " << size << " bytes: " << p << std::endl;
        load_custom_hooks();
        return p;
    }

private:
    static void load_custom_hooks()
    {
        __malloc_hook = malloc;
        __free_hook = free;
        __realloc_hook = realloc;
    }

    static void save_hooks()
    {
        _old_malloc = __malloc_hook;
        _old_free = __free_hook;
        _old_realloc = __realloc_hook;
    }

    static void restore_hooks()
    {
        __malloc_hook = _old_malloc;
        __free_hook = _old_free;
        __realloc_hook = _old_realloc;
    }

    static malloc_hook  _old_malloc;
    static free_hook    _old_free;
    static realloc_hook _old_realloc;
};

malloc_hook mtrace::_old_malloc;
free_hook mtrace::_old_free;
realloc_hook mtrace::_old_realloc;

