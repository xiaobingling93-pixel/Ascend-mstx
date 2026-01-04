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

import os
import sys
import logging
import subprocess
import multiprocessing
import shutil
import argparse


def exec_cmd(env, cmd):
    result = subprocess.run(cmd, env=env, capture_output=False, text=True, timeout=36000)
    if result.returncode != 0:
        logging.error("execute command %s failed, please check the log", " ".join(cmd))
        sys.exit(result.returncode)


def execute_make(build_path, cmake_cmd, make_cmd, install_cmd):
    logging.info("*************** Create build_path: "+ build_path +" *****************")
    if not os.path.exists(build_path):
        os.makedirs(build_path, mode=0o755)
    os.chdir(build_path)
    logging.info("*************** Execute cmake, make and make install *****************")
    env = os.environ.copy()
    exec_cmd(env, cmake_cmd)
    exec_cmd(env, make_cmd)
    if install_cmd != "":
        exec_cmd(env, install_cmd)


def execute_ctest(build_path, clang):
    logging.info("*************** TEST "+ clang +" WITHOUT INIT *****************")
    os.chdir(build_path)
    env = os.environ.copy()
    exec_cmd(env, ["./test/c/mstx_test_" + clang])
    library_path = os.path.join(build_path, "c") + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
    env["LD_LIBRARY_PATH"] = library_path
    exec_cmd(env, ["./test/c/mstx_test_lib_" + clang])

    logging.info("*************** TEST "+ clang +" INIT WITH PRELOAD *****************")
    env = os.environ.copy()
    env["LD_PRELOAD"] = "./test/c/libinjection_test.so"
    exec_cmd(env, ["./test/c/mstx_test_" + clang])
    env["LD_LIBRARY_PATH"] = library_path
    exec_cmd(env, ["./test/c/mstx_test_" + clang])

    logging.info("*************** TEST "+ clang +" INIT WITH NORMAL ENV *****************")
    env = os.environ.copy()
    env["MSTX_INJECTION_PATH"] = os.getcwd() + "/test/c/libinjection_test.so"
    exec_cmd(env, ["./test/c/mstx_test_" + clang])
    env["LD_LIBRARY_PATH"] = library_path
    exec_cmd(env, ["./test/c/mstx_test_" + clang])

    logging.info("*************** TEST "+ clang +" INIT WITH ABNORMAL ENV *****************")
    env = os.environ.copy()
    env["MSTX_INJECTION_PATH"] = "1234"
    exec_cmd(env, ["./test/c/mstx_test_" + clang])
    env["LD_LIBRARY_PATH"] = library_path
    exec_cmd(env, ["./test/c/mstx_test_" + clang])


def execute_python_test(build_path):
    logging.info("*************** INIT PYTHON TEST WORKDIR *****************")
    os.chdir(build_path)
    test_dir = os.path.join(build_path, "test", "python")
    mstxSoDir = os.path.join(build_path, "python")
    os.makedirs(test_dir, exist_ok=True)
    shutil.copytree(os.path.join(build_path, "../test/python"), test_dir, dirs_exist_ok=True)
    library_path = os.path.join(build_path, "c") + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
    logging.info("*************** TEST PYTHON WITHOUT INIT *****************")
    env = os.environ.copy()
    env["PYTHONPATH"] = mstxSoDir + os.pathsep + os.environ.get('PYTHONPATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_without_init.py"])
    env["LD_LIBRARY_PATH"] = library_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_without_init.py"])    

    logging.info("*************** TEST PYTHON INIT WITH PRELOAD *****************")
    env = os.environ.copy()
    env["LD_PRELOAD"] = "./test/c/libinjection_test.so"
    env["PYTHONPATH"] = mstxSoDir + os.pathsep + os.environ.get('PYTHONPATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_with_init.py"])
    env["LD_LIBRARY_PATH"] = library_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_with_init.py"])

    logging.info("*************** TEST PYTHON INIT WITH NORMAL ENV *****************")
    env = os.environ.copy()
    env["MSTX_INJECTION_PATH"] = os.getcwd() + "/test/c/libinjection_test.so"
    env["PYTHONPATH"] = mstxSoDir + os.pathsep + os.environ.get('PYTHONPATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_with_init.py"])
    env["LD_LIBRARY_PATH"] = library_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_with_init.py"])
     
    logging.info("*************** TEST PYTHON INIT WITH ABNORMAL ENV *****************")
    env = os.environ.copy()
    env["MSTX_INJECTION_PATH"] = "1234"
    env["PYTHONPATH"] = mstxSoDir + os.pathsep + os.environ.get('PYTHONPATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_without_init.py"])
    env["LD_LIBRARY_PATH"] = library_path + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
    exec_cmd(env, ["pytest", "./test/python/test_mstx_without_init.py"])

def create_arg_parser():
    parser = argparse.ArgumentParser(description='Build script with optional testing')
    parser.add_argument('command', nargs='*', default='[]', 
                    choices=['[]', 'local', 'test'],
                    help='Command to execute (python build.py [ |local|test]):\n')
    parser.add_argument('-r', '--revision',
                        help="Build with specific revision or tag")
    return parser

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    parser = create_arg_parser()
    args = parser.parse_args()

    current_dir = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))
    os.chdir(current_dir)
    
    build_path = os.path.join(current_dir, "build")
    cmake_cmd = ["cmake", ".."]
    cpu_cores = multiprocessing.cpu_count()
    make_cmd = ["make", "-j", str(cpu_cores)]
    install_cmd = ["make", "install"]

    if 'test' not in args.command:
        # 执行CPP代码构建
        execute_make(build_path, cmake_cmd, make_cmd, install_cmd)
    else:
        # ut使用单独的目录构建，与build区分开，避免相互影响，并传入对应的参数
        build_path = os.path.join(current_dir, "build_ut")
        cmake_cmd.append("-DBUILD_TESTS=ON")
        install_cmd = ""  # ut没有检查output输出的用例，因此不需要执行install
        # 只有测试构建时才需更新子仓
        from download_dependencies import update_submodule
        update_submodule(args)
        # 执行C测试构建
        cmake_cmd_c = cmake_cmd.copy()
        cmake_cmd_c.append("-DLANG=C")
        execute_make(build_path+"_c", cmake_cmd_c, make_cmd, install_cmd)
        # 执行CPP测试构建
        cmake_cmd_cpp = cmake_cmd.copy()
        cmake_cmd_cpp.append("-DLANG=CPP")
        execute_make(build_path+"_cpp", cmake_cmd_cpp, make_cmd, install_cmd)
        # 运行测试用例
        execute_ctest(build_path+"_c", "C")
        execute_ctest(build_path+"_cpp", "CPP")
        execute_python_test(build_path+"_cpp")
