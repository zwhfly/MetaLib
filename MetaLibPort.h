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


#ifndef MetaLibPort_H_
#define MetaLibPort_H_


#ifndef MetaLib_EXTERN_C
#ifdef __cplusplus
#define MetaLib_EXTERN_C extern "C"
#else
#define MetaLib_EXTERN_C extern
#endif
#endif


#ifndef MetaLib_CONCAT
#define MetaLib_CONCAT_(a, b) a ## b
#define MetaLib_CONCAT(a, b) MetaLib_CONCAT_(a, b)
#endif

#ifndef MetaLib_STRINGIFY
#define MetaLib_STRINGIFY_(a) #a
#define MetaLib_STRINGIFY(a) MetaLib_STRINGIFY_(a)
#endif


/* define these macros for every compiler and platform that's used */
#ifdef _MSC_VER
/* #if 1 */


#if   _MSC_VER >= 2000//unknown at written time
#error "Unsupported '_MSC_VER'"
#elif _MSC_VER >= 1900
#define MetaLib_MSC_VER_MAJOR 14
#elif _MSC_VER >= 1800
#define MetaLib_MSC_VER_MAJOR 12
#elif _MSC_VER >= 1700
#define MetaLib_MSC_VER_MAJOR 11
#elif _MSC_VER >= 1600
#define MetaLib_MSC_VER_MAJOR 10
#elif _MSC_VER >= 1500
#define MetaLib_MSC_VER_MAJOR 9
#elif _MSC_VER >= 1400
#define MetaLib_MSC_VER_MAJOR 8
#elif _MSC_VER >= 1300
#define MetaLib_MSC_VER_MAJOR 7
#elif _MSC_VER >= 1200
#define MetaLib_MSC_VER_MAJOR 6
#elif _MSC_VER >= 1100
#define MetaLib_MSC_VER_MAJOR 5
#else
#error "Unsupported '_MSC_VER'"
#endif


#define MetaLib_ABI_COMPILER_DOMAIN "MSVC" MetaLib_STRINGIFY(MetaLib_MSC_VER_MAJOR)


//TODO: NDEBUG?
#ifdef _DEBUG
#define MetaLib_DEBUG 1
#else
#define MetaLib_DEBUG 0
#endif


#ifndef MetaLib_STATIC_ASSERT
#ifdef __cplusplus
#define MetaLib_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
#define MetaLib_STATIC_ASSERT(cond, msg) typedef char STATIC_ASSERT_HELPER[(cond) ? 1 : -1]
#endif
#endif


#ifndef MetaLib_INLINE
#if (defined(__cplusplus) || _MSC_VER >= 1900)
#define MetaLib_INLINE inline
#else
#define MetaLib_INLINE __inline
#endif
#endif


#ifndef MetaLib_INLINE_URGE
#define MetaLib_INLINE_URGE __forceinline
#endif


#ifndef MetaLib_CONSTEXPR
#if (defined(__cplusplus) && _MSC_VER >= 1900)
#define MetaLib_CONSTEXPR constexpr
#else
#define MetaLib_CONSTEXPR
#endif
#endif


#ifndef MetaLib_ALIGNOF
#if (defined(__cplusplus) && _MSC_VER >= 1900)
#define MetaLib_ALIGNOF(a) alignof(a)
#else
#define MetaLib_ALIGNOF(a) __alignof(a)
#endif
#endif

#ifndef MetaLib_ALIGNAS
#if (defined(__cplusplus) && _MSC_VER >= 1900)
#define MetaLib_ALIGNAS(a) alignas(a)
#else
#define MetaLib_ALIGNAS(a) __declspec(align(a))
#endif
#endif


#ifndef MetaLib_CPP_EXPLICIT_CONVERSION
#if (defined(__cplusplus) && _MSC_VER >= 1800)
#define MetaLib_CPP_EXPLICIT_CONVERSION explicit
#else
#define MetaLib_CPP_EXPLICIT_CONVERSION
#endif
#endif


#ifndef MetaLib_FINAL
#if (defined(__cplusplus) && _MSC_VER >= 1700)
#define MetaLib_FINAL final
#else
#define MetaLib_FINAL sealed
#endif
#endif


#ifndef MetaLib_NOEXCEPT
#if (defined(__cplusplus) && _MSC_VER >= 1900)
#define MetaLib_HAS_NOEXCEPT 1
#define MetaLib_NOEXCEPT noexcept
#else
#define MetaLib_HAS_NOEXCEPT 0
#define MetaLib_NOEXCEPT
#endif
#endif


#ifndef MetaLib_DL_EXP
#define MetaLib_DL_EXP(...) __declspec(dllexport) __VA_ARGS__
#endif

#ifndef MetaLib_DL_IMP
#define MetaLib_DL_IMP(...) __declspec(dllimport) __VA_ARGS__
#endif


