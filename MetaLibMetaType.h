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


#ifndef MetaLibMetaType_H_
#define MetaLibMetaType_H_


#include "MetaLibObject.h"
#include "MetaLibScopeGuard.h"
#include <map>
#include <string>
#include <functional>
#include <type_traits>
#include <utility>
#include <limits>
#include <tuple>


namespace MetaLibMetaTypePrivate
{
;

template <typename T>
void pass(std::initializer_list<T>)
{}


template <typename Ox_, typename Access_>
class OxAccess : public Ox_, public Access_
{
public:
    typedef Ox_ Ox;
    typedef Access_ Access;
public:
    Ox & ox()
    {
        return *this;
    }
    Ox const & ox() const
    {
        return *this;
    }
    Access & access()
    {
        return *this;
    }
    Access const & access() const
    {
        return *this;
    }
};

template <typename T> struct IsOxAccess : std::false_type {};
template <typename Ox, typename Access> struct IsOxAccess<OxAccess<Ox, Access>> : std::true_type {};

template <typename T> struct IsObjAccess : std::false_type {};
template <typename Ox, typename Access> struct IsObjAccess<OxAccess<Ox, Access>> : ::MetaLib::IsObjType<Ox> {};

template <typename T> struct IsOObjAccess : std::false_type {};
template <typename Ox, typename Access> struct IsOObjAccess<OxAccess<Ox, Access>> : ::MetaLib::IsOObjType<Ox> {};


class EmptyType : protected MetaLib_Type
{
protected:
    EmptyType()
    {
        MetaLib_Type::p_call = &call;
        MetaLib_Type::p_cast = &cast;
        MetaLib_Type::p_kill = &kill;
    }
    static MetaLib_Obj call(
        MetaLib_U8 * /*ptr*/,
        MetaLib_Type * /*type*/,
        MetaLib_USz /*key_length*/, MetaLib_U8 const * /*key_pointer*/,
        MetaLib_Obj * /*argv*/
    ) MetaLib_NOEXCEPT
    {
        return MetaLib_Obj::makeMark_NotCalled();
    }
    static MetaLib_U8 * cast(MetaLib_U8 * /*ptr*/, MetaLib_Type * /*type*/, MetaLib_Type * /*dest_type*/) MetaLib_NOEXCEPT
    {
        return nullptr;
    }
    static void kill(MetaLib_U8 * /*ptr*/, MetaLib_Type * /*type*/) MetaLib_NOEXCEPT
    {
        return;//for trivial dtors
    }
};


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Type Mixins"
#endif
/*************************************************************************************************************************************/

template <typename Type, typename Access, bool Return_Access = true>
class ObjectCreatingMixin
{
    //MetaLib_STATIC_ASSERT((std::is_base_of<ObjectCreatingMixin, Type>::value), "Type must be derived from ObjectCreatingMixin<Type, ...>");
    typedef OxAccess<::MetaLib::O, Access> OA;
protected:
    typedef ObjectCreatingMixin CreatingMixin;
public:
    typedef Access CreatingAccess;
public://TODO: some of these methods should be protected
#if MetaLib_HAS_VARIADIC_TEMPLATE
    //"Ac" is for VS2010. It has a bug that overided methods does not precede 'using'ed methods in function overload resolution.
    template <typename Obj, typename Ac, typename ... A>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value>::type
        build(Obj & obj, Ac && p_ac, A && ... a)
    {
        Access * p_access = p_ac;
        auto concrete = static_cast<Type *>(this);
        size_t size, align;
        std::tie(size, align) = concrete->alloc(p_access, std::forward<A>(a) ...);
        auto d = MetaLib_Dyn::make(size, align);
        auto type = concrete->init(p_access, d->getPtr(), std::forward<A>(a) ...);
        d->type = type;
        obj.setDyn(d.release());
    }
#else
    template <typename Obj, typename Ac>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value>::type
        build(Obj & obj, Ac && p_ac)
    {
        Access * p_access = p_ac;
        auto concrete = static_cast<Type *>(this);
        size_t size, align;
        std::tie(size, align) = concrete->alloc(p_access);
        auto d = MetaLib_Dyn::make(size, align);
        auto type = concrete->init(p_access, d->getPtr());
        d->type = type;
        obj.setDyn(d.release());
    }
    template <typename Obj, typename Ac, typename A0>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value>::type
        build(Obj & obj, Ac && p_ac, A0 && a0)
    {
        Access * p_access = p_ac;
        auto concrete = static_cast<Type *>(this);
        size_t size, align;
        std::tie(size, align) = concrete->alloc(p_access, std::forward<A0>(a0));
        auto d = MetaLib_Dyn::make(size, align);
        auto type = concrete->init(p_access, d->getPtr(), std::forward<A0>(a0));
        d->type = type;
        obj.setDyn(d.release());
    }
    template <typename Obj, typename Ac, typename A0, typename A1>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<Obj>::value>::type
        build(Obj & obj, Ac && p_ac, A0 && a0, A1 && a1)
    {
        Access * p_access = p_ac;
        auto concrete = static_cast<Type *>(this);
        size_t size, align;
        std::tie(size, align) = concrete->alloc(p_access, std::forward<A0>(a0), std::forward<A1>(a1));
        auto d = MetaLib_Dyn::make(size, align);
        auto type = concrete->init(p_access, d->getPtr(), std::forward<A0>(a0), std::forward<A1>(a1));
        d->type = type;
        obj.setDyn(d.release());
    }
#endif
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    OA makeOA(A && ... a)
    {
        auto concrete = static_cast<Type *>(this);
        OA r;
        MetaLib_Obj obj;
        concrete->build(obj, static_cast<Access *>(&r), std::forward<A>(a) ...);
        static_cast<::MetaLib::O &>(r) = ::MetaLib::O(::MetaLib::O::Owned(), obj);
        return r;
    }
#else
    OA makeOA()
    {
        auto concrete = static_cast<Type *>(this);
        OA r;
        MetaLib_Obj obj;
        concrete->build(obj, static_cast<Access *>(&r));
        static_cast<::MetaLib::O &>(r) = ::MetaLib::O(::MetaLib::O::Owned(), obj);
        return r;
    }
    template <typename A0>
    OA makeOA(A0 && a0)
    {
        auto concrete = static_cast<Type *>(this);
        OA r;
        MetaLib_Obj obj;
        concrete->build(obj, static_cast<Access *>(&r), std::forward<A0>(a0));
        static_cast<::MetaLib::O &>(r) = ::MetaLib::O(::MetaLib::O::Owned(), obj);
        return r;
    }
    template <typename A0, typename A1>
    OA makeOA(A0 && a0, A1 && a1)
    {
        auto concrete = static_cast<Type *>(this);
        OA r;
        MetaLib_Obj obj;
        concrete->build(obj, static_cast<Access *>(&r), std::forward<A0>(a0), std::forward<A1>(a1));
        static_cast<::MetaLib::O &>(r) = ::MetaLib::O(::MetaLib::O::Owned(), obj);
        return r;
    }
#endif
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    ::MetaLib::O makeO(A && ... a)
    {
        auto concrete = static_cast<Type *>(this);
        MetaLib_Obj obj;
        Access access;
        concrete->build(obj, &access, std::forward<A>(a) ...);
        return ::MetaLib::O(::MetaLib::O::Owned(), obj);
    }
#else
    ::MetaLib::O makeO()
    {
        auto concrete = static_cast<Type *>(this);
        MetaLib_Obj obj;
        Access access;
        concrete->build(obj, &access);
        return ::MetaLib::O(::MetaLib::O::Owned(), obj);
    }
    template <typename A0>
    ::MetaLib::O makeO(A0 && a0)
    {
        auto concrete = static_cast<Type *>(this);
        MetaLib_Obj obj;
        Access access;
        concrete->build(obj, &access, std::forward<A0>(a0));
        return ::MetaLib::O(::MetaLib::O::Owned(), obj);
    }
    template <typename A0, typename A1>
    ::MetaLib::O makeO(A0 && a0, A1 && a1)
    {
        auto concrete = static_cast<Type *>(this);
        MetaLib_Obj obj;
        Access access;
        concrete->build(obj, &access, std::forward<A0>(a0), std::forward<A1>(a1));
        return ::MetaLib::O(::MetaLib::O::Owned(), obj);
    }
#endif
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    typename std::enable_if<Return_Access && sizeof(std::tuple<A ...>) >= 0, OA>::type
        operator()(A && ... a)
    {
        return makeOA(std::forward<A>(a) ...);
    }
    template <typename ... A>
    typename std::enable_if<(!Return_Access) && sizeof(std::tuple<A ...>) >= 0, ::MetaLib::O>::type
        operator()(A && ... a)
    {
        return makeO(std::forward<A>(a) ...);
    }
#else
private:
    template <typename A>
    typename std::enable_if<Return_Access && sizeof(A) >= 0, OA>::type
        call_0(A &&)
    {
        return makeOA();
    }
    template <typename A>
    typename std::enable_if<(!Return_Access) && sizeof(A) >= 0, ::MetaLib::O>::type
        call_0(A &&)
    {
        return makeO();
    }
public:
    typename std::conditional<Return_Access, OA, ::MetaLib::O>::type
        operator()()
    {
        return call_0(0);
    }
public:
    template <typename A0>
    typename std::enable_if<Return_Access && sizeof(A0) >= 0, OA>::type
        operator()(A0 && a0)
    {
        return makeOA(std::forward<A0>(a0));
    }
    template <typename A0, typename A1>
    typename std::enable_if<Return_Access && sizeof(A0) >= 0, OA>::type
        operator()(A0 && a0, A1 && a1)
    {
        return makeOA(std::forward<A0>(a0), std::forward<A1>(a1));
    }
    template <typename A0>
    typename std::enable_if<(!Return_Access) && sizeof(A0) >= 0, ::MetaLib::O>::type
        operator()(A0 && a0)
    {
        return makeO(std::forward<A0>(a0));
    }
    template <typename A0, typename A1>
    typename std::enable_if<(!Return_Access) && sizeof(A0) >= 0, ::MetaLib::O>::type
        operator()(A0 && a0, A1 && a1)
    {
        return makeO(std::forward<A0>(a0), std::forward<A1>(a1));
    }
#endif
};


/***********************************************************************************
 *
 *           o          obj         dyn         pti         pte
 *    access # >>>>>>>>> #           *           *           *
 *                       H           |           |           |
 *                       H   ,=======+===========+========,  |
 *                       H  /        |           |         \ |
 *                       V /         V           V          VV
 *  retrieve # ========> @ --------> * --------> * --------> @
 *                       | _\        |\          |          7
 *                       |   _\      | \         |         /
 *                       |     _\    |  '--------+--------'
 *                       V       _\  V           V
 *    locate * --------> * --------> # ========> @
 *
 * pti: ptr-type of inherit type
 * pte: ptr-type of exact type
 * @,#: existing method
 *  * : possible method in the future
 * ==>: current calling, "virtually"
 * >>>: current calling, "non-virtually", may change to "virtually" in the future
 * -->: possible calling in the future
***********************************************************************************/

template <typename Type, typename Access>
class ObjectExposingMixin
{
    //MetaLib_STATIC_ASSERT((std::is_base_of<ObjectExposingMixin, Type>::value), "Type must be derived from ObjectExposingMixin<Type, ...>");
    typedef OxAccess<::MetaLib::O, Access> OA;
protected:
    typedef ObjectExposingMixin ExposingMixin;
public:
    typedef Access ExposingAccess;
public://TODO: some of these methods should be protected
    MetaLib_Type * getCastType()
    {
        auto concrete = static_cast<Type *>(this);
        return static_cast<MetaLib_Type *>(concrete);
    }
public:
    std::pair<MetaLib_U8 *, MetaLib_Type *> locate(MetaLib_U8 * ptr, MetaLib_Type * type)//pti
    {
        auto concrete = static_cast<Type *>(this);
        auto dest_type = concrete->getCastType();
        MetaLib_U8 * dest_ptr = ptr;
        if (dest_type != type)
            dest_ptr = type->p_cast(ptr, type, dest_type);
        //return locate(dest_ptr, dest_type, nullptr);
        return std::make_pair(dest_ptr, dest_type);
    }
    std::pair<MetaLib_U8 *, MetaLib_Type *> locate(MetaLib_Dyn * dyn)
    {
        auto concrete = static_cast<Type *>(this);
        return concrete->locate(dyn->getPtr(), dyn->type);
    }
public:
    //retrieve returns true if success
    //"Ac" is for VS2010. It has a bug that overided methods does not precede 'using'ed methods in function overload resolution.
    template <typename Ac>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::OKType<Ac>::value, bool>::type
        retrieve(Ac && p_ac, MetaLib_U8 * ptr, MetaLib_Type * type, std::nullptr_t)//pte
    {
        Access * p_access = p_ac;
        p_access->bind(ptr, type);
        return true;
    }
    template <typename Ac, typename Obj>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsObjType<typename std::decay<Obj>::type>::value, bool>::type
        retrieve(Ac && p_ac, Obj && obj)
    {
        Access * p_access = p_ac;
        auto concrete = static_cast<Type *>(this);
        if (obj.isDyn())
        {
            auto d = obj.getDyn();
            MetaLib_U8 * ptr = nullptr;
            MetaLib_Type * type = nullptr;
            std::tie(ptr, type) = locate(d);
            if (ptr)
                return concrete->retrieve(p_access, ptr, type, nullptr);
        }
        return false;
    }
    template <typename Ac, typename OObj>//TODO: overload resolution !!!HACK!!!
    typename std::enable_if<::MetaLib::IsAnOObj<typename std::decay<OObj>::type>::value, bool>::type
        retrieve(Ac && p_ac, OObj && o)
    {
        Access * p_access = p_ac;
        auto concrete = static_cast<Type *>(this);
        return concrete->retrieve(p_access, o.getObj());
    }
public:
    template <typename Obj>
    typename std::enable_if<::MetaLib::IsObjType<typename std::decay<Obj>::type>::value, OA>::type//O is for newly created object, if any.
        access(Obj && obj)
    {
        OA r;
        //static_cast<::MetaLib::O &>(r) = ::MetaLib::O::None;//r is default constructed, i.e. None.
        auto concrete = static_cast<Type *>(this);
        if (concrete->retrieve(static_cast<Access *>(&r), std::forward<Obj>(obj)))
            return r;
        throw 1;//TODO
    }
    template <typename OObj>
    typename std::enable_if<::MetaLib::IsAnOObj<typename std::decay<OObj>::type>::value, OA>::type
        access(OObj && o)
    {
        return access(o.getObj());
    }
};


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Dummy Type"
#endif
/*************************************************************************************************************************************/

struct DummyAccess
{
public:
    void clear()
    {}
    void bind(MetaLib_U8 *, MetaLib_Type *)
    {}
};

template <typename T = void>//static data member of a class template can be defined in header file
class DummyTypeImpl//not inheritable
    :
    private EmptyType,
    public ObjectCreatingMixin<DummyTypeImpl<T>, DummyAccess>,
    public ObjectExposingMixin<DummyTypeImpl<T>, DummyAccess>
{
public:
    static DummyTypeImpl Static;
public:
    std::pair<size_t, size_t> alloc(DummyAccess * /*p_access*/) MetaLib_NOEXCEPT
    {
        return std::make_pair(1, 1);
    }
    MetaLib_Type * init(DummyAccess * /*p_access*/, MetaLib_U8 * ptr)
    {
        *ptr = 'D';
        return this;
    }
};
template <typename T> DummyTypeImpl<T> DummyTypeImpl<T>::Static;

typedef DummyTypeImpl<void> DummyType;
static auto & Dummy_Type = DummyType::Static;


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Compound Type"
#endif
/*************************************************************************************************************************************/

template <typename Access, size_t I, bool Visible = true> class AccessFactor;
template <typename A, size_t I>
class AccessFactor<A, I, true>
    :
    public A
{
protected:
    typedef A Access;
    Access & getAccess()
    {
        return *this;
    }
};
template <typename A, size_t I>
class AccessFactor<A, I, false>
    :
    private A
{
protected:
    typedef A Access;
    Access & getAccess()
    {
        return *this;
    }
};

#if MetaLib_HAS_VARIADIC_TEMPLATE
//allow inherit from more than one of the "same" bases
template <typename IS, typename ... AFactor> class CompoundAccessImpl;
template <typename ... AFactor>
using CompoundAccess = CompoundAccessImpl<std::make_index_sequence<sizeof...(AFactor)>, AFactor ...>;
template <size_t ... I, typename ... AFactor>
class CompoundAccessImpl<std::index_sequence<I ...>, AFactor ...> : public AFactor ...
{
    typedef std::tuple<AFactor ...> Factors;
#else
template <
    typename AFactor0 = AccessFactor<DummyAccess, 0, false>,
    typename AFactor1 = AccessFactor<DummyAccess, 1, false>,
    typename AFactor2 = AccessFactor<DummyAccess, 2, false>,
    typename AFactor3 = AccessFactor<DummyAccess, 3, false>
>
class CompoundAccess : public AFactor0, AFactor1, AFactor2, AFactor3
{
    typedef std::tuple<AFactor0, AFactor1, AFactor2, AFactor3> Factors;
#endif
protected:
    template <size_t J>
    struct FactorInfo
    {
        typedef typename std::tuple_element<J, Factors>::type Factor;
        typedef typename Factor::Access Access;
    };
public:
    void clear()
    {
#if MetaLib_HAS_VARIADIC_TEMPLATE
        pass({ (factor<I>().clear(), 0) ... });
#else
        factor<0>.clear();
        factor<1>.clear();
        factor<2>.clear();
        factor<3>.clear();
#endif
    }
    template <size_t J>
    auto getFactor() -> typename FactorInfo<J>::Access &
    {
        typedef typename std::tuple_element<J, Factors>::type Base;
        return Base::getAccess();
    }
    template <size_t J>
    auto bindFactor(MetaLib_U8 * ptr, MetaLib_Type * type) -> typename FactorInfo<J>::Access &
    {
        auto & f = getFactor<J>();
        f.bind(ptr, type);
        return f;
    }
};

template <typename Traits, typename Anchor>
class TypeFactor : public Anchor
{
public:
    typedef Traits CompoundTraits;
public:
    template <typename A>
    TypeFactor(A && a, typename std::enable_if<!std::is_same<TypeFactor, typename std::decay<A>::type>::value>::type * = nullptr)
        :
        Anchor(std::forward<A>(a))
    {}
};
template <typename Traits, typename Type>
class TypeFactor<Traits, Type *>
{
public:
    typedef Traits CompoundTraits;
private:
    Type * pointer;
public:
    TypeFactor(Type * pointer)
        :
        pointer(pointer)
    {}
public:
    Type * get() const
    {
        return pointer;
    }
    Type * operator->() const
    {
        return get();
    }
    Type & operator*() const
    {
        return *get();
    }
};

namespace Compound
{
    enum VisibilityEnum
    {
        Null = 0,
        Private = 1,
        Public = 2,
    };
};
typedef Compound::VisibilityEnum CompoundVisibility;

template <typename ThinTraits> struct ExpandedCT;
template <CompoundVisibility Visibility>
struct ExpandedCT<std::integral_constant<CompoundVisibility, Visibility>>
{
    static MetaLib_CONSTEXPR CompoundVisibility const Visibility_CreatingAccess = Visibility;
    static MetaLib_CONSTEXPR CompoundVisibility const Visibility_ExposingAccess = Visibility;
    static MetaLib_CONSTEXPR CompoundVisibility const Visibility_Call = Visibility;
    static MetaLib_CONSTEXPR CompoundVisibility const Visibility_Cast = Visibility;
};
template <>
struct ExpandedCT<std::false_type>
    : public ExpandedCT<std::integral_constant<CompoundVisibility, Compound::Private>>
{};
template <>
struct ExpandedCT<std::true_type>
    : public ExpandedCT<std::integral_constant<CompoundVisibility, Compound::Public>>
{};
template <>
struct ExpandedCT<void>
    : public ExpandedCT<std::true_type>
{};
//TODO: support Thin Traits, e.g. struct Traits { static MetaLib_CONSTEXPR CompoundVisibility const Visibility_Call = Compound::Private; };

struct Test_CompoundTraits
{
    template <typename T>
    static typename std::enable_if<::MetaLib::OKType<typename T::CompoundTraits>::value, char>::type
        test(T *);
    static long long test(...);
};
template <typename T>
struct Has_CompoundTraits
    : public std::integral_constant<bool, sizeof(Test_CompoundTraits::test((T *)nullptr)) == 1>
{};
template <typename T, typename = void> struct ExtractedCT;
template <typename T>
struct ExtractedCT<T, typename std::enable_if<Has_CompoundTraits<T>::value>::type>
    : public ExpandedCT<typename T::CompoundTraits>
{};
template <typename T>
struct ExtractedCT<T, typename std::enable_if<!Has_CompoundTraits<T>::value>::type>
    : public ExpandedCT<void>
{};

template <typename ThinTraits, typename Anchor>
auto makeTypeFactor(Anchor && anchor) -> TypeFactor<ThinTraits, typename std::decay<Anchor>::type>
{
    return TypeFactor<ThinTraits, typename std::decay<Anchor>::type>(std::forward<Anchor>(anchor));
}

//Factor can be TypeFactor or naked type (pointer or smart pointer) which is equivalent to TypeFactor<void, Factor>
#if MetaLib_HAS_VARIADIC_TEMPLATE
template <typename IS, typename ... Factor> class CompoundBaseImpl;
template <typename ... Factor>
using CompoundBase = CompoundBaseImpl<std::make_index_sequence<sizeof...(Factor)>, Factor ...>;
template <size_t ... I, typename ... Factor>
class CompoundBaseImpl<std::index_sequence<I ...>, Factor ...>
    : protected MetaLib_Type
{
    static MetaLib_CONSTEXPR size_t const Count = sizeof...(Factor);
    MetaLib_STATIC_ASSERT(Count <= std::numeric_limits<int>::max(), "Too many Bases!");
    MetaLib_STATIC_ASSERT(Count == sizeof...(I), "Unexpected template parameter number.");
    typedef std::tuple<Factor ...> Factors;
#else
template <
    typename Factor0 = TypeFactor<std::false_type, DummyType *>,
    typename Factor1 = TypeFactor<std::false_type, DummyType *>,
    typename Factor2 = TypeFactor<std::false_type, DummyType *>,
    typename Factor3 = TypeFactor<std::false_type, DummyType *>
>
class CompoundBase
    : protected MetaLib_Type
{
    typedef CompoundBase CompoundBaseImpl;
    static MetaLib_CONSTEXPR size_t const Count = 4;
    typedef std::tuple<Factor0, Factor1, Factor2, Factor3> Factors;
#endif
private:
    Factors factors;
private:
    struct Head//TODO: optimize static base info
    {
        union Location
        {
            MetaLib_U8 * ptr;
            size_t offset;
        };
        std::array<Location, Count> locations;
        std::array<MetaLib_Type *, Count> types;
        void clear()
        {
            Location n;
            n.ptr = nullptr;
            locations.fill(n);
            types.fill(nullptr);
        }
    };
private:
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... AFactor>
    class AccessBase : public CompoundAccess<AFactor ...>
    {
        template <typename, typename ...> friend class CompoundBaseImpl;
        typedef CompoundAccess<AFactor ...> CompoundAccessBase;
#else
    template <typename AFactor0, typename AFactor1, typename AFactor2, typename AFactor3>
    class AccessBase : public CompoundAccess<AFactor0, AFactor1, AFactor2, AFactor3>
    {
        //template <typename, typename, typename, typename> friend class CompoundBaseImpl;
        friend class CompoundBaseImpl;
        typedef CompoundAccess<AFactor0, AFactor1, AFactor2, AFactor3> CompoundAccessBase;
#endif
        Head head;
    protected:
        typedef AccessBase TheAccessBase;
    public:
        AccessBase()
        {
            head.clear();
        }
        void clear()
        {
            CompoundAccessBase::clear();
            head.clear();
        }
    public:
        void bind(MetaLib_U8 * ptr, MetaLib_Type *)
        {
            head = *reinterpret_cast<Head *>(ptr);
            //TODO: skip private, also see below
#if MetaLib_HAS_VARIADIC_TEMPLATE
            pass({ (bindFactor<I>(), 0) ... });
#else
            bindFactor<0>();
            bindFactor<1>();
            bindFactor<2>();
            bindFactor<3>();
#endif
        }
        template <size_t J>
        auto bindFactor() -> typename CompoundAccessBase::template FactorInfo<J>::Access &
        {
            auto ptr = head.locations[J].ptr;
            auto type = head.types[J];
            return CompoundAccessBase::bindFactor<J>(ptr, type);
        }
    };
private:
    template <typename FP>
    struct DR//dereference
    {
        template<class T>
        static typename std::add_rvalue_reference<T>::type
            DV() MetaLib_NOEXCEPT;//std::declval for VS2010
        typedef typename std::remove_reference<decltype(*(DV<FP>()))>::type Type;
    };
    template <typename F, typename = void> struct VCA {};
    template <typename F>
    struct VCA<F, typename std::enable_if<ExtractedCT<F>::Visibility_CreatingAccess == Compound::Private>::type>
        : public std::false_type
    {};
    template <typename F>
    struct VCA<F, typename std::enable_if<ExtractedCT<F>::Visibility_CreatingAccess == Compound::Public>::type>
        : public std::true_type
    {};
    template <typename F, typename = void> struct VEA {};
    template <typename F>
    struct VEA<F, typename std::enable_if<ExtractedCT<F>::Visibility_ExposingAccess == Compound::Private>::type>
        : public std::false_type
    {};
    template <typename F>
    struct VEA<F, typename std::enable_if<ExtractedCT<F>::Visibility_ExposingAccess == Compound::Public>::type>
        : public std::true_type
    {};
private:
    template <typename F, size_t J>
    struct CAF
    {
        typedef AccessFactor<typename DR<F>::Type::CreatingAccess, J, VCA<F>::value> Type;
    };
    template <typename F, size_t J>
    struct EAF
    {
        typedef AccessFactor<typename DR<F>::Type::ExposingAccess, J, VEA<F>::value> Type;
    };
public:
    class CreatingAccessBase : public AccessBase<
#if MetaLib_HAS_VARIADIC_TEMPLATE
        typename CAF<Factor, I>::Type ...
#else
        typename CAF<Factor0, 0>::Type,
        typename CAF<Factor1, 1>::Type,
        typename CAF<Factor2, 2>::Type,
        typename CAF<Factor3, 3>::Type
#endif
    >
    {
    public:
        template <size_t J>
        auto factor() -> typename TheAccessBase::template FactorInfo<J>::Access &
        {
            return getFactor<J>();
        }
    };
    class ExposingAccessBase : public AccessBase<
#if MetaLib_HAS_VARIADIC_TEMPLATE
        typename EAF<Factor, I>::Type ...
#else
        typename EAF<Factor0, 0>::Type,
        typename EAF<Factor1, 1>::Type,
        typename EAF<Factor2, 2>::Type,
        typename EAF<Factor3, 3>::Type
#endif
    >
    {
    public:
        template <size_t J>
        auto factor() -> typename TheAccessBase::template FactorInfo<J>::Access &
        {
            //TODO: bind private, see above
            return getFactor<J>();
        }
    };
private:
    template <size_t J>
    struct ECT
        : public ExtractedCT<typename std::tuple_element<J, Factors>::type>
    {};
private:
    template <typename ECT, typename = void> struct SkipCall {};
    template <typename ECT>
    struct SkipCall<ECT, typename std::enable_if<ECT::Visibility_Call == Compound::Private>::type>
        : public std::true_type
    {};
    template <typename ECT>
    struct SkipCall<ECT, typename std::enable_if<ECT::Visibility_Call == Compound::Public>::type>
        : public std::false_type
    {};
#if 0
    //These triggers a bug in VS2010
    template <size_t J, typename = void> struct SkipCall;
    template <size_t J>
    struct SkipCall<J, typename std::enable_if<ECT<J>::Visibility_Call == Compound::Private>::type>
        : public std::true_type
    {};
    template <size_t J>
    struct SkipCall<J, typename std::enable_if<ECT<J>::Visibility_Call == Compound::Public>::type>
        : public std::false_type
    {};
    /*
template <typename> struct A
{
    template <typename> struct Trait { static bool const V = true; };
    template <typename, typename = void> struct Check {};
    template <typename J> struct Check<J, typename std::enable_if<Trait<J>::V>::type> : public std::true_type {};
    A() { Check<Trait<int>>::value; }
};
int main() { A<int> a; }
    */
#endif
protected:
    //TODO: static expand
    static MetaLib_Obj call(
        MetaLib_U8 * ptr,
        MetaLib_Type * /*type*/,
        MetaLib_USz key_length, MetaLib_U8 const * key_pointer,
        MetaLib_Obj * argv
    ) MetaLib_NOEXCEPT
    {
        //the following process does not throw C++ exceptions, so no try...catch...
        auto head = reinterpret_cast<Head *>(ptr);
#if 0
        for (int i = ((int)Count) - 1; i >= 0 ; --i)
        {
            auto t = head->types[i];
            auto p = head->locations[i].ptr;
            auto r = t->p_call(p, t, key_length, key_pointer, argv);
            if (false == r.isMark_NotCalled())
                return r;
        }
        return MetaLib_Obj::makeMark_NotCalled();
#endif
        auto call_factor = [&](size_t index) -> MetaLib_Obj
        {
            auto t = head->types[index];
            auto p = head->locations[index].ptr;
            auto r = t->p_call(p, t, key_length, key_pointer, argv);
            return r;
        };
        MetaLib_Obj r;
        r.setMark_NotCalled();
#if MetaLib_HAS_VARIADIC_TEMPLATE
        bool called = false;
        pass({(
            called ||
            SkipCall<ECT<Count - I - 1>>::value ||
            (r = call_factor(Count - I - 1)).isMark_NotCalled() ||
            (called = true, 0)
            ) ... });
#else
#if _MSC_VER == 1600
#pragma warning(push)
#pragma warning(disable: 4127)
#endif
        if (!(SkipCall<ECT<3>>::value || (r = call_factor(3)).isMark_NotCalled())) return r;
        if (!(SkipCall<ECT<2>>::value || (r = call_factor(2)).isMark_NotCalled())) return r;
        if (!(SkipCall<ECT<1>>::value || (r = call_factor(1)).isMark_NotCalled())) return r;
        if (!(SkipCall<ECT<0>>::value || (r = call_factor(0)).isMark_NotCalled())) return r;
#if _MSC_VER == 1600
#pragma warning(pop)
#endif
#endif
        return r;
    }
private:
    template <typename ECT, typename = void> struct SkipCast {};
    template <typename ECT>
    struct SkipCast<ECT, typename std::enable_if<ECT::Visibility_Cast == Compound::Private>::type>
        : public std::true_type
    {};
    template <typename ECT>
    struct SkipCast<ECT, typename std::enable_if<ECT::Visibility_Cast == Compound::Public>::type>
        : public std::false_type
    {};
#if 0
    //These triggers a bug in VS2010
    template <size_t J, typename = void> struct SkipCast {};
    template <size_t J>
    struct SkipCast<J, typename std::enable_if<ECT<J>::Visibility_Cast == Compound::Private>::type>
        : public std::true_type
    {};
    template <size_t J>
    struct SkipCast<J, typename std::enable_if<ECT<J>::Visibility_Cast == Compound::Public>::type>
        : public std::false_type
    {};
#endif
protected:
    static MetaLib_U8 * cast(MetaLib_U8 * ptr, MetaLib_Type * /*type*/, MetaLib_Type * dest_type) MetaLib_NOEXCEPT
    {
        auto head = reinterpret_cast<Head *>(ptr);
#if 0
        for (int i = ((int)Count) - 1; i >= 0; --i)
        {
            auto t = head->types[i];
            auto p = head->locations[i].ptr;
            if (t == dest_type)
                return p;
            auto r = t->p_cast(p, t, dest_type);
            if (r)
                return r;
        }
#endif
        auto cast_factor = [&](size_t index) -> MetaLib_U8 *
        {
            auto t = head->types[index];
            auto p = head->locations[index].ptr;
            if (t == dest_type)
                return p;
            auto r = t->p_cast(p, t, dest_type);
            return r;
        };
        MetaLib_U8 * r = nullptr;
#if MetaLib_HAS_VARIADIC_TEMPLATE
        bool casted = false;
        pass({ (
            casted ||
            SkipCast<ECT<Count - I - 1>>::value ||
            (r = cast_factor(Count - I - 1)) == nullptr ||
            (casted = true, 0)
            ) ... });
#else
#if _MSC_VER == 1600
#pragma warning(push)
#pragma warning(disable: 4127)
#endif
        if (!(SkipCast<ECT<3>>::value || (r = cast_factor(3)) == nullptr)) return r;
        if (!(SkipCast<ECT<2>>::value || (r = cast_factor(2)) == nullptr)) return r;
        if (!(SkipCast<ECT<1>>::value || (r = cast_factor(1)) == nullptr)) return r;
        if (!(SkipCast<ECT<0>>::value || (r = cast_factor(0)) == nullptr)) return r;
#if _MSC_VER == 1600
#pragma warning(pop)
#endif
#endif
        return r;
    }
    static void kill(MetaLib_U8 * ptr, MetaLib_Type * /*type*/) MetaLib_NOEXCEPT
    {
        //TODO: exception

        auto head = reinterpret_cast<Head *>(ptr);
#if 0
        for (int i = ((int)Count) - 1; i >= 0; --i)
        {
            auto t = head->types[i];
            if (t)
            {
                auto p = head->locations[i].ptr;
                t->p_kill(p, t);
            }
        }
#endif
        auto kill_factor = [&](size_t index) -> int
        {
            auto t = head->types[index];
            if (t)
            {
                auto p = head->locations[index].ptr;
                t->p_kill(p, t);
            }
            return 0;
        };
#if MetaLib_HAS_VARIADIC_TEMPLATE
        pass({ kill_factor(Count - I - 1) ... });
#else
        kill_factor(3);
        kill_factor(2);
        kill_factor(1);
        kill_factor(0);
#endif
        head->~Head();
    }
private:
    void fillTypeOps()
    {
        MetaLib_Type::p_call = &call;
        MetaLib_Type::p_cast = &cast;
        MetaLib_Type::p_kill = &kill;
    }
public:
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    CompoundBaseImpl(A && ... a)
        :
        factors(std::forward<A>(a) ...)
    {
        fillTypeOps();
    }
#else
    CompoundBase()
        :
        factors(&Dummy_Type, &Dummy_Type, &Dummy_Type, &Dummy_Type)
    {
        fillTypeOps();
    }
    template <typename A0>
    CompoundBase(A0 && a0)
        :
        factors(std::forward<A0>(a0), &Dummy_Type, &Dummy_Type, &Dummy_Type)
    {
        fillTypeOps();
    }
    template <typename A0, typename A1>
    CompoundBase(A0 && a0, A1 && a1)
        :
        factors(std::forward<A0>(a0), std::forward<A1>(a1), &Dummy_Type, &Dummy_Type)
    {
        fillTypeOps();
    }
    template <typename A0, typename A1, typename A2>
    CompoundBase(A0 && a0, A1 && a1, A2 && a2)
        :
        factors(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), &Dummy_Type)
    {
        fillTypeOps();
    }
    template <typename A0, typename A1, typename A2, typename A3>
    CompoundBase(A0 && a0, A1 && a1, A2 && a2, A3 && a3)
        :
        factors(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3))
    {
        fillTypeOps();
    }
#endif
private:
    class AllocMachine
    {
        CompoundBaseImpl * ct;
        CreatingAccessBase * p_access;
        size_t size;
        size_t align;
    public:
        AllocMachine(CompoundBaseImpl * ct, CreatingAccessBase * p_access)
            :
            ct(ct), p_access(p_access)
        {
            size = sizeof(Head);
            align = std::alignment_of<Head>::value;
        }
#if MetaLib_HAS_VARIADIC_TEMPLATE
        template <size_t J, typename ... A>
        void alloc(A && ... a)
        {
            auto & h = p_access->head;
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            size_t s, a;
            std::tie(s, a) = factor->alloc(&factor_access, std::forward<A>(a) ...);
            size = (size + a - 1) / a * a;
            h.locations[J].offset = size;
            size += s;
            if (a > align)
                align = a;
        }
#else
        template <size_t J>
        void alloc()
        {
            auto & h = p_access->head;
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            size_t s, a;
            std::tie(s, a) = factor->alloc(&factor_access);
            size = (size + a - 1) / a * a;
            h.locations[J].offset = size;
            size += s;
            if (a > align)
                align = a;
        }
        template <size_t J, typename A0>
        void alloc(A0 && a0)
        {
            auto & h = p_access->head;
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            size_t s, a;
            std::tie(s, a) = factor->alloc(&factor_access, std::forward<A0>(a0));
            size = (size + a - 1) / a * a;
            h.locations[J].offset = size;
            size += s;
            if (a > align)
                align = a;
        }
        template <size_t J, typename A0, typename A1>
        void alloc(A0 && a0, A1 && a1)
        {
            auto & h = p_access->head;
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            size_t s, a;
            std::tie(s, a) = factor->alloc(&factor_access, std::forward<A0>(a0), std::forward<A1>(a1));
            size = (size + a - 1) / a * a;
            h.locations[J].offset = size;
            size += s;
            if (a > align)
                align = a;
        }
#endif
    private:
#if MetaLib_HAS_VARIADIC_TEMPLATE
        void allocAllDummies()
        {}
#else
        template <size_t J>
        typename std::enable_if<std::is_same<
            TypeFactor<std::false_type, DummyType *>,
            typename std::tuple_element<J, Factors>::type
        >::value>::type
            allocDummy()
        {
            alloc<J>();
        }
        template <size_t J>
        typename std::enable_if<!std::is_same<
            TypeFactor<std::false_type, DummyType *>,
            typename std::tuple_element<J, Factors>::type
        >::value>::type
            allocDummy()
        {}
        void allocAllDummies()
        {
            allocDummy<0>();
            allocDummy<1>();
            allocDummy<2>();
            allocDummy<3>();
        }
#endif
    public:
        std::pair<size_t, size_t> finish() MetaLib_NOEXCEPT
        {
            allocAllDummies();
            size = (size + align - 1) / align * align;//size should be dividable by align
            return std::make_pair(size, align);
        }
    };
protected:
    AllocMachine startAlloc(CreatingAccessBase * p_access) MetaLib_NOEXCEPT
    {
        return AllocMachine(this, p_access);
    }
/**************************************************************************************************************
 * How the Concrete Type 'alloc'
 * 
 *  std::pair<size_t, size_t> alloc(CreatingAccessBase * p_access, ...)
 *  {
 *      auto am = Base::startAlloc(p_access);
 *      ...
 *      am.alloc<0>(...);
 *      ...
 *      am.alloc<1>(...);
 *      ...
 *      am.alloc<N>(...);
 *      ...
 *      return am.finish();
 *  }
 *************************************************************************************************************/
private:
    class InitMachine
    {
        CompoundBaseImpl * ct;
        CreatingAccessBase * p_access;
        MetaLib_U8 * ptr;
    private:
        void rewind()
        {
            kill(ptr, ct);
        }
    public:
        InitMachine(CompoundBaseImpl * ct, CreatingAccessBase * p_access, MetaLib_U8 * ptr)
            :
            ct(ct), p_access(p_access), ptr(ptr)
        {
            auto head = new(ptr) Head;
            auto & l = p_access->head.locations;
#if MetaLib_HAS_VARIADIC_TEMPLATE
            pass({ head->locations[I].ptr = l[I].ptr = ptr + l[I].offset ... });
#else
            head->locations[0].ptr = l[0].ptr = ptr + l[0].offset;
            head->locations[1].ptr = l[1].ptr = ptr + l[1].offset;
            head->locations[2].ptr = l[2].ptr = ptr + l[2].offset;
            head->locations[3].ptr = l[3].ptr = ptr + l[3].offset;
#endif
        }
        InitMachine(InitMachine && rhs)
            :
            ct(rhs.ct), p_access(rhs.p_access), ptr(rhs.ptr)
        {
            rhs.ptr = nullptr;
        }
        ~InitMachine()
        {
            if (ct)
                rewind();
        }
#if MetaLib_HAS_VARIADIC_TEMPLATE
        template <size_t J, typename ... A>
        void init(A && ... a)
        {
            auto head = reinterpret_cast<Head *>(ptr);
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            auto type = factor->init(&factor_access, head->locations[J].ptr, std::forward<A>(a) ...);
            head->types[J] = type;
        }
#else
        template <size_t J>
        void init()
        {
            auto head = reinterpret_cast<Head *>(ptr);
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            auto type = factor->init(&factor_access, head->locations[J].ptr);
            head->types[J] = type;
        }
        template <size_t J, typename A0>
        void init(A0 && a0)
        {
            auto head = reinterpret_cast<Head *>(ptr);
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            auto type = factor->init(&factor_access, head->locations[J].ptr, std::forward<A0>(a0));
            head->types[J] = type;
        }
        template <size_t J, typename A0, typename A1>
        void init(A0 && a0, A1 && a1)
        {
            auto head = reinterpret_cast<Head *>(ptr);
            auto & factor = std::get<J>(ct->factors);
            auto & factor_access = p_access->getFactor<J>();
            auto type = factor->init(&factor_access, head->locations[J].ptr, std::forward<A0>(a0), std::forward<A1>(a1));
            head->types[J] = type;
        }
#endif
    private:
#if MetaLib_HAS_VARIADIC_TEMPLATE
        void initAllDummies()
        {}
#else
        template <size_t J>
        typename std::enable_if<std::is_same<
            TypeFactor<std::false_type, DummyType *>,
            typename std::tuple_element<J, Factors>::type
        >::value>::type
            initDummy()
        {
            init<J>();
        }
        template <size_t J>
        typename std::enable_if<!std::is_same<
            TypeFactor<std::false_type, DummyType *>,
            typename std::tuple_element<J, Factors>::type
        >::value>::type
            initDummy()
        {}
        void initAllDummies()
        {
            initDummy<0>();
            initDummy<1>();
            initDummy<2>();
            initDummy<3>();
        }
#endif
    public:
        MetaLib_Type * finish() MetaLib_NOEXCEPT
        {
            initAllDummies();
            auto r = ct;
            ct = nullptr;
            return r;
        }
    };
protected:
    InitMachine startInit(CreatingAccessBase * p_access, MetaLib_U8 * ptr) MetaLib_NOEXCEPT
    {
        return InitMachine(this, p_access, ptr);
    }
/**************************************************************************************************************
 * How the Concrete Type 'init'
 * 
 *  MetaLib_Type * init(CreatingAccessBase * p_access, MetaLib_U8 * ptr, ...)
 *  {
 *      auto im = Base::startInit(p_access, ptr);
 *      ...
 *      im.init<0>(...);
 *      ...
 *      im.init<1>(...);
 *      ...
 *      im.init<N>(...);
 *      ...
 *      return im.finish();
 *  }
 *************************************************************************************************************/
};


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "C++ Type"
#endif
/*************************************************************************************************************************************/

template <typename T>
class CxxTypeAccess
{
    template <typename> friend class CxxType;
    T * p;
public:
    CxxTypeAccess()
    {
        p = nullptr;
    }
    void clear()
    {
        p = nullptr;
    }
public:
    void bind(MetaLib_U8 * ptr, MetaLib_Type *)
    {
        p = reinterpret_cast<T *>(ptr);
    }
public:
    T * get() const
    {
        return p;
    }
    T & operator*() const
    {
        return *get();
    }
    T * operator->() const
    {
        return get();
    }
};

template <typename T>
class CxxType
    :
    protected EmptyType,
    public ObjectCreatingMixin<CxxType<T>, CxxTypeAccess<T>>,
    public ObjectExposingMixin<CxxType<T>, CxxTypeAccess<T>>
{
    friend class CreatingMixin;
    friend class ExposingMixin;
private:
    typedef CxxTypeAccess<T> Access;
protected:
    static void kill(MetaLib_U8 * ptr, MetaLib_Type * /*type*/) MetaLib_NOEXCEPT
    {
        //TODO: exception

        auto p = reinterpret_cast<T *>(ptr);
        p->~T();
        (void)(p);//TODO: MSVC warning C4189?
    }
public:
    CxxType()
    {
        MetaLib_Type::p_kill = &kill;
    }
public:
    static CxxType Static;
public:
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    std::pair<size_t, size_t> alloc(Access *, A && ...) MetaLib_NOEXCEPT
    {
        return std::make_pair(sizeof(T), std::alignment_of<T>::value);
    }
    template <typename ... A>
    MetaLib_Type * init(Access * p_access, MetaLib_U8 * ptr, A && ... a)
    {
        auto p = new(ptr) T(std::forward<A>(a) ...);
        p_access->p = p;
        return this;
    }
#else
    std::pair<size_t, size_t> alloc(Access *) MetaLib_NOEXCEPT
    {
        return std::make_pair(sizeof(T), std::alignment_of<T>::value);
    }
    template <typename A0>
    std::pair<size_t, size_t> alloc(Access *, A0 &&) MetaLib_NOEXCEPT
    {
        return std::make_pair(sizeof(T), std::alignment_of<T>::value);
    }
    template <typename A0, typename A1>
    std::pair<size_t, size_t> alloc(Access *, A0 &&, A1 &&) MetaLib_NOEXCEPT
    {
        return std::make_pair(sizeof(T), std::alignment_of<T>::value);
    }
    MetaLib_Type * init(Access * p_access, MetaLib_U8 * ptr)
    {
#if _MSC_VER == 1600
#pragma warning(suppress: 4345)
#endif
        auto p = new(ptr) T();
        p_access->p = p;
        return this;
    }
    template <typename A0>
    MetaLib_Type * init(Access * p_access, MetaLib_U8 * ptr, A0 && a0)
    {
        auto p = new(ptr) T(std::forward<A0>(a0));
        p_access->p = p;
        return this;
    }
    template <typename A0, typename A1>
    MetaLib_Type * init(Access * p_access, MetaLib_U8 * ptr, A0 && a0, A1 && a1)
    {
        auto p = new(ptr) T(std::forward<A0>(a0), std::forward<A1>(a1));
        p_access->p = p;
        return this;
    }
#endif
};
template <typename T> CxxType<T> CxxType<T>::Static;

struct Empty {};
template <>
class CxxType<void> : public CxxType<Empty>
{};


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Methods Type"
#endif
/*************************************************************************************************************************************/

class MethodTable
{
public:
    typedef MetaLib_Obj MethodSignature(MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv);
    //typedef MetaLib_Obj AnotherMethodSignature(MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_USz key_length, MetaLib_U8 const * key_pointer, MetaLib_Obj * argv);
protected:
    typedef std::function<MethodSignature> Method;
    typedef std::basic_string<MetaLib_U8> Key;
    //typedef size_t Key;
    typedef std::map<Key, Method> Methods;
    Methods methods;
public:
    MetaLib_INLINE MetaLib_Obj call(
        MetaLib_U8 * ptr,
        MetaLib_Type * type,
        ::MetaLib::CallKey key,
        MetaLib_Obj * argv
    ) MetaLib_NOEXCEPT
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto & m = methods;
            //if (false == m.empty())
            {
                auto iter = m.find(Key(key.pointer, key.length));
                //auto iter = m.find(Key(0xDEADBEEFDEADBEEF));
                if (iter != m.end())
                {
                    auto r = iter->second(ptr, type, argv);
                    //if (false == r.isMark_NotCalled())
                    return r;
                }
            }
            return MetaLib_Obj::makeMark_NotCalled();
        }
        catch (...)//TODO
        {
            argv[0].setGeneralException();//TODO: make a real exception
            return MetaLib_Obj::makeMark_Throw();
        }
    }
    template <typename Invoker, typename F>
    MethodTable & setf(::MetaLib::CallKey key, F && f)
    {
        Key k(key.pointer, key.length);
        //Key k(0xDEADBEEFDEADBEEF);

#if _MSC_VER == 1600//VS2010 bug
        static Invoker * const invoker = nullptr;
#endif

        methods[std::move(k)] = [f](MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv) -> MetaLib_Obj
        {
#if _MSC_VER == 1600//VS2010 bug
            return invoker->invoke(f, ptr, type, argv);
#else
            return Invoker::invoke(f, ptr, type, argv);
#endif
        };
        return *this;
    }
    template <typename F>
    MethodTable & setfRaw(::MetaLib::CallKey key, F && f)
    {
        Key k(key.pointer, key.length);
        //Key k(0xDEADBEEFDEADBEEF);
        methods[std::move(k)] = std::forward<F>(f);
        return *this;
    }
};

