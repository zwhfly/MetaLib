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


#ifndef MetaLibBool_H_
#define MetaLibBool_H_


#include "MetaLibMetaType.h"
#include "MetaLibObject.h"
#include "MetaLibPort.h"
#include <type_traits>
#include <utility>


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Type *) MetaLib_BoolType_getShared();


namespace MetaLibBoolPrivate
{
;

struct BoolHead
{
    MetaLib_U8 value;
};

class BoolAccess
{
    template <typename> friend class BoolTypeImpl;
    MetaLib_U8 v;
public:
    BoolAccess()
    {
        clear();
    }
    void clear()
    {
        v = 0;
        --v;
    }
    bool value() const
    {
        return v != 0;
    }
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type *)
    {
        auto p = reinterpret_cast<BoolHead *>(ptr);
        v = p->value;
    }
    template <typename Obj>
    //no rvalue reference and is Obj type
    typename std::enable_if<!std::is_rvalue_reference<Obj &&>::value && ::MetaLib::IsObjType<typename std::decay<Obj>::type>::value>::type
        bind(Obj && obj)
    {
        if (obj.isTrue())
            v = 1;
        else if (obj.isFalse())
            v = 0;
        else
        {
            v = 0;
            --v;
        }
    }
};

template <typename T = void>//static data member of a class template can be defined in header file
class BoolTypeImpl//not inheritable
    :
    private ::MetaLib::EmptyType,
    public ::MetaLib::ObjectCreatingMixin<BoolTypeImpl<T>, BoolAccess, false>,
    public ::MetaLib::ObjectExposingMixin<BoolTypeImpl<T>, BoolAccess>
{
private:
    typedef ::MetaLib::OxAccess<::MetaLib::O, BoolAccess> OA;
private:
    BoolTypeImpl()
    {
        //MetaLib_Type::p_call = &call;
    }
    friend MetaLib_Type * ::MetaLib_BoolType_getShared();
    static MetaLib_Type * const Shared_Type;
public:
    static BoolTypeImpl Static;
public:
    std::pair<size_t, size_t> alloc(BoolAccess * p_access, bool value) MetaLib_NOEXCEPT
    {
        p_access->v = value;
        return std::make_pair(sizeof(BoolHead), std::alignment_of<BoolHead>::value);
    }
    MetaLib_Type * init(BoolAccess * p_access, MetaLib_U8 * ptr, bool value)
    {
        MetaLib_ASSERT(p_access->v == value);

        auto d = new(ptr) BoolHead;

        d->value = value;

        p_access->v = value;

        return Shared_Type;//instead of returning 'this'
    }
public:
    using CreatingMixin::build;
    template <typename Obj, typename Ac>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value>::type
        build(Obj & obj, Ac * p_ac, bool v)//override
    {
        BoolAccess * p_access = p_ac;
        obj.setBool(v);
        p_access->v = v;
        return;
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
        BoolAccess * p_access = p_ac;
        if (obj.isTrue())
        {
            p_access->v = 1;
            return true;
        }

        if (obj.isFalse())
        {
            p_access->v = 0;
            return true;
        }

        if (obj.isDyn())
            return ExposingMixin::retrieve(p_access, std::forward<Obj>(obj));

        return false;
    }
    //TODO: see ::MetaLib::BytesType::access
};
template <typename T> MetaLib_Type * const BoolTypeImpl<T>::Shared_Type = MetaLib_BoolType_getShared();
template <typename T> BoolTypeImpl<T> BoolTypeImpl<T>::Static;

typedef BoolTypeImpl<void> BoolType;
static auto & Bool = BoolType::Static;
static auto & DMBool = ::MetaLib::DMType<decltype(&BoolType::Static), &BoolType::Static>::Static;

;
}


#ifdef MetaLibBool_SHARED_IMPL

MetaLib_INLINE MetaLib_Type * MetaLib_BoolType_getShared()
{
    return &(MetaLibBoolPrivate::BoolType::Static);
}

#endif


namespace MetaLib
{
    using MetaLibBoolPrivate::BoolType;
    using MetaLibBoolPrivate::Bool;
    using MetaLibBoolPrivate::DMBool;
}


#endif
