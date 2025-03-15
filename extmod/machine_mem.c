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

#include "py/runtime.h"
#include "extmod/modmachine.h"
#if MICROPY_PY_MACHINE_MEMX_SLICE
#include "py/objarray.h"
#endif


#if MICROPY_PY_MACHINE_MEMX

// If you wish to override the functions for mapping the machine_mem read/write
// address, then add a #define for MICROPY_MACHINE_MEM_GET_READ_ADDR and/or
// MICROPY_MACHINE_MEM_GET_WRITE_ADDR in your mpconfigport.h. Since the
// prototypes are identical, it is allowable for both of the macros to evaluate
// the to same function.
//
// It is expected that the modmachine.c file for a given port will provide the
// implementations, if the default implementation isn't used.

#if !defined(MICROPY_MACHINE_MEM_GET_READ_ADDR) || !defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
static uintptr_t machine_mem_get_addr(mp_obj_t addr_o, uint align) {
    uintptr_t addr = mp_obj_get_int_truncated(addr_o);
    if ((addr & (align - 1)) != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("address %08x is not aligned to %d bytes"), addr, align);
    }
    return addr;
}
#if !defined(MICROPY_MACHINE_MEM_GET_READ_ADDR)
#define MICROPY_MACHINE_MEM_GET_READ_ADDR machine_mem_get_addr
#endif
#if !defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
#define MICROPY_MACHINE_MEM_GET_WRITE_ADDR machine_mem_get_addr
#endif
#endif

static void machine_mem_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<%u-bit memory>", 8 * self->elem_size);
}

#if MICROPY_PY_MACHINE_MEMX_SLICE
typedef struct {
    uintptr_t start;
    uintptr_t stop;
    ptrdiff_t step;
} bound_ptr_slice_t;

// ordinary `mp_obj_slice_indices` into `mp_bound_slice_t` doesn't handle values above the signed integer limit
// much simpler version here:
static void ptr_slice_indices(mp_obj_t self_in, uint elem_size, bound_ptr_slice_t *result) {
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    uintptr_t start, stop;
    ptrdiff_t step;

    if (self->step == mp_const_none) {
        step = elem_size;
    } else {
        step = mp_obj_get_int(self->step);
        if (step != 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("slice step values (other than 1) are not supported"));
        }
        step *= elem_size;
    }

    if (self->start == mp_const_none) {
        start = 0;
    } else {
        start = mp_obj_get_int(self->start);
    }

    if (self->stop == mp_const_none) {
        stop = SIZE_MAX;
    } else {
        stop = mp_obj_get_int(self->stop);
    }

    if (stop < start) {
        mp_raise_ValueError(MP_ERROR_TEXT("only forward slices are permitted"));
    }

    if ((stop - start) % step != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("slice length not aligned to %d bytes"), step);
    }

    result->start = start;
    result->stop = stop;
    result->step = step;
}
#endif

static mp_obj_t machine_mem_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    } else if (value == MP_OBJ_SENTINEL) {
        // load

        #if MICROPY_PY_MACHINE_MEMX_SLICE && (MICROPY_MACHINE_MEM_GET_READ_ADDR == MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
        if (mp_obj_is_type(index, &mp_type_slice)) {
            bound_ptr_slice_t slice;
            ptr_slice_indices(index, self->elem_size, &slice);

            byte typecode = MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
            switch (self->elem_size) {
                case 1:
                    typecode |= 'B';
                    break;
                case 2:
                    typecode |= 'H';
                    break;
                default:
                    typecode |= 'I';
                    break;
            }

            uintptr_t addr = MICROPY_MACHINE_MEM_GET_READ_ADDR(mp_obj_new_int(slice.start), self->elem_size);
            size_t len = (slice.stop - slice.start) / self->elem_size;

            mp_obj_array_t *mv = m_new_obj(mp_obj_array_t);
            mp_obj_memoryview_init(mv, typecode, 0, len, (void *)addr);

            return MP_OBJ_FROM_PTR(mv);
        }
        #endif

        uintptr_t addr = MICROPY_MACHINE_MEM_GET_READ_ADDR(index, self->elem_size);
        uint32_t val;
        switch (self->elem_size) {
            case 1:
                val = (*(uint8_t *)addr);
                break;
            case 2:
                val = (*(uint16_t *)addr);
                break;
            default:
                val = (*(uint32_t *)addr);
                break;
        }
        return mp_obj_new_int(val);
    } else {
        // store

        #if MICROPY_PY_MACHINE_MEMX_SLICE
        if (mp_obj_is_type(index, &mp_type_slice)) {
            bound_ptr_slice_t slice;
            ptr_slice_indices(index, self->elem_size, &slice);

            mp_obj_iter_buf_t it_buf;
            mp_obj_t it = mp_getiter(value, &it_buf);
            mp_obj_t val_obj;

            for (uintptr_t idx = slice.start; idx < slice.stop; idx += slice.step) {
                val_obj = mp_iternext(it);
                if (val_obj == MP_OBJ_STOP_ITERATION) {
                    break;
                }

                uintptr_t addr = MICROPY_MACHINE_MEM_GET_READ_ADDR(mp_obj_new_int(idx), self->elem_size);
                uint32_t val = mp_obj_get_int_truncated(val_obj);

                switch (self->elem_size) {
                    case 1:
                        (*(uint8_t *)addr) = val;
                        break;
                    case 2:
                        (*(uint16_t *)addr) = val;
                        break;
                    default:
                        (*(uint32_t *)addr) = val;
                        break;
                }
            }
            return mp_const_none;
        }
        #endif

        uintptr_t addr = MICROPY_MACHINE_MEM_GET_WRITE_ADDR(index, self->elem_size);
        uint32_t val = mp_obj_get_int_truncated(value);
        switch (self->elem_size) {
            case 1:
                (*(uint8_t *)addr) = val;
                break;
            case 2:
                (*(uint16_t *)addr) = val;
                break;
            default:
                (*(uint32_t *)addr) = val;
                break;
        }
        return mp_const_none;
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_mem_type,
    MP_QSTR_mem,
    MP_TYPE_FLAG_NONE,
    print, machine_mem_print,
    subscr, machine_mem_subscr
    );

const machine_mem_obj_t machine_mem8_obj = {{&machine_mem_type}, 1};
const machine_mem_obj_t machine_mem16_obj = {{&machine_mem_type}, 2};
const machine_mem_obj_t machine_mem32_obj = {{&machine_mem_type}, 4};

#endif // MICROPY_PY_MACHINE_MEMX
