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


#ifndef MetaLibObject_H_
#define MetaLibObject_H_


#include "MetaLibPort.h"
#include "MetaLibRefCount.h"
#include "MetaLibScopeGuard.h"

#ifdef __cplusplus
#include <type_traits>
#include <limits>
#include <array>
#include <utility>
#include <initializer_list>
#include <new>
#include <memory>
#include <cstddef>
#endif


typedef struct MetaLib_Obj_Word_ MetaLib_Obj_Word;
typedef struct MetaLib_Type_ MetaLib_Type;
typedef struct MetaLib_Dyn_ MetaLib_Dyn;//Dynamic Unit
typedef MetaLib_Obj_Word MetaLib_Obj;//the general Obj type


#ifdef __cplusplus

namespace MetaLibObjectPrivate
{
;

template <typename Obj> class OObj;
template <typename Obj> class WObj;

template <typename T>
void pass(std::initializer_list<T>)
{}

struct Key_LP//length/pointer
{
    MetaLib_USz length;
    MetaLib_U8 const * pointer;
public:
    template <typename C>
    Key_LP(size_t length, C const * pointer)
        :
        length(length),
        pointer(reinterpret_cast<MetaLib_U8 const *>(pointer))
    {
        MetaLib_STATIC_ASSERT(sizeof(C) == 1, "Wrong 'C' Type");
    }
    Key_LP(::MetaLib::StrRef sr)
        :
        length(sr.len()),
        pointer(sr.p<MetaLib_U8>())
    {}
};

template <typename T> struct IsObjType : std::false_type {};
template <> struct IsObjType<MetaLib_Obj_Word> : std::true_type {};

template <typename T> struct IsOObjType : std::false_type {};
template <typename T> struct IsOObjType<OObj<T>> : IsObjType<T> {};

struct Test_Is_An_OObj
{
    template <typename T>
    static typename std::enable_if<T::MetaLib_Is_An_OObj, char>::type
        test(T *);
    static long long test(...);
};
template <typename T>
struct IsAnOObj : std::integral_constant<bool, sizeof(Test_Is_An_OObj::test((T *)nullptr)) == 1>//VS2010 does not have std::bool_constant
{};


typedef Key_LP CallKey;
#define MetaLib_CK(literal) ::MetaLibObjectPrivate::CallKey(sizeof(literal) - 1, literal)//Calling Key i.e. method name
#define MetaLib_EXTRACT_CK(key) key.length, key.pointer

#if MetaLib_HAS_VARIADIC_TEMPLATE
template <typename Obj, typename ... A>
typename std::enable_if<IsObjType<Obj>::value, OObj<MetaLib_Obj>>::type
MetaLib_INLINE call(Obj const & obj, CallKey key, A && ... a);
#else
template <typename Obj>
typename std::enable_if<IsObjType<Obj>::value, OObj<MetaLib_Obj>>::type
MetaLib_INLINE call(Obj const & obj, CallKey key);
template <typename Obj, typename A0>
typename std::enable_if<IsObjType<Obj>::value, OObj<MetaLib_Obj>>::type
MetaLib_INLINE call(Obj const & obj, CallKey key, A0 && a0);
template <typename Obj, typename A0, typename A1>
typename std::enable_if<IsObjType<Obj>::value, OObj<MetaLib_Obj>>::type
MetaLib_INLINE call(Obj const & obj, CallKey key, A0 && a0, A1 && a1);
#endif
MetaLib_INLINE MetaLib_Obj callRaw(MetaLib_Obj obj, CallKey key, MetaLib_Obj * argv) MetaLib_NOEXCEPT;

;
}

namespace MetaLib
{
    using MetaLibObjectPrivate::IsObjType;
    using MetaLibObjectPrivate::IsOObjType;
    using MetaLibObjectPrivate::IsAnOObj;
    using MetaLibObjectPrivate::OObj;
    using MetaLibObjectPrivate::WObj;

    using MetaLibObjectPrivate::CallKey;
    using MetaLibObjectPrivate::call;
    using MetaLibObjectPrivate::callRaw;

    typedef ::MetaLib_Type Type;
    typedef ::MetaLib_Dyn Dyn;
    typedef ::MetaLib_Obj Obj;
    typedef OObj<Obj> O;
    typedef WObj<Obj> WO;
}

#endif


//TODO: extern "C"
//TODO: noexcept
typedef MetaLib_Obj
MetaLib_Op_Call(
    MetaLib_U8 * ptr,
    MetaLib_Type * type,
    MetaLib_USz key_length, MetaLib_U8 const * key_pointer,//key is method name, <anything, 0> is one valid method name
    //MetaLib_Obj key,
    MetaLib_Obj * argv//argv[0] is slot for returning exception; arg[1] is reserved; argv[2] is argc; arg[3] is self; argv[argc + 3] is Null that marks the end of the array.
);
typedef MetaLib_U8 * MetaLib_Op_Cast(MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Type * dest_type);
typedef void MetaLib_Op_Kill(MetaLib_U8 * ptr, MetaLib_Type * type);
typedef void MetaLib_Op_Drop(MetaLib_Dyn * dyn);


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Dyn"
#endif
/*************************************************************************************************************************************/

