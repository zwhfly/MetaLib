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


#include "MetaLibModule.h"


#include <Windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <system_error>


using namespace MetaLib;


static inline void throwLastError()
{
    throw std::system_error(GetLastError(), std::system_category());
}

MetaLib_Obj MetaLib_import(MetaLib_USz module_name_length, MetaLib_U8 const * module_name_pointer, MetaLib_Obj * p_exception)
{
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        auto module_name = reinterpret_cast<char const *>(module_name_pointer);
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
        auto ws = conv.from_bytes(module_name, module_name + module_name_length);
        auto h_dll = LoadLibraryW(ws.c_str());
        if (h_dll == NULL)
            throwLastError();
        auto proc = GetProcAddress(h_dll, u8"MetaLib_initModule");
        if (proc == NULL)
            throwLastError();
        auto init = reinterpret_cast<MetaLib_InitModule *>(proc);
        return init(p_exception);
    }
    catch (...)//TODO
    {
        //TODO: make a real exception
        p_exception->setGeneralException();
        return MetaLib_Obj::makeMark_Throw();
    }
}

