/*
 * Copyright (C) 2015 Wenhua Zheng - All Rights Reserved
 * 
 * This file is part of MetaLib. MetaLib is licensed under the terms which are
 * described in the "LICENSE" file. You should have received the LICENSE file
 * along with this file; if not, please contact the author.
 * 
 * File Author    : Wenhua Zheng
 * File Written At: April, 2015
 */


#ifndef MetaLibScopeGuard_H_
#define MetaLibScopeGuard_H_


#include "MetaLibPort.h"
#include <utility>
#include <type_traits>


//#if defined(_MSC_VER) && (_MSC_VER >= 1900)//TODO
namespace MetaLibScopeGuardPrivate
{
;

template<typename F>
class Guard//<F, std::enable_if_t<noexcept(std::declval<F>().~F())>>
{
#if defined(_MSC_VER) && _MSC_VER < 1900
#else
    static_assert(std::is_nothrow_destructible<F>::value, "~F() must be noexcept");
#endif
    bool valid;
#if MetaLib_HAS_UNRESTRICTED_UNION
private:
	union
    {
        F f;
    };
	F * ptr() MetaLib_NOEXCEPT
    {
        return &f;
    }
    F const * ptr() const MetaLib_NOEXCEPT
    {
        return &f;
    }
#else
private:
    typename std::aligned_storage<sizeof(F), MetaLib_ALIGNOF(F)>::type f_storage;
    //MetaLib_ALIGNAS(8) std::array<char, sizeof(F)> f_storage;
    F * ptr() MetaLib_NOEXCEPT
    {
        return reinterpret_cast<F *>(&f_storage);
    }
    F const * ptr() const MetaLib_NOEXCEPT
    {
        return reinterpret_cast<F const *>(&f_storage);
    }
#endif
private:
    F & ref() MetaLib_NOEXCEPT
    {
        return *ptr();
    }
    F const & ref() const MetaLib_NOEXCEPT
    {
        return *ptr();
    }
public:
    Guard()
        :
        valid(false)
    {}
    template <typename FF>
    Guard(FF && ff, int)//do not override copy/move ctor
    {
        new (ptr()) F(std::forward<FF>(ff));
        valid = true;
    }
    Guard(Guard && rhs)
    {
        if (rhs.valid)
        {
            new (ptr()) F(std::move(rhs.ref()));
            valid = true;
            rhs.ref().~F();
            rhs.valid = false;
        }
    }
    void invoke()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(ref()()))
#endif
    {
        if (valid)
        {
            valid = false;
            try
            {
                ref()();
            }
            catch (...)
            {
                ref().~F();
                throw;
            }
            ref().~F();
        }
    }
    ~Guard()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(invoke()))
#endif
    {
        invoke();
    }
    void release() MetaLib_NOEXCEPT
    {
        if (valid)
        {
            ref().~F();
            valid = false;
        }
    }
#if MetaLib_HAS_DEFAULTED_DELETED_FUNCTIONS
    Guard(Guard const &) = delete;
    Guard & operator=(Guard const &) = delete;
    Guard & operator=(Guard &&) = delete;
#else
private:
    Guard(Guard const &);
    Guard & operator=(Guard const &);
    Guard & operator=(Guard &&);
#endif
};

template<typename F>
auto makeGuard(F && f)
    -> Guard<typename std::remove_reference<F>::type>
{
    return Guard<typename std::remove_reference<F>::type>(std::forward<F>(f), 0);
}

#if defined(_MSC_VER) && (_MSC_VER >= 1900)//TODO
MetaLib_INLINE int uncaught_exceptions()
{
    return std::uncaught_exceptions();
}
#elif defined(_MSC_VER) && (_MSC_VER == 1600)//TODO: maybe others
extern "C" char * _getptd();
MetaLib_INLINE size_t ue_offset(std::integral_constant<size_t, 4>)
{
    return 0x90;
}
MetaLib_INLINE size_t ue_offset(std::integral_constant<size_t, 8>)
{
    return 0x100;
}
MetaLib_INLINE int uncaught_exceptions()
{
    auto p = _getptd();
    p += ue_offset(std::integral_constant<size_t, sizeof(void *)>());
    return *(reinterpret_cast<int *>(p));
}
#else
#error "Not supported, yet."
#endif

