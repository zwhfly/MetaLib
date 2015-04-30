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


#ifndef MetaLibRefCount_H_
#define MetaLibRefCount_H_


#include "MetaLibPort.h"


typedef MetaLib_ASz() MetaLib_RefCount;

static MetaLib_INLINE void MetaLib_RefCount_init(MetaLib_RefCount volatile * ref_count, MetaLib_USz v)
{
    MetaLib_ASz() t;

    MetaLib_ATOM_PACK(t, MetaLib_USz, v);
    MetaLib_ASz(_init)(ref_count, t);
}

static MetaLib_INLINE void MetaLib_RefCount_store(MetaLib_RefCount volatile * ref_count, MetaLib_USz v)
{
    MetaLib_ASz() t;

    MetaLib_ATOM_PACK(t, MetaLib_USz, v);
    MetaLib_ASz(_store)(ref_count, t);
}

static MetaLib_INLINE MetaLib_USz MetaLib_RefCount_load(MetaLib_RefCount const volatile * ref_count)
{
    MetaLib_ASz() r = MetaLib_ASz(_load)(ref_count);

    return MetaLib_ATOM_EXTRACT(r, MetaLib_USz);
}

static MetaLib_INLINE MetaLib_USz MetaLib_RefCount_xchg(MetaLib_RefCount volatile * ref_count, MetaLib_USz v)
{
    MetaLib_ASz() t;
    MetaLib_ASz() r;

    MetaLib_ATOM_PACK(t, MetaLib_USz, v);
    r = MetaLib_ASz(_xchg)(ref_count, t);
    return MetaLib_ATOM_EXTRACT(r, MetaLib_USz);
}

/* success: return non-0 */
/* failure: return 0 */
/* weak */
static MetaLib_INLINE MetaLib_U8 MetaLib_RefCount_cmpxchg(MetaLib_RefCount volatile * ref_count, MetaLib_USz * p_expected, MetaLib_USz exchange)
{
    MetaLib_ASz() expected;
    MetaLib_ASz() a_exchange;
    MetaLib_U8 r;

    MetaLib_ATOM_PACK(expected, MetaLib_USz, *p_expected);
    MetaLib_ATOM_PACK(a_exchange, MetaLib_USz, exchange);
    r = MetaLib_ASz(_cmpxchg)(ref_count, &expected, a_exchange);
    if (r == 0) *p_expected = MetaLib_ATOM_EXTRACT(expected, MetaLib_USz);
    return r;
}

/* success: return 0 */
/* failure: return non-0 */
static MetaLib_INLINE MetaLib_USz MetaLib_RefCount_getIncremented(MetaLib_USz * p_result, MetaLib_USz orig, MetaLib_SSz increment)
{
    MetaLib_USz n = (increment >= 0) ? (MetaLib_USz)(increment) : (0U - (MetaLib_USz)(increment));
    if (increment > 0)
    {
        MetaLib_USz r = orig + n;
        if (r < orig) return 1;/* TODO: error code */
        *p_result = r;
        return 0;
    }
    else if (increment < 0)
    {
        MetaLib_USz r = orig - n;
        if (r > orig) return 2;/* TODO: error code */
        *p_result = r;
        return 0;
    }
    else
    {
        *p_result = orig;
        return 0;
    }
}

/* success: return 0 */
/* failure: return non-0 */
static MetaLib_INLINE MetaLib_USz MetaLib_RefCount_increase(MetaLib_RefCount volatile * ref_count, MetaLib_USz * p_result, MetaLib_SSz increment)
{
    MetaLib_USz orig = MetaLib_RefCount_load(ref_count);
    if (increment == 0)
    {
        *p_result = orig;
        return 0;
    }
    for (;;)
    {
        MetaLib_USz t;
        if (MetaLib_USz err = MetaLib_RefCount_getIncremented(&t, orig, increment)) return err;
        if (MetaLib_RefCount_cmpxchg(ref_count, &orig, t))
        {
            *p_result = t;
            return 0;
        }
    }
}

/* success: return 0 */
/* failure: return non-0 */
/* If the original value is 0, this function returns success without increasing the ref_count. */
/* If increment < 0, this function returns failure. */
static MetaLib_INLINE MetaLib_USz MetaLib_RefCount_increaseIfNot0(MetaLib_RefCount volatile * ref_count, MetaLib_USz * p_result, MetaLib_SSz increment)
{
    if (increment < 0) return 3;/* TODO: error code */
    MetaLib_USz orig = MetaLib_RefCount_load(ref_count);
    if (increment == 0)
    {
        *p_result = orig;
        return 0;
    }
    for (;;)
    {
        MetaLib_USz t;
        if (orig == 0)
        {
            *p_result = 0;
            return 0;
        }
        if (MetaLib_USz err = MetaLib_RefCount_getIncremented(&t, orig, increment)) return err;
        if (MetaLib_RefCount_cmpxchg(ref_count, &orig, t))
        {
            *p_result = t;
            return 0;
        }
    }
}


#endif
