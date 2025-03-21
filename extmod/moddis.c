#include <string.h>
#include <stdio.h>
#include "py/runtime.h"
#include "py/stream.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/objfun.h"
#include "py/objcode.h"
#include "py/parse.h"
#include "py/compile.h"
#include "py/frozenmod.h"
#include "py/persistentcode.h"
#include "extmod/moddis.h"

#if MICROPY_PY_DIS

struct _dis_bytecode_t {
    mp_obj_base_t base;
    mp_obj_code_t *code;
    mp_int_t first_line;
    mp_int_t current_offset;
    bool show_caches;
    bool adaptive;
    bool show_offsets;
};

enum _attrtuple_dis_instruction_field_t {
    FIELD_opcode,
    FIELD_opname,
    FIELD_arg,
    FIELD_argval,
    FIELD_argrepr,
    FIELD_offset,
    FIELD_starts_line,
    FIELD_line_number,
    FIELD_is_jump_target,
};
static qstr instruction_fields[] = {
    [FIELD_opcode        ] = MP_QSTR_opcode,
    [FIELD_opname        ] = MP_QSTR_opname,
    [FIELD_arg           ] = MP_QSTR_arg,
    [FIELD_argval        ] = MP_QSTR_argval,
    [FIELD_argrepr       ] = MP_QSTR_argrepr,
    [FIELD_offset        ] = MP_QSTR_offset,
    [FIELD_starts_line   ] = MP_QSTR_starts_line,
    [FIELD_line_number   ] = MP_QSTR_line_number,
    [FIELD_is_jump_target] = MP_QSTR_is_jump_target,
};
typedef mp_obj_t instruction_items_t[MP_ARRAY_SIZE(instruction_fields)];


typedef enum {
    DIS_PRINT_STR = PRINT_STR,
    DIS_PRINT_REPR = PRINT_REPR,
    DIS_PRINT_EXC = PRINT_EXC,
    DIS_PRINT_JSON = PRINT_JSON,
    DIS_PRINT_RAW = PRINT_RAW,
    DIS_PRINT_DIS,
    DIS_PRINT_INFO,
    DIS_PRINT_EXC_SUBCLASS = PRINT_EXC_SUBCLASS,
} dis_print_kind_t;

mp_compiled_module_t load_module(const char *file_str) {
    mp_compiled_module_t cm = {
        .context = m_new_obj(mp_module_context_t),
        .rc = NULL,
        #if MICROPY_PERSISTENT_CODE_SAVE
        .has_native = false,
        .n_qstr = 0,
        .n_obj = 0,
        #endif
    };

    // If we support frozen modules (either as str or mpy) then try to find the
    // requested filename in the list of frozen module filenames.
    #if MICROPY_MODULE_FROZEN
    void *modref;
    int frozen_type;
    const int frozen_path_prefix_len = strlen(MP_FROZEN_PATH_PREFIX);
    if (strncmp(file_str, MP_FROZEN_PATH_PREFIX, frozen_path_prefix_len) == 0) {
        mp_find_frozen_module(file_str + frozen_path_prefix_len, &frozen_type, &modref);

        // If we support frozen str modules and the compiler is enabled, and we
        // found the filename in the list of frozen files, then load and execute it.
        #if MICROPY_MODULE_FROZEN_STR
        if (frozen_type == MP_FROZEN_STR) {
            mp_lexer_t *lex = modref;
            mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
            mp_compile_to_raw_code(&parse_tree, lex->source_name, false, &cm);
            return cm;
        }
        #endif

        // If we support frozen mpy modules and we found a corresponding file (and
        // its data) in the list of frozen files, execute it.
        #if MICROPY_MODULE_FROZEN_MPY
        if (frozen_type == MP_FROZEN_MPY) {
            mp_frozen_module_t *frozen = modref;
            cm.context->constants = frozen->constants;

            if (mp_proto_fun_is_bytecode(frozen->proto_fun)) {
                const uint8_t *bc = frozen->proto_fun;
                MP_BC_PRELUDE_SIG_DECODE(bc);
                mp_obj_fun_bc_t *obj_fun_bc = MP_OBJ_TO_PTR(mp_obj_new_fun_bc(NULL, bc, cm.context, NULL));
                *cm.context = *obj_fun_bc->context;
                cm.rc = obj_fun_bc->rc;
                return cm;
            } else {
                cm.rc = (mp_raw_code_t *)frozen->proto_fun;
                return cm;
            }
        }
        #endif
    }

    #endif // MICROPY_MODULE_FROZEN

    size_t file_str_len = strlen(file_str);
    qstr file_qstr = qstr_from_str(file_str);

    // If we support loading .mpy files then check if the file extension is of
    // the correct format and, if so, load and execute the file.
    #if MICROPY_HAS_FILE_READER && MICROPY_PERSISTENT_CODE_LOAD
    if (file_str[file_str_len - 3] == 'm') {
        mp_raw_code_load_file(file_qstr, &cm);
        return cm;
    }
    #endif

    // If we can compile scripts then load the file and compile and execute it.
    #if MICROPY_ENABLE_COMPILER
    {
        mp_lexer_t *lex = mp_lexer_new_from_file(file_qstr);
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_compile_to_raw_code(&parse_tree, lex->source_name, false, &cm);
        return cm;
    }
    #else
    // If we get here then the file was not frozen and we can't compile scripts.
    mp_raise_msg(&mp_type_ImportError, MP_ERROR_TEXT("script compilation not supported"));
    #endif
}