struct MetaLib_Dyn_
{
    MetaLib_RefCount _ref_count_content;
    MetaLib_RefCount _ref_count_memory;
    MetaLib_Op_Drop * p_drop;
    MetaLib_Type * type;//Most Derived Type
    //sizeof(MetaLib_Dyn_) % Max_Align should be 0

#ifdef __cplusplus

private:
    struct Deleter
    {
        static void drop(MetaLib_Dyn * dyn) MetaLib_NOEXCEPT
        {
            auto p = reinterpret_cast<MetaLib_U8 *>(dyn);
            delete[] p;
        }
        void operator()(MetaLib_Dyn * dyn) const MetaLib_NOEXCEPT
        {
            drop(dyn);
        }
    };
    typedef std::unique_ptr<MetaLib_Dyn, Deleter> MakeResult;
public:
    static MakeResult make(size_t size, size_t align)
    {
        if (MetaLib_MAX_ALIGN % align)
            throw 1;//TODO

        auto space = new MetaLib_U8[sizeof(MetaLib_Dyn) + size];
        auto d = new(space) MetaLib_Dyn;
        MakeResult r(d);

        r->initRefCount();
        r->p_drop = &Deleter::drop;
        r->type = nullptr;

        return r;
    }
    MetaLib_U8 * getPtr() MetaLib_NOEXCEPT
    {
        auto r = reinterpret_cast<MetaLib_U8 *>(this);
        return r + sizeof(MetaLib_Dyn);
    }
    template <typename>
    void kill_() MetaLib_NOEXCEPT
    {
        auto ptr = getPtr();
        type->p_kill(ptr, type);
    }
    void kill() MetaLib_NOEXCEPT
    {
        return kill_<void>();
    }
    template <typename Obj>
    typename std::enable_if<::MetaLib::OKType<Obj>::value, MetaLib_Obj>::type
        call(::MetaLib::CallKey key, Obj * argv) MetaLib_NOEXCEPT
    {
        MetaLib_Obj * argv_ = argv;
        auto ptr = getPtr();
        return type->p_call(ptr, type, MetaLib_EXTRACT_CK(key), argv_);
    }
    template <typename Type>
    MetaLib_U8 * cast(Type * dest_type) MetaLib_NOEXCEPT
    {
        MetaLib_Type * dtype = dest_type;
        auto ptr = getPtr();
        if (type == dtype)
            return ptr;
        return type->p_cast(ptr, type, dtype);
    }

public:
    void initRefCount() MetaLib_NOEXCEPT
    {
        MetaLib_RefCount_init(&_ref_count_memory, 1);
        MetaLib_RefCount_init(&_ref_count_content, 1);
    }
    MetaLib_USz getrefMemory() const MetaLib_NOEXCEPT
    {
        return MetaLib_RefCount_load(&_ref_count_memory);
    }
    MetaLib_USz getContentRef() const MetaLib_NOEXCEPT
    {
        return MetaLib_RefCount_load(&_ref_count_content);
    }
    void incMemoryRef() MetaLib_NOEXCEPT
    {
        MetaLib_USz result_ref_count_memory;
        auto err = MetaLib_RefCount_increase(&_ref_count_memory, &result_ref_count_memory, 1);
        (void)(err);
        MetaLib_ASSERT(0 == err);
    }
    void decMemoryRef() MetaLib_NOEXCEPT
    {
        MetaLib_USz result_ref_count_memory;
        auto err = MetaLib_RefCount_increase(&_ref_count_memory, &result_ref_count_memory, -1);
        (void)(err);
        MetaLib_ASSERT(0 == err);
        if (result_ref_count_memory == 0)
        {
            MetaLib_ASSERT(0 == getContentRef());
            p_drop(this);
        }
    }
    void incContentRef() MetaLib_NOEXCEPT
    {
        MetaLib_USz result_ref_count_content;
        auto err = MetaLib_RefCount_increase(&_ref_count_content, &result_ref_count_content, 1);
        (void)(err);
        MetaLib_ASSERT(0 == err);
    }
    void decContentRef() MetaLib_NOEXCEPT
    {
        MetaLib_USz result_ref_count_content;
        auto err = MetaLib_RefCount_increase(&_ref_count_content, &result_ref_count_content, -1);
        (void)(err);
        MetaLib_ASSERT(0 == err);
        if (result_ref_count_content == 0)
        {
            kill();
            decMemoryRef();
        }
    }
    bool incContentRefIfNot0() MetaLib_NOEXCEPT//return true <=> not 0
    {
        MetaLib_USz result_ref_count_content;
        auto err = MetaLib_RefCount_increaseIfNot0(&_ref_count_content, &result_ref_count_content, 1);
        (void)(err);
        MetaLib_ASSERT(0 == err);
        MetaLib_ASSERT(result_ref_count_content != 1);
        return (result_ref_count_content != 0);
    }

#endif
};
MetaLib_STATIC_ASSERT(sizeof(MetaLib_Dyn) % MetaLib_MAX_ALIGN == 0, "Unacceptable size of MetaLib_Dyn.");
MetaLib_STATIC_ASSERT(MetaLib_ALIGNOF(MetaLib_Dyn) >= 4, "Unacceptable alignment of MetaLib_Dyn.");


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "OObj"
#endif
/*************************************************************************************************************************************/

#ifdef __cplusplus