template <typename Type>
class ObjectInvokingMixin
{
    //MetaLib_STATIC_ASSERT((std::is_base_of<ObjectInvokingMixin, Type>::value), "Type must be derived from ObjectInvokingMixin<Type, ...>");
protected:
    typedef ObjectInvokingMixin InvokingMixin;
protected:
    static Type * retrieveType(MetaLib_U8 * /*ptr*/, MetaLib_Type * type, std::nullptr_t)//overridable
    {
        auto concrete = static_cast<Type *>(type);
        return concrete;
    }
protected:
    static MetaLib_Obj & pickArg(MetaLib_Obj * argv, size_t i)
    {
        auto & r = argv[i + 3];
        if (r.isNull())
            throw 1;//TODO
        return r;
    }
    template <size_t I, typename R, typename = void> struct ArgMaker {};
    template <size_t I>
    struct ArgMaker<I, MetaLib_Obj, void>
    {
        static MetaLib_Obj make(MetaLib_U8 *, MetaLib_Type *, MetaLib_Obj * argv)
        {
            auto & obj = pickArg(argv, I);
            auto r = obj;
            if (r.isDyn())
                r.clearDynOwning();
            return r;
        }
    };
    template <size_t I>
    struct ArgMaker<I, ::MetaLib::O, void>
    {
        static ::MetaLib::O make(MetaLib_U8 *, MetaLib_Type *, MetaLib_Obj * argv)
        {
            auto & obj = pickArg(argv, I);
            if (obj.isDyn())
            {
                if (obj.getDynOwning())
                {
                    obj.clearDynOwning();
                    return ::MetaLib::O(MetaLib::O::Owned(), obj);
                }
                obj.incDynContentRef();
            }
            return ::MetaLib::O(::MetaLib::O::Owned(), obj);
        }
    };
protected:
    template <typename Access>
    struct ArgMaker<0, OxAccess<MetaLib_Obj, Access>, void>
    {
        typedef OxAccess<MetaLib_Obj, Access> Self;