#ifndef MetaLib_HAS_VARIADIC_TEMPLATE
#if (defined(__cplusplus) && _MSC_VER >= 1800)
#define MetaLib_HAS_VARIADIC_TEMPLATE 1
#else
#define MetaLib_HAS_VARIADIC_TEMPLATE 0
#endif
#endif


#ifndef MetaLib_HAS_UNRESTRICTED_UNION
#if (defined(__cplusplus) && _MSC_VER >= 1900)
//VS2015 RC bug
#define MetaLib_HAS_UNRESTRICTED_UNION 0
#else
#define MetaLib_HAS_UNRESTRICTED_UNION 0
#endif
#endif


#ifndef MetaLib_HAS_DEFAULTED_DELETED_FUNCTIONS
#if (defined(__cplusplus) && _MSC_VER >= 1800)
#define MetaLib_HAS_DEFAULTED_DELETED_FUNCTIONS 1
#else
#define MetaLib_HAS_DEFAULTED_DELETED_FUNCTIONS 0
#endif
#endif


#ifndef MetaLib_HAS_RETURN_TYPE_DEDUCTION_FOR_NORMAL_FUNCTION
#if (defined(__cplusplus) && _MSC_VER >= 1900)
#define MetaLib_HAS_RETURN_TYPE_DEDUCTION_FOR_NORMAL_FUNCTION 1
#else
#define MetaLib_HAS_RETURN_TYPE_DEDUCTION_FOR_NORMAL_FUNCTION 0
#endif
#endif


#ifndef MetaLib_HAS_REF_QUALIFIERS
#if (defined(__cplusplus) && _MSC_VER >= 1900)
#define MetaLib_HAS_REF_QUALIFIERS 1
#else
#define MetaLib_HAS_REF_QUALIFIERS 0
#endif
#endif


#ifndef MetaLib_HAS_TEMPLATE_ALIAS
#if (defined(__cplusplus) && _MSC_VER >= 1800)
#define MetaLib_HAS_TEMPLATE_ALIAS 1
#else
#define MetaLib_HAS_TEMPLATE_ALIAS 0
#endif
#endif


#ifndef MetaLib_HAS_EXPRESSION_SFINAE
#if (defined(__cplusplus) && (_MSC_VER == 0))/* Expression SFINAE is still not fully supported in Visual Studio 2015 RC */
#define MetaLib_HAS_EXPRESSION_SFINAE 1
#else
#define MetaLib_HAS_EXPRESSION_SFINAE 0
#endif
#endif


#if (defined(__cplusplus) && _MSC_VER < 1900)

//Rename for VS2010

#ifndef is_nothrow_copy_constructible     
#define is_nothrow_copy_constructible      has_nothrow_copy_constructor   
#endif
#ifndef is_nothrow_default_constructible  
#define is_nothrow_default_constructible   has_nothrow_default_constructor
#endif
#ifndef is_trivially_copy_constructible   
#define is_trivially_copy_constructible    has_trivial_copy_constructor   
#endif
#ifndef is_trivially_default_constructible
#define is_trivially_default_constructible has_trivial_default_constructor
#endif
#ifndef is_trivially_destructible         
#define is_trivially_destructible          has_trivial_destructor         
#endif

#endif


#include <stddef.h>

#if _MSC_VER == 1600//TODO: and others
static MetaLib_CONSTEXPR size_t const MetaLib_MAX_ALIGN = MetaLib_ALIGNOF(long double);
#elif defined(__cplusplus)
#include <cstddef>
static MetaLib_CONSTEXPR size_t const MetaLib_MAX_ALIGN = MetaLib_ALIGNOF(std::max_align_t);
#else
#include <stddef.h>
static MetaLib_CONSTEXPR size_t const MetaLib_MAX_ALIGN = MetaLib_ALIGNOF(max_align_t);
#endif

typedef unsigned char      MetaLib_U8 ;
typedef signed char        MetaLib_S8 ;
typedef unsigned short     MetaLib_U16;
typedef signed short       MetaLib_S16;
typedef unsigned int       MetaLib_U32;
typedef signed int         MetaLib_S32;
typedef unsigned long long MetaLib_U64;
typedef signed long long   MetaLib_S64;
typedef float              MetaLib_F32;/* IEEE 754 Single-precision floating-point format */
typedef double             MetaLib_F64;/* IEEE 754 Double-precision floating-point format */

static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_NonABI_TypeName[] = "";

