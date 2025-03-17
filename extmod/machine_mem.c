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
#include "py/objarray.h"
#include "extmod/modmachine.h"

#if MICROPY_PY_MACHINE_MEMX

const mp_obj_array_t machine_mem8_obj = {{&mp_type_memx}, MP_OBJ_ARRAY_TYPECODE_FLAG_RW | 'B', 0, SIZE_MAX + 1, (void *)0};
const mp_obj_array_t machine_mem16_obj = {{&mp_type_memx}, MP_OBJ_ARRAY_TYPECODE_FLAG_RW | 'H', 0, SIZE_MAX / 2 + 1, (void *)0};
const mp_obj_array_t machine_mem32_obj = {{&mp_type_memx}, MP_OBJ_ARRAY_TYPECODE_FLAG_RW | 'I', 0, SIZE_MAX / 4 + 1, (void *)0};

#endif // MICROPY_PY_MACHINE_MEMX