        //TODO: better criteria
        MetaLib_STATIC_ASSERT((std::is_same<Self, typename Type::Self>::value), "This type is not supported.");

        static Self make(MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            Self r;
            auto obj = pickArg(argv, 0);
            if (obj.isDyn())//TODO: may this check be removed?
                obj.clearDynOwning();
            static_cast<MetaLib_Obj &>(r) = obj;
            auto concrete = Type::retrieveType(ptr, type, nullptr);
            if (false == concrete->retrieve(&r, ptr, type, nullptr))
                throw 1;//TODO
            return r;
        }
    };
protected:
    struct RetMaker
    {
        static MetaLib_Obj make(::MetaLib::O o)
        {
            return o.release();
        }
    };
protected:
    template <typename UnpackSignature> struct Invoker;
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename IS, typename UnpackSignature> struct InvokerImpl;
    template <typename R, typename ... A>
    struct Invoker<R (A ...)> : public InvokerImpl<std::make_index_sequence<sizeof...(A)>, R (A ...)>
    {};
    template <size_t ... I, typename R, typename ... A>
    struct InvokerImpl<std::index_sequence<I ...>, R (A ...)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            (void)(ptr); (void)(type); (void)(argv);
            auto r = RetMaker::make(f(ArgMaker<I, A>::make(ptr, type, argv) ...));
            return r;
        }
    };
    template <size_t ... I, typename ... A>
    struct InvokerImpl<std::index_sequence<I ...>, void (A ...)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            (void)(ptr); (void)(type); (void)(argv);
            f(ArgMaker<I, A>::make(ptr, type, argv) ...);
            return MetaLib_Obj::makeNone();
        }
    };
