//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#pragma once

#include "ten_runtime/ten_config.h"

#include <stdbool.h>

#include "include_internal/ten_runtime/binding/python/common/python_stuff.h"
#include "ten_utils/lib/buf.h"

typedef struct ten_py_buf_t {
  PyObject_HEAD

  ten_buf_t *c_buf;
} ten_py_buf_t;

TEN_RUNTIME_PRIVATE_API PyTypeObject *ten_py_buf_py_type(void);

TEN_RUNTIME_PRIVATE_API ten_py_buf_t *ten_py_buf_wrap(ten_buf_t *buf);

TEN_RUNTIME_PRIVATE_API void ten_py_buf_destroy(PyObject *self);

TEN_RUNTIME_PRIVATE_API int ten_py_buf_get_buffer(PyObject *self, Py_buffer *view,
                                                int flags);

TEN_RUNTIME_PRIVATE_API bool ten_py_buf_init_for_module(PyObject *module);