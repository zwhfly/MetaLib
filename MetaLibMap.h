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


#ifndef MetaLibMap_H_
#define MetaLibMap_H_


#include "MetaLibMetaType.h"
#include "MetaLibObject.h"
#include "MetaLibPort.h"
#include "MetaLibInt.h"
#include "MetaLibBool.h"
#include <type_traits>
#include <tuple>


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Type *) MetaLib_MapType_getShared();


namespace MetaLibMapPrivate
{
;

typedef struct MapHead_ MapHead;

//TODO: extern "C"
//TODO: noexcept
typedef MetaLib_Obj MapOps_Get       (MapHead * head, MetaLib_Obj key, MetaLib_Obj * p_exception);//does not return ownership
typedef MetaLib_Obj MapOps_GetB      (MapHead * head, MetaLib_USz key_length, MetaLib_U8 const * key_pointer, MetaLib_Obj * p_exception);//does not return ownership
typedef MetaLib_Obj MapOps_Has       (MapHead * head, MetaLib_Obj key, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_HasB      (MapHead * head, MetaLib_USz key_length, MetaLib_U8 const * key_pointer, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_Size      (MapHead * head, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_Set       (MapHead * head, MetaLib_Obj key, MetaLib_Obj obj, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_SetB      (MapHead * head, MetaLib_USz key_length, MetaLib_U8 const * key_pointer, MetaLib_Obj obj, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_Remove    (MapHead * head, MetaLib_Obj key, MetaLib_Obj * p_exception);//return True if existed and removed
typedef MetaLib_Obj MapOps_RemoveB   (MapHead * head, MetaLib_USz key_length, MetaLib_U8 const * key_pointer, MetaLib_Obj * p_exception);//return True if existed and removed
typedef MetaLib_Obj MapOps_Reserve   (MapHead * head, MetaLib_USz size, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_Clone     (MapHead * head, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_Iter      (MapHead * head, MetaLib_Obj * p_exception);
typedef MetaLib_Obj MapOps_Next      (MetaLib_Obj iter, MetaLib_Obj * p_value, MetaLib_Obj * p_exception);//p_value can be nullptr, *p_value does not have ownership
typedef void        MapOps_Release   (MapHead * head);

typedef struct MapOps_
{
    MapOps_Get       * p_get;
    MapOps_GetB      * p_getB;
    MapOps_Has       * p_has;
    MapOps_HasB      * p_hasB;
    MapOps_Size      * p_size;
    MapOps_Set       * p_set;
    MapOps_SetB      * p_setB;
    MapOps_Remove    * p_remove;
    MapOps_RemoveB   * p_removeB;
    MapOps_Reserve   * p_reserve;
    MapOps_Clone     * p_clone;
    MapOps_Iter      * p_iter;
    MapOps_Next      * p_next;
    MapOps_Release   * p_release;
} MapOps;

//These are initialized in C++11 compile units.
#if _MSC_VER == 1600
#pragma warning(push)
#pragma warning(disable: 4510 4512 4610)
#endif

typedef struct MapVTable_
{
    MapOps const ops;
} MapVTable;

struct MapHead_//MapHead
{
    MapVTable const * const vtable;
};

#if _MSC_VER == 1600
#pragma warning(pop)
#endif

class MapAccess
{
    template <typename> friend class MapTypeImpl;
    MapHead * head;
    MapVTable const * vtable;
public:
    MapAccess()
    {
        clear();
    }
    void clear()
    {
        head = nullptr;
        vtable = nullptr;
    }
public:
    ::MetaLib::O get(::MetaLib::StrRef key) const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_getB(head, key.len(), key.p(), &exception);
        if (r.isMark())
        {
            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            //if (r.isMark_NotExist())
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        return ::MetaLib::O(r);
    }
    std::pair<bool, ::MetaLib::O> query(::MetaLib::StrRef key, ::MetaLib::O defaulted) const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_getB(head, key.len(), key.p(), &exception);
        if (r.isMark())
        {
            if (r.isMark_NotExist())
                return std::make_pair(false, std::move(defaulted));

            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        return std::make_pair(true, ::MetaLib::O(r));
    }
    std::pair<::MetaLib::O, bool> query(::MetaLib::StrRef key, MetaLib_Obj defaulted = MetaLib_Obj::makeNone()) const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_getB(head, key.len(), key.p(), &exception);
        if (r.isMark())
        {
            if (r.isMark_NotExist())
                return std::make_pair(::MetaLib::O(defaulted), false);

            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        return std::make_pair(::MetaLib::O(r), true);
    }
    bool has(::MetaLib::StrRef key) const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_hasB(head, key.len(), key.p(), &exception);
        if (r.isTrue())
            return true;
        if (r.isFalse())
            return false;
        if (r.isMark_Throw())
        {
            MetaLib_ASSERT(false == exception.isMark());
            throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
        }

        //If it's other marks, access would throw.
        
        ::MetaLib::O t(::MetaLib::O::Owned(), r);
        auto access = ::MetaLib::Bool.access(r);
        return access.value();
    }
    size_t size() const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_size(head, &exception);
        if (r.isInt())//fast path
        {
            auto v = r.getInt();
            return ::MetaLib::castInt<size_t>(v);
        }
        if (r.isMark())
        {
            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        auto s = r.toO_Owned();
        auto access = ::MetaLib::Int.access(s);
        return access.toInt<size_t>();
    }
    void set(::MetaLib::StrRef key, ::MetaLib::O o) const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_setB(head, key.len(), key.p(), o.release(), &exception);
        if (r.isNone())//fast path
            return;
        if (r.isMark_Throw())
        {
            MetaLib_ASSERT(false == exception.isMark());
            throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
        }
        MetaLib_ASSERT(false == r.isDyn());
        throw ::MetaLib::O::GeneralException;//TODO: make a real exception
    }
    bool remove(::MetaLib::StrRef key)
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_removeB(head, key.len(), key.p(), &exception);
        if (r.isTrue())
            return true;
        if (r.isFalse())
            return false;
        if (r.isMark_Throw())
        {
            MetaLib_ASSERT(false == exception.isMark());
            throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
        }

        //If it's other marks, access would throw.
        //And we assume it's not removed.

        ::MetaLib::O t(::MetaLib::O::Owned(), r);
        auto access = ::MetaLib::Bool.access(r);
        return access.value();
    }
    void reserve(MetaLib_USz size) const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_reserve(head, size, &exception);
        if (r.isNone())//fast path
            return;
        if (r.isMark_Throw())
        {
            MetaLib_ASSERT(false == exception.isMark());
            throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
        }
        MetaLib_ASSERT(false == r.isDyn());
        throw ::MetaLib::O::GeneralException;//TODO: make a real exception
    }
    ::MetaLib::O clone() const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_clone(head, &exception);
        if (r.isMark())
        {
            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        return ::MetaLib::O(r);
    }
    ::MetaLib::O iter() const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = vtable->ops.p_iter(head, &exception);
        if (r.isMark())
        {
            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        return ::MetaLib::O(r);
    }
    std::tuple<::MetaLib::O, ::MetaLib::O, bool> next(::MetaLib::O const & iterator) const
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto value = MetaLib_Obj::makeNone();
        auto r = vtable->ops.p_next(iterator.getObj(), &value, &exception);
        if (r.isMark())
        {
            if (r.isMark_IterEnd())
            {
                return std::make_tuple(::MetaLib::O::None, ::MetaLib::O::None, false);
            }
            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        return std::make_tuple(::MetaLib::O(::MetaLib::O::Owned(), r), ::MetaLib::O(value), true);
    }
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type *)
    {
        head = reinterpret_cast<MapHead *>(ptr);
        vtable = head->vtable;
    }
};

template <typename T = void>//static data member of a class template can be defined in header file
class MapTypeImpl//not inheritable
    :
    private ::MetaLib::EmptyType,
    public ::MetaLib::ObjectExposingMixin<MapTypeImpl<T>, MapAccess>
{
private:
    typedef ::MetaLib::OxAccess<::MetaLib::O, MapAccess> OA;
private:
    MapTypeImpl()
    {
        //MetaLib_Type::p_call = &call;//TODO: methods of basic map
        MetaLib_Type::p_kill = &kill;
    }
    friend MetaLib_Type * ::MetaLib_MapType_getShared();
    static MetaLib_Type * const Shared_Type;
public:
    static MapTypeImpl Static;
private:
    static void kill(MetaLib_U8 * ptr, MetaLib_Type * /*type*/) MetaLib_NOEXCEPT
    {
        auto h = reinterpret_cast<MapHead *>(ptr);
        h->vtable->ops.p_release(h);
    }
public:
    MetaLib_Type * getCastType()//override
    {
        return Shared_Type;
    }
    //TODO: see ::MetaLib::BytesType::access
};
template <typename T> MetaLib_Type * const MapTypeImpl<T>::Shared_Type = MetaLib_MapType_getShared();
template <typename T> MapTypeImpl<T> MapTypeImpl<T>::Static;

typedef MapTypeImpl<void> MapType;
static auto & Map = MapType::Static;

;
}


#ifdef MetaLibMap_SHARED_IMPL

MetaLib_INLINE MetaLib_Type * MetaLib_MapType_getShared()
{
    return &(MetaLibMapPrivate::MapType::Static);
}

#endif


namespace MetaLib
{
    using MetaLibMapPrivate::MapType;
    using MetaLibMapPrivate::Map;
    //typedef MetaLibMapPrivate::MapAccess MapCreatingAccess;
    //typedef MetaLibMapPrivate::MapAccess MapExposingAccess;
}


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Obj) MetaLib_DictType_alloc(MetaLib_USz * p_size, MetaLib_USz * p_align, MetaLib_Obj * p_exception);
MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Type *) MetaLib_DictType_init(MetaLib_U8 * ptr, MetaLib_Obj * p_exception);//return nullptr when throw
MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Obj) MetaLib_DictType_create(MetaLib_Obj * p_exception);