#else
    template <typename R>
    struct Invoker<R ()>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            (void)(ptr); (void)(type); (void)(argv);
            auto r = RetMaker::make(f());
            return r;
        }
    };
    template <typename R, typename A0>
    struct Invoker<R (A0)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            auto r = RetMaker::make(f(
                ArgMaker<0, A0>::make(ptr, type, argv)
            ));
            return r;
        }
    };
    template <typename R, typename A0, typename A1>
    struct Invoker<R (A0, A1)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            auto r = RetMaker::make(f(
                ArgMaker<0, A0>::make(ptr, type, argv),
                ArgMaker<1, A1>::make(ptr, type, argv)
            ));
            return r;
        }
    };
    template <typename R, typename A0, typename A1, typename A2>
    struct Invoker<R (A0, A1, A2)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            auto r = RetMaker::make(f(
                ArgMaker<0, A0>::make(ptr, type, argv),
                ArgMaker<1, A1>::make(ptr, type, argv),
                ArgMaker<2, A2>::make(ptr, type, argv)
            ));
            return r;
        }
    };
    template <typename R, typename A0, typename A1, typename A2, typename A3>
    struct Invoker<R (A0, A1, A2, A3)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            auto r = RetMaker::make(f(
                ArgMaker<0, A0>::make(ptr, type, argv),
                ArgMaker<1, A1>::make(ptr, type, argv),
                ArgMaker<2, A2>::make(ptr, type, argv),
                ArgMaker<3, A3>::make(ptr, type, argv)
            ));
            return r;
        }
    };

    template <>
    struct Invoker<void ()>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            (void)(ptr); (void)(type); (void)(argv);
            f();
            return MetaLib_Obj::makeNone();
        }
    };
    template <typename A0>
    struct Invoker<void (A0)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            f(
                ArgMaker<0, A0>::make(ptr, type, argv)
            );
            return MetaLib_Obj::makeNone();
        }
    };
    template <typename A0, typename A1>
    struct Invoker<void (A0, A1)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            f(
                ArgMaker<0, A0>::make(ptr, type, argv),
                ArgMaker<1, A1>::make(ptr, type, argv)
            );
            return MetaLib_Obj::makeNone();
        }
    };
    template <typename A0, typename A1, typename A2>
    struct Invoker<void (A0, A1, A2)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            f(
                ArgMaker<0, A0>::make(ptr, type, argv),
                ArgMaker<1, A1>::make(ptr, type, argv),
                ArgMaker<2, A2>::make(ptr, type, argv)
            );
            return MetaLib_Obj::makeNone();
        }
    };
    template <typename A0, typename A1, typename A2, typename A3>
    struct Invoker<void (A0, A1, A2, A3)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            f(
                ArgMaker<0, A0>::make(ptr, type, argv),
                ArgMaker<1, A1>::make(ptr, type, argv),
                ArgMaker<2, A2>::make(ptr, type, argv),
                ArgMaker<3, A3>::make(ptr, type, argv)
            );
            return MetaLib_Obj::makeNone();
        }
    };
    //...
    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    struct Invoker<void (A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)>
    {
        template <typename F>
        static MetaLib_Obj invoke(F & f, MetaLib_U8 * ptr, MetaLib_Type * type, MetaLib_Obj * argv)
        {
            f(
                ArgMaker<0, A0>::make(ptr, type, argv),
                ArgMaker<1, A1>::make(ptr, type, argv),
                ArgMaker<2, A2>::make(ptr, type, argv),
                ArgMaker<3, A3>::make(ptr, type, argv),
                ArgMaker<4, A4>::make(ptr, type, argv),
                ArgMaker<5, A5>::make(ptr, type, argv),
                ArgMaker<6, A6>::make(ptr, type, argv),
                ArgMaker<7, A7>::make(ptr, type, argv),
                ArgMaker<8, A8>::make(ptr, type, argv),
                ArgMaker<9, A9>::make(ptr, type, argv)
            );
            return MetaLib_Obj::makeNone();
        }
    };
