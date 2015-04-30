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


#ifndef MetaLibMisc_H_
#define MetaLibMisc_H_


#include "MetaLibPort.h"
#include "MetaLibObject.h"


MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Obj) MetaLib_readFile(MetaLib_USz file_path_length, MetaLib_U8 const * file_path_pointer, MetaLib_Obj * p_exception);
MetaLib_EXTERN_C MetaLib_EXP(MetaLib_Obj) MetaLib_writeFile(MetaLib_USz file_path_length, MetaLib_U8 const * file_path_pointer, MetaLib_USz data_length, MetaLib_U8 const * data_pointer, MetaLib_Obj * p_exception);


#ifdef __cplusplus


#include <string>


namespace MetaLibMiscPrivate
{
;

MetaLib_INLINE ::MetaLib::O readFile(::MetaLib::StrRef file_path)
{
    MetaLib_Obj exception;
    exception.setNull();
    auto r = MetaLib_readFile(file_path.len(), file_path.p(), &exception);
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

MetaLib_INLINE void writeFile(::MetaLib::StrRef file_path, ::MetaLib::StrRef data)
{
    MetaLib_Obj exception;
    exception.setNull();
    auto r = MetaLib_writeFile(file_path.len(), file_path.p(), data.len(), data.p(), &exception);
    if (r.isNone())
        return;
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
}

;
}


namespace MetaLib
{
    using MetaLibMiscPrivate::readFile;
    using MetaLibMiscPrivate::writeFile;
}


#endif


#endif