static mp_obj_t dis_bytecode_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_x, ARG_first_line, ARG_current_offset, ARG_show_caches, ARG_adaptive, ARG_show_offsets };
    mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_OBJ_NULL} },
        { MP_QSTR_first_line, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_current_offset, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_show_caches, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_adaptive, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_show_offsets, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    dis_bytecode_t *self = mp_obj_malloc(dis_bytecode_t, type);
    if (mp_obj_is_type(args[ARG_x].u_obj, &mp_type_code)) {
        self->code = MP_OBJ_TO_PTR(args[ARG_x].u_obj);
        goto have_code;
    }

    mp_obj_t dest[2];
    mp_load_method_maybe(args[ARG_x].u_obj, MP_QSTR___code__, dest);

    if (dest[1] == MP_OBJ_NULL && dest[0] != MP_OBJ_NULL) {
        self->code = MP_OBJ_TO_PTR(dest[0]);
        goto have_code;
    }

    mp_load_method_maybe(args[ARG_x].u_obj, MP_QSTR___file__, dest);

    if (dest[1] == MP_OBJ_NULL && dest[0] != MP_OBJ_NULL) {
        const char *fname = mp_obj_str_get_str(dest[0]);
        mp_compiled_module_t module = load_module(fname);
        self->code = mp_obj_new_code(module.context, module.rc, true);
        goto have_code;
    }

    mp_raise_NotImplementedError(MP_ERROR_TEXT("unknown object type, cannot retrieve associated code"));

have_code:

    if (args[ARG_first_line].u_int >= 0) {
        self->first_line = args[ARG_first_line].u_int;
    } else {
        self->first_line = mp_obj_int_get_truncated(mp_load_attr(MP_OBJ_FROM_PTR(self->code), MP_QSTR_co_firstlineno));
    }

    self->current_offset = args[ARG_current_offset].u_int;
    self->show_caches = args[ARG_show_caches].u_bool;
    self->adaptive = args[ARG_adaptive].u_bool;
    self->show_offsets = args[ARG_show_offsets].u_bool;

    return MP_OBJ_FROM_PTR(self);
}