namespace MetaLibObjectPrivate
{
;

struct OObjOwned {};

template <typename Obj>
class OObj//Obj with reference ownership
          //: private Obj
{
    MetaLib_STATIC_ASSERT(IsObjType<Obj>::value, "Illegal Obj type.");
    template <typename T> friend class OObj;
    template <typename T> friend class WObj;

#ifdef __INTELLISENSE__
    MetaLib_Obj
#else
    Obj
#endif
        obj;
    //MetaLib_Dyn * dyn;//not neccesarry (lock xadd qword ptr [rax*4], rcx)
public:
    static MetaLib_CONSTEXPR bool const MetaLib_Is_An_OObj = true;
public:
    typedef Obj ObjType;
public:
    Obj const & getObj() const MetaLib_NOEXCEPT
    {
        return obj;
    }
    bool is(OObj const & rhs) const MetaLib_NOEXCEPT
    {
        return obj.is_NoOwning(rhs.obj);
    }
    bool isNone() const MetaLib_NOEXCEPT
    {
        return obj.isNone();
    }
    bool notNone() const MetaLib_NOEXCEPT
    {
        return !isNone();
    }
#if 0
    Obj const * get() const MetaLib_NOEXCEPT
    {
        return &obj;
    }
    Obj const & operator*() const MetaLib_NOEXCEPT
    {
        return *get();
    }
    Obj const * operator->() const MetaLib_NOEXCEPT
    {
        return get();
    }
#endif
public:
    Obj release() MetaLib_NOEXCEPT
    {
        Obj r = obj;
        obj.setNone();
        return r;
    }
    void clear() MetaLib_NOEXCEPT
    {
        if (obj.isDyn())
            obj.decDynContentRef();
        obj.setNone();
    }
    Obj acquire() const MetaLib_NOEXCEPT
    {
        if (obj.isDyn())
            obj.incDynContentRef();
        return obj;
    }
public:
    ~OObj() MetaLib_NOEXCEPT
    {
        clear();
    }
    OObj() MetaLib_NOEXCEPT
    {
        obj.setNone();
    }
    typedef OObjOwned Owned;
    OObj(MetaLib_Dyn * d) MetaLib_NOEXCEPT
    {
        d->incContentRef();
        obj.setDyn(d);
    }
    OObj(Owned, MetaLib_Dyn * d) MetaLib_NOEXCEPT
    {
        obj.setDyn(d);
    }
    template <typename RObj>
    OObj(RObj robj, typename std::enable_if<IsObjType<RObj>::value>::type * = nullptr)
        :
        obj(std::move(robj))
    {
        if (obj.isDyn())
            obj.incDynContentRef();
    }
    template <typename RObj>
    OObj(Owned, RObj robj, typename std::enable_if<IsObjType<RObj>::value>::type * = nullptr)
        :
        obj(std::move(robj))
    {}
    OObj(Obj robj) MetaLib_NOEXCEPT
        :
        obj(std::move(robj))
    {
        if (obj.isDyn())
            obj.incDynContentRef();
    }
    OObj(Owned, Obj robj) MetaLib_NOEXCEPT
        :
        obj(std::move(robj))
    {}
#if 0
    //TODO: RO const &&
    template <typename RO>
    OObj(RO && ro, typename std::enable_if<IsAnOObj<typename std::decay<RO>::type>::value && std::is_rvalue_reference<RO &&>::value>::type * = nullptr)
        :
        obj(ro.obj)
    {
        ro.obj.setNone();
    }
    template <typename RO>
    OObj(RO const & ro, typename std::enable_if<IsAnOObj<RO>::value>::type * = nullptr)
        :
        obj(ro.obj)
    {
        if (obj.isDyn())
            obj.incDynContentRef();
    }
#endif
    OObj(OObj && ro) MetaLib_NOEXCEPT
        :
        obj(ro.obj)
    {
        ro.obj.setNone();
    }
    OObj(OObj const & ro) MetaLib_NOEXCEPT
        :
        obj(ro.obj)
    {
        if (obj.isDyn())
            obj.incDynContentRef();
    }
private:
    template <typename RObj>
    typename std::enable_if<IsObjType<RObj>::value, OObj &>::type
        assignObj(RObj robj)
    {
        Obj t(std::move(robj));
        clear();
        if (t.isDyn())
            t.incDynContentRef();
        obj = std::move(t);

        return *this;
    }
public:
    template <typename RObj>
    typename std::enable_if<IsObjType<RObj>::value, OObj &>::type
        operator=(RObj robj)
    {
        return assignObj(robj);
    }
    OObj & operator=(Obj robj) MetaLib_NOEXCEPT
    {
        return assignObj(robj);
    }
private:
    template <typename RO>
    OObj & assignOObj(RO && ro)
    {
        auto & robj = ro.obj;

        Obj t1(robj);
        typename RO::ObjType t2(obj);

        obj = std::move(t1);
        robj = std::move(t2);

        return *this;
    }
    template <typename RO>
    OObj & assignOObj(RO const & ro)
    {
        Obj t(ro.obj);
        clear();
        if (t.isDyn())
            t.incDynContentRef();
        obj = std::move(t);

        return *this;
    }
public:
#if 0
    //TODO: RO const &&
    template <typename RO>
    typename std::enable_if<IsAnOObj<typename std::decay<RO>::type>::value && std::is_rvalue_reference<RO &&>::value, OObj &>::type
        operator=(RO && ro)
    {
        return assignOObj(std::move(ro));
    }
    template <typename RO>
    typename std::enable_if<IsAnOObj<RO>::value, OObj &>::type
        operator=(RO const & ro)
    {
        return assignOObj(ro);
    }
#endif
    OObj & operator=(OObj && ro) MetaLib_NOEXCEPT
    {
        return assignOObj(std::move(ro));
    }
    OObj & operator=(OObj const & ro) MetaLib_NOEXCEPT
    {
        return assignOObj(ro);
    }
public:
    //using Obj::c;
    //using Obj::operator();
    //template <typename ... A>
    //auto c(A && ... a) const
    //{
    //    return obj.c(std::forward<A>(a) ...);
    //}
    //template <typename ... A>
    //auto operator()(A && ... a) const
    //{
    //    return obj(std::forward<A>(a) ...);
    //}
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    auto c(CallKey key, A && ... a) const
    {
        return call(obj, key, std::forward<A>(a)...);
    }
    template <typename ... A>
    auto operator()(A && ... a) const
    {
        return call(obj, MetaLib_CK(""), std::forward<A>(a)...);
    }
#else
    OObj<MetaLib_Obj> c(CallKey key) const { return call(obj, key); }
    template <typename A0> OObj<MetaLib_Obj> c(CallKey key, A0 && a0) const { return call(obj, key, std::forward<A0>(a0)); }
    template <typename A0, typename A1> OObj<MetaLib_Obj> c(CallKey key, A0 && a0, A1 && a1) const { return call(obj, key, std::forward<A0>(a0), std::forward<A1>(a1)); }

    OObj<MetaLib_Obj> operator()() const { return call(obj, MetaLib_CK("")); }
    template <typename A0> OObj<MetaLib_Obj> operator()(A0 && a0) const { return call(obj, MetaLib_CK(""), std::forward<A0>(a0)); }
    template <typename A0, typename A1> OObj<MetaLib_Obj> operator()(A0 && a0, A1 && a1) const { return call(obj, MetaLib_CK(""), std::forward<A0>(a0), std::forward<A1>(a1)); }
#endif
public:
    static Obj const GeneralException_Obj;
    static Obj const None_Obj;
    static Obj const False_Obj;
    static Obj const True_Obj;
    static OObj const GeneralException;
    static OObj const None;
    static OObj const False;
    static OObj const True;
public:
    static OObj makeBool(bool v) MetaLib_NOEXCEPT
    {
        if (v)
            return OObj(Owned(), Obj::makeTrue());
        else
            return OObj(Owned(), Obj::makeFalse());
    }
};
template <typename Obj> Obj const OObj<Obj>::GeneralException_Obj(Obj::makeGeneralException());
template <typename Obj> Obj const OObj<Obj>::None_Obj(Obj::makeNone());
template <typename Obj> Obj const OObj<Obj>::False_Obj(Obj::makeFalse());
template <typename Obj> Obj const OObj<Obj>::True_Obj(Obj::makeTrue());
template <typename Obj> OObj<Obj> const OObj<Obj>::GeneralException(Owned(), Obj::makeGeneralException());
template <typename Obj> OObj<Obj> const OObj<Obj>::None(Owned(), Obj::makeNone());
template <typename Obj> OObj<Obj> const OObj<Obj>::False(Owned(), Obj::makeFalse());
template <typename Obj> OObj<Obj> const OObj<Obj>::True(Owned(), Obj::makeTrue());


template <typename Obj>
class WObj//Obj with weak ownership
{
    MetaLib_STATIC_ASSERT(IsObjType<Obj>::value, "Illegal Obj type.");

#ifdef __INTELLISENSE__
    MetaLib_Obj
#else
    Obj
#endif
        obj;
public:
    Obj getObj() const MetaLib_NOEXCEPT
    {
        return obj;
    }
    bool isNone() const MetaLib_NOEXCEPT
    {
        return obj.isNone();
    }
    bool notNone() const MetaLib_NOEXCEPT
    {
        return !isNone();
    }
public:
    WObj() MetaLib_NOEXCEPT
    {
        obj.setNone();
    }
    WObj(WObj const & rhs) MetaLib_NOEXCEPT
    {
        obj = rhs.obj;
        if (obj.isDyn())
            obj.incDynMemoryRef();
    }
    WObj(WObj && rhs) MetaLib_NOEXCEPT
    {
        obj = rhs.obj;
        rhs.obj.setNone();
    }
    WObj & operator=(WObj const & rhs) MetaLib_NOEXCEPT
    {
        return ((*this) = WObj(rhs));
    }
    WObj & operator=(WObj && rhs) MetaLib_NOEXCEPT
    {
        MetaLib_ASSERT(this != &rhs);
        std::swap(obj, rhs.obj);
        rhs.clear();
        return *this;
    }
    ~WObj() MetaLib_NOEXCEPT
    {
        clear();
    }
    void clear() MetaLib_NOEXCEPT
    {
        if (obj.isDyn())
        {
            obj.decDynMemoryRef();
            obj.setNone();
        }
    }

