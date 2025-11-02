#include "py/obj.h"
#include "py/bc0.h"
#include "py/bc.h"
#include "py/objtuple.h"


// MP_QSTR_opcode         // numeric code for operation, corresponding to the opcode values listed below and the bytecode values in the Opcode collections.
// MP_QSTR_opname         // human readable name for operation
// MP_QSTR_arg            // numeric argument to operation (if any), otherwise None
// MP_QSTR_oparg          // alias for arg
// MP_QSTR_argval         // resolved arg value (if any), otherwise None
// MP_QSTR_offset         // start index of operation within bytecode sequence
// MP_QSTR_starts_line    // True if this opcode starts a source line, otherwise False
// MP_QSTR_line_number    // source line number associated with this opcode (if any), otherwise None
// MP_QSTR_is_jump_target // True if other code jumps to here, otherwise False
// MP_QSTR_jump_target    // bytecode index of the jump target if this is a jump operation, otherwise None
// MP_QSTR_positions      // dis.Positions object holding the start and end locations that are covered by this instruction.



// MP_QSTR_opmap      // Dictionary mapping operation names to bytecodes.
// MP_QSTR_cmp_op     // Sequence of all compare operation names.
// MP_QSTR_opname     // Sequence of operation names, indexable using the bytecode.
// MP_QSTR_hasarg     // Sequence of bytecodes that use their argument.
// MP_QSTR_hasconst   // Sequence of bytecodes that access a constant.
// MP_QSTR_hasfree    // Sequence of bytecodes that access a free (closure) variable. ‘free’ in this context refers to names in the current scope that are referenced by inner scopes or names in outer scopes that are referenced from this scope. It does not include references to global or builtin scopes.
// MP_QSTR_hasname    // Sequence of bytecodes that access an attribute by name.
// MP_QSTR_hasjump    // Sequence of bytecodes that have a jump target. All jumps are relative.
// MP_QSTR_haslocal   // Sequence of bytecodes that access a local variable.
// MP_QSTR_hascompare // Sequence of bytecodes of Boolean operations.
// MP_QSTR_hasexc     // Sequence of bytecodes that set an exception handler.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
static const mp_rom_obj_tuple_t opname_obj = {
    .base = {.type = &mp_type_tuple},
    .len = 0x100,
    .items = {
        [0 ... 0xFF] = MP_ROM_NONE,
        [MP_BC_LOAD_CONST_SMALL_INT_MULTI ... MP_BC_LOAD_CONST_SMALL_INT_MULTI + MP_BC_LOAD_CONST_SMALL_INT_MULTI_NUM] = MP_ROM_QSTR(MP_QSTR_LOAD_CONST_SMALL_INT_MULTI),
        [MP_BC_LOAD_FAST_MULTI            ... MP_BC_LOAD_FAST_MULTI            + MP_BC_LOAD_FAST_MULTI_NUM           ] = MP_ROM_QSTR(MP_QSTR_LOAD_FAST_MULTI           ),
        [MP_BC_STORE_FAST_MULTI           ... MP_BC_STORE_FAST_MULTI           + MP_BC_STORE_FAST_MULTI_NUM          ] = MP_ROM_QSTR(MP_QSTR_STORE_FAST_MULTI          ),
        [MP_BC_UNARY_OP_MULTI             ... MP_BC_UNARY_OP_MULTI             + MP_BC_UNARY_OP_MULTI_NUM            ] = MP_ROM_QSTR(MP_QSTR_UNARY_OP_MULTI            ),
        [MP_BC_BINARY_OP_MULTI            ... MP_BC_BINARY_OP_MULTI            + MP_BC_BINARY_OP_MULTI_NUM           ] = MP_ROM_QSTR(MP_QSTR_BINARY_OP_MULTI           ),
        [MP_BC_LOAD_CONST_FALSE     ] = MP_ROM_QSTR(MP_QSTR_LOAD_CONST_FALSE     ),
        [MP_BC_LOAD_CONST_NONE      ] = MP_ROM_QSTR(MP_QSTR_LOAD_CONST_NONE      ),
        [MP_BC_LOAD_CONST_TRUE      ] = MP_ROM_QSTR(MP_QSTR_LOAD_CONST_TRUE      ),
        [MP_BC_LOAD_CONST_SMALL_INT ] = MP_ROM_QSTR(MP_QSTR_LOAD_CONST_SMALL_INT ),
        [MP_BC_LOAD_CONST_STRING    ] = MP_ROM_QSTR(MP_QSTR_LOAD_CONST_STRING    ),
        [MP_BC_LOAD_CONST_OBJ       ] = MP_ROM_QSTR(MP_QSTR_LOAD_CONST_OBJ       ),
        [MP_BC_LOAD_NULL            ] = MP_ROM_QSTR(MP_QSTR_LOAD_NULL            ),
        [MP_BC_LOAD_FAST_N          ] = MP_ROM_QSTR(MP_QSTR_LOAD_FAST_N          ),
        [MP_BC_LOAD_DEREF           ] = MP_ROM_QSTR(MP_QSTR_LOAD_DEREF           ),
        [MP_BC_LOAD_NAME            ] = MP_ROM_QSTR(MP_QSTR_LOAD_NAME            ),
        [MP_BC_LOAD_GLOBAL          ] = MP_ROM_QSTR(MP_QSTR_LOAD_GLOBAL          ),
        [MP_BC_LOAD_ATTR            ] = MP_ROM_QSTR(MP_QSTR_LOAD_ATTR            ),
        [MP_BC_LOAD_METHOD          ] = MP_ROM_QSTR(MP_QSTR_LOAD_METHOD          ),
        [MP_BC_LOAD_SUPER_METHOD    ] = MP_ROM_QSTR(MP_QSTR_LOAD_SUPER_METHOD    ),
        [MP_BC_LOAD_BUILD_CLASS     ] = MP_ROM_QSTR(MP_QSTR_LOAD_BUILD_CLASS     ),
        [MP_BC_LOAD_SUBSCR          ] = MP_ROM_QSTR(MP_QSTR_LOAD_SUBSCR          ),
        [MP_BC_STORE_FAST_N         ] = MP_ROM_QSTR(MP_QSTR_STORE_FAST_N         ),
        [MP_BC_STORE_DEREF          ] = MP_ROM_QSTR(MP_QSTR_STORE_DEREF          ),
        [MP_BC_STORE_NAME           ] = MP_ROM_QSTR(MP_QSTR_STORE_NAME           ),
        [MP_BC_STORE_GLOBAL         ] = MP_ROM_QSTR(MP_QSTR_STORE_GLOBAL         ),
        [MP_BC_STORE_ATTR           ] = MP_ROM_QSTR(MP_QSTR_STORE_ATTR           ),
        [MP_BC_STORE_SUBSCR         ] = MP_ROM_QSTR(MP_QSTR_STORE_SUBSCR         ),
        [MP_BC_DELETE_FAST          ] = MP_ROM_QSTR(MP_QSTR_DELETE_FAST          ),
        [MP_BC_DELETE_DEREF         ] = MP_ROM_QSTR(MP_QSTR_DELETE_DEREF         ),
        [MP_BC_DELETE_NAME          ] = MP_ROM_QSTR(MP_QSTR_DELETE_NAME          ),
        [MP_BC_DELETE_GLOBAL        ] = MP_ROM_QSTR(MP_QSTR_DELETE_GLOBAL        ),
        [MP_BC_DUP_TOP              ] = MP_ROM_QSTR(MP_QSTR_DUP_TOP              ),
        [MP_BC_DUP_TOP_TWO          ] = MP_ROM_QSTR(MP_QSTR_DUP_TOP_TWO          ),
        [MP_BC_POP_TOP              ] = MP_ROM_QSTR(MP_QSTR_POP_TOP              ),
        [MP_BC_ROT_TWO              ] = MP_ROM_QSTR(MP_QSTR_ROT_TWO              ),
        [MP_BC_ROT_THREE            ] = MP_ROM_QSTR(MP_QSTR_ROT_THREE            ),
        [MP_BC_UNWIND_JUMP          ] = MP_ROM_QSTR(MP_QSTR_UNWIND_JUMP          ),
        [MP_BC_JUMP                 ] = MP_ROM_QSTR(MP_QSTR_JUMP                 ),
        [MP_BC_POP_JUMP_IF_TRUE     ] = MP_ROM_QSTR(MP_QSTR_POP_JUMP_IF_TRUE     ),
        [MP_BC_POP_JUMP_IF_FALSE    ] = MP_ROM_QSTR(MP_QSTR_POP_JUMP_IF_FALSE    ),
        [MP_BC_JUMP_IF_TRUE_OR_POP  ] = MP_ROM_QSTR(MP_QSTR_JUMP_IF_TRUE_OR_POP  ),
        [MP_BC_JUMP_IF_FALSE_OR_POP ] = MP_ROM_QSTR(MP_QSTR_JUMP_IF_FALSE_OR_POP ),
        [MP_BC_SETUP_WITH           ] = MP_ROM_QSTR(MP_QSTR_SETUP_WITH           ),
        [MP_BC_SETUP_EXCEPT         ] = MP_ROM_QSTR(MP_QSTR_SETUP_EXCEPT         ),
        [MP_BC_SETUP_FINALLY        ] = MP_ROM_QSTR(MP_QSTR_SETUP_FINALLY        ),
        [MP_BC_POP_EXCEPT_JUMP      ] = MP_ROM_QSTR(MP_QSTR_POP_EXCEPT_JUMP      ),
        [MP_BC_FOR_ITER             ] = MP_ROM_QSTR(MP_QSTR_FOR_ITER             ),
        [MP_BC_WITH_CLEANUP         ] = MP_ROM_QSTR(MP_QSTR_WITH_CLEANUP         ),
        [MP_BC_END_FINALLY          ] = MP_ROM_QSTR(MP_QSTR_END_FINALLY          ),
        [MP_BC_GET_ITER             ] = MP_ROM_QSTR(MP_QSTR_GET_ITER             ),
        [MP_BC_GET_ITER_STACK       ] = MP_ROM_QSTR(MP_QSTR_GET_ITER_STACK       ),
        [MP_BC_BUILD_TUPLE          ] = MP_ROM_QSTR(MP_QSTR_BUILD_TUPLE          ),
        [MP_BC_BUILD_LIST           ] = MP_ROM_QSTR(MP_QSTR_BUILD_LIST           ),
        [MP_BC_BUILD_MAP            ] = MP_ROM_QSTR(MP_QSTR_BUILD_MAP            ),
        [MP_BC_STORE_MAP            ] = MP_ROM_QSTR(MP_QSTR_STORE_MAP            ),
        [MP_BC_BUILD_SET            ] = MP_ROM_QSTR(MP_QSTR_BUILD_SET            ),
        [MP_BC_BUILD_SLICE          ] = MP_ROM_QSTR(MP_QSTR_BUILD_SLICE          ),
        [MP_BC_STORE_COMP           ] = MP_ROM_QSTR(MP_QSTR_STORE_COMP           ),
        [MP_BC_UNPACK_SEQUENCE      ] = MP_ROM_QSTR(MP_QSTR_UNPACK_SEQUENCE      ),
        [MP_BC_UNPACK_EX            ] = MP_ROM_QSTR(MP_QSTR_UNPACK_EX            ),
        [MP_BC_RETURN_VALUE         ] = MP_ROM_QSTR(MP_QSTR_RETURN_VALUE         ),
        [MP_BC_RAISE_LAST           ] = MP_ROM_QSTR(MP_QSTR_RAISE_LAST           ),
        [MP_BC_RAISE_OBJ            ] = MP_ROM_QSTR(MP_QSTR_RAISE_OBJ            ),
        [MP_BC_RAISE_FROM           ] = MP_ROM_QSTR(MP_QSTR_RAISE_FROM           ),
        [MP_BC_YIELD_VALUE          ] = MP_ROM_QSTR(MP_QSTR_YIELD_VALUE          ),
        [MP_BC_YIELD_FROM           ] = MP_ROM_QSTR(MP_QSTR_YIELD_FROM           ),
        [MP_BC_MAKE_FUNCTION        ] = MP_ROM_QSTR(MP_QSTR_MAKE_FUNCTION        ),
        [MP_BC_MAKE_FUNCTION_DEFARGS] = MP_ROM_QSTR(MP_QSTR_MAKE_FUNCTION_DEFARGS),
        [MP_BC_MAKE_CLOSURE         ] = MP_ROM_QSTR(MP_QSTR_MAKE_CLOSURE         ),
        [MP_BC_MAKE_CLOSURE_DEFARGS ] = MP_ROM_QSTR(MP_QSTR_MAKE_CLOSURE_DEFARGS ),
        [MP_BC_CALL_FUNCTION        ] = MP_ROM_QSTR(MP_QSTR_CALL_FUNCTION        ),
        [MP_BC_CALL_FUNCTION_VAR_KW ] = MP_ROM_QSTR(MP_QSTR_CALL_FUNCTION_VAR_KW ),
        [MP_BC_CALL_METHOD          ] = MP_ROM_QSTR(MP_QSTR_CALL_METHOD          ),
        [MP_BC_CALL_METHOD_VAR_KW   ] = MP_ROM_QSTR(MP_QSTR_CALL_METHOD_VAR_KW   ),
        [MP_BC_IMPORT_NAME          ] = MP_ROM_QSTR(MP_QSTR_IMPORT_NAME          ),
        [MP_BC_IMPORT_FROM          ] = MP_ROM_QSTR(MP_QSTR_IMPORT_FROM          ),
        [MP_BC_IMPORT_STAR          ] = MP_ROM_QSTR(MP_QSTR_IMPORT_STAR          ),
    },
};
#pragma GCC diagnostic pop



