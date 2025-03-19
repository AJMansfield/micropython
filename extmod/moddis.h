#ifndef MICROPY_INCLUDED_EXTMOD_DIS_H
#define MICROPY_INCLUDED_EXTMOD_DIS_H

#include "py/builtin.h"
#include "py/obj.h"

#if MICROPY_PY_DIS
typedef struct _dis_bytecode_t dis_bytecode_t;
typedef struct _dis_bytecode_iter_t dis_bytecode_iter_t;
extern const mp_obj_type_t dis_bytecode_type;
extern const mp_obj_type_t dis_bytecode_iterator_type;
#endif

#endif