    WObj(OObj<Obj> const & o) MetaLib_NOEXCEPT
    {
        obj = o.obj;
        if (obj.isDyn())
            obj.incDynMemoryRef();
    }
    WObj & operator=(OObj<Obj> const & rhs) MetaLib_NOEXCEPT
    {
        return ((*this) = WObj(rhs));
    }
    bool valid() const MetaLib_NOEXCEPT
    {
        if (obj.isDyn())
            return obj.getDyn()->getContentRef() > 0;
        return true;
    }
    OObj<Obj> lock() const MetaLib_NOEXCEPT
    {
        if (obj.isDyn())
        {
            if (obj.getDyn()->incContentRefIfNot0())
                return OObj<Obj>(OObj<Obj>::Owned(), obj);
            else
                return OObj<Obj>(OObj<Obj>::Owned(), Obj::makeMark_WeakObj());
        }
        return OObj<Obj>(OObj<Obj>::Owned(), obj);
    }

    friend void swap(WObj & a, WObj & b) MetaLib_NOEXCEPT
    {
        std::swap(a.obj, b.obj);
    }
};

;
}

#endif


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Obj"
#endif
/*************************************************************************************************************************************/

#if defined(_M_IX86) || defined(__i386__) || defined(_M_AMD64) || defined(__x86_64__)

struct MetaLib_Obj_Word_
{
    union Union
    {
        MetaLib_U8 bytes[sizeof(MetaLib_UPtr)];
        MetaLib_UPtr _u;//for alignment and debug purpose, never use it
    } word;

#ifdef __cplusplus

public:
    static MetaLib_CONSTEXPR bool const Is_MetaLib_Obj_Type = true;

private:
#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__) || defined(__MINGW64__)
    void encodeLE(MetaLib_UPtr u) MetaLib_NOEXCEPT
    {
        new (word.bytes) MetaLib_UPtr(u);
    }
    MetaLib_UPtr decodeLE() const MetaLib_NOEXCEPT
    {
        //TODO: is this C/C++ standard compliant?
        auto p = reinterpret_cast<MetaLib_UPtr const *>(word.bytes);
        auto u = *p;
        return u;
    }
#else
#error "This compiler is not supported (yet)."
#endif

public:
    //Zero is ensured not to be dyn
    void setZero() MetaLib_NOEXCEPT
    {
        encodeLE(0);
    }
    static MetaLib_Obj_Word makeZero() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setZero();
        return r;
    }

public:
    MetaLib_UPtr id() const MetaLib_NOEXCEPT
    {
        return decodeLE() & ~Pointer_Flag_Mask;
    }
    bool is(MetaLib_Obj_Word const & rhs) const MetaLib_NOEXCEPT
    {
        return id() == rhs.id();
    }
    bool is_NoOwning(MetaLib_Obj_Word const & rhs) const MetaLib_NOEXCEPT
    {

        return decodeLE() == rhs.decodeLE();
    }

private:
    static MetaLib_CONSTEXPR MetaLib_UPtr const Type_Shift = (sizeof(MetaLib_UPtr) - 1) * 8;
    static MetaLib_CONSTEXPR MetaLib_UPtr const Type_Mask = ((MetaLib_UPtr)0xFF) << Type_Shift;

#if _MSC_VER == 1600
#pragma warning(push)
#pragma warning(disable: 4480)
#endif
    enum Type : MetaLib_U8
    {
        Type_BytesMin  = 0x00,
        Type_BytesMax  = 0x1F,
        Type_Mark      = 0x20,
        Type_Exception = 0x21,
        Type_None      = 0x22,
        Type_Bool      = 0x23,
        Type_Int       = 0x24,
        Type_Float     = 0x25,
        Type_DynPtrMin = 0x80,
        Type_DynPtrMax = 0xFF,
    };
#if _MSC_VER == 1600
#pragma warning(pop)
#endif

    static MetaLib_CONSTEXPR MetaLib_UPtr const Compose_V_Max = ~Type_Mask;

    template <Type T, MetaLib_UPtr V>
    struct Compose
    {
        MetaLib_STATIC_ASSERT(V <= Compose_V_Max, "V is too large.");
        static MetaLib_CONSTEXPR MetaLib_UPtr const U = V | (MetaLib_UPtr(T) << Type_Shift);
    };
    template <MetaLib_UPtr U>
    struct ExtractU
    {
        static MetaLib_CONSTEXPR Type const T = Type(U >> Type_Shift);
        static MetaLib_CONSTEXPR MetaLib_UPtr const V = U & (~Type_Mask);
    };

    void compose(Type t, MetaLib_UPtr v) MetaLib_NOEXCEPT
    {
        MetaLib_ASSERT(v <= Compose_V_Max);
        MetaLib_UPtr u = v | ((MetaLib_UPtr(t) << Type_Shift));
        encodeLE(u);
    }
    MetaLib_UPtr extract() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        u &= (~Type_Mask);
        return u;
    }

    Type type() const MetaLib_NOEXCEPT
    {
        //return Type(bytes[sizeof(MetaLib_UPtr) - 1]);
        return Type(decodeLE() >> Type_Shift);
    }

private:
#if _MSC_VER == 1600
#pragma warning(push)
#pragma warning(disable: 4480)
#endif
    enum Mark : MetaLib_UPtr
    {
        Mark_Null = 0x00,
        Mark_Throw = 0x01,
        Mark_NotCalled = 0x02,
        Mark_NotExist = 0x03,
        Mark_IterEnd = 0x04,
        Mark_WeakObj = 0x05,
    };
