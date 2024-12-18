"""
Test hello_world_app.
"""

import subprocess
import os
import sys
from sys import stdout
from .common import msgpack


def install_app(app_name: str):
    base_path = os.path.dirname(os.path.abspath(__file__))
    root_dir = os.path.join(base_path, "../../../../../")

    my_env = os.environ.copy()

    app_root_path = os.path.join(base_path, app_name)

    tman_install_cmd = [
        os.path.join(root_dir, "ten_manager/bin/tman"),
        "--config-file",
        os.path.join(root_dir, "tests/local_registry/config.json"),
        "install",
    ]

    tman_install_process = subprocess.Popen(
        tman_install_cmd,
        stdout=stdout,
        stderr=subprocess.STDOUT,
        env=my_env,
        cwd=app_root_path,
    )
    tman_install_process.wait()


def start_app(app_name: str, port: int) -> subprocess.Popen:
    base_path = os.path.dirname(os.path.abspath(__file__))
    app_root_path = os.path.join(base_path, app_name)

    my_env = os.environ.copy()

    if sys.platform == "win32":
        my_env["PATH"] = (
            os.path.join(
                base_path, f"{app_name}/ten_packages/system/ten_runtime/lib"
            )
            + ";"
            + my_env["PATH"]
        )
        server_cmd = os.path.join(
            base_path, f"{app_name}/bin/{app_name}_source.exe"
        )
    elif sys.platform == "darwin":
        my_env["DYLD_LIBRARY_PATH"] = os.path.join(
            base_path, f"{app_name}/ten_packages/system/ten_runtime/lib"
        )
        server_cmd = os.path.join(
            base_path, f"{app_name}/bin/{app_name}_source"
        )
    else:
        # client depends on some libraries in the TEN app.
        my_env["LD_LIBRARY_PATH"] = os.path.join(
            base_path, f"{app_name}/ten_packages/system/ten_runtime/lib"
        )
        server_cmd = os.path.join(
            base_path, f"{app_name}/bin/{app_name}_source"
        )

        if os.path.exists(os.path.join(base_path, "use_asan_lib_marker")):
            libasan_path = os.path.join(
                base_path,
                f"{app_name}/ten_packages/system/ten_runtime/lib/libasan.so",
            )
            if os.path.exists(libasan_path):
                my_env["LD_PRELOAD"] = libasan_path

    server = subprocess.Popen(
        server_cmd,
        stdout=stdout,
        stderr=subprocess.STDOUT,
        env=my_env,
        cwd=app_root_path,
    )

    is_started, sock = msgpack.is_app_started("127.0.0.1", port, 10)
    if not is_started:
        print(f"The {app_name} is not started after 30 seconds.")

        server.kill()
        exit_code = server.wait()
        print(f"The exit code of {app_name}: {exit_code}")

        assert 0

    sock.close()
    return server


def start_client(app_name: str) -> subprocess.Popen:
    base_path = os.path.dirname(os.path.abspath(__file__))
    my_env = os.environ.copy()

    if sys.platform == "win32":
        my_env["PATH"] = (
            os.path.join(
                base_path, f"{app_name}/ten_packages/system/ten_runtime/lib"
            )
            + ";"
            + my_env["PATH"]
        )
        client_cmd = os.path.join(base_path, "multi_apps_client.exe")
    elif sys.platform == "darwin":
        # client depends on some libraries in the TEN app.
        my_env["DYLD_LIBRARY_PATH"] = os.path.join(
            base_path, f"{app_name}/ten_packages/system/ten_runtime/lib"
        )
        client_cmd = os.path.join(base_path, "multi_apps_client")
    else:
        # client depends on some libraries in the TEN app.
        my_env["LD_LIBRARY_PATH"] = os.path.join(
            base_path, f"{app_name}/ten_packages/system/ten_runtime/lib"
        )
        client_cmd = os.path.join(base_path, "multi_apps_client")

        if os.path.exists(os.path.join(base_path, "use_asan_lib_marker")):
            libasan_path = os.path.join(
                base_path,
                f"{app_name}/ten_packages/system/ten_runtime/lib/libasan.so",
            )
            if os.path.exists(libasan_path):
                my_env["LD_PRELOAD"] = libasan_path

    client = subprocess.Popen(
        client_cmd, stdout=stdout, stderr=subprocess.STDOUT, env=my_env
    )

    return client


def test_hello_world_app():
    """Test client and app server."""
    install_app("app_1")
    install_app("app_2")

    server1 = start_app("app_1", 8001)
    server2 = start_app("app_2", 8002)

    client = start_client("app_1")

    # The client will quit after the test is completed.
    client_rc = client.wait()
    if client_rc != 0:
        server1.kill()
        server2.kill()

    server1_rc = server1.wait()
    server2_rc = server2.wait()
    print("server: ", server1_rc, server2_rc)
    print("client: ", client_rc)
    assert server1_rc == 0
    assert server2_rc == 0
    assert client_rc == 0
