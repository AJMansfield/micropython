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

#include <stdint.h>

#include "py/obj.h"

// Basic typecodes
#define MP_TYPECODE_CHAR 'b'
#define MP_TYPECODE_INT8 'b'
#define MP_TYPECODE_UINT8 'B'
#define MP_TYPECODE_INT16 'h'
#define MP_TYPECODE_UINT16 'H'
#define MP_TYPECODE_INT32 'i'
#define MP_TYPECODE_UINT32 'I'
#define MP_TYPECODE_INT64 'l'
#define MP_TYPECODE_UINT64 'L'
#define MP_TYPECODE_INT128 'q'
#define MP_TYPECODE_UINT128 'Q'

#define MP_TYPECODE_FLOAT16 'e'
#define MP_TYPECODE_FLOAT32 'f'
#define MP_TYPECODE_FLOAT64 'd'

// "Unsafe" typecodes used by unix ffi and struct extensions
#define MP_TYPECODE_OBJECT 'O'
#define MP_TYPECODE_CALLBACK 'C'
#define MP_TYPECODE_STRING 'S'
#define MP_TYPECODE_STRING_CONST 's'
#define MP_TYPECODE_POINTER 'P'
#define MP_TYPECODE_POINTER_CONST 'p'
#define MP_TYPECODE_VOID 'v'

// Special typecode to differentiate repr() of bytearray vs array.array('B')
// (underlyingly they're same).  Can't use 0 here because that's used to detect
// type-specification errors due to end-of-string.
#define MP_TYPECODE_BYTEARRAY 1

// Used only for memoryview types; OR with the typecode value to indicate a writable memoryview
#define MP_TYPECODE_FLAG_RW (0x80)

/*UNCRUSTIFY-OFF*/
// Helper macro to get typecode from C expression type
#define MP_TYPECODE_C(expr) _Generic(expr, \
    char : MP_TYPECODE_CHAR, \
    int8_t: MP_TYPECODE_INT8, \
    uint8_t: MP_TYPECODE_UINT8, \
    int16_t: MP_TYPECODE_INT16, \
    uint16_t: MP_TYPECODE_UINT16, \
    int32_t: MP_TYPECODE_INT32, \
    uint32_t: MP_TYPECODE_UINT32, \
    int64_t: MP_TYPECODE_INT64, \
    uint64_t: MP_TYPECODE_UINT64, \
    long long : MP_TYPECODE_INT128, \
    unsigned long long : MP_TYPECODE_UINT128, \
    float : MP_TYPECODE_FLOAT32, \
    double : MP_TYPECODE_FLOAT64, \
    char *: MP_TYPECODE_STRING, \
    const char *: MP_TYPECODE_STRING_CONST, \
    void *: MP_TYPECODE_POINTER, \
    const void *: MP_TYPECODE_POINTER_CONST)
/*UNCRUSTIFY-ON*/


size_t mp_binary_get_size(char struct_type, char val_type, size_t *palign);
mp_obj_t mp_binary_get_val_array(char typecode, void *p, size_t index);
void mp_binary_set_val_array(char typecode, void *p, size_t index, mp_obj_t val_in);
void mp_binary_set_val_array_from_int(char typecode, void *p, size_t index, mp_int_t val);
mp_obj_t mp_binary_get_val(char struct_type, char val_type, byte *p_base, byte **ptr);
void mp_binary_set_val(char struct_type, char val_type, mp_obj_t val_in, byte *p_base, byte **ptr);
long long mp_binary_get_int(size_t size, bool is_signed, bool big_endian, const byte *src);
void mp_binary_set_int(size_t val_sz, bool big_endian, byte *dest, mp_uint_t val);

#endif // MICROPY_INCLUDED_PY_BINARY_H