static void dis_bytecode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    dis_bytecode_t *self = MP_OBJ_TO_PTR(self_in);
    const mp_raw_code_t *rc = self->code->rc;
    size_t fun_data_len = rc->fun_data_len;
    const mp_module_constants_t *cm = &self->code->context->constants;

    switch ((dis_print_kind_t)kind) {
        case DIS_PRINT_STR:
        case DIS_PRINT_REPR:
        default:
            mp_printf(print, "<dis.bytecode code=");
            mp_obj_print_helper(print, MP_OBJ_FROM_PTR(self->code), PRINT_REPR);
            mp_printf(print, ", first_line=%d, current_offset=%d, show_caches=%b, adaptive=%b, show_offsets=%b>",
                self->first_line, self->current_offset, self->show_caches, self->adaptive, self->show_offsets);
            break;

        case DIS_PRINT_DIS:
            mp_bytecode_print(print, rc, fun_data_len, cm);
            break;

        case DIS_PRINT_INFO:
            const byte *const ip_start = rc->fun_data;
            const byte *ip = rc->fun_data;

            // Decode prelude
            MP_BC_PRELUDE_SIG_DECODE(ip);
            MP_BC_PRELUDE_SIZE_DECODE(ip);
            const byte *code_info = ip;

            qstr block_name = mp_decode_uint(&code_info);
            #if MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE
            block_name = cm->qstr_table[block_name];
            qstr source_file = cm->qstr_table[0];
            #else
            qstr source_file = cm->source_file;
            #endif
            mp_printf(print, "File %s, code block '%s' (descriptor: %p, bytecode @%p %u bytes)\n",
                qstr_str(source_file), qstr_str(block_name), rc, ip_start, (unsigned)fun_data_len);
            break;
    }
}

static mp_obj_t dis_bytecode_dis(mp_obj_t self_in) {
    dis_bytecode_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16 * self->code->rc->fun_data_len, &print);
    mp_obj_print_helper(&print, self_in, (mp_print_kind_t)DIS_PRINT_DIS);
    return mp_obj_new_str_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(dis_bytecode_dis_obj, dis_bytecode_dis);

static mp_obj_t dis_bytecode_info(mp_obj_t self_in) {
    dis_bytecode_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16 * self->code->rc->fun_data_len, &print);
    mp_obj_print_helper(&print, self_in, (mp_print_kind_t)DIS_PRINT_INFO);
    return mp_obj_new_str_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(dis_bytecode_info_obj, dis_bytecode_info);

static void dis_bytecode_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    dis_bytecode_t *self = MP_OBJ_TO_PTR(self_in);
    switch (attr) {
        case MP_QSTR_codeobj:
            dest[0] = MP_OBJ_FROM_PTR(self->code);
            break;
        case MP_QSTR_first_line:
            dest[0] = MP_OBJ_NEW_SMALL_INT(self->first_line);
            break;
        case MP_QSTR_dis:
            dest[0] = MP_OBJ_FROM_PTR(&dis_bytecode_dis_obj);
            dest[1] = self_in;
            break;
        case MP_QSTR_info:
            dest[0] = MP_OBJ_FROM_PTR(&dis_bytecode_info_obj);
            dest[1] = self_in;
            break;
    }
}

struct _dis_bytecode_iter_t {
    mp_obj_base_t base;
    const byte *ip;
    const byte *start;
    const byte *end;
    const mp_raw_code_t *rc;
    const mp_module_constants_t *cm;
};