static mp_obj_t decode(mp_obj_t buf_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    mp_opcode_t op = mp_decode_opcode(bufinfo.buf);
    if (op.size > bufinfo.len) {
        mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("not enough bytes left to decode"));
    }

    enum {
        FIELD_opcode,
        FIELD_format,
        FIELD_size,
        FIELD_arg,
        FIELD_extra_arg,
    };
    static const qstr fields[] = {
        [FIELD_opcode   ] = MP_QSTR_opcode,
        [FIELD_format   ] = MP_QSTR_format,
        [FIELD_size     ] = MP_QSTR_size,
        [FIELD_arg      ] = MP_QSTR_arg,
        [FIELD_extra_arg] = MP_QSTR_extra_arg,
    };
    mp_obj_t items[] = {
        [FIELD_opcode   ] = mp_obj_new_int(op.opcode   ),
        [FIELD_format   ] = mp_obj_new_int(op.format   ),
        [FIELD_size     ] = mp_obj_new_int(op.size     ),
        [FIELD_arg      ] = mp_obj_new_int(op.arg      ),
        [FIELD_extra_arg] = mp_obj_new_int(op.extra_arg),
    };
    return mp_obj_new_attrtuple(fields, MP_ARRAY_SIZE(items), items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(decode_obj, decode);

static const mp_rom_map_elem_t mp_module__dis_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__dis) },
    { MP_ROM_QSTR(MP_QSTR_opname), MP_ROM_PTR(&opname_obj) },
    { MP_ROM_QSTR(MP_QSTR__decode), MP_ROM_PTR(&decode_obj) },
};
static MP_DEFINE_CONST_DICT(mp_module__dis_globals, mp_module__dis_globals_table);

const mp_obj_module_t mp_module__dis = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module__dis_globals,
};

MP_REGISTER_MODULE(MP_QSTR__dis, mp_module__dis);
