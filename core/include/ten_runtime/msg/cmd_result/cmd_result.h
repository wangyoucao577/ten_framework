//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#pragma once

#include "ten_runtime/ten_config.h"

#include "ten_runtime/common/status_code.h"
#include "ten_utils/lib/error.h"
#include "ten_utils/lib/smart_ptr.h"

typedef struct ten_cmd_result_t ten_cmd_result_t;

TEN_RUNTIME_API ten_shared_ptr_t *ten_cmd_result_create(
    TEN_STATUS_CODE status_code);

TEN_RUNTIME_API ten_shared_ptr_t *ten_cmd_result_create_from_cmd(
    TEN_STATUS_CODE status_code, ten_shared_ptr_t *original_cmd);

TEN_RUNTIME_API TEN_STATUS_CODE
ten_cmd_result_get_status_code(ten_shared_ptr_t *self);

TEN_RUNTIME_API bool ten_cmd_result_get_is_final(ten_shared_ptr_t *self,
                                               ten_error_t *err);

TEN_RUNTIME_API bool ten_cmd_result_set_is_final(ten_shared_ptr_t *self,
                                               bool is_final, ten_error_t *err);