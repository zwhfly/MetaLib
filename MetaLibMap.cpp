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


#define MetaLibMap_SHARED_IMPL

#include "MetaLibMap.h"

#include <unordered_map>
#include "MetaLibBytes.h"


using namespace MetaLib;
using namespace MetaLibMapPrivate;


struct DictData : MapHead
{
    typedef std::basic_string<U8> S;
    typedef std::unordered_map<S, O> Map;
    Map map;
#if 0
public:
    ~DictData()
    {
        __debugbreak();
    }
#endif
private:
    static auto & getMap(MapHead * head)
    {
        return static_cast<DictData *>(head)->map;
    }
    static Obj get(MapHead * /*head*/, Obj /*key*/, Obj * p_exception)
    {
        p_exception->setGeneralException();//TODO: make a real exception
        return Obj::makeMark_Throw();
    }
    static Obj getB(MapHead * head, USz key_length, U8 const * key_pointer, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto & map = getMap(head);
            auto iter = map.find(S(key_pointer, key_length));
            if (iter == map.end())
            {
                return Obj::makeMark_NotExist();
            }
            return iter->second.getObj();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static Obj has(MapHead * /*head*/, Obj /*key*/, Obj * p_exception)
    {
        p_exception->setGeneralException();//TODO: make a real exception
        return Obj::makeMark_Throw();
    }
    static Obj hasB(MapHead * head, USz key_length, U8 const * key_pointer, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto & map = getMap(head);
            auto count = map.count(S(key_pointer, key_length));
            if (count)
                return Obj::makeTrue();
            else
                return Obj::makeFalse();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static Obj size(MapHead * head, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto & map = getMap(head);
            auto size = map.size();
            auto r = Int(size);
            return r.release();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static Obj set(MapHead * /*head*/, Obj /*key*/, Obj /*obj*/, Obj * p_exception)
    {
        p_exception->setGeneralException();//TODO: make a real exception
        return Obj::makeMark_Throw();
    }
    static Obj setB(MapHead * head, USz key_length, U8 const * key_pointer, Obj obj, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            O o(O::Owned(), obj);
            auto & map = getMap(head);
            map.insert_or_assign(S(key_pointer, key_length), std::move(o));

            return Obj::makeNone();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static Obj remove(MapHead * /*head*/, Obj /*key*/, Obj * p_exception)
    {
        p_exception->setGeneralException();//TODO: make a real exception
        return Obj::makeMark_Throw();
    }
    static Obj removeB(MapHead * head, USz key_length, U8 const * key_pointer, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto & map = getMap(head);
            auto count = map.erase(S(key_pointer, key_length));

            return Obj::makeBool(!!count);
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static Obj reserve(MapHead * head, USz size, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto & map = getMap(head);
            map.reserve(size);

            return Obj::makeNone();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static Obj clone(MapHead * head, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto t = MetaLib_DictType_create(p_exception);
            if (t.isMark())
                return t;
            O r(t);

            auto & map = getMap(head);
            auto dyn = t.getDyn();
            auto ptr = dyn->getPtr();
            auto new_data = reinterpret_cast<DictData *>(ptr);//TODO
            new_data->map = map;

            return r.release();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    struct IterStatus
    {
        Map::const_iterator current;
        Map::const_iterator end;
    };
    static CxxType<IterStatus> Iter;
    static Obj iter(MapHead * head, Obj * p_exception)
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto & map = getMap(head);

            auto r = Iter(IterStatus{ map.cbegin(), map.cend() });

            return r.release();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static Obj next(Obj iter, Obj * p_value, Obj * p_exception)//p_value can be nullptr
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto access = Iter.access(iter);
            auto & iter_status = *access;

            if (iter_status.current == iter_status.end)
                return Obj::makeMark_IterEnd();

            auto & k = iter_status.current->first;
            auto & v = iter_status.current->second;

            auto r = Bytes(StrRef(k.length(), k.data()));

            ++(iter_status.current);

            if (p_value)
                *p_value = v.getObj();
            return r.release();
        }
        catch (...)//TODO
        {
            //TODO: make a real exception
            p_exception->setGeneralException();
            return Obj::makeMark_Throw();
        }
    }
    static void release(MapHead *)
    {
        //do nothing
    }
private:
    static MapOps makeOps()
    {
        MapOps r;

        r.p_get        = &get       ;
        r.p_getB       = &getB      ;
        r.p_has        = &has       ;
        r.p_hasB       = &hasB      ;
        r.p_size       = &size      ;
        r.p_set        = &set       ;
        r.p_setB       = &setB      ;
        r.p_remove     = &remove    ;
        r.p_removeB    = &removeB   ;
        r.p_reserve    = &reserve   ;
        r.p_clone      = &clone     ;
        r.p_iter       = &iter      ;
        r.p_next       = &next      ;
        r.p_release    = &release   ;

        return r;
    }
    static MapVTable vtable;
public:
    DictData()
        :
        MapHead{ &vtable }
    {}
};
MapVTable DictData::vtable{ makeOps() };
CxxType<DictData::IterStatus> DictData::Iter;


class SharedDictType
    :
    private EmptyType
{
private:
    SharedDictType()
    {
        //MetaLib_Type::p_call = &call;//TODO: methods of dict
        MetaLib_Type::p_cast = &cast;
        MetaLib_Type::p_kill = &kill;
    }
public:
    static SharedDictType Static;
private:
    static U8 * cast(U8 * ptr, MetaLib_Type * /*type*/, MetaLib_Type * dest_type) MetaLib_NOEXCEPT
    {
        if (dest_type != Map.getCastType())
            return nullptr;
        auto d = reinterpret_cast<DictData *>(ptr);
        auto r = static_cast<MapHead *>(d);
        return reinterpret_cast<U8 *>(r);//TODO: is this OK?
    }
    static void kill(U8 * ptr, MetaLib_Type * /*type*/) MetaLib_NOEXCEPT
    {
        auto d = reinterpret_cast<DictData *>(ptr);
        d->~DictData();
    }
public:
    std::pair<size_t, size_t> alloc() MetaLib_NOEXCEPT
    {
        return std::make_pair(sizeof(DictData), std::alignment_of<DictData>::value);
    }
    MetaLib_Type * init(U8 * ptr)
    {
        new(ptr) DictData();
        return this;
    }
};
SharedDictType SharedDictType::Static{};


Obj MetaLib_DictType_alloc(USz * p_size, USz * p_align, Obj * p_exception)
{
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        auto r = SharedDictType::Static.alloc();
        *p_size = r.first;
        *p_align = r.second;
        return Obj::makeNone();
    }
    catch (...)//TODO
    {
        //TODO: make a real exception
        p_exception->setGeneralException();
        return Obj::makeMark_Throw();
    }
}

MetaLib_Type * MetaLib_DictType_init(U8 * ptr, Obj * p_exception)
{
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        auto r = SharedDictType::Static.init(ptr);
        return r;
    }
    catch (...)//TODO
    {
        //TODO: make a real exception
        p_exception->setGeneralException();
        return nullptr;
    }
}

Obj MetaLib_DictType_create(Obj * p_exception)
{
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        size_t size, align;
        std::tie(size, align) = SharedDictType::Static.alloc();
        auto d = MetaLib_Dyn::make(size, align);
        auto type = SharedDictType::Static.init(d->getPtr());
        d->type = type;

        Obj r;
        r.setDyn(d.release());
        return r;
    }
    catch (...)//TODO
    {
        //TODO: make a real exception
        p_exception->setGeneralException();
        return Obj::makeMark_Throw();
    }
}


#if 0
static void test()
{
    auto a = ::MetaLib::Dict();
    a.size();
    auto access = ::MetaLib::Map.access(a);
    access.size();
}
#endif
