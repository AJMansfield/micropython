/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
 * Copyright (c) 2014-2017 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_PY_BINARY_H
#define MICROPY_INCLUDED_PY_BINARY_H

#include "py/obj.h"

// Use special typecode to differentiate repr() of bytearray vs array.array('B')
// (underlyingly they're same).  Can't use 0 here because that's used to detect
// type-specification errors due to end-of-string.
#define BYTEARRAY_TYPECODE 1


#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
#define _MP_TYPECODES_LONGINT \
    long long : 'q', \
    unsigned long long : 'Q',
#else
#define _MP_TYPECODES_LONGINT
#endif

#if MICROPY_PY_BUILTINS_FLOAT
#define _MP_TYPECODES_FLOAT \
    float : 'f', \
    double : 'd',
// array/memoryview of half-precision float (16-bit)
#define MP_TYPECODE_HALFFLOAT 'e'
#else
#define _MP_TYPECODES_FLOAT
#endif

#if MICROPY_PY_STRUCT_UNSAFE_TYPECODES
/* UNCRUSTIFY-OFF */
#define _MP_TYPECODES_UNSAFE_UNCRUSTIFY_WORKAROUND_STAR *
// note that mp_obj_t is equivalent to void *, and can't be distinguished by _Generic
#define _MP_TYPECODES_UNSAFE \
    char _MP_TYPECODES_UNSAFE_UNCRUSTIFY_WORKAROUND_STAR: 'S', \
    void _MP_TYPECODES_UNSAFE_UNCRUSTIFY_WORKAROUND_STAR: 'P',
/* UNCRUSTIFY-ON */
// array/memoryview of mp_obj_t
#define MP_TYPECODE_OBJECT 'O'
#else
#define _MP_TYPECODES_UNSAFE
#endif

// array/memoryview of the given C type
#define MP_TYPECODE_C(type) _Generic((type)0, \
    _MP_TYPECODES_LONGINT \
    _MP_TYPECODES_FLOAT \
    _MP_TYPECODES_UNSAFE \
    char : 'b', \
    signed char : 'b', \
    unsigned char : 'B', \
    short : 'h', \
    unsigned short : 'H', \
    int : 'i', \
    unsigned int : 'I', \
    long : 'l', \
    unsigned long : 'L' \
    )

// Used only for memoryview types, set in "typecode" to indicate a writable memoryview
#define MP_TYPECODE_FLAG_RW (0x80)

size_t mp_binary_get_size(char struct_type, char val_type, size_t *palign);
mp_obj_t mp_binary_get_val_array(char typecode, void *p, size_t index);
void mp_binary_set_val_array(char typecode, void *p, size_t index, mp_obj_t val_in);
void mp_binary_set_val_array_from_int(char typecode, void *p, size_t index, mp_int_t val);
mp_obj_t mp_binary_get_val(char struct_type, char val_type, byte *p_base, byte **ptr);
void mp_binary_set_val(char struct_type, char val_type, mp_obj_t val_in, byte *p_base, byte **ptr);
long long mp_binary_get_int(size_t size, bool is_signed, bool big_endian, const byte *src);
void mp_binary_set_int(size_t val_sz, bool big_endian, byte *dest, mp_uint_t val);

#endif // MICROPY_INCLUDED_PY_BINARY_H