#define MetaLib_U8_TYPE_NAME  "U8"
#define MetaLib_S8_TYPE_NAME  "S8"
#define MetaLib_U16_TYPE_NAME "U16"
#define MetaLib_S16_TYPE_NAME "S16"
#define MetaLib_U32_TYPE_NAME "U32"
#define MetaLib_S32_TYPE_NAME "S32"
#define MetaLib_U64_TYPE_NAME "U64"
#define MetaLib_S64_TYPE_NAME "S64"
#define MetaLib_F32_TYPE_NAME "F32"
#define MetaLib_F64_TYPE_NAME "F64"
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_U8_TypeName [] = MetaLib_U8_TYPE_NAME ;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_S8_TypeName [] = MetaLib_S8_TYPE_NAME ;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_U16_TypeName[] = MetaLib_U16_TYPE_NAME;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_S16_TypeName[] = MetaLib_S16_TYPE_NAME;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_U32_TypeName[] = MetaLib_U32_TYPE_NAME;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_S32_TypeName[] = MetaLib_S32_TYPE_NAME;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_U64_TypeName[] = MetaLib_U64_TYPE_NAME;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_S64_TypeName[] = MetaLib_S64_TYPE_NAME;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_F32_TypeName[] = MetaLib_F32_TYPE_NAME;
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_F64_TypeName[] = MetaLib_F64_TYPE_NAME;

MetaLib_STATIC_ASSERT(sizeof(MetaLib_U8 ) == 1, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_S8 ) == 1, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_U16) == 2, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_S16) == 2, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_U32) == 4, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_S32) == 4, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_U64) == 8, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_S64) == 8, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_F32) == 4, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_F64) == 8, "Assertion Failure.");
#define MetaLib_U8_SIZE  1
#define MetaLib_S8_SIZE  1
#define MetaLib_U16_SIZE 2
#define MetaLib_S16_SIZE 2
#define MetaLib_U32_SIZE 4
#define MetaLib_S32_SIZE 4
#define MetaLib_U64_SIZE 8
#define MetaLib_S64_SIZE 8
#define MetaLib_F32_SIZE 4
#define MetaLib_F64_SIZE 8

#if defined(_M_AMD64)

typedef MetaLib_U64 MetaLib_USz ;
typedef MetaLib_S64 MetaLib_SSz ;
typedef MetaLib_U64 MetaLib_UOff;
typedef MetaLib_S64 MetaLib_SOff;
typedef MetaLib_U64 MetaLib_UPtr;
typedef MetaLib_S64 MetaLib_SPtr;

#define MetaLib_USz_TYPE_NAME  MetaLib_U64_TYPE_NAME
#define MetaLib_SSz_TYPE_NAME  MetaLib_S64_TYPE_NAME
#define MetaLib_UOff_TYPE_NAME MetaLib_U64_TYPE_NAME
#define MetaLib_SOff_TYPE_NAME MetaLib_S64_TYPE_NAME
#define MetaLib_UPtr_TYPE_NAME MetaLib_U64_TYPE_NAME
#define MetaLib_SPtr_TYPE_NAME MetaLib_S64_TYPE_NAME
#define MetaLib_USz_TypeName  MetaLib_U64_TypeName
#define MetaLib_SSz_TypeName  MetaLib_S64_TypeName
#define MetaLib_UOff_TypeName MetaLib_U64_TypeName
#define MetaLib_SOff_TypeName MetaLib_S64_TypeName
#define MetaLib_UPtr_TypeName MetaLib_U64_TypeName
#define MetaLib_SPtr_TypeName MetaLib_S64_TypeName

MetaLib_STATIC_ASSERT(sizeof(MetaLib_USz ) == 8, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_SSz ) == 8, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_UOff) == 8, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_SOff) == 8, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_UPtr) == 8, "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_SPtr) == 8, "Assertion Failure.");
#define MetaLib_USz_SIZE  8
#define MetaLib_SSz_SIZE  8
#define MetaLib_UOff_SIZE 8
#define MetaLib_SOff_SIZE 8
#define MetaLib_UPtr_SIZE 8
#define MetaLib_SPtr_SIZE 8

#elif defined(_M_IX86)/* TODO */

#elif defined(_M_ARM)/* TODO */

#endif


/* Atomic */
//TODO: aligned types cannot be used as function parameters
//TODO: is it possible to static initialize atomic types?
typedef struct { MetaLib_ALIGNAS(1 ) MetaLib_U8 bytes[1 ]; } MetaLib_A8  ;
typedef struct { MetaLib_ALIGNAS(2 ) MetaLib_U8 bytes[2 ]; } MetaLib_A16 ;
typedef struct { MetaLib_ALIGNAS(4 ) MetaLib_U8 bytes[4 ]; } MetaLib_A32 ;
typedef struct { MetaLib_ALIGNAS(8 ) MetaLib_U8 bytes[8 ]; } MetaLib_A64 ;
typedef struct { MetaLib_ALIGNAS(16) MetaLib_U8 bytes[16]; } MetaLib_A128;
MetaLib_STATIC_ASSERT(sizeof(MetaLib_A8  ) == 1 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_A16 ) == 2 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_A32 ) == 4 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_A64 ) == 8 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_A128) == 16, "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_A8  ) == 1 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_A16 ) == 2 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_A32 ) == 4 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_A64 ) == 8 , "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_A128) == 16, "Assertion Failure.");
#define MetaLib_A8_SIZE   1
#define MetaLib_A16_SIZE  2
#define MetaLib_A32_SIZE  4
#define MetaLib_A64_SIZE  8
#define MetaLib_A128_SIZE 16

