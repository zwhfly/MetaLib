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


#ifndef MetaLibPointer_H_
#define MetaLibPointer_H_


#include "MetaLibMetaType.h"
#include "MetaLibObject.h"
#include "MetaLibPort.h"
#include <type_traits>
#include <utility>


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Type *) MetaLib_PointerType_getShared();


namespace MetaLibPointerPrivate
{
;

struct PointerHead
{
    void * pointer;
};

template <typename T>
struct PointerStore : PointerHead, T
{
    MetaLib_STATIC_ASSERT(std::is_standard_layout<T>::value, "This kind of T type is not supported (yet).");
    MetaLib_STATIC_ASSERT(std::is_trivially_destructible<T>::value, "This kind of T type is not supported.");
};

template <>
struct PointerStore<void> : PointerHead
{};

class PointerAccess
{
    template <typename> friend class PointerTypeImpl;
    void * pointer;
public:
    PointerAccess()
    {
        clear();
    }
    void clear()
    {
        pointer = nullptr;
    }
    void * value() const
    {
        return pointer;
    }
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type *)
    {
        auto h = reinterpret_cast<PointerHead *>(ptr);
        pointer = h->pointer;
    }
};

template <typename T = void>//static data member of a class template can be defined in header file
class PointerTypeImpl//not inheritable
    :
    private ::MetaLib::EmptyType,
    public ::MetaLib::ObjectCreatingMixin<PointerTypeImpl<T>, PointerAccess>,
    public ::MetaLib::ObjectExposingMixin<PointerTypeImpl<T>, PointerAccess>
{
private:
    typedef ::MetaLib::OxAccess<::MetaLib::O, PointerAccess> OA;
private:
    PointerTypeImpl()
    {
        //MetaLib_Type::p_call = &call;
    }
    friend MetaLib_Type * ::MetaLib_PointerType_getShared();
    static MetaLib_Type * const Shared_Type;
public:
    static PointerTypeImpl Static;
public:
    std::pair<size_t, size_t> alloc(PointerAccess * p_access, void * pointer) MetaLib_NOEXCEPT
    {
        p_access->pointer = pointer;
        return std::make_pair(sizeof(PointerStore<void>), std::alignment_of<PointerStore<void>>::value);
    }
    MetaLib_Type * init(PointerAccess * p_access, MetaLib_U8 * ptr, void * pointer)
    {
        MetaLib_ASSERT(p_access->pointer == pointer);

        auto d = new(ptr) PointerStore<void>;

        d->pointer = pointer;

        p_access->pointer = pointer;

        return Shared_Type;//instead of returning 'this'
    }
public:
    MetaLib_Type * getCastType()//override
    {
        return Shared_Type;
    }
};
template <typename T> MetaLib_Type * const PointerTypeImpl<T>::Shared_Type = MetaLib_PointerType_getShared();
template <typename T> PointerTypeImpl<T> PointerTypeImpl<T>::Static;

typedef PointerTypeImpl<void> PointerType;
static auto & Pointer = PointerType::Static;
static auto & DMPointer = ::MetaLib::DMType<decltype(&PointerType::Static), &PointerType::Static>::Static;

;
}


#ifdef MetaLibPointer_SHARED_IMPL

MetaLib_INLINE MetaLib_Type * MetaLib_PointerType_getShared()
{
    return &(MetaLibPointerPrivate::PointerType::Static);
}

#endif


namespace MetaLib
{
    using MetaLibPointerPrivate::PointerType;
    using MetaLibPointerPrivate::Pointer;
    using MetaLibPointerPrivate::DMPointer;
}


#endif
