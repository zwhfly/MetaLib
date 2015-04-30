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


#ifndef MetaLibFloat_H_
#define MetaLibFloat_H_


#include "MetaLibMetaType.h"
#include "MetaLibObject.h"
#include "MetaLibPort.h"
#include <type_traits>
#include <utility>


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Type *) MetaLib_FloatType_getShared();


namespace MetaLibFloatPrivate
{
;

struct FloatHead
{
    MetaLib_F64 value;
};

class FloatAccess
{
    template <typename> friend class FloatTypeImpl;
    MetaLib_F64 v;
public:
    FloatAccess()
    {
        clear();
    }
    void clear()
    {
        v = std::numeric_limits<MetaLib_F64>::quiet_NaN();
    }
    MetaLib_F64 value() const
    {
        return v;
    }
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type *)
    {
        auto p = reinterpret_cast<FloatHead *>(ptr);
        v = p->value;
    }
    template <typename Obj>
    //no rvalue reference and is Obj type
    typename std::enable_if<!std::is_rvalue_reference<Obj &&>::value && ::MetaLib::IsObjType<typename std::decay<Obj>::type>::value>::type
        bind(Obj && obj)
    {
        v = obj.getFloat();
    }
};

template <typename T = void>//static data member of a class template can be defined in header file
class FloatTypeImpl//not inheritable
    :
    private ::MetaLib::EmptyType,
    public ::MetaLib::ObjectCreatingMixin<FloatTypeImpl<T>, FloatAccess, false>,
    public ::MetaLib::ObjectExposingMixin<FloatTypeImpl<T>, FloatAccess>
{
private:
    typedef ::MetaLib::OxAccess<::MetaLib::O, FloatAccess> OA;
private:
    FloatTypeImpl()
    {
        //MetaLib_Type::p_call = &call;
    }
    friend MetaLib_Type * ::MetaLib_FloatType_getShared();
    static MetaLib_Type * const Shared_Type;
public:
    static FloatTypeImpl Static;
public:
    std::pair<size_t, size_t> alloc(FloatAccess * p_access, MetaLib_F64 value) MetaLib_NOEXCEPT
    {
        p_access->v = value;
        return std::make_pair(sizeof(FloatHead), std::alignment_of<FloatHead>::value);
    }
    MetaLib_Type * init(FloatAccess * p_access, MetaLib_U8 * ptr, MetaLib_F64 value)
    {
        MetaLib_ASSERT(p_access->v == value);

        auto d = new(ptr) FloatHead;

        d->value = value;

        p_access->v = value;

        return Shared_Type;//instead of returning 'this'
    }
public:
    using CreatingMixin::build;
    template <typename Obj, typename Ac, typename F>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value && std::is_floating_point<F>::value>::type
        build(Obj & obj, Ac * p_ac, F v)//override
    {
        FloatAccess * p_access = p_ac;
        static_assert(sizeof(MetaLib_F64) >= sizeof(F), "Unexpected type.");
        if (Obj::isFloatFit(v))
        {
            obj.setFloat(v);
            p_access->v = v;
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
        FloatAccess * p_access = p_ac;
        if (obj.isFloat())
        {
            p_access->bind(std::forward<Obj>(obj));
            return true;
        }
        return ExposingMixin::retrieve(p_access, std::forward<Obj>(obj));
    }
    //TODO: see ::MetaLib::BytesType::access
};
template <typename T> MetaLib_Type * const FloatTypeImpl<T>::Shared_Type = MetaLib_FloatType_getShared();
template <typename T> FloatTypeImpl<T> FloatTypeImpl<T>::Static;

typedef FloatTypeImpl<void> FloatType;
static auto & Float = FloatType::Static;
static auto & DMFloat = ::MetaLib::DMType<decltype(&FloatType::Static), &FloatType::Static>::Static;

;
}


#ifdef MetaLibFloat_SHARED_IMPL

MetaLib_INLINE MetaLib_Type * MetaLib_FloatType_getShared()
{
    return &(MetaLibFloatPrivate::FloatType::Static);
}

#endif


namespace MetaLib
{
    using MetaLibFloatPrivate::FloatType;
    using MetaLibFloatPrivate::Float;
    using MetaLibFloatPrivate::DMFloat;
}


#endif
