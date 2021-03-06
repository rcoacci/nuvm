#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../util/strings.h"

#include "../type-info.h"
#include "../memory.h"

#include "strings.h"

struct NString {
	NObject parent;
	uint32_t length;
	char* contents;
};

static NType   _type;
static int32_t _type_id;

void
n_init_strings() {
	NError error;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_type, "org.nuvm.String");

	_type_id = n_type_registry_add_type(registry, &_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Strings. Aborting.\n");
		exit(1);
	}
}


NString*
n_string_new(const char* contents, NError* error) {
	NError inner_error;
	NString* self;
	char* dup_contents;
	
	if (contents != NULL) {
		dup_contents = duplicate_string(contents);
		if (dup_contents == NULL) {
			n_error_set(error, N_E_BAD_ALLOCATION);
			n_error_set_msg(error, "contents");
			return NULL;
		}
	}
	else {
		dup_contents = NULL;
	}

	if (error == NULL) {
		error = &inner_error;
	}
	
	self = n_string_wrap(dup_contents, error);
	if (error->code != N_E_OK) {
		n_free(dup_contents);
		self = NULL;
	}

	return self;
}


NString*
n_string_wrap(char* contents, NError* error) {
	NString* self;
	n_error_clear(error);
	if (contents == NULL) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "contents");
		return NULL;
	}

	self = n_alloc(sizeof(NString));
	if (self == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "self");
		return NULL;
	}

	self->parent.type_id = _type_id;
	self->contents = contents;
	self->length = strlen(contents);
	return self;
}


bool
n_is_string(NValue val) {
	return n_typeof(val) == _type_id;
}


const char*
n_string_contents(NString* self) {
	return self->contents;
}


bool
n_string_is_equal(NString* self, NString* other) {
	if (self == NULL || other == NULL) {
		return false;
	}
	else if (self == other) {
		return true;
	}
	else {
		return strcmp(self->contents, other->contents) == 0;
	}
}


uint32_t
n_string_length(NString* self) {
	return self->length;
}
