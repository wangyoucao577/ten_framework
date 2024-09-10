//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#pragma once

#include "ten_utils/ten_config.h"

#include <stdbool.h>

#include "ten_utils/lib/error.h"
#include "ten_utils/macro/mark.h"
#include "ten_utils/value/value_kv.h"

TEN_UTILS_API bool ten_value_construct_for_smart_ptr(
    ten_value_t *self, TEN_UNUSED ten_error_t *err);

TEN_UTILS_API bool ten_value_copy_for_smart_ptr(ten_value_t *dest,
                                                ten_value_t *src,
                                                TEN_UNUSED ten_error_t *err);

TEN_UTILS_API bool ten_value_destruct_for_smart_ptr(
    ten_value_t *self, TEN_UNUSED ten_error_t *err);