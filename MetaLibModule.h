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


#ifndef MetaLibModule_H_
#define MetaLibModule_H_


#include "MetaLibPort.h"
#include "MetaLibObject.h"


typedef MetaLib_Obj MetaLib_InitModule(MetaLib_Obj * p_exception) /*MetaLib_NOEXCEPT*/;//TODO: c++1z noexcept? (N4320)


#ifdef MetaLib_MakeModule
#ifdef __cplusplus//C++ is needed to make a MetaLib module

#include <type_traits>

extern ::MetaLib::O initMetaLibModule();

namespace MetaLibModulePrivate
{
;

MetaLib_INLINE MetaLib_Obj getModuleObj()
{
    auto f = []() -> MetaLib_Obj
    {
        auto obj = initMetaLibModule().release();
        return obj;
    };
    static MetaLib_Obj const export_object = f();
    return export_object;
}

;
}

MetaLib_EXTERN_C MetaLib_INLINE MetaLib_DL_EXP(MetaLib_Obj) MetaLib_initModule(MetaLib_Obj * p_exception) MetaLib_NOEXCEPT//DO NOT call directly
{
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        MetaLib_STATIC_ASSERT((std::is_same<MetaLib_InitModule *, decltype(&MetaLib_initModule)>::value), "Unexpected type.");

        auto r = MetaLibModulePrivate::getModuleObj();
        if (r.isDyn())
            r.incDynContentRef();
        return r;
    }
    catch (...)//TODO
    {
        //TODO: make a real exception
        p_exception->setGeneralException();
        return MetaLib_Obj::makeMark_Throw();
    }
}

#endif
#endif


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Obj) MetaLib_import(MetaLib_USz module_name_length, MetaLib_U8 const * module_name_pointer, MetaLib_Obj * p_exception);


#ifdef __cplusplus

#include <string>

namespace MetaLibModulePrivate
{
;

MetaLib_INLINE ::MetaLib::O import(::MetaLib::StrRef module_name)
{
    MetaLib_Obj exception;
    exception.setNull();
    auto r = MetaLib_import(module_name.len(), module_name.p(), &exception);
    if (r.isMark())
    {
        if (r.isMark_Throw())
        {
            MetaLib_ASSERT(false == exception.isMark());
            throw ::MetaLib::O(::MetaLib::O::Owned(), exception);
        }
        throw ::MetaLib::O::GeneralException;//TODO: make a real exception
    }
    return ::MetaLib::O(::MetaLib::O::Owned(), r);
}

;
}


namespace MetaLib
{
    using MetaLibModulePrivate::import;
}

#endif


#endif
