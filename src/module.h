#ifndef __NUVM__MODULE_H__
#define __NUVM__MODULE_H__

#include "util/types.h"
#include "error.h"

typedef struct NModule NModule;

NModule*
n_new_module(uint16_t register_count, uint16_t entry, NError* error);

void
n_destroy_module(NModule* self);

#endif

