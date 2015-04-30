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


#include "MetaLibMisc.h"


#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <locale>
#include <codecvt>
#include "MetaLibScopeGuard.h"
#include <limits>
#include "MetaLibBytes.h"


using namespace MetaLib;


static inline void throwLastError()
{
    throw std::system_error(GetLastError(), std::system_category());
}

MetaLib_Obj MetaLib_readFile(MetaLib_USz file_path_length, MetaLib_U8 const * file_path_pointer, MetaLib_Obj * p_exception)
{
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        auto file_path = reinterpret_cast<char const *>(file_path_pointer);
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
        auto ws = conv.from_bytes(file_path, file_path + file_path_length);
        HANDLE f = CreateFileW(ws.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
            throwLastError();
        auto guard_f = makeGuardIfSuccess([f](bool success)
        {
            if ((0 == CloseHandle(f)) && success)
                throwLastError();
        });

        DWORD size;
        {
            LARGE_INTEGER s;
            if (0 == GetFileSizeEx(f, &s))
                throwLastError();
            if (s.QuadPart > std::numeric_limits<DWORD>::max())
                throw 1;//TODO
            size = static_cast<DWORD>(s.QuadPart);
        }

        ::MetaLib::O r;

        constexpr size_t const Static_Buffer_Size = 32;
        U8 buffer[Static_Buffer_Size + 1] = { 0 };
        auto p = buffer;

        bool dynamic = size > Static_Buffer_Size;
        if (dynamic)
        {
            BytesType::CreatingAccess access;
            MetaLib_Obj t;
            Bytes.build(t, &access, size);
            r = ::MetaLib::O(::MetaLib::O::Owned(), t);
            p = const_cast<U8 *>(access.data());//TODO: const_cast
        }

        DWORD read = 0;
        auto left = size;
        while (left > 0)
        {
            DWORD bytes_read = 0;
            if (0 == ReadFile(f, p + read, left, &bytes_read, NULL))
                throwLastError();
            if (bytes_read > left)
                throw 1;//TODO
            left -= bytes_read;
            read += bytes_read;
        }

        if (!dynamic)
            r = Bytes(StrRef(size, buffer));

        return r.release();
    }
    catch (...)//TODO
    {
        p_exception->setGeneralException();//TODO: make a real exception
        return MetaLib_Obj::makeMark_Throw();
    }
}

MetaLib_Obj MetaLib_writeFile(MetaLib_USz file_path_length, MetaLib_U8 const * file_path_pointer, MetaLib_USz data_length, MetaLib_U8 const * data_pointer, MetaLib_Obj * p_exception)
{
    try
    {
        MetaLib_DEBUG_GUARD_FAILURE;

        auto file_path = reinterpret_cast<char const *>(file_path_pointer);
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
        auto ws = conv.from_bytes(file_path, file_path + file_path_length);
        HANDLE f = CreateFileW(ws.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
            throwLastError();
        auto guard_f_fail = makeGuardIfSuccess([&ws](bool success)
        {
            if ((0 == DeleteFileW(ws.c_str())) && success)
                throwLastError();
        });
        auto guard_f = makeGuardIfSuccess([f](bool success)
        {
            if ((0 == CloseHandle(f)) && success)
                throwLastError();
        });

        if (data_length > std::numeric_limits<DWORD>::max())
            throw 1;//TODO
        DWORD size = static_cast<DWORD>(data_length);

        while (size > 0)
        {
            DWORD bytes_written = 0;
            if (0 == WriteFile(f, data_pointer, size, &bytes_written, NULL))
                throwLastError();
            if (bytes_written > size)
                throw 1;
            size -= bytes_written;
            data_pointer += bytes_written;
        }

        guard_f_fail.release();

        return MetaLib_Obj::makeNone();
    }
    catch (...)//TODO
    {
        p_exception->setGeneralException();//TODO: make a real exception
        return MetaLib_Obj::makeMark_Throw();
    }
}