#endif
};


template <typename BaseType>
class MethodsType
    :
    public BaseType,
    protected MethodTable,
    public ObjectInvokingMixin<MethodsType<BaseType>>
{
    MetaLib_STATIC_ASSERT((std::is_base_of<MetaLib_Type, BaseType>::value), "BaseType must be derived from MetaLib_Type.");
    friend class InvokingMixin;
public:
    typedef OxAccess<MetaLib_Obj, typename BaseType::ExposingAccess> Self;
protected:
    static MetaLib_Obj call(
        MetaLib_U8 * ptr,
        MetaLib_Type * type,
        MetaLib_USz key_length, MetaLib_U8 const * key_pointer,
        MetaLib_Obj * argv
    ) MetaLib_NOEXCEPT
    {
        //noexcept, so no need to try-catch

        auto pself = retrieveType(ptr, type, nullptr);
        auto pmt = static_cast<MethodTable *>(pself);
        auto r = pmt->call(ptr, type, ::MetaLib::CallKey(key_length, key_pointer), argv);
        if (false == r.isMark_NotCalled())
            return r;

        return BaseType::call(ptr, type, key_length, key_pointer, argv);
    }
public:
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    MethodsType(A && ... a)
        :
        BaseType(std::forward<A>(a) ...)
    {
        MetaLib_Type::p_call = &call;
    }
#else
    MethodsType()
    {
        MetaLib_Type::p_call = &call;
    }
    template <typename A0>
    MethodsType(A0 && a0)
        :
        BaseType(std::forward<A0>(a0))
    {
        MetaLib_Type::p_call = &call;
    }
    template <typename A0, typename A1>
    MethodsType(A0 && a0, A1 && a1)
        :
        BaseType(std::forward<A0>(a0), std::forward<A1>(a1))
    {
        MetaLib_Type::p_call = &call;
    }
#endif
    static MethodsType Static;//TODO: ctor args
public:
    template <typename UnpackSignature, typename F>
    MethodsType & def(::MetaLib::CallKey key, F && f)
    {
        MethodTable::setf<InvokingMixin::Invoker<UnpackSignature>>(std::move(key), std::forward<F>(f));
        return *this;
    }
    template <typename F>
    MethodsType & defRaw(::MetaLib::CallKey key, F && f)
    {
        MethodTable::setfRaw(std::move(key), std::forward<F>(f));
        return *this;
    }
};
template <typename BaseType> MethodsType<BaseType> MethodsType<BaseType>::Static;


