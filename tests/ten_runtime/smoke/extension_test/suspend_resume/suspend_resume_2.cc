//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#include <atomic>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

#include "gtest/gtest.h"
#include "include_internal/ten_runtime/binding/cpp/ten.h"
#include "ten_utils/lang/cpp/lib/value.h"
#include "ten_utils/lib/alloc.h"
#include "ten_utils/lib/thread.h"
#include "ten_utils/lib/time.h"
#include "ten_utils/macro/check.h"
#include "tests/common/client/cpp/msgpack_tcp.h"
#include "tests/ten_runtime/smoke/extension_test/util/binding/cpp/check.h"

#define SENT_DATA_CNT 100

namespace {

class test_extension_1 : public ten::extension_t {
 public:
  explicit test_extension_1(const std::string &name) : ten::extension_t(name) {}

  void on_start(ten::ten_env_t &ten_env) override {
    auto *ten_env_proxy = ten::ten_env_proxy_t::create(ten_env);

    // Create a C++ thread to call ten.xxx in it.
    outer_thread = new std::thread(&test_extension_1::outer_thread_main, this,
                                   ten_env_proxy);

    ten_env.on_start_done();
  }

  void on_stop(ten::ten_env_t &ten_env) override {
    // Reclaim the C++ thread.
    outer_thread->join();
    delete outer_thread;

    ten_env.on_stop_done();
  }

  void on_cmd(ten::ten_env_t &ten_env,
              std::unique_ptr<ten::cmd_t> cmd) override {
    if (std::string(cmd->get_name()) == "hello_world") {
      // Trigger the C++ thread to call ten.xxx function.
      trigger = true;

      ten_env.send_cmd(
          std::move(cmd), [](ten::ten_env_t &ten_env,
                             std::unique_ptr<ten::cmd_result_t> cmd_result) {
            if (cmd_result->get_property_string("detail") ==
                "hello world, too") {
              cmd_result->set_property("detail", "hello world 1, too");
              ten_env.return_result_directly(std::move(cmd_result));
            }
          });
    }
  }

 private:
  std::thread *outer_thread{nullptr};
  std::atomic<bool> trigger{false};
  std::int64_t sent_data{0};

  void outer_thread_main(ten::ten_env_proxy_t *ten_env_proxy) {
    while (true) {
      if (trigger) {
        // Create a memory buffer to contain some important data.
        auto *test_data = static_cast<int64_t *>(TEN_MALLOC(sizeof(int64_t)));
        TEN_ASSERT(test_data, "Failed to allocate memory.");

        *test_data = sent_data++;

        ten_env_proxy->notify(send_data_from_outer_thread, test_data);

        if (sent_data == SENT_DATA_CNT) {
          break;
        }
      }

      ten_sleep(100);
    }

    delete ten_env_proxy;
  }

  static void send_data_from_outer_thread(ten::ten_env_t &ten_env,
                                          void *user_data) {
    // Create a 'ten::data_t' with the same important data.
    auto ten_data = ten::data_t::create("data");
    ten_data->set_property("test_data", user_data);
    ten_env.send_data(std::move(ten_data));
  }
};

class test_extension_2 : public ten::extension_t {
 public:
  explicit test_extension_2(const std::string &name) : ten::extension_t(name) {}

  void on_cmd(ten::ten_env_t &ten_env,
              std::unique_ptr<ten::cmd_t> cmd) override {
    if (std::string(cmd->get_name()) == "hello_world") {
      if (received_all) {
        auto cmd_result = ten::cmd_result_t::create(TEN_STATUS_CODE_OK);
        cmd_result->set_property("detail", "hello world, too");
        ten_env.return_result(std::move(cmd_result),
                              std::move(hello_world_cmd));
      } else {
        hello_world_cmd = std::move(cmd);
        return;
      }
    }
  }