#define MetaLib_ATOM_EXTRACT(a, T) (*(T *)((a).bytes))
#define MetaLib_ATOM_PACK(a, T, t) (*(T *)((a).bytes) = (t))

/* These lists can be used to choose a type/function based on preprocess-time value using BOOST_PP_SEQ_ELEM. */
/* However MetaLib itself is not dependent on boost. */
/* e.g.
    #define ATOM_DUAL_POINTER_SIZE BOOST_PP_ADD(MetaLib_UPtr_SIZE, MetaLib_UPtr_SIZE)
    #define ATOM_DUAL_POINTER(...) BOOST_PP_SEQ_ELEM(ATOM_DUAL_POINTER_SIZE, MetaLib_ATOM_SELECTION_SEQ(__VA_ARGS__))
    typedef struct { void * p; void * q; } T;
    MetaLib_STATIC_ASSERT(sizeof(ATOM_DUAL_POINTER()) >= sizeof(T), "");
    ATOM_DUAL_POINTER() a;
    ATOM_DUAL_POINTER() v;
    T t = { 0, 0, };
    MetaLib_ATOM_PACK(v, T, t);
    ATOM_DUAL_POINTER(_store)(&a, v);
    v = ATOM_DUAL_POINTER(_xchg)(&a, v);
*/
#define MetaLib_ATOM_SELECTION_SEQ(...) /*  0 */(MetaLib_CONCAT(MetaLib_A8  , __VA_ARGS__))\
                                        /*  1 */(MetaLib_CONCAT(MetaLib_A8  , __VA_ARGS__))\
                                        /*  2 */(MetaLib_CONCAT(MetaLib_A16 , __VA_ARGS__))\
                                        /*  3 */(MetaLib_CONCAT(MetaLib_A32 , __VA_ARGS__))\
                                        /*  4 */(MetaLib_CONCAT(MetaLib_A32 , __VA_ARGS__))\
                                        /*  5 */(MetaLib_CONCAT(MetaLib_A64 , __VA_ARGS__))\
                                        /*  6 */(MetaLib_CONCAT(MetaLib_A64 , __VA_ARGS__))\
                                        /*  7 */(MetaLib_CONCAT(MetaLib_A64 , __VA_ARGS__))\
                                        /*  8 */(MetaLib_CONCAT(MetaLib_A64 , __VA_ARGS__))\
                                        /*  9 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))\
                                        /* 10 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))\
                                        /* 11 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))\
                                        /* 12 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))\
                                        /* 13 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))\
                                        /* 14 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))\
                                        /* 15 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))\
                                        /* 16 */(MetaLib_CONCAT(MetaLib_A128, __VA_ARGS__))


#include <intrin.h>
#include <immintrin.h>


#if (defined(_M_AMD64) || defined(_M_IX86))

static MetaLib_INLINE void MetaLib_Intrin_pause()
{
    _mm_pause();
}

static MetaLib_INLINE void MetaLib_Intrin_nop()
{
    __nop();
}

static MetaLib_INLINE void MetaLib_Intrin_DebugBreak()
{
    __debugbreak();
}

#if MetaLib_DEBUG
#define MetaLib_ASSERT(condition) ((condition) ? MetaLib_Intrin_nop() : MetaLib_Intrin_DebugBreak())
#else
#define MetaLib_ASSERT(condition)
#endif

#ifdef _WIN32
#if MetaLib_DEBUG
#ifdef __cplusplus
extern "C"
#endif
__declspec(dllimport)
int __stdcall IsDebuggerPresent(void);//TODO: non-VC
#define MetaLib_BreakIfDebug() \
{\
    static int volatile skip = 0;\
    if ((!skip) && IsDebuggerPresent())\
        __debugbreak();\
    else\
        MetaLib_Intrin_nop();\
}
#else
#define MetaLib_BreakIfDebug() \
    MetaLib_Intrin_nop()
#endif
#else
#error "This platform is not supported, yet."
#endif


static MetaLib_INLINE void MetaLib_Intrin_CompilerBarrier()
{
    _ReadWriteBarrier();
}

static MetaLib_INLINE void MetaLib_Intrin_CPUBarrier()
{
    _mm_mfence();
}

#elif defined(_M_ARM)/* TODO */

#endif


static MetaLib_INLINE void MetaLib_A8_init(MetaLib_A8 volatile * p, MetaLib_A8 v)
{
    *(char volatile *)(p->bytes) = MetaLib_ATOM_EXTRACT(v, char);
    MetaLib_Intrin_CPUBarrier();
    MetaLib_Intrin_CompilerBarrier();
}
static MetaLib_INLINE void MetaLib_A64_init(MetaLib_A64 volatile * p, MetaLib_A64 v)
{
    *(long long volatile *)(p->bytes) = MetaLib_ATOM_EXTRACT(v, long long);
    MetaLib_Intrin_CPUBarrier();
    MetaLib_Intrin_CompilerBarrier();
}

