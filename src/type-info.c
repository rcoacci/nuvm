#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type-info.h"
#include "memory.h"


#define ARRAY_TYPE_NAME     NTypeArray
#define ARRAY_PREFIX        n_type_array
#define ARRAY_CONTENTS_TYPE NType*
#include "util/resizable_array.h"


struct NTypeRegistry {
	/* FIXME: Using an array as the pool, but a better mapping data structure
	 * should be used. */
	NTypeArray types;
};

static NTypeRegistry* _default_registry = NULL;


static char*
_default_to_string_impl(const NType*, NValue);

NTypeRegistry*
n_type_registry_get_default() {
	if (_default_registry == NULL) {
		_default_registry = n_type_registry_new(NULL);
	}
	return _default_registry;
}

void
n_type_init(NType* self, const char* name) {
	self->name = name;
	self->to_string = _default_to_string_impl;
}


NTypeRegistry*
n_type_registry_new(NError* error) {
	NTypeRegistry* self = n_alloc_unmanaged(sizeof(NTypeRegistry));

	n_error_clear(error);
	if (self == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "self");
		return NULL;
	}

	n_type_array_init(&self->types, 64);

	if (n_type_array_elements(&self->types) == NULL) {
		n_free(self);
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "types");
		return NULL;
	}

	return self;
}


void
n_type_registry_destroy(NTypeRegistry* self) {
	n_type_array_destroy(&self->types);
	n_free(self);
}

int32_t
n_type_registry_add_type(NTypeRegistry* self, NType* type, NError* error) {
	int32_t index;
	n_error_clear(error);

	if (n_type_registry_find_type(self, type->name, &index) != NULL) {
		n_error_set(error, N_E_TYPE_REGISTRY_CONFLICT);
		return -1;
	}

	index = n_type_array_append(&self->types, type);
	if (index == -1) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "types");
	}

	return index;
}

int32_t
n_type_registry_count_types(NTypeRegistry* self) {
	return n_type_array_count(&self->types);
}


NType*
n_type_registry_fetch_type(NTypeRegistry*self, int32_t id) {
	return n_type_array_get(&self->types, id);
}


NType*
n_type_registry_find_type(NTypeRegistry* self,
                          const char* name,
                          int32_t* out_id) {
	int32_t i;
	NType** types = n_type_array_elements(&self->types);
	int32_t count = n_type_array_count(&self->types);
	for (i = 0; i < count; i++) {
	   if (strcmp(types[i]->name, name) == 0) {
		   *out_id = i;
		   return types[i];
	   }
	}
	return NULL;
}


bool
n_type_registry_has_type(NTypeRegistry* self, const char* name) {
	int id;
	return n_type_registry_find_type(self, name, &id) != NULL;
}



static char*
_default_to_string_impl(const NType* type, NValue value) {
	int length = strlen(type->name) + 20;
	char* result = malloc(sizeof(char) * length);
	if (n_is_pointer(value)) {
		sprintf(result, "%s at %p", type->name, n_unwrap_pointer(value));
	}
	else {
		sprintf(result, "%s", type->name);
	}
	return result;
}