#if _MSC_VER == 1600
#pragma warning(pop)
#endif
    static MetaLib_CONSTEXPR MetaLib_UPtr const Mark_Null_U = Compose<Type_Mark, Mark_Null>::U;
    static MetaLib_CONSTEXPR MetaLib_UPtr const Mark_Throw_U = Compose<Type_Mark, Mark_Throw>::U;
    static MetaLib_CONSTEXPR MetaLib_UPtr const Mark_NotCalled_U = Compose<Type_Mark, Mark_NotCalled>::U;
    static MetaLib_CONSTEXPR MetaLib_UPtr const Mark_NotExist_U = Compose<Type_Mark, Mark_NotExist>::U;
    static MetaLib_CONSTEXPR MetaLib_UPtr const Mark_IterEnd_U = Compose<Type_Mark, Mark_IterEnd>::U;
    static MetaLib_CONSTEXPR MetaLib_UPtr const Mark_WeakObj_U = Compose<Type_Mark, Mark_WeakObj>::U;
public:
    bool isMark() const MetaLib_NOEXCEPT
    {
        auto t = type();
        return Type_Mark == t;
    }
    bool isNull() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == Mark_Null_U;
    }
    void setNull() MetaLib_NOEXCEPT
    {
        encodeLE(Mark_Null_U);
    }
    static MetaLib_Obj_Word makeNull() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setNull();
        return r;
    }
    bool isMark_Throw() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == Mark_Throw_U;
    }
    void setMark_Throw() MetaLib_NOEXCEPT
    {
        encodeLE(Mark_Throw_U);
    }
    static MetaLib_Obj_Word makeMark_Throw() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setMark_Throw();
        return r;
    }
    bool isMark_NotCalled() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == Mark_NotCalled_U;
    }
    void setMark_NotCalled() MetaLib_NOEXCEPT
    {
        encodeLE(Mark_NotCalled_U);
    }
    static MetaLib_Obj_Word makeMark_NotCalled() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setMark_NotCalled();
        return r;
    }
    bool isMark_NotExist() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == Mark_NotExist_U;
    }
    void setMark_NotExist() MetaLib_NOEXCEPT
    {
        encodeLE(Mark_NotExist_U);
    }
    static MetaLib_Obj_Word makeMark_NotExist() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setMark_NotExist();
        return r;
    }
    bool isMark_IterEnd() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == Mark_IterEnd_U;
    }
    void setMark_IterEnd() MetaLib_NOEXCEPT
    {
        encodeLE(Mark_IterEnd_U);
    }
    static MetaLib_Obj_Word makeMark_IterEnd() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setMark_IterEnd();
        return r;
    }
    bool isMark_WeakObj() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == Mark_WeakObj_U;
    }
    void setMark_WeakObj() MetaLib_NOEXCEPT
    {
        encodeLE(Mark_WeakObj_U);
    }
    static MetaLib_Obj_Word makeMark_WeakObj() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setMark_WeakObj();
        return r;
    }

private:
#if _MSC_VER == 1600
#pragma warning(push)
#pragma warning(disable: 4480)
#endif
    enum Exception : MetaLib_UPtr
    {
        Exception_Root = 0x00,
        Exception_General = 0x01,
    };
#if _MSC_VER == 1600
#pragma warning(pop)
#endif
    static MetaLib_CONSTEXPR MetaLib_UPtr const GeneralException_U = Compose<Type_Exception, Exception_General>::U;
public:
    bool isGeneralException() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == GeneralException_U;
    }
    void setGeneralException() MetaLib_NOEXCEPT
    {
        encodeLE(GeneralException_U);
    }
    static MetaLib_Obj_Word makeGeneralException() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setGeneralException();
        return r;
    }
    bool isException() const MetaLib_NOEXCEPT
    {
        auto t = type();
        return Type_Exception == t;
    }

private:
    static MetaLib_CONSTEXPR MetaLib_UPtr const None_U = Compose<Type_None, 0>::U;
public:
    bool isNone() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == None_U;
    }
    void setNone() MetaLib_NOEXCEPT
    {
        encodeLE(None_U);
    }
    static MetaLib_Obj_Word makeNone() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setNone();
        return r;
    }

private:
    static MetaLib_CONSTEXPR MetaLib_UPtr const False_U = Compose<Type_Bool, 0>::U;
    static MetaLib_CONSTEXPR MetaLib_UPtr const True_U = Compose<Type_Bool, 1>::U;
public:
    bool isFalse() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == False_U;
    }
    void setFalse() MetaLib_NOEXCEPT
    {
        encodeLE(False_U);
    }
    static MetaLib_Obj_Word makeFalse() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setFalse();
        return r;
    }
    bool isTrue() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        return u == True_U;
    }
    void setTrue() MetaLib_NOEXCEPT
    {
        encodeLE(True_U);
    }
    static MetaLib_Obj_Word makeTrue() MetaLib_NOEXCEPT
    {
        MetaLib_Obj_Word r;
        r.setTrue();
        return r;
    }
    bool isBool() const MetaLib_NOEXCEPT
    {
        auto t = type();
        return Type_Bool == t;
    }
    void setBool(bool v) MetaLib_NOEXCEPT
    {
        return v ? setTrue() : setFalse();
    }
    static MetaLib_Obj_Word makeBool(bool v) MetaLib_NOEXCEPT
    {
        if (v)
            return makeTrue();
        else
            return makeFalse();
    }

public:
    typedef MetaLib_SPtr IntValue;
    static MetaLib_CONSTEXPR IntValue const Int_Max = (((MetaLib_UPtr)1) << Type_Shift) - 1;
    static MetaLib_CONSTEXPR IntValue const Int_Min = -Int_Max - 1;
    bool isInt() const MetaLib_NOEXCEPT
    {
        auto t = type();
        return Type_Int == t;
    }
    void setInt(IntValue s) MetaLib_NOEXCEPT
    {
        MetaLib_ASSERT(Int_Min <= s && s <= Int_Max);
        MetaLib_UPtr v = s;
        v &= ~Type_Mask;
        compose(Type_Int, v);
    }
    IntValue getInt() const MetaLib_NOEXCEPT
    {
        auto v = decodeLE();
        v <<= 8;

        auto u2s = [](MetaLib_UPtr x) -> MetaLib_SPtr
        {
            //https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior

            auto const min = std::numeric_limits<MetaLib_SPtr>::min();
            auto const max = std::numeric_limits<MetaLib_SPtr>::max();

            if (x <= static_cast<MetaLib_UPtr>(max))
                return static_cast<MetaLib_SPtr>(x);

            if (x >= static_cast<MetaLib_UPtr>(min))
                return static_cast<MetaLib_SPtr>(x - min) + min;

            throw x;
        };

        auto r = u2s(v);
        r >>= 8;

        return r;
    }

public:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127)
#endif
    bool isFloat() const MetaLib_NOEXCEPT
    {
        if (sizeof(MetaLib_UPtr) <= sizeof(MetaLib_F32))
            return false;
        auto t = type();
        return Type_Float == t;
    }
    static MetaLib_CONSTEXPR bool isFloatFit(MetaLib_F32)
    {
        return sizeof(MetaLib_UPtr) > sizeof(MetaLib_F32);
    }
    template <typename F>
    static MetaLib_CONSTEXPR
        typename std::enable_if<std::is_floating_point<F>::value, bool>::type
        isFloatFit(F v) MetaLib_NOEXCEPT
    {
        return v == MetaLib_F32(v);
    }
    MetaLib_F64 getFloat() const MetaLib_NOEXCEPT
    {
        return *reinterpret_cast<MetaLib_F32 const *>(word.bytes);
    }
    template <typename F>
    typename std::enable_if<std::is_floating_point<F>::value>::type
        setFloat(F v) MetaLib_NOEXCEPT
    {
        encodeLE(0);
        MetaLib_ASSERT(sizeof(MetaLib_UPtr) > sizeof(MetaLib_F32));
        auto t = static_cast<MetaLib_F32>(v);
        new(word.bytes) MetaLib_F32(t);
        word.bytes[sizeof(MetaLib_UPtr) - 1] = Type_Float;
    }
#ifdef _MSC_VER
#pragma warning(pop)
#endif

private:
    static MetaLib_CONSTEXPR MetaLib_UPtr const Pointer_Mask = ((MetaLib_UPtr)1) << (sizeof(MetaLib_UPtr) * 8 - 1);
    static MetaLib_CONSTEXPR MetaLib_UPtr const Pointer_Flag_Mask = ((MetaLib_UPtr)1) << (sizeof(MetaLib_UPtr) * 8 - 2);
public:
    bool isDyn() const MetaLib_NOEXCEPT
    {
        //auto t = type();
        //return Type_DynPtrMin <= t && t <= Type_DynPtrMax;
        return ((decodeLE() & Pointer_Mask) != 0);
    }
    MetaLib_Dyn * getDyn() const MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        u <<= 2;
        return reinterpret_cast<MetaLib_Dyn *>(u);
    }
    void setDyn(MetaLib_Dyn * p, bool owning = false) MetaLib_NOEXCEPT
    {
        auto u = reinterpret_cast<MetaLib_UPtr>(p);
        MetaLib_ASSERT((u & 0x3) == 0);
        u |= 0x02;
        if (owning)
            u |= 0x01;

        //TODO: wrap this in MetaLibPort.h
#if defined(_M_IX86) || defined(__i386__)
        u = _rotr(u, 2);
#elif defined(_M_AMD64) || defined(__x86_64__)
        u = _rotr64(u, 2);
#else
#error "?"
#endif

        encodeLE(u);
    }
    bool getDynOwning() const MetaLib_NOEXCEPT
    {
        return ((decodeLE() & Pointer_Flag_Mask) != 0);
    }
    void setDynOwning() MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        u |= Pointer_Flag_Mask;
        encodeLE(u);
    }
    void clearDynOwning() MetaLib_NOEXCEPT
    {
        auto u = decodeLE();
        u &= ~Pointer_Flag_Mask;
        encodeLE(u);
    }
    bool isOwningDyn() const MetaLib_NOEXCEPT
    {
        auto mask = Pointer_Mask | Pointer_Flag_Mask;
        auto u = decodeLE();
        //return (u & mask) == mask;
        return u >= mask;
    }

public:
    void incDynContentRef() const MetaLib_NOEXCEPT
    {
        return getDyn()->incContentRef();
    }
    void decDynContentRef() const MetaLib_NOEXCEPT
    {
        return getDyn()->decContentRef();
    }
    void incDynMemoryRef() const MetaLib_NOEXCEPT
    {
        return getDyn()->incMemoryRef();
    }
    void decDynMemoryRef() const MetaLib_NOEXCEPT
    {
        return getDyn()->decMemoryRef();
    }

public:
    static MetaLib_CONSTEXPR size_t const Bytes_Max_Size = sizeof(MetaLib_UPtr) - 1;
    bool isBytes() const MetaLib_NOEXCEPT
    {
        //TODO: return decodeLE() < ...
        auto t = type();
        return Type_BytesMin <= t && t <= Type_BytesMax;
    }
    MetaLib_U8 const * getBytesPtr() const MetaLib_NOEXCEPT
    {
        return word.bytes;
    }
    size_t getBytesLen() const MetaLib_NOEXCEPT
    {
        //return (unsigned char)(((unsigned char)sizeof(MetaLib_UPtr)) - bytes[sizeof(MetaLib_UPtr) - 1] - (unsigned char)1);
        return sizeof(MetaLib_UPtr) - word.bytes[sizeof(MetaLib_UPtr) - 1] - 1;
    }
    template <typename C>
    typename std::enable_if<std::is_integral<C>::value && sizeof(C) == 1>::type setBytes(C const * p, size_t n) MetaLib_NOEXCEPT
    {
        MetaLib_ASSERT(n <= Bytes_Max_Size);
        if (n > Bytes_Max_Size)
            n = Bytes_Max_Size;

        //TODO: compliance, optimization, and performance for dynamic n
#if 0
        //This is compliant, but not optimized(MSVC2015 RC).
        auto f = [&](size_t i) -> MetaLib_U8
        {
            if (i == sizeof(MetaLib_UPtr) - 1)
                return bytes[i] = static_cast<MetaLib_U8>(sizeof(MetaLib_UPtr) - n - 1);
            if (i >= n)
                return bytes[i] = 0;
            return bytes[i] = p[i];
        };
        switch (sizeof(MetaLib_UPtr) - 1)
        {
        default:
        case 0x1F: f(0x1F);
        case 0x1E: f(0x1E);
        case 0x1D: f(0x1D);
        case 0x1C: f(0x1C);
        case 0x1B: f(0x1B);
        case 0x1A: f(0x1A);
        case 0x19: f(0x19);
        case 0x18: f(0x18);
        case 0x17: f(0x17);
        case 0x16: f(0x16);
        case 0x15: f(0x15);
        case 0x14: f(0x14);
        case 0x13: f(0x13);
        case 0x12: f(0x12);
        case 0x11: f(0x11);
        case 0x10: f(0x10);
        case 0x0F: f(0x0F);
        case 0x0E: f(0x0E);
        case 0x0D: f(0x0D);
        case 0x0C: f(0x0C);
        case 0x0B: f(0x0B);
        case 0x0A: f(0x0A);
        case 0x09: f(0x09);
        case 0x08: f(0x08);
        case 0x07: f(0x07);
        case 0x06: f(0x06);
        case 0x05: f(0x05);
        case 0x04: f(0x04);
        case 0x03: f(0x03);
        case 0x02: f(0x02);
        case 0x01: f(0x01);
        case 0x00: f(0x00);
            break;
        }
#else
        //This is possibly optimized, but not compliant.
        MetaLib_UPtr v = 0;
        auto f = [&](size_t i)
        {
            if (i == sizeof(MetaLib_UPtr) - 1)
            {
                auto t = static_cast<MetaLib_UPtr>(sizeof(MetaLib_UPtr) - n - 1);
                t <<= 8 * (sizeof(MetaLib_UPtr) - 1);
                v += t;
                return;
            }
            if (i < n)
            {
                auto c = static_cast<MetaLib_U8>(p[i]);
                auto t = static_cast<MetaLib_UPtr>(c);
                t <<= 8 * i;
                v += t;
            }
        };
        f(0x1F);
        f(0x1E);
        f(0x1D);
        f(0x1C);
        f(0x1B);
        f(0x1A);
        f(0x19);
        f(0x18);
        f(0x17);
        f(0x16);
        f(0x15);
        f(0x14);
        f(0x13);
        f(0x12);
        f(0x11);
        f(0x10);
        f(0x0F);
        f(0x0E);
        f(0x0D);
        f(0x0C);
        f(0x0B);
        f(0x0A);
        f(0x09);
        f(0x08);
        f(0x07);
        f(0x06);
        f(0x05);
        f(0x04);
        f(0x03);
        f(0x02);
        f(0x01);
        f(0x00);
        encodeLE(v);
#endif
    }
    template <typename C, size_t N>
    typename std::enable_if<std::is_integral<C>::value && sizeof(C) == 1>::type setBytes(C const (&s)[N]) MetaLib_NOEXCEPT
    {
        MetaLib_STATIC_ASSERT(N > 0, "Illegal bytes length.");
        MetaLib_STATIC_ASSERT(N - 1 <= Bytes_Max_Size, "The bytes is too long.");

        return setBytes(s, N - 1);
    }