#if MetaLib_HAS_TEMPLATE_ALIAS
template <typename T>
using CxxMType = MethodsType<CxxType<T>>;
#else
template <typename T>
class CxxMType : public MethodsType<CxxType<T>>
{};
#endif


template <typename BaseAccess, typename InvokingType>
class DynamicMethodsAccess : public BaseAccess
{
public:
    template <typename UnpackSignature, typename F>
    DynamicMethodsAccess & setf(::MetaLib::CallKey key, F && f)
    {
        typedef typename InvokingType::Invoker<UnpackSignature> Invoker;

        BaseAccess::factor<1>()->setf<Invoker>(std::move(key), std::forward<F>(f));
        return *this;
    }
    template <typename F>
    DynamicMethodsAccess & setfRaw(::MetaLib::CallKey key, F && f)
    {
        BaseAccess::factor<1>()->setfRaw(std::move(key), std::forward<F>(f));
        return *this;
    }
};

template <typename Factor>
#define MetaLib_DMCompound CompoundBase<Factor, TypeFactor<std::false_type, CxxType<MethodTable> *>>
#define MetaLib_DMType DynamicMethodsType<Factor>
class DynamicMethodsType
    :
    public MetaLib_DMCompound,
    protected MethodTable,
    public ObjectCreatingMixin<MetaLib_DMType, DynamicMethodsAccess<typename MetaLib_DMCompound::CreatingAccessBase, MetaLib_DMType>>,
    public ObjectExposingMixin<MetaLib_DMType, DynamicMethodsAccess<typename MetaLib_DMCompound::ExposingAccessBase, MetaLib_DMType>>,
    public ObjectInvokingMixin<MetaLib_DMType>
{
    friend class CreatingMixin;
    friend class ExposingMixin;
    friend class InvokingMixin;
    friend class DynamicMethodsAccess<typename MetaLib_DMCompound::CreatingAccessBase, MetaLib_DMType>;
public:
    typedef OxAccess<MetaLib_Obj, ExposingAccess> Self;
protected:
    static MetaLib_Obj call(
        MetaLib_U8 * ptr,
        MetaLib_Type * type,
        MetaLib_USz key_length, MetaLib_U8 const * key_pointer,
        MetaLib_Obj * argv
    ) MetaLib_NOEXCEPT
    {
        //noexcept, so no need to try-catch

        auto pself = retrieveType(ptr, type, nullptr);

        //dynamic methods
        {
            ExposingAccess access;
            if (false == pself->retrieve(&access, ptr, type, nullptr))
            {
                argv[0].setGeneralException();//TODO: make a real exception
                return MetaLib_Obj::makeMark_Throw();
            }
            auto r = access.factor<1>()->call(ptr, type, ::MetaLib::CallKey(key_length, key_pointer), argv);
            if (false == r.isMark_NotCalled())
                return r;
        }

        //"static"/"type" methods
        {
            auto pmt = static_cast<MethodTable *>(pself);
            auto r = pmt->call(ptr, type, ::MetaLib::CallKey(key_length, key_pointer), argv);
            if (false == r.isMark_NotCalled())
                return r;
        }

        return MetaLib_DMCompound::call(ptr, type, key_length, key_pointer, argv);
    }
public:
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename ... A>
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access, A && ... a)
    {
        auto am = MetaLib_DMCompound::startAlloc(p_access);
        am.alloc<0>(std::forward<A>(a) ...);
        am.alloc<1>();//MethodTable
        return am.finish();
    }
    template <typename ... A>
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr, A && ... a)
    {
        auto im = MetaLib_DMCompound::startInit(p_access, ptr);
        im.init<0>(std::forward<A>(a) ...);
        im.init<1>();//MethodTable
        return im.finish();
    }