template <typename F>
class GuardSuccess
{
    F f;
    int ec;
public:
    template <typename FF>
    GuardSuccess(FF && ff, int)//do not override copy/move ctor
        :
        f(std::forward<FF>(ff)),
        ec(uncaught_exceptions())
    {}
    void operator()()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(f()))
#endif
    {
        if (uncaught_exceptions() <= ec)
            f();
    }
};

template<typename F>
auto makeGuardSuccess(F && f)
    -> Guard<GuardSuccess<typename std::remove_reference<F>::type>>
{
#if 0
    return makeGuard([f{ std::forward<F>(f) }, ec{ uncaught_exceptions() }]()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(f()))
#endif
    {
        if (uncaught_exceptions() <= ec)
            f();
    });
#else
    return makeGuard(GuardSuccess<typename std::remove_reference<F>::type>(std::forward<F>(f), 0));
#endif
}

template <typename F>
class GuardFailure
{
    F f;
    int ec;
public:
    template <typename FF>
    GuardFailure(FF && ff, int)//do not override copy/move ctor
        :
        f(std::forward<FF>(ff)),
        ec(uncaught_exceptions())
    {}
    void operator()()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(f()))
#endif
    {
        if (uncaught_exceptions() > ec)
            f();
    }
};

template<typename F>
auto makeGuardFailure(F && f)
    -> Guard<GuardFailure<typename std::remove_reference<F>::type>>
{
#if 0
    return makeGuard([f{ std::forward<F>(f) }, ec{ uncaught_exceptions() }]()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(f()))
#endif
    {
        if (uncaught_exceptions() > ec)
            f();
    });
#else
    return makeGuard(GuardFailure<typename std::remove_reference<F>::type>(std::forward<F>(f), 0));
#endif
}

template <typename F>
class GuardIfSuccess
{
    F f;
    int ec;
public:
    template <typename FF>
    GuardIfSuccess(FF && ff, int)//do not override copy/move ctor
        :
        f(std::forward<FF>(ff)),
        ec(uncaught_exceptions())
    {}
    void operator()()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(f(false)) && noexcept(f(true)))
#endif
    {
        f(ec >= uncaught_exceptions());
    }
};

template<typename F>
auto makeGuardIfSuccess(F && f)
-> Guard<GuardIfSuccess<typename std::remove_reference<F>::type>>
{
#if 0
    return makeGuard([f{ std::forward<F>(f) }, ec{ uncaught_exceptions() }]()
#if MetaLib_HAS_NOEXCEPT
        noexcept(noexcept(f(false)) && noexcept(f(true)))
#endif
    {
        f(ec >= uncaught_exceptions());
    });
#else
    return makeGuard(GuardIfSuccess<typename std::remove_reference<F>::type>(std::forward<F>(f), 0));
#endif
}

#ifdef _MSC_VER
__declspec(noinline)
#endif
inline void watchFailure()
{
    MetaLib_Intrin_nop();
}

}


namespace MetaLib
{
    using MetaLibScopeGuardPrivate::makeGuard;
    using MetaLibScopeGuardPrivate::makeGuardFailure;
    using MetaLibScopeGuardPrivate::makeGuardSuccess;
    using MetaLibScopeGuardPrivate::makeGuardIfSuccess;
}

//#endif


#define MetaLib_GUARD_FAILURE \
auto metalib_guard_failure = ::MetaLib::makeGuardFailure([]()\
{\
    int volatile skip = 0;\
    if (!skip)\
        MetaLibScopeGuardPrivate::watchFailure();\
})

#if MetaLib_DEBUG
#define MetaLib_DEBUG_GUARD_FAILURE MetaLib_GUARD_FAILURE
#else
#define MetaLib_DEBUG_GUARD_FAILURE (void)(0)
#endif


#endif