public:
    void clear() MetaLib_NOEXCEPT
    {
        setNone();
    }

public:
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    auto c(::MetaLib::CallKey key, A && ... a) const
    {
        return ::MetaLib::call(*this, key, std::forward<A>(a)...);
    }
    template <typename ... A>
    auto operator()(A && ... a) const
    {
        return ::MetaLib::call(*this, MetaLib_CK(""), std::forward<A>(a)...);
    }
#else
    ::MetaLib::O c(::MetaLib::CallKey key) const { return ::MetaLib::call(*this, key); }
    template <typename A0> ::MetaLib::O c(::MetaLib::CallKey key, A0 && a0) const { return ::MetaLib::call(*this, key, std::forward<A0>(a0)); }
    template <typename A0, typename A1> ::MetaLib::O c(::MetaLib::CallKey key, A0 && a0, A1 && a1) const { return ::MetaLib::call(*this, key, std::forward<A0>(a0), std::forward<A1>(a1)); }

    ::MetaLib::O operator()() const { return ::MetaLib::call(*this, MetaLib_CK("")); }
    template <typename A0> ::MetaLib::O operator()(A0 && a0) const { return ::MetaLib::call(*this, MetaLib_CK(""), std::forward<A0>(a0)); }
    template <typename A0, typename A1> ::MetaLib::O operator()(A0 && a0, A1 && a1) const { return ::MetaLib::call(*this, MetaLib_CK(""), std::forward<A0>(a0), std::forward<A1>(a1)); }
#endif

public:
    ::MetaLib::OObj<MetaLib_Obj_Word> toO() const MetaLib_NOEXCEPT
    {
        return ::MetaLib::OObj<MetaLib_Obj_Word>(*this);
    }
    ::MetaLib::OObj<MetaLib_Obj_Word> toO_Owned() const MetaLib_NOEXCEPT
    {
        return ::MetaLib::OObj<MetaLib_Obj_Word>(::MetaLib::OObj<MetaLib_Obj_Word>::Owned(), *this);
    }
    operator ::MetaLib::OObj<MetaLib_Obj_Word>() const MetaLib_NOEXCEPT
    {
        return toO();
    }

#endif
};

#else
#error "This architecture is not supported (yet)."
#endif

#ifdef __cplusplus
static_assert(std::is_pod<MetaLib_Obj_Word>::value, "MetaLib_Obj_Word ought to be POD!");
#endif


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Type"
#endif
/*************************************************************************************************************************************/

struct MetaLib_Type_
{
    //Dynamic
    //MetaLib_Dyn dyn;//TODO: Separate Storage and Dynamic Unit?

    //vtable
    MetaLib_Op_Call * p_call;
    MetaLib_Op_Cast * p_cast;
    MetaLib_Op_Kill * p_kill;//TODO: exception?

    //others
};


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Calling"
#endif
/*************************************************************************************************************************************/

#ifdef __cplusplus