#else
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access)
    {
        auto am = MetaLib_DMCompound::startAlloc(p_access);
        am.alloc<0>();
        am.alloc<1>();
        return am.finish();
    }
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr)
    {
        auto im = MetaLib_DMCompound::startInit(p_access, ptr);
        im.init<0>();
        im.init<1>();
        return im.finish();
    }
    template <typename A0>
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access, A0 && a0)
    {
        auto am = MetaLib_DMCompound::startAlloc(p_access);
        am.alloc<0>(std::forward<A0>(a0));
        am.alloc<1>();
        return am.finish();
    }
    template <typename A0>
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr, A0 && a0)
    {
        auto im = MetaLib_DMCompound::startInit(p_access, ptr);
        im.init<0>(std::forward<A0>(a0));
        im.init<1>();
        return im.finish();
    }
    template <typename A0, typename A1>
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access, A0 && a0, A1 && a1)
    {
        auto am = MetaLib_DMCompound::startAlloc(p_access);
        am.alloc<0>(std::forward<A0>(a0), std::forward<A1>(a1));
        am.alloc<1>();
        return am.finish();
    }
    template <typename A0, typename A1>
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr, A0 && a0, A1 && a1)
    {
        auto im = MetaLib_DMCompound::startInit(p_access, ptr);
        im.init<0>(std::forward<A0>(a0), std::forward<A1>(a1));
        im.init<1>();
        return im.finish();
    }
#endif
public:
    template <typename A>
    DynamicMethodsType(A && a)
        :
        MetaLib_DMCompound(std::forward<A>(a), &CxxType<MethodTable>::Static)
    {
        MetaLib_Type::p_call = &call;
    }
public:
    template <typename UnpackSignature, typename F>
    DynamicMethodsType & def(::MetaLib::CallKey key, F && f)
    {
        MethodTable::setf<InvokingMixin::Invoker<UnpackSignature>>(std::move(key), std::forward<F>(f));
        return *this;
    }
    template <typename F>
    DynamicMethodsType & defRaw(::MetaLib::CallKey key, F && f)
    {
        MethodTable::setfRaw(std::move(key), std::forward<F>(f));
        return *this;
    }
};

