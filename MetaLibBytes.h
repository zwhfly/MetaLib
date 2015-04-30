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


#ifndef MetaLibBytes_H_
#define MetaLibBytes_H_


#include "MetaLibMetaType.h"
#include "MetaLibObject.h"
#include "MetaLibPort.h"
#include <type_traits>
#include <utility>


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Type *) MetaLib_BytesType_getShared();


namespace MetaLibBytesPrivate
{
;

//TODO: should Bytes always have trailing zero? 
struct BytesHead
{
    MetaLib_U8 const * bytes_ptr;
    MetaLib_USz bytes_len;
};

template <typename T = void>
struct BytesStore : BytesHead, T
{
    MetaLib_STATIC_ASSERT(std::is_standard_layout<T>::value, "This kind of T type is not supported (yet).");
    MetaLib_STATIC_ASSERT(std::is_trivially_destructible<T>::value, "This kind of T type is not supported.");
};

template <>
struct BytesStore<void> : BytesHead
{
    BytesStore()
    {
        bytes_ptr = nullptr;
        bytes_len = 0;
    }
    BytesStore(MetaLib_U8 const * ptr, MetaLib_USz len)
    {
        bytes_ptr = ptr;
        bytes_len = len;
    }
};

class BytesAccess : private BytesHead
{
    template <typename> friend class BytesTypeImpl;
public:
    BytesAccess()
    {
        clear();
    }
    void clear()
    {
        bytes_ptr = nullptr;
        bytes_len = 0;
    }
    MetaLib_U8 const * data() const
    {
        return bytes_ptr;
    }
    MetaLib_USz length() const
    {
        return bytes_len;
    }
    template <typename C>
    typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), C const *>::type
        pointer() const
    {
        return reinterpret_cast<C const *>(bytes_ptr);
    }
public:
    ::MetaLib::StrRef toStrRef() const
    {
        return ::MetaLib::StrRef(bytes_len, bytes_ptr);
    }
    template <typename C>
    typename std::enable_if<std::is_integral<C>::value && (sizeof(C) == 1), std::basic_string<C>>::type
        toStdString() const
    {
        return std::basic_string<C>(reinterpret_cast<C const *>(bytes_ptr), bytes_len);
    }
    std::string toStdString() const
    {
        return toStdString<char>();
    }
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type *)
    {
        auto h = reinterpret_cast<BytesHead *>(ptr);
        bytes_ptr = h->bytes_ptr;
        bytes_len = h->bytes_len;
    }
    template <typename Obj>
    //no rvalue reference and is Obj type
    typename std::enable_if<!std::is_rvalue_reference<Obj &&>::value && ::MetaLib::IsObjType<typename std::decay<Obj>::type>::value>::type
        bind(Obj && obj)
    {
        bytes_ptr = obj.getBytesPtr();
        bytes_len = obj.getBytesLen();
    }
};

