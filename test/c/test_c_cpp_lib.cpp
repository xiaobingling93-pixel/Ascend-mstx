/* -------------------------------------------------------------------------
 * This file is part of the MindStudio project.
 * Copyright (c) 2025 Huawei Technologies Co.,Ltd.
 *
 * MindStudio is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * ------------------------------------------------------------------------- */

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <dlfcn.h>

#ifdef LANG_C_TEST
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

const int TEST_RANGE_ID = 123456;

using mstxRangeId = uint64_t;
using aclrtStream = void(*);
using mstxMarkAFunc = void (*)(const char* message, aclrtStream stream);
using mstxRangeStartAFunc = mstxRangeId (*)(const char* message, aclrtStream stream);
using mstxRangeEndFunc = void (*)(mstxRangeId id);
using GetRangeStateFunc = int (*)(void);
using GetMarkMessageFunc = char (*)(int index);

EXTERN void MstxInit();

void TestMultiThreadInit()
{
    void *handle = dlopen("libms_tools_ext.so", RTLD_NOW | RTLD_LOCAL);
    ASSERT_NE(handle, nullptr);
    mstxRangeStartAFunc MstxRangeStartA = (mstxRangeStartAFunc) dlsym(handle, "mstxRangeStartA");
    mstxRangeEndFunc MstxRangeEnd = (mstxRangeEndFunc) dlsym(handle, "mstxRangeEnd");
    mstxRangeId id = MstxRangeStartA("test", NULL); // in this case, init mstx with range start
    MstxRangeEnd(id);
    ASSERT_EQ(dlclose(handle), 0);
}

TEST(CoreApi, test_lib_init_with_multi_thread)
{
    std::vector<std::thread> threads;
    int threadNum = 100;
    for (int i = 0; i < threadNum; ++i) {
        threads.emplace_back(TestMultiThreadInit);
    }
    for (int i = 0; i < threadNum; ++i) {
        threads[i].join();
    }
}

TEST(CoreApi, test_lib_range_start_a_with_range_end_if_init_success_expect_success)
{
    void *handle = dlopen("libms_tools_ext.so", RTLD_NOW | RTLD_LOCAL);
    ASSERT_NE(handle, nullptr);
    mstxRangeStartAFunc MstxRangeStartA = (mstxRangeStartAFunc) dlsym(handle, "mstxRangeStartA");
    mstxRangeEndFunc MstxRangeEnd = (mstxRangeEndFunc) dlsym(handle, "mstxRangeEnd");
    mstxRangeId id = MstxRangeStartA("test", nullptr);
    if (id == 0) { //mstx init failed, do not run this ut
        return ;
    }
    ASSERT_TRUE(id == TEST_RANGE_ID);
    GetRangeStateFunc stateFunc = (GetRangeStateFunc)dlsym(nullptr, "GetRangeState");
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 1);
    }
    MstxRangeEnd(id);
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 0);
    }
    ASSERT_EQ(dlclose(handle), 0);
}

TEST(CoreApi, test_lib_range_start_a_with_range_end_if_init_failed_expect_invalid_id)
{
    void *handle = dlopen("libms_tools_ext.so", RTLD_NOW | RTLD_LOCAL);
    ASSERT_NE(handle, nullptr);
    mstxRangeStartAFunc MstxRangeStartA = (mstxRangeStartAFunc) dlsym(handle, "mstxRangeStartA");
    mstxRangeEndFunc MstxRangeEnd = (mstxRangeEndFunc) dlsym(handle, "mstxRangeEnd");
    mstxRangeId id = MstxRangeStartA("test", nullptr);
    if (id != 0) { //mstx init success, do not run this ut
        return ;
    }
    MstxRangeEnd(id);
    ASSERT_EQ(dlclose(handle), 0);
}

TEST(CoreApi, test_lib_init_with_mark_a_if_init_success_expect_correct_message)
{
    void *handle = dlopen("libms_tools_ext.so", RTLD_NOW | RTLD_LOCAL);
    ASSERT_NE(handle, nullptr);
    mstxRangeStartAFunc MstxRangeStartA = (mstxRangeStartAFunc) dlsym(handle, "mstxRangeStartA");
    if (MstxRangeStartA("test", nullptr) == 0) { //mstx init failed, do not run this ut
        return ;
    }
    mstxMarkAFunc mstxMarkA = (mstxMarkAFunc) dlsym(handle, "mstxMarkA");
    const char testMessage[5] = "test";
    int testIndex = 0;
    mstxMarkA(testMessage, nullptr); // init with mstxMarkA func
    GetMarkMessageFunc getMarkMessage = (GetMarkMessageFunc)dlsym(nullptr, "GetMarkMessage");
    if (getMarkMessage) {
        ASSERT_EQ(getMarkMessage(testIndex), testMessage[testIndex]);
    }
    ASSERT_EQ(dlclose(handle), 0);
}
