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


#ifndef MetaLibInt_H_
#define MetaLibInt_H_


#include "MetaLibMetaType.h"
#include "MetaLibObject.h"
#include "MetaLibPort.h"
#include <limits>
#include <type_traits>
#include <utility>
#include <stdexcept>


namespace MetaLibIntPrivate
{
;

//exclude floating types and bool
template <typename T>
struct S :
    std::integral_constant<bool, std::is_integral<T>::value && std::is_signed<T>::value>
{};
template <typename T>
struct U :
    std::integral_constant<bool, std::is_integral<T>::value && std::is_unsigned<T>::value && !std::is_same<T, bool>::value>
{};

/***********************************************************************************************
ISO/IEC 14882:2003

5.2.9/2 (Static cast)
An expression e can be explicitly converted to a type T using a static_cast of the form
static_cast<T>(e) if the declaration "T t(e);" is well-formed, for some invented temporary
variable t (8.5). The effect of such an explicit conversion is the same as performing the
declaration and initialization and then using the temporary variable as the result of the
conversion.
...

4.7/2 (Integral conversions)
If the destination type is unsigned, the resulting value is the least unsigned integer
congruent to the source integer (modulo 2n where n is the number of bits used to represent
the unsigned type).
...

4.7/3 (Integral conversions)
If the destination type is signed, the value is unchanged if it can be represented in the
destination type (and bit-field width); otherwise, the value is implementation-defined.
***********************************************************************************************/

//https://connect.microsoft.com/VisualStudio/feedback/details/811603
template <typename T1, typename T2> struct UU : std::integral_constant<bool, U<T1>::value && U<T2>::value> {};
template <typename T1, typename T2> struct US : std::integral_constant<bool, U<T1>::value && S<T2>::value> {};
template <typename T1, typename T2> struct SU : std::integral_constant<bool, S<T1>::value && U<T2>::value> {};
template <typename T1, typename T2> struct SS : std::integral_constant<bool, S<T1>::value && S<T2>::value> {};

template <typename To, typename From>
MetaLib_CONSTEXPR
typename std::enable_if<UU<From, To>::value, To>::type
castInt(From v)
{
    return v > std::numeric_limits<To>::max() ?
        throw std::overflow_error("integer cast overflow") :
        static_cast<To>(v);
}

template <typename To, typename From>
MetaLib_CONSTEXPR
typename std::enable_if<US<From, To>::value, To>::type
castInt(From v)
{
    return v > static_cast<typename std::make_unsigned<To>::type>(std::numeric_limits<To>::max()) ?
        throw std::overflow_error("integer cast overflow") :
        static_cast<To>(v);
}

template <typename To, typename From>
MetaLib_CONSTEXPR
typename std::enable_if<SU<From, To>::value, To>::type
castInt(From v)
{
    return v < 0 ?
        throw std::underflow_error("integer cast underflow") :
        (static_cast<typename std::make_unsigned<From>::type>(v) > std::numeric_limits<To>::max() ?
            throw std::overflow_error("integer cast overflow") :
            static_cast<To>(v)
            );
}

template <typename To, typename From>
MetaLib_CONSTEXPR
typename std::enable_if<SS<From, To>::value, To>::type
castInt(From v)
{
    return v < std::numeric_limits<To>::min() ?
        throw std::underflow_error("integer cast underflow") :
        (v > std::numeric_limits<To>::max() ?
            throw std::overflow_error("integer cast overflow") :
            static_cast<To>(v)
            );
}


template <typename A, typename B>
MetaLib_CONSTEXPR
typename std::enable_if<UU<A, B>::value, bool>::type
cmpIntLess(A a, B b)
{
    return a < b;
}

template <typename A, typename B>
MetaLib_CONSTEXPR
typename std::enable_if<SS<A, B>::value, bool>::type
cmpIntLess(A a, B b)
{
    return a < b;
}

template <typename A, typename B>
MetaLib_CONSTEXPR
typename std::enable_if<US<A, B>::value, bool>::type
cmpIntLess(A a, B b)
{
    return b >= 0 && a < static_cast<typename std::make_unsigned<B>::type>(b);
}

template <typename A, typename B>
MetaLib_CONSTEXPR
typename std::enable_if<SU<A, B>::value, bool>::type
cmpIntLess(A a, B b)
{
    return a < 0 || static_cast<typename std::make_unsigned<A>::type>(a) < b;
}

template <typename A, typename B>
MetaLib_CONSTEXPR
bool cmpIntGreater(A a, B b)
{
    return cmpIntLess(b, a);
}

template <typename A, typename B>
MetaLib_CONSTEXPR
bool cmpIntLessEqual(A a, B b)
{
    return !cmpIntLess(b, a);
}

template <typename A, typename B>
MetaLib_CONSTEXPR
bool cmpIntGreaterEqual(A a, B b)
{
    return !cmpIntLess(a, b);
}


//https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior
template <typename From>
MetaLib_CONSTEXPR
typename std::enable_if<std::is_unsigned<From>::value, typename std::make_signed<From>::type>::type
decode2sComplement(From x)
{
    typedef typename std::make_signed<From>::type To;
    typedef std::numeric_limits<To> NL;
    return
        x <= static_cast<From>(NL::max()) ?
        static_cast<To>(x) :
        (
            x >= static_cast<From>(NL::min()) ?
            static_cast<To>(x - NL::min()) + NL::min() :
            throw x
            );
};

;
}