template <typename T = void>//static data member of a class template can be defined in header file
class BytesTypeImpl//not inheritable
    :
    private ::MetaLib::EmptyType,
    public ::MetaLib::ObjectCreatingMixin<BytesTypeImpl<T>, BytesAccess, false>,
    public ::MetaLib::ObjectExposingMixin<BytesTypeImpl<T>, BytesAccess>
{
private:
    typedef ::MetaLib::OxAccess<::MetaLib::O, BytesAccess> OA;
private:
    BytesTypeImpl()
    {
        //MetaLib_Type::p_call = &call;//TODO: methods of Bytes
    }
    friend MetaLib_Type * ::MetaLib_BytesType_getShared();
    static MetaLib_Type * const Shared_Type;
public:
    static BytesTypeImpl Static;
private:
    struct BytesData
    {
        BytesHead head;
        MetaLib_U8 bytes[1];
    };
public:
    std::pair<size_t, size_t> alloc(BytesAccess * p_access, size_t length) MetaLib_NOEXCEPT
    {
        p_access->bytes_len = length;
        return std::make_pair(sizeof(BytesData) + length * sizeof(MetaLib_U8), std::alignment_of<BytesData>::value);
    }
    MetaLib_Type * init(BytesAccess * p_access, MetaLib_U8 * ptr, size_t length)
    {
        MetaLib_ASSERT(p_access->bytes_len == length);

        auto d = new(ptr) BytesData;
        auto p = d->bytes;

        d->head.bytes_ptr = p;
        d->head.bytes_len = length;

        p_access->bytes_ptr = p;
        p[length] = 0;

        return Shared_Type;//instead of returning 'this'
    }
public:
    std::pair<size_t, size_t> alloc(BytesAccess * p_access, ::MetaLib::StrRef s) MetaLib_NOEXCEPT
    {
        auto length = p_access->bytes_len = s.len();
        return std::make_pair(sizeof(BytesData) + length * sizeof(MetaLib_U8), std::alignment_of<BytesData>::value);
    }
    MetaLib_Type * init(BytesAccess * p_access, MetaLib_U8 * ptr, ::MetaLib::StrRef s)
    {
        MetaLib_ASSERT(p_access->bytes_len == s.len());
        auto length = s.len();

        auto d = new(ptr) BytesData;
        auto p = d->bytes;

        d->head.bytes_ptr = p;
        d->head.bytes_len = length;

        p_access->bytes_ptr = p;

        memcpy(p, s.p(), length);
        p[length] = 0;

        return Shared_Type;//instead of returning 'this'
    }
public:
    template <typename T>
    std::pair<size_t, size_t> alloc(BytesAccess * /*p_access*/, BytesStore<T> const & /*bs*/) MetaLib_NOEXCEPT
    {
        //p_access->bytes_len = bs.length;
        return std::make_pair(sizeof(BytesStore<T>), std::alignment_of<BytesStore<T>>::value);
    }
    template <typename T>
    MetaLib_Type * init(BytesAccess * p_access, MetaLib_U8 * ptr, BytesStore<T> const & bs)
    {
        //MetaLib_ASSERT(p_access->bytes_len == bs.length);

        auto d = new(ptr) BytesStore<T>(bs);

        p_access->bytes_ptr = d->bytes_ptr;
        p_access->bytes_len = d->bytes_len;

        return Shared_Type;//instead of returning 'this'
    }
public:
    using CreatingMixin::build;
    template <typename Obj, typename Ac, typename A0>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value && std::is_convertible<typename std::remove_reference<A0>::type, ::MetaLib::StrRef>::value>::type
        build(Obj & obj, Ac * p_ac, A0 && a0)//override
    {
        BytesAccess * p_access = p_ac;
        ::MetaLib::StrRef s(std::forward<A0>(a0));
        if (s.len() <= Obj::Bytes_Max_Size)
        {
            obj.setBytes(s.p(), s.len());
            p_access->bytes_ptr = obj.getBytesPtr();
            p_access->bytes_len = s.len();
            return;
        }
        return CreatingMixin::build(obj, p_access, s);
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
        BytesAccess * p_access = p_ac;
        if (obj.isBytes())
        {
            p_access->bind(std::forward<Obj>(obj));
            return true;
        }

        return ExposingMixin::retrieve(p_access, std::forward<Obj>(obj));
    }
#if 0//TODO: this and the equivalent in other headers
    template <typename Obj>
    typename std::enable_if<::MetaLib::IsObjType<typename std::decay<Obj>::type>::value, OA>::type
        access(Obj && obj)//override
    {
        OA r;
        if (retrieve(static_cast<BytesAccess &>(r), std::forward<Obj>(obj)))
            return r;
        auto new_obj = obj.c(MetaLib_CK("toBytes"));
        if (retrieve(static_cast<BytesAccess &>(r), new_obj))
        {
            static_cast<::MetaLib::O &>(r) = std::move(new_obj);
            return r;
        }
        throw 1;//TODO
    }
#endif
};
template <typename T> MetaLib_Type * const BytesTypeImpl<T>::Shared_Type = MetaLib_BytesType_getShared();
template <typename T> BytesTypeImpl<T> BytesTypeImpl<T>::Static;


typedef BytesTypeImpl<void> BytesType;
static auto & Bytes = BytesType::Static;
static auto & DMBytes = ::MetaLib::DMType<decltype(&BytesType::Static), &BytesType::Static>::Static;


MetaLib_INLINE MetaLib_Obj makeBytes_Static(MetaLib_U8 const * p, size_t length)
{
    MetaLibBytesPrivate::BytesStore<void> store;
    store.bytes_ptr = p;
    store.bytes_len = length;
    auto r = Bytes.makeO(store);
    return r.release();
}

MetaLib_INLINE MetaLib_Obj makeBytes_Dynamic(MetaLib_U8 const * p, size_t length)
{
    MetaLib::StrRef sr(length, p);
    auto r = Bytes.makeO(sr);
    return r.release();
}

;
}


#ifdef MetaLibBytes_SHARED_IMPL

MetaLib_INLINE MetaLib_Type * MetaLib_BytesType_getShared()
{
    return &(MetaLibBytesPrivate::BytesType::Static);
}

#endif


namespace MetaLib
{
    using MetaLibBytesPrivate::BytesType;
    using MetaLibBytesPrivate::Bytes;
    using MetaLibBytesPrivate::BytesStore;
    using MetaLibBytesPrivate::DMBytes;
}


#define MetaLib_BytesLiteral(string_literal) \
([]() -> MetaLib_Obj\
{\
    static MetaLib_U8 const s[] = string_literal;\
    auto length = sizeof(s) - 1;\
    if (length <= MetaLib_Obj::Bytes_Max_Size)\
    {\
        MetaLib_Obj obj;\
        obj.setBytes(s, length);\
        return obj;\
    }\
    static MetaLib_Obj obj;\
    /* TODO */\
    static long i = 2;\
    if (i)\
    {\
        auto r = _InterlockedCompareExchange(&i, 1, 2);\
        if (r == 2)\
        {\
            if (length < 9 * sizeof(void *))/* TODO */\
                obj = MetaLibBytesPrivate::makeBytes_Dynamic(s, length);\
            else\
                obj = MetaLibBytesPrivate::makeBytes_Static(s, length);\
            _mm_sfence();\
            i = 0;\
        }\
        else\
        {\
            while (i);\
        }\
    }\
    _mm_lfence();\
    return obj;\
}())


#endif