static MetaLib_INLINE void MetaLib_A8_store(MetaLib_A8 volatile * p, MetaLib_A8 v)
{
    _InterlockedExchange8((char volatile *)(p->bytes), MetaLib_ATOM_EXTRACT(v, char));
    MetaLib_Intrin_CompilerBarrier();
}
static MetaLib_INLINE void MetaLib_A64_store(MetaLib_A64 volatile * p, MetaLib_A64 v)
{
    _InterlockedExchange64((long long volatile *)(p->bytes), MetaLib_ATOM_EXTRACT(v, long long));
    MetaLib_Intrin_CompilerBarrier();
}

static MetaLib_INLINE MetaLib_A8 MetaLib_A8_load(MetaLib_A8 const volatile * p)
{
    MetaLib_A8 r;
    char orig = *(char const volatile *)(p->bytes);/* TODO: examine memory ordering */
    /* MetaLib_Intrin_CPUBarrier(); */
    MetaLib_Intrin_CompilerBarrier();
    MetaLib_ATOM_PACK(r, char, orig);
    return r;
}
static MetaLib_INLINE MetaLib_A64 MetaLib_A64_load(MetaLib_A64 const volatile * p)
{
    MetaLib_A64 r;
    long long orig = *(long long const volatile *)(p->bytes);/* TODO: examine memory ordering */
    /* MetaLib_Intrin_CPUBarrier(); */
    MetaLib_Intrin_CompilerBarrier();
    MetaLib_ATOM_PACK(r, long long, orig);
    return r;
}

static MetaLib_INLINE MetaLib_A8 MetaLib_A8_xchg(MetaLib_A8 volatile * p, MetaLib_A8 v)
{
    MetaLib_A8 r;
    char orig = _InterlockedExchange8((char volatile *)(p->bytes), MetaLib_ATOM_EXTRACT(v, char));
    MetaLib_Intrin_CompilerBarrier();
    MetaLib_ATOM_PACK(r, char, orig);
    return r;
}
static MetaLib_INLINE MetaLib_A64 MetaLib_A64_xchg(MetaLib_A64 volatile * p, MetaLib_A64 v)
{
    MetaLib_A64 r;
    long long orig = _InterlockedExchange64((long long volatile *)(p->bytes), MetaLib_ATOM_EXTRACT(v, long long));
    MetaLib_Intrin_CompilerBarrier();
    MetaLib_ATOM_PACK(r, long long, orig);
    return r;
}

/* success: return non-0 */
/* failure: return 0 */
/* weak */
static MetaLib_INLINE MetaLib_U8 MetaLib_A8_cmpxchg(MetaLib_A8 volatile * p, MetaLib_A8 * p_expected, MetaLib_A8 exchange)
{
    MetaLib_U8 r = 1;
    char expected = MetaLib_ATOM_EXTRACT(*p_expected, char);
    char orig = _InterlockedCompareExchange8((char volatile *)(p->bytes), MetaLib_ATOM_EXTRACT(exchange, char), expected);
    if (orig != expected)
    {
        MetaLib_ATOM_PACK(*p_expected, char, orig);
        r = 0;
    }
    MetaLib_Intrin_CompilerBarrier();
    return r;
}
static MetaLib_INLINE MetaLib_U8 MetaLib_A64_cmpxchg(MetaLib_A64 volatile * p, MetaLib_A64 * p_expected, MetaLib_A64 exchange)
{
    MetaLib_U8 r = 1;
    long long expected = MetaLib_ATOM_EXTRACT(*p_expected, long long);
    long long orig = _InterlockedCompareExchange64((long long volatile *)(p->bytes), MetaLib_ATOM_EXTRACT(exchange, long long), expected);
    if (orig != expected)
    {
        MetaLib_ATOM_PACK(*p_expected, long long, orig);
        r = 0;
    }
    MetaLib_Intrin_CompilerBarrier();
    return r;
}

/* TODO: other primitives such as 'xadd' */


/* TODO: other sizes */


#if defined(_M_AMD64)


#if _MSC_VER >= 1700
#define MetaLib_CPURandom_RETRY_LIMIT 10
static MetaLib_INLINE MetaLib_U8 MetaLib_Intrin_CPURandom64(MetaLib_U64 * prand)
{
    for (int i = 0; i < MetaLib_CPURandom_RETRY_LIMIT; ++i)
    {
        if (_rdrand64_step(prand)) return 1;
    }
    return 0;
}
#endif


