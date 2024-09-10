#
# This file is part of the TEN Framework project.
# See https://github.com/TEN-framework/ten_framework/LICENSE for license
# information.
#
import argparse
import sys
import os
import platform
from build.scripts import cmd_exec, fs_utils, timestamp_proxy


class ArgumentInfo(argparse.Namespace):
    def __init__(self):
        super().__init__()
        self.through_real_test: bool
        self.project_path: str
        self.build_type: str
        self.target_path: str
        self.env: list[str]
        self.log_level: int
        self.test_output_dir: str
        self.tg_timestamp_proxy_file: str | None = None


def copy_test_binaries(
    through_real_test: bool,
    source_dir: str,
    output_dir: str,
    log_level: int = 0,
):
    if log_level > 0:
        print(f"Copying test binaries from {source_dir} to {output_dir}")

    if through_real_test:
        if platform.system() == "Windows":
            executable_name = "tman_test.exe"
        else:
            executable_name = "tman_test"

        fs_utils.copy_file(
            os.path.join(source_dir, executable_name),
            os.path.join(output_dir, executable_name),
            True,
        )
    else:
        for f in os.listdir(source_dir):
            is_executable = os.access(os.path.join(source_dir, f), os.X_OK)
            if not is_executable:
                continue

            # The file name generated by `cargo build` always has a hash suffix,
            # ex: <library_name>-<hash>.<extension>. We need to remove the hash
            # suffix.

            # We have to deal with the extension of the executable file, as if
            # the `crate-type` is `cdylib`, the extension is `.so` on linux,
            # which is not what we want. The extension is not present on linux
            # or macos, and the extension is '.exe' on windows.
            extension = os.path.splitext(f)[1]
            if extension != "" and extension != ".exe":
                continue

            ff = f[: f.rindex("-") if "-" in f else -1] + extension

            fs_utils.copy_file(
                os.path.join(source_dir, f),
                os.path.join(output_dir, ff),
                True,
            )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument("--through-real-test", type=bool, default=False)
    parser.add_argument("--project-path", type=str, required=True)
    parser.add_argument("--manifest-path", type=str, required=False)
    parser.add_argument("--build-type", type=str, required=True)
    parser.add_argument("--target-path", type=str, required=True)
    parser.add_argument("--target", type=str, required=True)
    parser.add_argument("--env", type=str, action="append", default=[])
    parser.add_argument("--log-level", type=int, required=True)
    parser.add_argument(
        "--test-output-dir", type=str, default="", required=False
    )
    parser.add_argument(
        "--tg-timestamp-proxy-file", type=str, default="", required=False
    )

    arg_info = ArgumentInfo()
    args = parser.parse_args(namespace=arg_info)

    # Setup environment variables.
    for env in args.env:
        split_key_index = str(env).find("=")
        if split_key_index == -1:
            sys.exit(1)
        else:
            os.environ[(str(env)[:split_key_index])] = str(env)[
                split_key_index + len("=") :
            ]

    origin_wd = os.getcwd()

    returncode = 0
    try:
        os.chdir(args.project_path)

        if args.through_real_test:
            cmd = [
                "cargo",
                "test",
                "--target-dir",
                args.target_path,
                "--target",
                args.target,
            ]
        else:
            cmd = [
                "cargo",
                "build",
                "--target-dir",
                args.target_path,
                "--target",
                args.target,
                "--tests",
            ]

        if args.build_type == "release":
            cmd.append("--release")

        returncode, logs = cmd_exec.run_cmd(cmd, args.log_level)
        if returncode:
            raise Exception(f"Failed to build rust tests: {logs}")

        # The output of the dependencies will be in <target_path>/<build_type>
        # /deps, while the output of the tests will be in <target_path>/<target>
        # /<build_type>/deps.
        if args.test_output_dir != "":
            copy_test_binaries(
                args.through_real_test,
                os.path.join(
                    args.target_path,
                    args.target,
                    args.build_type,
                    "deps",
                ),
                args.test_output_dir,
                args.log_level,
            )

        # Success to build the app, update the stamp file to represent this
        # fact.
        timestamp_proxy.touch_timestamp_proxy_file(args.tg_timestamp_proxy_file)

    except Exception as exc:
        returncode = 1
        timestamp_proxy.remove_timestamp_proxy_file(
            args.tg_timestamp_proxy_file
        )
        print(exc)

    finally:
        os.chdir(origin_wd)
        sys.exit(-1 if returncode != 0 else 0)