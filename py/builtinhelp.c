/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/builtin.h"
#include "py/mpstate.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "py/qstr.h"

#if MICROPY_PY_BUILTINS_HELP

const char mp_help_default_text[] =
    "Welcome to MicroPython!\n"
    "\n"
    "For online docs please visit http://docs.micropython.org/\n"
    "\n"
    "Control commands:\n"
    "  CTRL-A        -- on a blank line, enter raw REPL mode\n"
    "  CTRL-B        -- on a blank line, enter normal REPL mode\n"
    "  CTRL-C        -- interrupt a running program\n"
    "  CTRL-D        -- on a blank line, exit or do a soft reset\n"
    "  CTRL-E        -- on a blank line, enter paste mode\n"
    "\n"
    "For further help on a specific object, type help(obj)\n"
;

#if MICROPY_PY_BUILTINS_HELP_MODULES
static void mp_help_add_from_map(mp_obj_t list, const mp_map_t *map) {
    for (size_t i = 0; i < map->alloc; i++) {
        if (mp_map_slot_is_filled(map, i)) {
            mp_obj_list_append(list, map->table[i].key);
        }
    }
}

#if MICROPY_MODULE_FROZEN
static void mp_help_add_from_names(mp_obj_t list, const char *name) {
    while (*name) {
        size_t len = strlen(name);
        // name should end in '.py' and we strip it off
        mp_obj_list_append(list, mp_obj_new_str(name, len - 3));
        name += len + 1;
    }
}
#endif

static void mp_help_print_modules(void) {
    mp_obj_t list = mp_obj_new_list(0, NULL);

    mp_help_add_from_map(list, &mp_builtin_module_map);
    #if MICROPY_HAVE_REGISTERED_EXTENSIBLE_MODULES
    mp_help_add_from_map(list, &mp_builtin_extensible_module_map);
    #endif

    #if MICROPY_MODULE_FROZEN
    extern const char mp_frozen_names[];
    mp_help_add_from_names(list, mp_frozen_names);
    #endif

    // sort the list so it's printed in alphabetical order
    mp_obj_list_sort(1, &list, (mp_map_t *)&mp_const_empty_map);

    // print the list of modules in a column-first order
    #define NUM_COLUMNS (4)
    #define COLUMN_WIDTH (18)
    size_t len;
    mp_obj_t *items;
    mp_obj_list_get(list, &len, &items);
    unsigned int num_rows = (len + NUM_COLUMNS - 1) / NUM_COLUMNS;
    for (unsigned int i = 0; i < num_rows; ++i) {
        unsigned int j = i;
        for (;;) {
            int l = mp_print_str(MP_PYTHON_PRINTER, mp_obj_str_get_str(items[j]));
            j += num_rows;
            if (j >= len) {
                break;
            }
            int gap = COLUMN_WIDTH - l;
            while (gap < 1) {
                gap += COLUMN_WIDTH;
            }
            while (gap--) {
                mp_print_str(MP_PYTHON_PRINTER, " ");
            }
        }
        mp_print_str(MP_PYTHON_PRINTER, "\n");
    }

    #if MICROPY_ENABLE_EXTERNAL_IMPORT
    // let the user know there may be other modules available from the filesystem
    mp_print_str(MP_PYTHON_PRINTER, "Plus any modules on the filesystem\n");
    #endif
}
#endif

static void mp_help_print_obj_attr(const mp_obj_t obj, const qstr name) {
    mp_obj_t dest[2];
    mp_load_method_protected(obj, name, dest, true);
    if (dest[0] == MP_OBJ_NULL) {
        return;
    }
    mp_print_str(MP_PYTHON_PRINTER, "  ");
    mp_obj_print(MP_OBJ_NEW_QSTR(name), PRINT_STR);
    mp_print_str(MP_PYTHON_PRINTER, " -- ");
    if (dest[1] != MP_OBJ_NULL) { // extra <class 'A'>.<function f>
        mp_obj_print(dest[1], PRINT_STR);
        mp_print_str(MP_PYTHON_PRINTER, ".");
        mp_obj_print(dest[0], PRINT_STR);
    } else {
        mp_obj_print(dest[0], PRINT_REPR);
    }
    mp_print_str(MP_PYTHON_PRINTER, "\n");

}

static void mp_help_print_obj(const mp_obj_t obj) {
    #if MICROPY_PY_BUILTINS_HELP_MODULES
    if (obj == MP_OBJ_NEW_QSTR(MP_QSTR_modules)) {
        mp_help_print_modules();
        return;
    }
    #endif

    const mp_obj_type_t *type = mp_obj_get_type(obj);

    // try to print something sensible about the given object
    mp_print_str(MP_PYTHON_PRINTER, "object ");
    mp_obj_print(obj, PRINT_STR);
    mp_printf(MP_PYTHON_PRINTER, " is of type %q\n", (qstr)type->name);

    mp_obj_t dest[2];
    mp_load_method_protected(obj, MP_QSTR___dir__, dest, true);
    if (dest[1] != MP_OBJ_NULL) {
        // probe __dir__ entries
        mp_obj_t dir = mp_call_method_n_kw(0, 0, dest);
        size_t len;
        mp_obj_t *items;
        mp_obj_list_get(dir, &len, &items);
        for (size_t i = 0; i < len; ++i) {
            mp_help_print_obj_attr(obj, mp_obj_str_get_qstr(items[i]));
        }
    } else {
        // probe all qstrs
        size_t nqstr = QSTR_TOTAL();
        for (qstr name = MP_QSTR_ + 1; name < nqstr; ++name) {
            mp_help_print_obj_attr(obj, name);
        }
    }
}

static mp_obj_t mp_builtin_help(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // print a general help message
        mp_print_str(MP_PYTHON_PRINTER, MICROPY_PY_BUILTINS_HELP_TEXT);
    } else {
        // try to print something sensible about the given object
        mp_help_print_obj(args[0]);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_help_obj, 0, 1, mp_builtin_help);

#endif // MICROPY_PY_BUILTINS_HELP
