//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#pragma once

#include "ten_runtime/ten_config.h"

#include <stdint.h>

#include "ten_utils/container/list.h"
#include "ten_utils/value/value.h"

typedef struct ten_extension_t ten_extension_t;
typedef struct ten_error_t ten_error_t;
typedef struct ten_msg_t ten_msg_t;

// TEN runtime supports 5 kinds of message mapping.
//
// > 1-to-1
//   Apply for : all messages.
//   This is the normal message mapping. The message will be transmitted to
//   the next node in the graph for non-status-command message, and to the
//   previous node in the graph for status-command message.
//
// > 1-to-N (when a message leaves an extension)
//   Apply for : all messages.
//   This can be declared in 'dests' in the graph declaration. The message
//   will be cloned to N copies, and sent to the N destinations.
//
// > 1-to-N (when a message enters an extension)
//   Apply for : all commands except cmd results.
//   This is the command conversion mechanism. The message will be converted
//   to N new messages before sending to an extension. In command conversion
//   mechanism, a result conversion rule can also be specified.
//
// > N-to-1 (TODO(Wei): Need to implement this if needed)
//   Apply for : all commands except cmd results.
//   TEN runtime can provide a special command named 'closure' command.
//   Users can use command conversion mechanism to map a command (said
//   command A) to the 'closure' command. What this 'closure' command does
//   is to _store_ the information of command A to a closure in the
//   destination extension. And users can specify (in graph declaration)
//   that a later command will use the information in a previously created
//   closure.
//
//     extension 1              extension 2
//      command A     ==>     closure command (create closure X)
//      command B     ==>     closure command (with closure X)
//      command C     ==>     command D (with closure X)
//
// > N-to-M
//   Combined with the above 1-to-N and N-to-1 message mapping, the effect
//   of N-to-M mapping can be generated.

// Note: To achieve the best compatibility, any new enum item, whether it is
// cmd/data/video_frame/audio_frame, should be added to the end to avoid
// changing the value of previous enum items.
typedef enum TEN_MSG_TYPE {
  TEN_MSG_TYPE_INVALID,
  TEN_MSG_TYPE_CMD,
  TEN_MSG_TYPE_CMD_RESULT,
  TEN_MSG_TYPE_CMD_CLOSE_APP,
  TEN_MSG_TYPE_CMD_START_GRAPH,
  TEN_MSG_TYPE_CMD_STOP_GRAPH,
  TEN_MSG_TYPE_CMD_TIMER,
  TEN_MSG_TYPE_CMD_TIMEOUT,
  TEN_MSG_TYPE_DATA,
  TEN_MSG_TYPE_VIDEO_FRAME,
  TEN_MSG_TYPE_AUDIO_FRAME,
  TEN_MSG_TYPE_LAST,
} TEN_MSG_TYPE;

/**
 * @brief The "clone" function of a command _does_ generate a new command ID.
 */
TEN_RUNTIME_API ten_shared_ptr_t *ten_msg_clone(ten_shared_ptr_t *self,
                                                ten_list_t *excluded_field_ids);

TEN_RUNTIME_API bool ten_msg_is_property_exist(ten_shared_ptr_t *self,
                                               const char *path,
                                               ten_error_t *err);

/**
 * @brief Note that the ownership of @a value_kv would be transferred into
 * the TEN runtime, so the caller of this function could _not_ consider the
 * value_kv instance is still valid.
 */
TEN_RUNTIME_API bool ten_msg_set_property(ten_shared_ptr_t *self,
                                          const char *path, ten_value_t *value,
                                          ten_error_t *err);

// Because each TEN extension has its own messages (in almost all cases, except
// for the data-type messages), so the returned value_kv of this function is
// from the message directly, not a cloned one.
TEN_RUNTIME_API ten_value_t *ten_msg_peek_property(ten_shared_ptr_t *self,
                                                   const char *path,
                                                   ten_error_t *err);

TEN_RUNTIME_API bool ten_msg_clear_and_set_dest(
    ten_shared_ptr_t *self, const char *app_uri, const char *graph_name,
    const char *extension_group_name, const char *extension_name,
    ten_extension_t *extension, ten_error_t *err);

TEN_RUNTIME_API bool ten_msg_from_json(ten_shared_ptr_t *self, ten_json_t *json,
                                       ten_error_t *err);

TEN_RUNTIME_API ten_json_t *ten_msg_to_json(ten_shared_ptr_t *self,
                                            ten_error_t *err);

TEN_RUNTIME_API bool ten_msg_add_locked_res_buf(ten_shared_ptr_t *self,
                                                const uint8_t *data,
                                                ten_error_t *err);

TEN_RUNTIME_API bool ten_msg_remove_locked_res_buf(ten_shared_ptr_t *self,
                                                   const uint8_t *data,
                                                   ten_error_t *err);

TEN_RUNTIME_API const char *ten_msg_get_name(ten_shared_ptr_t *self);

TEN_RUNTIME_API TEN_MSG_TYPE ten_msg_get_type(ten_shared_ptr_t *self);

TEN_RUNTIME_API bool ten_msg_set_name(ten_shared_ptr_t *self,
                                      const char *msg_name, ten_error_t *err);