  void on_data(TEN_UNUSED ten::ten_env_t &ten_env,
               std::unique_ptr<ten::data_t> data) override {
    auto *test_data =
        static_cast<int64_t *>(data->get_property_ptr("test_data"));
    TEN_ASSERT(*test_data == next_receive_data,
               "test_data has been destroyed.");

    next_receive_data++;

    // Destroy the important data.
    TEN_FREE(test_data);

    if (next_receive_data == SENT_DATA_CNT) {
      received_all = true;
      if (hello_world_cmd != nullptr) {
        auto cmd_result = ten::cmd_result_t::create(TEN_STATUS_CODE_OK);
        cmd_result->set_property("detail", "hello world, too");
        ten_env.return_result(std::move(cmd_result),
                              std::move(hello_world_cmd));
      }
    }
  }

 private:
  std::unique_ptr<ten::cmd_t> hello_world_cmd;
  bool received_all{false};
  std::int64_t next_receive_data{0};
};

class test_app : public ten::app_t {
 public:
  void on_init(ten::ten_env_t &ten_env) override {
    bool rc = ten_env.init_property_from_json(
        // clang-format off
                 R"({
                      "_ten": {
                        "uri": "msgpack://127.0.0.1:8001/",
                        "log_level": 1
                      }
                    })"
        // clang-format on
        ,
        nullptr);
    ASSERT_EQ(rc, true);

    ten_env.on_init_done();
  }
};

void *test_app_thread_main(TEN_UNUSED void *args) {
  auto *app = new test_app();
  app->run();
  delete app;

  return nullptr;
}

TEN_CPP_REGISTER_ADDON_AS_EXTENSION(suspend_resume_2__test_extension_1,
                                    test_extension_1);
TEN_CPP_REGISTER_ADDON_AS_EXTENSION(suspend_resume_2__test_extension_2,
                                    test_extension_2);

}  // namespace

TEST(ExtensionTest, SuspendResume2) {  // NOLINT
  // Start app.
  auto *app_thread =
      ten_thread_create("app thread", test_app_thread_main, nullptr);

  // Create a client and connect to the app.
  auto *client = new ten::msgpack_tcp_client_t("msgpack://127.0.0.1:8001/");

  // Send graph.
  nlohmann::json resp = client->send_json_and_recv_resp_in_json(
      R"({
           "_ten": {
             "type": "start_graph",
             "seq_id": "55",
             "nodes": [{
               "type": "extension_group",
               "name": "basic_extension_group",
               "addon": "default_extension_group",
               "app": "msgpack://127.0.0.1:8001/"
             },{
               "type": "extension",
               "name": "test extension 1",
               "addon": "suspend_resume_2__test_extension_1",
               "app": "msgpack://127.0.0.1:8001/",
               "extension_group": "basic_extension_group"
             },{
               "type": "extension",
               "name": "test extension 2",
               "addon": "suspend_resume_2__test_extension_2",
               "app": "msgpack://127.0.0.1:8001/",
               "extension_group": "basic_extension_group"
             }],
             "connections": [{
               "app": "msgpack://127.0.0.1:8001/",
               "extension_group": "basic_extension_group",
               "extension": "test extension 1",
               "cmd": [{
                 "name": "hello_world",
                 "dest": [{
                   "app": "msgpack://127.0.0.1:8001/",
                   "extension_group": "basic_extension_group",
                   "extension": "test extension 2"
                 }]
               }],
               "data": [{
                 "name": "data",
                 "dest": [{
                   "app": "msgpack://127.0.0.1:8001/",
                   "extension_group": "basic_extension_group",
                   "extension": "test extension 2"
                 }]
               }]
             }]
           }
         })"_json);
  ten_test::check_status_code_is(resp, TEN_STATUS_CODE_OK);

  // Send a user-defined 'hello world' command.
  resp = client->send_json_and_recv_resp_in_json(
      R"({
           "_ten": {
             "name": "hello_world",
             "seq_id": "137",
             "dest": [{
               "app": "msgpack://127.0.0.1:8001/",
               "extension_group": "basic_extension_group",
               "extension": "test extension 1"
             }]
           }
         })"_json);
  ten_test::check_result_is(resp, "137", TEN_STATUS_CODE_OK,
                            "hello world 1, too");

  delete client;

  ten_thread_join(app_thread, -1);
}