namespace MetaLib
{
    using MetaLibIntPrivate::castInt;
    using MetaLibIntPrivate::cmpIntLess;
    using MetaLibIntPrivate::cmpIntLessEqual;
    using MetaLibIntPrivate::cmpIntGreater;
    using MetaLibIntPrivate::cmpIntGreaterEqual;
    using MetaLibIntPrivate::decode2sComplement;
}


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Type *) MetaLib_IntType_getShared();


namespace MetaLibIntPrivate
{
;

//TODO: should digits use all the bits?

struct IntHead
{
    MetaLib_USz * digits;//little endian, digits[abs(length) - 1] is not 0 (if exists), digits[abs(length)] is 0
    MetaLib_SSz length;//negative for negative Int, 0 for Int 0
};

class IntAccess
{
    template <typename> friend class IntTypeImpl;
    MetaLib_USz * digits;
    union
    {
        MetaLib_SSz length;
        MetaLib_SSz value;//valid when digits==nullptr
    };
    //static MetaLib_CONSTEXPR MetaLib_SSz const Invalid_Value = std::numeric_limits<MetaLib_SSz>::min();
public:
    IntAccess()
    {
        clear();
    }
    void clear()
    {
        digits = nullptr;
        value = 0;//TODO
        //value = Invalid_Value;
    }
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type *)
    {
        auto h = reinterpret_cast<IntHead *>(ptr);
        length = h->length;
        digits = h->digits;
    }
    template <typename Obj>
    //no rvalue reference and is Obj type
    typename std::enable_if<!std::is_rvalue_reference<Obj &&>::value && ::MetaLib::IsObjType<typename std::decay<Obj>::type>::value>::type
        bind(Obj && obj)
    {
        //TODO
        //MetaLib_STATIC_ASSERT(Invalid_Value < std::decay<Obj>::type::Int_Min, "Obj::Int_Min out of range.");
        digits = nullptr;
        value = obj.getInt();//TODO: assert
    }
public:
    template <typename Int>
    typename std::enable_if<std::is_integral<Int>::value, Int>::type toInt() const
    {
        MetaLib_STATIC_ASSERT(sizeof(MetaLib_USz) >= sizeof(Int), "Not implemented (yet).");

        if (digits == nullptr)
            return ::MetaLib::castInt<Int>(value);

        if (length == 0)
            return 0;

        auto r = digits[0];
        if (length == 1)
            return ::MetaLib::castInt<Int>(r);

        auto is_signed = std::is_signed<Int>::value;
        if (length == -1 && is_signed)
        {
            MetaLib_USz max_abs = 0 - static_cast<MetaLib_USz>(std::numeric_limits<Int>::min());
            if (r <= max_abs)
            {
                auto v = ::MetaLib::decode2sComplement(0 - r);
                return static_cast<Int>(v);//TODO
            }
        }

        throw 1;//TODO
    }
    std::string repr() const
    {
        if (digits == nullptr)
            return std::to_string(value);

        if (length == 0)
            return "0";

        if (length == 1)
            return std::to_string(digits[0]);

        if (length == -1)
            return "-" + std::to_string(digits[0]);

        //TODO
        throw 1;//not implemented yet
    }
    MetaLib_F64 toF64() const
    {
        if (digits == nullptr)
            return MetaLib_F64(value);

        if (length == 0)
            return 0.0;

        if (length == 1)
            return MetaLib_F64(digits[0]);

        if (length == -1)
            return -MetaLib_F64(digits[0]);

        //TODO
        throw 1;//not implemented yet
    }
};