namespace MetaLibObjectPrivate
{
;

MetaLib_INLINE MetaLib_Obj callRaw(MetaLib_Obj obj, CallKey key, MetaLib_Obj * argv) MetaLib_NOEXCEPT
{
    MetaLib_Obj r;
    if (obj.isDyn())
    {
        auto d = obj.getDyn();
        r = d->call(key, argv);
        return r;
    }
    else
    {
        //TODO: call embedded type
        return MetaLib_Obj::makeMark_NotCalled();
    }
}


MetaLib_INLINE void clearArg(MetaLib_Obj & a) MetaLib_NOEXCEPT
{
    if (a.isOwningDyn())
        a.decDynContentRef();
    a.setNone();
}

template <size_t Array_Size>
MetaLib_INLINE void clearArgArrayCommon(std::array<MetaLib_Obj, Array_Size> & argv) MetaLib_NOEXCEPT
{
    argv[0].setNull();//exception object should have been taken
    clearArg(argv[1]);
    argv[2].setNull();//should not have been modified
    //argv[3].setNull();//should not have been modified
    argv[Array_Size - 1].setNull();//should not have been modified
}
#if MetaLib_HAS_VARIADIC_TEMPLATE
template <size_t Array_Size, size_t ... I>
MetaLib_INLINE void clearArgArrayImpl(std::index_sequence<I ...>, std::array<MetaLib_Obj, Array_Size> & argv) MetaLib_NOEXCEPT
{
    clearArgArrayCommon(argv);
    MetaLibObjectPrivate::pass({ (clearArg(argv[3 + I]), 0) ... });
}
template <size_t Array_Size>
MetaLib_INLINE void clearArgArray(std::array<MetaLib_Obj, Array_Size> & argv) MetaLib_NOEXCEPT
{
    return clearArgArrayImpl(std::make_index_sequence<Array_Size - 4>(), argv);
}
#else
template <size_t Array_Size>
MetaLib_INLINE void clearArgArray(std::array<MetaLib_Obj, Array_Size> & argv) MetaLib_NOEXCEPT
{
    clearArgArrayCommon(argv);
    for (size_t i = 3; i < Array_Size - 1; ++i)
        clearArg(argv[i]);
}
#endif


template <size_t Array_Size>
MetaLib_INLINE ::MetaLib::O makeResult(MetaLib_Obj ret, std::array<MetaLib_Obj, Array_Size> & argv)
{
    if (ret.isMark())
    {
        auto e = argv[0];//with ownership
        argv[0].setNone();
        clearArgArray(argv);
        if (ret.isMark_Throw())
        {
            MetaLib_ASSERT(false == e.isMark());
            throw ::MetaLib::O(::MetaLib::O::Owned(), e);
        }
        throw ::MetaLib::O::GeneralException;//TODO: make a real exception
    }

    clearArgArray(argv);
    return ::MetaLib::O(::MetaLib::O::Owned(), ret);
}


//TODO: noexcept

template <typename T>
typename std::enable_if<IsObjType<T>::value, MetaLib_Obj>::type
MetaLib_INLINE makeArg(T const & a)
{
    return a;
}

template <typename T>
typename std::enable_if<IsAnOObj<T>::value, MetaLib_Obj>::type
MetaLib_INLINE makeArg(T const & a)
{
    return a.getObj();
}

template <typename T>
typename std::enable_if<IsAnOObj<T>::value, MetaLib_Obj>::type
MetaLib_INLINE makeArg(T const && a)
{
    return a.getObj();
}

template <typename T>
typename std::enable_if<IsAnOObj<typename std::decay<T>::type>::value && std::is_rvalue_reference<T &&>::value, MetaLib_Obj>::type
MetaLib_INLINE makeArg(T && a)
{
    auto r = a.release();
    if (r.isDyn())
        r.setDynOwning();
    return r;
}

template <size_t Array_Size>
MetaLib_INLINE void fillArgArrayCommon(std::array<MetaLib_Obj, Array_Size> & argv) MetaLib_NOEXCEPT
{
#if MetaLib_HAS_VARIADIC_TEMPLATE
#else
    auto zero = MetaLib_Obj::makeZero();
    argv.fill(zero);
#endif
    argv[0].setNull();//exception
    argv[1].setNull();//reserved
    argv[2].setInt(Array_Size - 4);//argc
    argv[Array_Size - 1].setNull();//end
}

#if MetaLib_HAS_VARIADIC_TEMPLATE

template <size_t Array_Size, size_t ... I, typename ... A>
MetaLib_INLINE void fillArgArrayImpl(std::index_sequence<I ...>, std::array<MetaLib_Obj, Array_Size> & argv, A && ... a)
{
    MetaLib_STATIC_ASSERT(Array_Size == (sizeof...(A)) + 4, "Unexpected array size.");

    fillArgArrayCommon(argv);

    size_t count = 0;
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        MetaLibObjectPrivate::pass({ (argv[3 + I] = makeArg(std::forward<A>(a)), count = I + 1) ... });
    }
    catch (...)
    {
        for (; count < Array_Size; ++count)
            argv[count].setNull();
        clearArgArray(argv);
        throw;
    }
}
template <size_t Array_Size, typename ... A>
MetaLib_INLINE void fillArgArray(std::array<MetaLib_Obj, Array_Size> & argv, A && ... a)
{
    return fillArgArrayImpl(std::make_index_sequence<sizeof...(A)>(), argv, std::forward<A>(a) ...);
}

template <typename Obj, typename ... A>
typename std::enable_if<::MetaLibObjectPrivate::IsObjType<Obj>::value, ::MetaLib::O>::type
MetaLib_INLINE call(Obj const & obj, CallKey key, A && ... a)
{
    MetaLib_STATIC_ASSERT((sizeof...(A)) + 5 <= MetaLib_Obj::Int_Max, "Too many args.");
    std::array<MetaLib_Obj, 5 + sizeof...(A)> argv;

    fillArgArray(argv, obj, std::forward<A>(a) ...);

    auto r = callRaw(obj, key, argv.data());

    return makeResult(r, argv);
}

#else

template <typename Obj>
typename std::enable_if<::MetaLibObjectPrivate::IsObjType<Obj>::value, ::MetaLib::O>::type
MetaLib_INLINE call(Obj const & obj, CallKey key)
{
    std::array<MetaLib_Obj, 4 + 1> argv;
    fillArgArrayCommon(argv);
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;
        argv[3 + 0] = makeArg(obj);
    }
    catch (...)
    {
        clearArgArray(argv);
        throw;
    }
    auto r = callRaw(obj, key, argv.data());
    return makeResult(r, argv);
}

template <typename Obj, typename A0>
typename std::enable_if<::MetaLibObjectPrivate::IsObjType<Obj>::value, ::MetaLib::O>::type
MetaLib_INLINE call(Obj const & obj, CallKey key, A0 && a0)
{
    std::array<MetaLib_Obj, 4 + 2> argv;
    fillArgArrayCommon(argv);
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;
        argv[3 + 0] = makeArg(obj);
        argv[3 + 1] = makeArg(std::forward<A0>(a0));
    }
    catch (...)
    {
        clearArgArray(argv);
        throw;
    }
    auto r = callRaw(obj, key, argv.data());
    return makeResult(r, argv);
}

template <typename Obj, typename A0, typename A1>
typename std::enable_if<::MetaLibObjectPrivate::IsObjType<Obj>::value, ::MetaLib::O>::type
MetaLib_INLINE call(Obj const & obj, CallKey key, A0 && a0, A1 && a1)
{
    std::array<MetaLib_Obj, 4 + 3> argv;
    fillArgArrayCommon(argv);
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;
        argv[3 + 0] = makeArg(obj);
        argv[3 + 1] = makeArg(std::forward<A0>(a0));
        argv[3 + 2] = makeArg(std::forward<A1>(a1));
    }
    catch (...)
    {
        clearArgArray(argv);
        throw;
    }
    auto r = callRaw(obj, key, argv.data());
    return makeResult(r, argv);
}

#endif

;
}

#endif


#endif
