/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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
#ifndef MICROPY_INCLUDED_EXTMOD_MODUTIME_H
#define MICROPY_INCLUDED_EXTMOD_MODUTIME_H

#include "py/obj.h"

// Whether to use floating point output for `time.time()`.
// Single-precision floats are not adequate; REPR_C floats can't represent whole numbers precisely
// past 4194304.0, i.e. any unix timestamp later than Feb 18, 1970. Double precision is needed
// for floating point output to be meaningfully useful.
#ifndef MICROPY_PY_TIME_FLOAT
#define MICROPY_PY_TIME_FLOAT (MICROPY_CPYTHON_COMPAT ? (MICROPY_PY_BUILTINS_FLOAT && MICROPY_FLOAT_IMPL >= MICROPY_FLOAT_IMPL_DOUBLE) : 0)
#endif

MP_DECLARE_CONST_FUN_OBJ_1(mp_time_mktime_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_time_sleep_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_time_sleep_ms_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_time_sleep_us_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_time_ticks_ms_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_time_ticks_us_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_time_ticks_cpu_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_time_ticks_diff_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_time_ticks_add_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_time_time_ns_obj);

#endif // MICROPY_INCLUDED_EXTMOD_MODUTIME_H