namespace MetaLibMapPrivate
{
;

class DictAccess : public MapAccess
{
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type * type)
    {
        MetaLib_U8 * bm_ptr = nullptr;
        MetaLib_Type * bm_type = nullptr;
        std::tie(bm_ptr, bm_type) = ::MetaLib::Map.locate(ptr, type);
        if (bm_ptr == nullptr)
            throw 1;//TODO
        MapAccess::bind(bm_ptr, bm_type);
    }
};

template <typename T = void>//static data member of a class template can be defined in header file
class DictTypeImpl//not inheritable
    :
    //private ::MetaLib::EmptyType,
    public ::MetaLib::ObjectCreatingMixin<DictTypeImpl<T>, DictAccess>,
    public ::MetaLib::ObjectExposingMixin<DictTypeImpl<T>, DictAccess>
{
private:
    typedef ::MetaLib::OxAccess<::MetaLib::O, DictAccess> OA;
public:
    static DictTypeImpl Static;
public:
    std::pair<size_t, size_t> alloc(DictAccess * /*p_access*/)
    {
        std::pair<size_t, size_t> result;
        MetaLib_Obj exception;
        exception.setNull();
        auto r = MetaLib_DictType_alloc(&result.first, &result.second, &exception);
        if (r.isNone())//fast path
            return result;

        if (r.isMark())
        {
            if (r.isMark_Throw())
            {
                MetaLib_ASSERT(false == exception.isMark());
                throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
            }
            throw ::MetaLib::O::GeneralException;//TODO: make a real exception
        }
        if (r.isDyn())
            r.decDynContentRef();
        return result;
    }
    MetaLib_Type * init(DictAccess * p_access, MetaLib_U8 * ptr)
    {
        MetaLib_Obj exception;
        exception.setNull();
        auto r = MetaLib_DictType_init(ptr, &exception);
        if (r)
        {
            p_access->bind(ptr, r);

            return r;
        }

        MetaLib_ASSERT(false == exception.isMark());
        throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
    }
public:
    using CreatingMixin::build;
    template <typename Obj, typename Ac>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value>::type
        build(Obj & obj, Ac * p_ac)//override//slightly faster path
    {
        DictAccess * p_access = p_ac;
        MetaLib_Obj exception;
        exception.setNull();
        auto r = MetaLib_DictType_create(&exception);
        if (r.isDyn())
        {
            auto guard = ::MetaLib::O(::MetaLib::O::Owned(), r);
            auto dyn = r.getDyn();
            p_access->bind(dyn->getPtr(), dyn->type);
            obj.setDyn(dyn);
            guard.release();
            return;
        }

        if (r.isMark_Throw())
        {
            MetaLib_ASSERT(false == exception.isMark());
            throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
        }
        throw ::MetaLib::O::GeneralException;//TODO: make a real exception
    }
};
template <typename T> DictTypeImpl<T> DictTypeImpl<T>::Static;

typedef DictTypeImpl<void> DictType;
static auto & Dict = DictType::Static;
static auto & DMDict = ::MetaLib::DMType<decltype(&DictType::Static), &DictType::Static>::Static;

;
}


namespace MetaLib
{
    using MetaLibMapPrivate::DictType;
    using MetaLibMapPrivate::Dict;
    using MetaLibMapPrivate::DMDict;
    //typedef MetaLibMapPrivate::DictAccess DictCreatingAccess;
    //typedef MetaLibMapPrivate::DictAccess DictExposingAccess;
}


#endif