static mp_obj_t dis_bytecode_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    (void)iter_buf;

    dis_bytecode_t *self = MP_OBJ_TO_PTR(self_in);
    dis_bytecode_iter_t *iter = mp_obj_malloc(dis_bytecode_iter_t, &dis_bytecode_iterator_type);

    const mp_raw_code_t *rc = self->code->rc;

    const byte *pre_start = rc->fun_data;
    const byte *code_start = rc->prelude.opcodes;
    const byte *pre_end = code_start;
    const byte *code_end = pre_start + rc->fun_data_len;

    iter->ip = code_start;
    iter->start = code_start;
    iter->end = code_end;
    iter->rc = rc;
    iter->cm = &self->code->context->constants;

    (void)pre_start;
    (void)code_start;
    (void)pre_end;
    (void)code_end;

    return MP_OBJ_FROM_PTR(iter);
}
static inline size_t mp_bytecode_get_source_starts_line(const byte *line_info, const byte *line_info_top, size_t bc_offset, bool *starts_line) {
    size_t source_line = 1;
    while (line_info < line_info_top) {
        size_t c = *line_info;
        size_t b, l;
        if ((c & 0x80) == 0) {
            // 0b0LLBBBBB encoding
            b = c & 0x1f;
            l = c >> 5;
            line_info += 1;
        } else {
            // 0b1LLLBBBB 0bLLLLLLLL encoding (l's LSB in second byte)
            b = c & 0xf;
            l = ((c << 4) & 0x700) | line_info[1];
            line_info += 2;
        }
        if (bc_offset >= b) {
            bc_offset -= b;
            source_line += l;
        } else {
            // found source line corresponding to bytecode offset
            break;
        }
    }
    *starts_line = (bc_offset == 0);
    return source_line;
}
static mp_obj_t dis_bytecode_iternext(mp_obj_t self_in) {
    dis_bytecode_iter_t *iter = MP_OBJ_TO_PTR(self_in);
    instruction_items_t instr = { [0 ... MP_ARRAY_SIZE(instruction_fields) - 1] = mp_const_none};

    if (iter->ip >= iter->end) {
        return MP_OBJ_STOP_ITERATION;
    }

    size_t start_offset = iter->ip - iter->start;
    instr[FIELD_offset] = mp_obj_new_int_from_uint(start_offset);

    const mp_bytecode_prelude_t *prelude = &iter->rc->prelude;
    bool starts_line;
    instr[FIELD_line_number] = MP_OBJ_NEW_SMALL_INT(mp_bytecode_get_source_starts_line(prelude->line_info, prelude->line_info_top, start_offset, &starts_line));
    instr[FIELD_starts_line] = mp_obj_new_bool(starts_line);

    instr[FIELD_opcode] = MP_OBJ_NEW_SMALL_INT(*iter->ip);

    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16, &print);
    iter->ip = mp_bytecode_print_str(&print, iter->start, iter->ip, iter->rc->children, iter->cm);

    char *str = vstr_null_terminated_str(&vstr);
    char *split = strstr(str, " ");
    if (!split) {
        split = str + vstr.len;
    }
    instr[FIELD_opname] = mp_obj_new_str(str, split - str);
    instr[FIELD_arg] = mp_obj_new_str(split, vstr.len - (split - str));

    return mp_obj_new_attrtuple(instruction_fields, MP_ARRAY_SIZE(instruction_fields), instr);
}
static const mp_getiter_iternext_custom_t dis_bytecode_getiter_iternext = {
    .getiter = dis_bytecode_getiter,
    .iternext = dis_bytecode_iternext,
};
MP_DEFINE_CONST_OBJ_TYPE(
    dis_bytecode_iterator_type,
    MP_QSTR_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, dis_bytecode_iternext
    );

static const mp_rom_map_elem_t dis_bytecode_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_dis), MP_ROM_PTR(&dis_bytecode_dis_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&dis_bytecode_info_obj) },

    // class methods
    { MP_ROM_QSTR(MP_QSTR_from_traceback), MP_ROM_NONE },
};
static MP_DEFINE_CONST_DICT(dis_bytecode_locals_dict, dis_bytecode_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    dis_bytecode_type,
    MP_QSTR_Bytecode,
    MP_TYPE_FLAG_ITER_IS_CUSTOM,
    make_new, dis_bytecode_make_new,
    print, dis_bytecode_print,
    attr, dis_bytecode_attr,
    iter, &dis_bytecode_getiter_iternext,
    locals_dict, &dis_bytecode_locals_dict
    );

static mp_print_t get_print_for_file(mp_obj_t file) {
    #if MICROPY_PY_IO
    if (file != mp_const_none) {
        mp_get_stream_raise(file, MP_STREAM_OP_WRITE);
        return (mp_print_t) {file, mp_stream_write_adaptor};
    }
    #endif
    return *(MP_PYTHON_PRINTER);
}

