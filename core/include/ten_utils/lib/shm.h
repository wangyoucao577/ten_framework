//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#pragma once

#include "ten_utils/ten_config.h"

#include <stddef.h>

TEN_UTILS_API void *ten_shm_map(const char *name, size_t size);

TEN_UTILS_API size_t ten_shm_get_size(void *addr);

TEN_UTILS_API void ten_shm_unmap(void *addr);

TEN_UTILS_API void ten_shm_unlink(const char *name);