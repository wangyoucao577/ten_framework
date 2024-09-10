//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#pragma once

#include "ten_runtime/ten_config.h"

#include "ten_utils/container/list.h"

typedef struct ten_extension_t ten_extension_t;

typedef struct ten_all_msg_type_dest_static_info_t {
  ten_list_t cmd;          // ten_shared_ptr_t of ten_msg_dest_static_info_t
  ten_list_t data;         // ten_shared_ptr_t of ten_msg_dest_static_info_t
  ten_list_t video_frame;  // ten_shared_ptr_t of ten_msg_dest_static_info_t
  ten_list_t audio_frame;  // ten_shared_ptr_t of ten_msg_dest_static_info_t
  ten_list_t interface;    // ten_shared_ptr_t of ten_msg_dest_static_info_t
} ten_all_msg_type_dest_static_info_t;

TEN_RUNTIME_PRIVATE_API void ten_all_msg_type_dest_static_info_init(
    ten_all_msg_type_dest_static_info_t *self);

TEN_RUNTIME_PRIVATE_API void ten_all_msg_type_dest_static_info_deinit(
    ten_all_msg_type_dest_static_info_t *self);

TEN_RUNTIME_PRIVATE_API void
ten_all_msg_type_dest_static_info_translate_localhost_to_app_uri(
    ten_all_msg_type_dest_static_info_t *self, const char *uri);

typedef struct ten_all_msg_type_dest_runtime_info_t {
  ten_list_t cmd;          // ten_shared_ptr_t of ten_msg_dest_runtime_info_t
  ten_list_t data;         // ten_shared_ptr_t of ten_msg_dest_runtime_info_t
  ten_list_t video_frame;  // ten_shared_ptr_t of ten_msg_dest_runtime_info_t
  ten_list_t audio_frame;  // ten_shared_ptr_t of ten_msg_dest_runtime_info_t
  ten_list_t interface;    // ten_shared_ptr_t of ten_msg_dest_runtime_info_t
} ten_all_msg_type_dest_runtime_info_t;

TEN_RUNTIME_PRIVATE_API void ten_all_msg_type_dest_runtime_info_init(
    ten_all_msg_type_dest_runtime_info_t *self);

TEN_RUNTIME_PRIVATE_API void ten_all_msg_type_dest_runtime_info_deinit(
    ten_all_msg_type_dest_runtime_info_t *self);

TEN_RUNTIME_PRIVATE_API void
ten_all_msg_type_dest_runtime_info_direct_all_msg_to_extension(
    ten_all_msg_type_dest_runtime_info_t *self, ten_extension_t *extension);