#if ((MetaLib_USz_SIZE == 8) && (MetaLib_UOff_SIZE == 8) && (MetaLib_UPtr_SIZE == 8))
#define MetaLib_ASz(...)  MetaLib_CONCAT(MetaLib_A64, __VA_ARGS__)
#define MetaLib_AOff(...) MetaLib_CONCAT(MetaLib_A64, __VA_ARGS__)
#define MetaLib_APtr(...) MetaLib_CONCAT(MetaLib_A64, __VA_ARGS__)
MetaLib_STATIC_ASSERT(sizeof(MetaLib_ASz() ) == sizeof(MetaLib_USz ), "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_AOff()) == sizeof(MetaLib_UOff), "Assertion Failure.");
MetaLib_STATIC_ASSERT(sizeof(MetaLib_APtr()) == sizeof(MetaLib_UPtr), "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_ASz() ) >= MetaLib_ALIGNOF(MetaLib_USz ), "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_AOff()) >= MetaLib_ALIGNOF(MetaLib_UOff), "Assertion Failure.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_APtr()) >= MetaLib_ALIGNOF(MetaLib_UPtr), "Assertion Failure.");
#else
#error "Size assertion failed."
#endif


#elif defined(_M_IX86)/* TODO */

#elif defined(_M_ARM)/* TODO */

#endif

#else

#error This compiler has not been supported, yet.

#endif



#define MetaLib_void_TYPE_NAME "v"
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_void_TypeName[] = MetaLib_void_TYPE_NAME;

struct MetaLib_Void
{
    MetaLib_U8 _u;//place holder
};
#define MetaLib_Void_TYPE_NAME "V"
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_Void_TypeName[] = MetaLib_Void_TYPE_NAME;
MetaLib_STATIC_ASSERT(sizeof(MetaLib_Void) == 1, "Assertion Failure.");
#define MetaLib_Void_SIZE  1

struct MetaLib_B
{
    MetaLib_U8 value_;
#ifdef __cplusplus
#if MetaLib_HAS_DEFAULTED_DELETED_FUNCTIONS
    MetaLib_B() = default;
    MetaLib_B(bool v) : value_(v ? 1 : 0)
    {}
#endif
    bool to_bool() const
    {
        return (value_ != 0);
    }
    MetaLib_CPP_EXPLICIT_CONVERSION operator bool() const
    {
        return to_bool();
    }
    MetaLib_B & operator=(bool v)
    {
        value_ = v ? 1 : 0;
        return *this;
    }
#endif
};
#ifdef __cplusplus
#include <type_traits>
MetaLib_STATIC_ASSERT(std::is_pod<MetaLib_B>::value, "Assertion Failure.");
#endif
#define MetaLib_B_TYPE_NAME "B"
static MetaLib_CONSTEXPR MetaLib_U8 const MetaLib_B_TypeName[] = MetaLib_B_TYPE_NAME;
MetaLib_STATIC_ASSERT(sizeof(MetaLib_B) == 1, "Assertion Failure.");
#define MetaLib_B_SIZE  1


#ifdef MetaLib_EXPORTING
#define MetaLib_EXP(...) MetaLib_DL_EXP(__VA_ARGS__)
#else
#define MetaLib_EXP(...) MetaLib_DL_IMP(__VA_ARGS__)
#endif


#ifdef __cplusplus


#include <utility>
#include <algorithm>
#include <string>
#include <cstring>
#include <utility>
#include <exception>


namespace MetaLib
{
    template <typename>
    struct OKType : std::integral_constant<bool, true>
    {};
}


namespace MetaLib
{
    template <typename F>
    auto catchIfDebug(F && f) -> decltype(std::forward<F>(f)())
    {
#if MetaLib_DEBUG
        try
        {
#endif
            return std::forward<F>(f)();
#if MetaLib_DEBUG
        }
        catch (...)
        {
            MetaLib_BreakIfDebug();
            auto e = std::current_exception();//TODO: pre-C++11
            //TODO: Is any info can be retrieved when debugging?
            std::rethrow_exception(e);
        }
#endif
    }
}


namespace MetaLibPortPrivate
{


struct PlaceHolder {};
class StrRef
{
protected:
    MetaLib_USz length;
    MetaLib_U8 const * pointer;
public:
    MetaLib_CONSTEXPR StrRef()
        :
        length(0),
        pointer(nullptr)
    {}
    MetaLib_CONSTEXPR StrRef(MetaLib_USz length, MetaLib_U8 const * pointer)
        :
        length(length),
        pointer(pointer)
    {}
    //template <typename C>
    //StrRef(C const * pointer, typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), PlaceHolder>::type = PlaceHolder())
    //    :
    //    length(std::strlen(reinterpret_cast<char const *>(pointer))),
    //    pointer(std::strlen(reinterpret_cast<MetaLib_U8 const *>(pointer)))
    //{}
    template <typename C, MetaLib_USz N>
    StrRef(C const (&array)[N], typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), PlaceHolder>::type = PlaceHolder())
        :
        length(N - 1),
        pointer(reinterpret_cast<MetaLib_U8 const *>(array))
    {}
    template <typename C>
    StrRef(MetaLib_USz length, C const * pointer, typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), PlaceHolder>::type = PlaceHolder())
        :
        length(length),
        pointer(reinterpret_cast<MetaLib_U8 const *>(pointer))
    {}
    template <typename C, typename T, typename A>//look out for object life time
    StrRef(std::basic_string<C, T, A> const & string, typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), PlaceHolder>::type = PlaceHolder())
        :
        length(string.length()),
        pointer(reinterpret_cast<MetaLib_U8 const *>(&(string[0])))
    {}