template <typename Factor>
auto makeDMType(Factor && factor) -> DynamicMethodsType<typename std::decay<Factor>::type>
{
    return DynamicMethodsType<typename std::decay<Factor>::type>(std::forward<Factor>(factor));
}

template <typename T, T Type>
struct DMType
{
    static DynamicMethodsType<T> Static;
};
template <typename T, T Type> DynamicMethodsType<T> DMType<T, Type>::Static(Type);


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Callable Type"
#endif
/*************************************************************************************************************************************/

template <typename F>
class CxxRawCallableType
    :
    public CxxType<F>
{
protected:
    static MetaLib_Obj call(
        MetaLib_U8 * ptr,
        MetaLib_Type * type,
        MetaLib_USz key_length, MetaLib_U8 const * /*key_pointer*/,
        MetaLib_Obj * argv
    ) MetaLib_NOEXCEPT
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto pself = static_cast<CxxRawCallableType *>(type);
            if (key_length == 0)
            {
                ExposingAccess access;
                if (false == pself->retrieve(&access, ptr, type, nullptr))
                    throw 1;//TODO

                return (*access)(ptr, type, argv);
            }

            return MetaLib_Obj::makeMark_NotCalled();
        }
        catch (...)//TODO
        {
            argv[0].setGeneralException();//TODO: make a real exception
            return MetaLib_Obj::makeMark_Throw();
        }
    }
public:
    CxxRawCallableType()
    {
        MetaLib_Type::p_call = &call;
    }
    static CxxRawCallableType Static;
};
template <typename F> CxxRawCallableType<F> CxxRawCallableType<F>::Static;

template <typename F>
auto makeRCO(F && f) -> decltype(CxxRawCallableType<typename std::decay<F>::type>::Static(std::forward<F>(f)))
{
    return CxxRawCallableType<typename std::decay<F>::type>::Static(std::forward<F>(f));
}


template <typename UnpackSignature, typename F>
class CxxCallableType
    :
    public CxxType<F>,
    public ObjectInvokingMixin<CxxCallableType<UnpackSignature, F>>
{
public:
    typedef OxAccess<MetaLib_Obj, ExposingAccess> Self;
protected:
    static MetaLib_Obj call(
        MetaLib_U8 * ptr,
        MetaLib_Type * type,
        MetaLib_USz key_length, MetaLib_U8 const * /*key_pointer*/,
        MetaLib_Obj * argv
    ) MetaLib_NOEXCEPT
    {
        try
        {
            MetaLib_DEBUG_GUARD_FAILURE;

            auto pself = static_cast<CxxCallableType *>(type);
            if (key_length == 0)
            {
                ExposingAccess access;
                if (false == pself->retrieve(&access, ptr, type, 0))
                    throw 1;//TODO

                return Invoker<UnpackSignature>::invoke(*access, ptr, type, argv);
            }

            return MetaLib_Obj::makeMark_NotCalled();
        }
        catch (...)//TODO
        {
            argv[0].setGeneralException();//TODO: make a real exception
            return MetaLib_Obj::makeMark_Throw();
        }
    }
public:
    CxxCallableType()
    {
        MetaLib_Type::p_call = &call;
    }
    static CxxCallableType Static;
};
template <typename UnpackSignature, typename F> CxxCallableType<UnpackSignature, F> CxxCallableType<UnpackSignature, F>::Static;

template <typename UnpackSignature, typename F>//the first argument is the resulting object itself
auto makeCO(F && f) -> decltype(CxxCallableType<UnpackSignature, typename std::decay<F>::type>::Static(std::forward<F>(f)))
{
    return CxxCallableType<UnpackSignature, typename std::decay<F>::type>::Static(std::forward<F>(f));
}


/*************************************************************************************************************************************/
#ifdef __INTELLISENSE__
#error "Cxx Affix Type"
#endif
/*************************************************************************************************************************************/

template <typename BaseAccess>
class CxxAffixAccess : public BaseAccess
{
    auto affix() -> typename BaseAccess::template FactorInfo<1>::Access &
    {
        return BaseAccess::factor<1>();
    }
};

template <typename Affix, typename Factor>
#define MetaLib_CACompound CompoundBase<Factor, TypeFactor<std::false_type, CxxType<Affix> *>>
class CxxAffixType
    :
    public MetaLib_CACompound,
    public ObjectCreatingMixin<CxxAffixType<Affix, Factor>, CxxAffixAccess<typename MetaLib_CACompound::CreatingAccessBase>>,
    public ObjectExposingMixin<CxxAffixType<Affix, Factor>, CxxAffixAccess<typename MetaLib_CACompound::ExposingAccessBase>>
{
    friend class CreatingMixin;
    friend class ExposingMixin;
public:
#if MetaLib_HAS_VARIADIC_TEMPLATE
    template <typename AffixArg, typename ... A>
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access, AffixArg && affix_arg, A && ... a)
    {
        auto am = MetaLib_CACompound::startAlloc(p_access);
        am.alloc<0>(std::forward<A>(a) ...);
        am.alloc<1>(std::forward<AffixArg>(affix_arg));
        return am.finish();
    }
    template <typename AffixArg, typename ... A>
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr, AffixArg && affix_arg, A && ... a)
    {
        auto im = MetaLib_CACompound::startInit(p_access, ptr);
        im.init<0>(std::forward<A>(a) ...);
        im.init<1>(std::forward<AffixArg>(affix_arg));
        return im.finish();
    }
#else
    template <typename AffixArg>
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access, AffixArg && affix_arg)
    {
        auto am = MetaLib_CACompound::startAlloc(p_access);
        am.alloc<0>();
        am.alloc<1>(std::forward<AffixArg>(affix_arg));
        return am.finish();
    }
    template <typename AffixArg>
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr, AffixArg && affix_arg)
    {
        auto im = MetaLib_CACompound::startInit(p_access, ptr);
        im.init<0>();
        im.init<1>(std::forward<AffixArg>(affix_arg));
        return im.finish();
    }
    template <typename AffixArg, typename A0>
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access, AffixArg && affix_arg, A0 && a0)
    {
        auto am = MetaLib_CACompound::startAlloc(p_access);
        am.alloc<0>(std::forward<A0>(a0));
        am.alloc<1>(std::forward<AffixArg>(affix_arg));
        return am.finish();
    }
    template <typename AffixArg, typename A0>
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr, AffixArg && affix_arg, A0 && a0)
    {
        auto im = MetaLib_CACompound::startInit(p_access, ptr);
        im.init<0>(std::forward<A0>(a0));
        im.init<1>(std::forward<AffixArg>(affix_arg));
        return im.finish();
    }
    template <typename AffixArg, typename A0, typename A1>
    std::pair<size_t, size_t> alloc(CreatingAccess * p_access, AffixArg && affix_arg, A0 && a0, A1 && a1)
    {
        auto am = MetaLib_CACompound::startAlloc(p_access);
        am.alloc<0>(std::forward<A0>(a0), std::forward<A1>(a1));
        am.alloc<1>(std::forward<AffixArg>(affix_arg));
        return am.finish();
    }
    template <typename AffixArg, typename A0, typename A1>
    MetaLib_Type * init(CreatingAccess * p_access, MetaLib_U8 * ptr, AffixArg && affix_arg, A0 && a0, A1 && a1)
    {
        auto im = MetaLib_CACompound::startInit(p_access, ptr);
        im.init<0>(std::forward<A0>(a0), std::forward<A1>(a1));
        im.init<1>(std::forward<AffixArg>(affix_arg));
        return im.finish();
    }
#endif
public:
    template <typename A>
    CxxAffixType(A && a)
        :
        MetaLib_CACompound(std::forward<A>(a), &CxxType<Affix>::Static)
    {}
};

template <typename Affix, typename Factor>
auto makeCAType(Factor && factor) -> CxxAffixType<Affix, typename std::decay<Factor>::type>
{
    return CxxAffixType<Affix, typename std::decay<Factor>::type>(std::forward<Factor>(factor));
}

template <typename Affix, typename T, T Type>
struct CAType
{
    static CxxAffixType<Affix, T> Static;
};
template <typename Affix, typename T, T Type> CxxAffixType<Affix, T> CAType<Affix, T, Type>::Static(Type);


;
}


namespace MetaLib
{
    using MetaLibMetaTypePrivate::OxAccess;
    using MetaLibMetaTypePrivate::EmptyType;
    using MetaLibMetaTypePrivate::ObjectCreatingMixin;
    using MetaLibMetaTypePrivate::ObjectExposingMixin;
    using MetaLibMetaTypePrivate::DummyAccess;
    using MetaLibMetaTypePrivate::DummyType;
    using MetaLibMetaTypePrivate::Dummy_Type;
    using namespace MetaLibMetaTypePrivate::Compound;
    using MetaLibMetaTypePrivate::makeTypeFactor;
    using MetaLibMetaTypePrivate::CompoundBase;
    using MetaLibMetaTypePrivate::CxxType;
    using MetaLibMetaTypePrivate::MethodsType;
    using MetaLibMetaTypePrivate::CxxMType;
    using MetaLibMetaTypePrivate::DynamicMethodsType;
    using MetaLibMetaTypePrivate::makeDMType;
    using MetaLibMetaTypePrivate::DMType;
    using MetaLibMetaTypePrivate::makeRCO;
    using MetaLibMetaTypePrivate::makeCO;
    using MetaLibMetaTypePrivate::CAType;
}


#endif