static mp_obj_t dis_codeinfo(mp_obj_t x) {
    mp_obj_t o = dis_bytecode_make_new(&dis_bytecode_type, 1, 0, &x);
    return dis_bytecode_info(o);
}
static MP_DEFINE_CONST_FUN_OBJ_1(dis_codeinfo_obj, dis_codeinfo);

static mp_obj_t dis_showcode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // dis, disassemble, disco
    enum { ARG_x, ARG_file };
    mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_OBJ_NULL} },
        { MP_QSTR_file, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_print_t print = get_print_for_file(args[ARG_file].u_obj);

    mp_obj_t o = dis_bytecode_make_new(&dis_bytecode_type, 1, 0, &args[ARG_x].u_obj);
    dis_bytecode_print(&print, o, (mp_print_kind_t)DIS_PRINT_INFO);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(dis_showcode_obj, 1, dis_showcode);

static mp_obj_t dis_dis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // dis, disassemble, disco
    enum { ARG_x, ARG_lasti, ARG_file, ARG_depth, ARG_show_caches, ARG_adaptive };
    mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_OBJ_NULL} },
        { MP_QSTR_lasti, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_file, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_depth, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_show_caches, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_adaptive, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_print_t print = get_print_for_file(args[ARG_file].u_obj);

    mp_obj_t o = dis_bytecode_make_new(&dis_bytecode_type, 1, 0, &args[ARG_x].u_obj);
    dis_bytecode_print(&print, o, (mp_print_kind_t)DIS_PRINT_DIS);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(dis_dis_obj, 1, dis_dis);

static mp_obj_t dis_getinstructions(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // dis, disassemble, disco
    enum { ARG_x, ARG_first_line, ARG_show_caches, ARG_adaptive };
    mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_OBJ_NULL} },
        { MP_QSTR_first_line, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_show_caches, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_adaptive, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t o = dis_bytecode_make_new(&dis_bytecode_type, 2, 0, &args[ARG_x].u_obj);
    return dis_bytecode_getiter(o, NULL);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(dis_getinstructions_obj, 1, dis_getinstructions);

static const mp_rom_map_elem_t mp_module_dis_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dis) },
    { MP_ROM_QSTR(MP_QSTR_Bytecode), MP_ROM_PTR(&dis_bytecode_type) },
    { MP_ROM_QSTR(MP_QSTR_Instruction), MP_ROM_PTR(&mp_type_attrtuple) },
    { MP_ROM_QSTR(MP_QSTR_Positions), MP_ROM_PTR(&mp_type_attrtuple) },
    { MP_ROM_QSTR(MP_QSTR_code_info),  MP_ROM_PTR(&dis_codeinfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_show_code),  MP_ROM_PTR(&dis_showcode_obj) },
    { MP_ROM_QSTR(MP_QSTR_dis), MP_ROM_PTR(&dis_dis_obj) },
    // { MP_ROM_QSTR(MP_QSTR_distb), MP_ROM_NONE },
    { MP_ROM_QSTR(MP_QSTR_disassemble), MP_ROM_PTR(&dis_dis_obj) },
    { MP_ROM_QSTR(MP_QSTR_disco), MP_ROM_PTR(&dis_dis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_instructions), MP_ROM_PTR(&dis_getinstructions_obj) },
    // { MP_ROM_QSTR(MP_QSTR_findlinestarts), MP_ROM_NONE },
    // { MP_ROM_QSTR(MP_QSTR_findlabels), MP_ROM_NONE },
    // { MP_ROM_QSTR(MP_QSTR_stack_effect), MP_ROM_NONE },
};
static MP_DEFINE_CONST_DICT(mp_module_dis_globals, mp_module_dis_globals_table);

const mp_obj_module_t mp_module_dis = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_dis_globals,
};

MP_REGISTER_MODULE(MP_QSTR_dis, mp_module_dis);
#endif // MICROPY_PY_DIS