#if MetaLib_HAS_EXPRESSION_SFINAE
    template <typename S>//type with length() and data(); look out for object life time
    StrRef(S const & string, typename std::enable_if<
        std::is_integral<typename std::decay<decltype(*((*(static_cast<S const *>(nullptr))).data()))>::type>::value &&
        (sizeof(typename std::decay<decltype(*((*(static_cast<S const *>(nullptr))).data()))>::type) == 1) &&
        std::is_integral<decltype((*(static_cast<S const *>(nullptr))).length())>::value,
        PlaceHolder>::type = PlaceHolder())
        :
        length(string.length()),
        pointer(reinterpret_cast<MetaLib_U8 const *>(&(*(string.data()))))
    {}
#endif
public:
    //length
    MetaLib_CONSTEXPR MetaLib_USz len() const
    {
        return length;
    }
    //pointer
    MetaLib_CONSTEXPR MetaLib_U8 const * p() const
    {
        return pointer;
    }
    template <typename C>
    typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), C const *>::type
        p() const
    {
        return reinterpret_cast<C const *>(pointer);
    }
    template <typename C>
    typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), std::basic_string<C>>::type
        toStdString() const
    {
        return std::basic_string<C>(p<C>(), len());
    }
};


}


namespace MetaLib
{


//typedef std::pair<MetaLib_USz, MetaLib_U8 const *> TypenameString;
using MetaLibPortPrivate::StrRef;

template <bool IsABI>
struct ABIType_IsABI
{
    static MetaLib_CONSTEXPR bool const Is_ABI = IsABI;
};
template <typename T, typename = void>
struct ABIType : ABIType_IsABI<false>
{
    static MetaLib_CONSTEXPR StrRef getName() { return StrRef(0, MetaLib_NonABI_TypeName); }
};


#define MetaLib_ABI_CUSTOM_TYPE_PREFIX "!"//TODO


//TODO
#define MetaLib_ABI_FULL_TYPE_NAME(name_string, version_string) name_string "{C" MetaLib_ABI_COMPILER_DOMAIN "}" "{M" version_string "}"


#if MetaLib_HAS_EXPRESSION_SFINAE

template <typename T> void MetaLib_ABI_getABITypeADL(T *);
template <typename T>
struct ABIType<T, typename std::enable_if<std::is_class<decltype(MetaLib_ABI_getABITypeADL(static_cast<T *>(nullptr)))>::value>::type>
    : decltype(MetaLib_ABI_getABITypeADL(static_cast<T *>(nullptr)))
{};

#endif


template <> struct ABIType<void, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_void_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_void_TypeName) - 1, MetaLib_void_TypeName); } };

template <> struct ABIType<MetaLib_Void, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_Void_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_Void_TypeName) - 1, MetaLib_Void_TypeName); } };
template <> struct ABIType<MetaLib_B, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_B_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_B_TypeName) - 1, MetaLib_B_TypeName); } };

