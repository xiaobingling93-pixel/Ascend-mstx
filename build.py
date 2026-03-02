#!/usr/bin/python3
# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
# This file is part of the MindStudio project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# MindStudio is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
# -------------------------------------------------------------------------
import argparse
import logging
import multiprocessing
import os
import shutil
import subprocess
import sys
import traceback
from pathlib import Path

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


class BuildManager:
    """
    统一构建管理：依赖拉取 → CMake 配置 → 并行编译 → 安装 / 测试。

    用法:
        python build.py                  完整构建（拉取依赖 + Release 编译）
        python build.py local            本地构建（跳过依赖拉取, Release 编译）
        python build.py test             单元测试（拉取依赖 + Debug 编译 + 执行测试）
        python build.py test local       单元测试（跳过依赖拉取, Debug 编译 + 执行测试）
        python build.py -r <revision>    指定依赖的内部源码仓(例如msopcom)的 Git 分支/标签/commit

    参数说明:
        - 参数: command : 构建动作: 为空时为全构建, local 为跳过依赖下载, test 为运行单元测试。
        - 参数: -r, --revision : 指定 Git 修订版本或标签用于依赖检出。
    """

    def __init__(self):
        self.project_root = Path(__file__).resolve().parent
        self.build_jobs = multiprocessing.cpu_count()
        argument_parser = argparse.ArgumentParser(description='Build the project and optionally run tests.')
        argument_parser.add_argument('command', nargs='*', default=[],
                                     choices=[[], 'local', 'test'],
                                     help='Build action: omit for full build, "local" to skip dependency download, "test" to run unit tests')
        argument_parser.add_argument('-r', '--revision',
                                     help='Specify Git revision for internal dependent repo (e.g., msopcom).')
        self.parsed_arguments = argument_parser.parse_args()

    def _execute_command(self, command_sequence, timeout_seconds=36000, cwd=None, env=None):
        logging.info("Running: %s", " ".join(command_sequence))
        subprocess.run(command_sequence, timeout=timeout_seconds, check=True, cwd=cwd, env=env)

    def _build_test_target(self, lang, unit_test_build_dir):
        """构建指定语言变体（C 或 CPP）的测试目标。"""
        test_build_path = Path(f"{unit_test_build_dir}_{lang.lower()}")
        test_build_path.mkdir(exist_ok=True)

        self._execute_command(["cmake", "..", "-DBUILD_TESTS=ON", f"-DLANG={lang}"],
                              cwd=str(test_build_path))
        self._execute_command(["make", "-j", str(self.build_jobs)],
                              cwd=str(test_build_path))

    def _run_c_tests(self, build_dir, lang):
        """运行 C/CPP 单元测试，覆盖无初始化 / preload / 正常环境 / 异常环境等场景。"""
        cwd = str(build_dir)
        test_binary = f"./test/c/mstx_test_{lang}"
        test_lib_binary = f"./test/c/mstx_test_lib_{lang}"
        lib_path = str(build_dir / "c") + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')

        logging.info("Testing %s without init", lang)
        self._execute_command([test_binary], cwd=cwd)
        env = os.environ.copy()
        env["LD_LIBRARY_PATH"] = lib_path
        self._execute_command([test_lib_binary], cwd=cwd, env=env)

        logging.info("Testing %s init with preload", lang)
        env = os.environ.copy()
        env["LD_PRELOAD"] = "./test/c/libinjection_test.so"
        self._execute_command([test_binary], cwd=cwd, env=env)
        env["LD_LIBRARY_PATH"] = lib_path
        self._execute_command([test_binary], cwd=cwd, env=env)

        logging.info("Testing %s init with normal env", lang)
        env = os.environ.copy()
        env["MSTX_INJECTION_PATH"] = cwd + "/test/c/libinjection_test.so"
        self._execute_command([test_binary], cwd=cwd, env=env)
        env["LD_LIBRARY_PATH"] = lib_path
        self._execute_command([test_binary], cwd=cwd, env=env)

        logging.info("Testing %s init with abnormal env", lang)
        env = os.environ.copy()
        env["MSTX_INJECTION_PATH"] = "1234"
        self._execute_command([test_binary], cwd=cwd, env=env)
        env["LD_LIBRARY_PATH"] = lib_path
        self._execute_command([test_binary], cwd=cwd, env=env)

    def _run_python_tests(self, build_dir):
        """运行 Python 单元测试，覆盖无初始化 / preload / 正常环境 / 异常环境等场景。"""
        cwd = str(build_dir)
        test_dir = str(build_dir / "test" / "python")
        mstx_so_dir = str(build_dir / "python")
        lib_path = str(build_dir / "c") + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
        base_pythonpath = mstx_so_dir + os.pathsep + os.environ.get('PYTHONPATH', '')

        os.makedirs(test_dir, exist_ok=True)
        shutil.copytree(str(self.project_root / "test" / "python"), test_dir, dirs_exist_ok=True)

        logging.info("Testing Python without init")
        env = os.environ.copy()
        env["PYTHONPATH"] = base_pythonpath
        self._execute_command(["pytest", "./test/python/test_mstx_without_init.py"], cwd=cwd, env=env)
        env["LD_LIBRARY_PATH"] = lib_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
        self._execute_command(["pytest", "./test/python/test_mstx_without_init.py"], cwd=cwd, env=env)

        logging.info("Testing Python init with preload")
        env = os.environ.copy()
        env["LD_PRELOAD"] = "./test/c/libinjection_test.so"
        env["PYTHONPATH"] = base_pythonpath
        self._execute_command(["pytest", "./test/python/test_mstx_with_init.py"], cwd=cwd, env=env)
        env["LD_LIBRARY_PATH"] = lib_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
        self._execute_command(["pytest", "./test/python/test_mstx_with_init.py"], cwd=cwd, env=env)

        logging.info("Testing Python init with normal env")
        env = os.environ.copy()
        env["MSTX_INJECTION_PATH"] = cwd + "/test/c/libinjection_test.so"
        env["PYTHONPATH"] = base_pythonpath
        self._execute_command(["pytest", "./test/python/test_mstx_with_init.py"], cwd=cwd, env=env)
        env["LD_LIBRARY_PATH"] = lib_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
        self._execute_command(["pytest", "./test/python/test_mstx_with_init.py"], cwd=cwd, env=env)

        logging.info("Testing Python init with abnormal env")
        env = os.environ.copy()
        env["MSTX_INJECTION_PATH"] = "1234"
        env["PYTHONPATH"] = base_pythonpath
        self._execute_command(["pytest", "./test/python/test_mstx_without_init.py"], cwd=cwd, env=env)
        env["LD_LIBRARY_PATH"] = lib_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
        self._execute_command(["pytest", "./test/python/test_mstx_without_init.py"], cwd=cwd, env=env)

    def run(self):
        os.chdir(self.project_root)

        # 在非 local 场景下按需更新依赖；在 local 场景下仅使用本地已有代码，不更新依赖。
        if 'local' not in self.parsed_arguments.command:
            from download_dependencies import DependencyManager
            DependencyManager(self.parsed_arguments).run()

        if 'test' in self.parsed_arguments.command:
            # -------------------- 单元测试 --------------------
            # ut 使用单独的目录构建，与 build 区分开，避免相互影响
            unit_test_build_dir = self.project_root / "build_ut"

            for lang in ("C", "CPP"):
                self._build_test_target(lang, unit_test_build_dir)

            ut_c_dir = Path(f"{unit_test_build_dir}_c")
            ut_cpp_dir = Path(f"{unit_test_build_dir}_cpp")

            self._run_c_tests(ut_c_dir, "C")
            self._run_c_tests(ut_cpp_dir, "CPP")
            self._run_python_tests(ut_cpp_dir)
        else:
            # -------------------- 产品构建 --------------------
            product_build_dir = self.project_root / "build"
            product_build_dir.mkdir(exist_ok=True)
            os.chdir(product_build_dir)

            self._execute_command(["cmake", ".."])
            self._execute_command(["make", "-j", str(self.build_jobs)])
            self._execute_command(["make", "install"])


if __name__ == "__main__":
    try:
        BuildManager().run()
    except Exception:
        logging.error(f"Unexpected error: {traceback.format_exc()}")
        sys.exit(1)