template <typename T = void>//static data member of a class template can be defined in header file
class IntTypeImpl//not inheritable
    :
    private ::MetaLib::EmptyType,
    public ::MetaLib::ObjectCreatingMixin<IntTypeImpl<T>, IntAccess, false>,
    public ::MetaLib::ObjectExposingMixin<IntTypeImpl<T>, IntAccess>
{
private:
    typedef ::MetaLib::OxAccess<::MetaLib::O, IntAccess> OA;
private:
    IntTypeImpl()
    {
        //MetaLib_Type::p_call = &call;//TODO: methods of Int
    }
    friend MetaLib_Type * ::MetaLib_IntType_getShared();
    static MetaLib_Type * const Shared_Type;
public:
    static IntTypeImpl Static;
private:
    struct IntData
    {
        IntHead head;
        MetaLib_USz digits[1];
    };
private:
    std::pair<size_t, size_t> allocForLength(IntAccess * p_access, MetaLib_SSz length) MetaLib_NOEXCEPT
    {
        p_access->length = length;
        if (length < 0)
            length = -length;
        return std::make_pair(sizeof(IntData) + length * sizeof(MetaLib_USz), std::alignment_of<IntData>::value);
    }
    template <typename Int>
    MetaLib_SSz getLength(Int v)
    {
        MetaLib_STATIC_ASSERT(sizeof(MetaLib_USz) >= sizeof(Int), "Not implemented (yet).");
        if (v == 0)
            return 0;
        if (v > 0)
            return 1;
        if (v < 0)
            return -1;
        return 0;
    }
public:
    template <typename Int>
    typename std::enable_if<std::is_integral<Int>::value, std::pair<size_t, size_t>>::type
        alloc(IntAccess * p_access, Int v) MetaLib_NOEXCEPT
    {
        return allocForLength(p_access, getLength(v));
    }
private:
    MetaLib_Type * initData(IntAccess * p_access, MetaLib_U8 * ptr)
    {
        auto d = new(ptr) IntData;
        auto p = d->digits;

        d->head.digits = p;
        d->head.length = p_access->length;

        p_access->digits = p;

        return Shared_Type;//instead of returning 'this'
    }
public:
    template <typename Int>
    typename std::enable_if<std::is_integral<Int>::value, MetaLib_Type *>::type
        init(IntAccess * p_access, MetaLib_U8 * ptr, Int v)
    {
        MetaLib_STATIC_ASSERT(sizeof(MetaLib_USz) >= sizeof(Int), "Not implemented (yet).");
        MetaLib_ASSERT(p_access->length == getLength(v));

        auto r = initData(p_access, ptr);

        p_access->digits[v ? 1 : 0] = 0;
        if (v > 0)
            p_access->digits[0] = static_cast<MetaLib_USz>(v);
        if (v < 0)
            p_access->digits[0] = 0 - static_cast<MetaLib_USz>(v);

        return r;
    }
public:
    using CreatingMixin::build;
    template <typename Obj, typename Ac, typename Int>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value && std::is_integral<Int>::value>::type
        build(Obj & obj, Ac * p_ac, Int v)//override
    {
        IntAccess * p_access = p_ac;
        typedef decltype(p_access->value) AV;
        if (cmpIntLessEqual(Obj::Int_Min, v) && cmpIntLessEqual(v, Obj::Int_Max) &&
            cmpIntLessEqual(std::numeric_limits<AV>::min(), v) && cmpIntLessEqual(v, std::numeric_limits<AV>::max()))
        {
            obj.setInt(static_cast<Obj::IntValue>(v));
            p_access->digits = nullptr;
            p_access->value = static_cast<AV>(v);
            return;
        }
        return CreatingMixin::build(obj, p_access, v);
    }
public:
    MetaLib_Type * getCastType()//override
    {
        return Shared_Type;
    }
    using ExposingMixin::retrieve;
    template <typename Ac, typename Obj>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<typename std::decay<Obj>::type>::value, bool>::type
        retrieve(Ac * p_ac, Obj && obj)//override
    {
        IntAccess * p_access = p_ac;
        if (obj.isInt())
        {
            p_access->bind(std::forward<Obj>(obj));
            return true;
        }
        return ExposingMixin::retrieve(p_access, std::forward<Obj>(obj));
    }
    //TODO: see ::MetaLib::BytesType::access
};
template <typename T> MetaLib_Type * const IntTypeImpl<T>::Shared_Type = MetaLib_IntType_getShared();
template <typename T> IntTypeImpl<T> IntTypeImpl<T>::Static;

typedef IntTypeImpl<void> IntType;
static auto & Int = IntType::Static;
static auto & DMInt = ::MetaLib::DMType<decltype(&IntType::Static), &IntType::Static>::Static;

;
}


#ifdef MetaLibInt_SHARED_IMPL

MetaLib_Type * MetaLib_IntType_getShared()
{
    return &(MetaLibIntPrivate::IntType::Static);
}

#endif


namespace MetaLib
{
    using MetaLibIntPrivate::IntType;
    using MetaLibIntPrivate::Int;
    using MetaLibIntPrivate::DMInt;
}


#endif
