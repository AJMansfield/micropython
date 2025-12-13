/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_OBJTUPLE_H
#define MICROPY_INCLUDED_PY_OBJTUPLE_H

#include "py/obj.h"

typedef struct _mp_obj_tuple_t {
    mp_obj_base_t base;
    size_t len;
    mp_obj_t items[];
} mp_obj_tuple_t;

typedef struct _mp_rom_obj_tuple_t {
    mp_obj_base_t base;
    size_t len;
    mp_rom_obj_t items[];
} mp_rom_obj_tuple_t;

void mp_obj_tuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
mp_obj_t mp_obj_tuple_unary_op(mp_unary_op_t op, mp_obj_t self_in);
mp_obj_t mp_obj_tuple_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs);
mp_obj_t mp_obj_tuple_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t value);
mp_obj_t mp_obj_tuple_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf);

extern const mp_obj_type_t mp_type_attrtuple;

#define MP_DEFINE_ATTRTUPLE(tuple_obj_name, fields, nitems, ...) \
    const mp_rom_obj_tuple_t tuple_obj_name = { \
        .base = {.type = &mp_type_attrtuple}, \
        .len = nitems, \
        .items = { __VA_ARGS__, MP_ROM_PTR((void *)fields) } \
    }

/*
Macro to create an attrtuple object from an xlist enumerating the attrtuple's contained pairs of names and values.
```C
#define MY_ATTRS(X) \
    X(attr1, value1) \
    X(attr2, value2) \
    ...
mp_obj_tuple_t *my_attrtuple = MP_MAKE_ATTRTUPLE(my_attrtuple, MY_ATTRS);
#undef MY_ATTRS
```
*/
#define MP_MAKE_ATTRTUPLE(tuple_obj_name, XLIST) \
    mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, 1 XLIST(_MP_MAKE_ATTRTUPLE_COUNT_X), &mp_type_attrtuple); \
    do { \
        enum tuple_obj_name##_ATTRS: size_t { \
            XLIST(_MP_MAKE_ATTRTUPLE_INDEX_X) \
            tuple_obj_name##_NATTRS = XLIST(_MP_MAKE_ATTRTUPLE_COUNT_X) \
        }; \
        static const qstr tuple_obj_name##_FIELDS[tuple_obj_name##_NATTRS] = { XLIST(_MP_MAKE_ATTRTUPLE_FIELD_X) }; \
        tuple_obj_name->len = tuple_obj_name##_NATTRS; \
        do { \
            mp_obj_tuple_t *_MP_MAKE_ATTRTUPLE_TARGET = tuple_obj_name; \
            XLIST(_MP_MAKE_ATTRTUPLE_PLACE_X) \
        } while (0); \
        tuple_obj_name->items[tuple_obj_name##_NATTRS] = MP_OBJ_FROM_PTR(tuple_obj_name##_FIELDS); \
    } while (0)

#define _MP_MAKE_ATTRTUPLE_COUNT_X(...) + 1
#define _MP_MAKE_ATTRTUPLE_INDEX_X(name, ...) ATTRTUPLE_ATTR_##name,
#define _MP_MAKE_ATTRTUPLE_FIELD_X(name, ...) [ATTRTUPLE_ATTR_##name] = MP_QSTR_##name,
#define _MP_MAKE_ATTRTUPLE_VALUE_X(name, ...) [ATTRTUPLE_ATTR_##name] = __VA_ARGS__,
#define _MP_MAKE_ATTRTUPLE_PLACE_X(name, ...) _MP_MAKE_ATTRTUPLE_TARGET->items[ATTRTUPLE_ATTR_##name] = __VA_ARGS__;

#if MICROPY_PY_COLLECTIONS
void mp_obj_attrtuple_print_helper(const mp_print_t *print, const qstr *fields, mp_obj_tuple_t *o);
#endif

mp_obj_t mp_obj_new_attrtuple(const qstr *fields, size_t n, const mp_obj_t *items);

// type check is done on getiter method to allow tuple, namedtuple, attrtuple
#define mp_obj_is_tuple_compatible(o) (MP_OBJ_TYPE_GET_SLOT_OR_NULL(mp_obj_get_type(o), iter) == mp_obj_tuple_getiter)

#endif // MICROPY_INCLUDED_PY_OBJTUPLE_H