template <> struct ABIType<MetaLib_U8 , void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_U8_TypeName ) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_U8_TypeName ) - 1, MetaLib_U8_TypeName ); } };
template <> struct ABIType<MetaLib_S8 , void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_S8_TypeName ) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_S8_TypeName ) - 1, MetaLib_S8_TypeName ); } };
template <> struct ABIType<MetaLib_U16, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_U16_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_U16_TypeName) - 1, MetaLib_U16_TypeName); } };
template <> struct ABIType<MetaLib_S16, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_S16_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_S16_TypeName) - 1, MetaLib_S16_TypeName); } };
template <> struct ABIType<MetaLib_U32, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_U32_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_U32_TypeName) - 1, MetaLib_U32_TypeName); } };
template <> struct ABIType<MetaLib_S32, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_S32_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_S32_TypeName) - 1, MetaLib_S32_TypeName); } };
template <> struct ABIType<MetaLib_U64, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_U64_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_U64_TypeName) - 1, MetaLib_U64_TypeName); } };
template <> struct ABIType<MetaLib_S64, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_S64_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_S64_TypeName) - 1, MetaLib_S64_TypeName); } };
template <> struct ABIType<MetaLib_F32, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_F32_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_F32_TypeName) - 1, MetaLib_F32_TypeName); } };
template <> struct ABIType<MetaLib_F64, void> : ABIType_IsABI<true> { static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = sizeof(MetaLib_F64_TypeName) - 1; static MetaLib_CONSTEXPR StrRef getName() { return StrRef(sizeof(MetaLib_F64_TypeName) - 1, MetaLib_F64_TypeName); } };
/* TODO: cv types, pointer types, array types, function types */
/* TODO: add non-ABI type info in compiler domain */
template <typename T>
struct ABIType<T [], typename std::enable_if<ABIType<T>::Is_ABI>::type> : ABIType_IsABI<true>
{
    static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = ABIType<T>::Name_Length + 2;
    static StrRef getName()/* TODO: make it constexpr */
    {
        struct NameMaker
        {
            static MetaLib_U8 const * make()
            {
                static MetaLib_U8 Name[Name_Length + 1];
                auto T_name = ABIType<T>::getName();
                std::copy(T_name.p(), T_name.p() + T_name.len(), Name);
                Name[T_name.len()    ] = '[';
                Name[T_name.len() + 1] = ']';
                Name[T_name.len() + 2] = 0;
                return Name;
            }
        };
        /*
#ifdef _MSC_VER
#if _MSC_VER < 1900
        struct __declspec(deprecated("Microsoft Visual C++ compilers before Visual Studio 2015 do not guarantee thread safety on static variable initialization.")) Warning {};
        Warning();
#endif
#endif
        */
        static MetaLib_U8 const * Name_Ptr = NameMaker::make();
        return StrRef(Name_Length, Name_Ptr);
    }
};
template <typename T>
struct ABIType<T *, typename std::enable_if<ABIType<T>::Is_ABI>::type> : ABIType_IsABI<true>
{
    static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = ABIType<T>::Name_Length + 1;
    static StrRef getName()/* TODO: make it constexpr */
    {
        struct NameMaker
        {
            static MetaLib_U8 const * make()
            {
                static MetaLib_U8 Name[Name_Length + 1];
                auto T_name = ABIType<T>::getName();
                std::copy(T_name.p(), T_name.p() + T_name.len(), Name);
                Name[T_name.len()    ] = '*';
                Name[T_name.len() + 1] = 0;
                return Name;
            }
        };
        /*
#ifdef _MSC_VER
#if _MSC_VER < 1900
        struct __declspec(deprecated("Microsoft Visual C++ compilers before Visual Studio 2015 do not guarantee thread safety on static variable initialization.")) Warning {};
        Warning();
#endif
#endif
        */
        static MetaLib_U8 const * Name_Ptr = NameMaker::make();
        return StrRef(Name_Length, Name_Ptr);
    }
};
template <typename T>
struct ABIType<T const, typename std::enable_if<ABIType<T>::Is_ABI>::type> : ABIType_IsABI<true>
{
    static MetaLib_CONSTEXPR MetaLib_USz const Name_Length = ABIType<T>::Name_Length + 2;
    static StrRef getName()/* TODO: make it constexpr */
    {
        struct NameMaker
        {
            static MetaLib_U8 const * make()
            {
                static MetaLib_U8 Name[Name_Length + 1];
                auto T_name = ABIType<T>::getName();
                std::copy(T_name.p(), T_name.p() + T_name.len(), Name);
                Name[T_name.len()    ] = '#';
                Name[T_name.len() + 1] = 'c';
                Name[T_name.len() + 2] = 0;
                return Name;
            }
        };
        /*
#ifdef _MSC_VER
#if _MSC_VER < 1900
        struct __declspec(deprecated("Microsoft Visual C++ compilers before Visual Studio 2015 do not guarantee thread safety on static variable initialization.")) Warning {};
        Warning();
#endif
#endif
        */
        static MetaLib_U8 const * Name_Ptr = NameMaker::make();
        return StrRef(Name_Length, Name_Ptr);
    }
};


/* TODO:
template <typename A, typename T> R Atom_operate(A volatile & a, T t)
*/

/* TODO: POD
template <typename T> struct Atom { Axx _storage; R operate(); };
*/


}


namespace MetaLib
{


typedef ::MetaLib_Void Void;
typedef ::MetaLib_B B;

typedef ::MetaLib_U8   U8  ;
typedef ::MetaLib_S8   S8  ;
typedef ::MetaLib_U16  U16 ;
typedef ::MetaLib_S16  S16 ;
typedef ::MetaLib_U32  U32 ;
typedef ::MetaLib_S32  S32 ;
typedef ::MetaLib_U64  U64 ;
typedef ::MetaLib_S64  S64 ;
typedef ::MetaLib_F32  F32 ;
typedef ::MetaLib_F64  F64 ;

typedef ::MetaLib_USz  USz ;
typedef ::MetaLib_SSz  SSz ;
typedef ::MetaLib_UOff UOff;
typedef ::MetaLib_SOff SOff;
typedef ::MetaLib_UPtr UPtr;
typedef ::MetaLib_SPtr SPtr;

typedef ::MetaLib_A8   A8  ;
typedef ::MetaLib_A16  A16 ;
typedef ::MetaLib_A32  A32 ;
typedef ::MetaLib_A64  A64 ;
typedef ::MetaLib_A128 A128;

typedef ::MetaLib_ASz()  ASz ;
typedef ::MetaLib_AOff() AOff;
typedef ::MetaLib_APtr() APtr;


}


#endif


#endif
