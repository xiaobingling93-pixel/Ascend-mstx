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
#define MSTX_NO_IMPL
#include "mstx/ms_tools_ext.h"
#undef MSTX_NO_IMPL

#ifdef LANG_C_TEST
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
using GetRangeStateFunc = int (*)();
using GetDomainStateFunc = int (*)();
using GetDomainMarkMessageFunc = const char *(*)();
const int TEST_RANGE_ID = 123456;
const uint64_t TOOL_ID = 1234;

EXTERN mstxMemHeapHandle_t MstxMemHeapRegisterTest(mstxDomainHandle_t domain, mstxMemHeapDesc_t const *desc);
EXTERN void MstxMemHeapUnregisterTest(mstxDomainHandle_t domain, mstxMemHeapHandle_t heap);
EXTERN void MstxMemRegionsRegisterTest(mstxDomainHandle_t domain, mstxMemRegionsRegisterBatch_t const* desc);
EXTERN void MstxMemRegionsUnregisterTest(mstxDomainHandle_t domain, mstxMemRegionsUnregisterBatch_t const* desc);
EXTERN void MstxMarkATest(const char *message, aclrtStream stream);
EXTERN mstxRangeId MstxRangeStartATest(const char *message, aclrtStream stream);
EXTERN void MstxRangeEndTest(mstxRangeId id);
EXTERN void MstxGetToolIdTest(uint64_t *id);
EXTERN mstxDomainHandle_t MstxDomainCreateATest(const char *name);
EXTERN void MstxDomainDestroyTest(mstxDomainHandle_t handle);
EXTERN void MstxDomainMarkATest(mstxDomainHandle_t handle, const char *message, aclrtStream stream);
EXTERN mstxRangeId MstxDomainRangeStartATest(mstxDomainHandle_t handle, const char *message, aclrtStream stream);
EXTERN void MstxDomainRangeEndTest(mstxDomainHandle_t handle, mstxRangeId id);
EXTERN int GetInitResult();
EXTERN void MstxDeInit();
EXTERN int MstxGetModuleFuncTableTest(mstxFuncModule module, mstxFuncTable* outTable, unsigned int* outSize);
EXTERN void MstxInit();
EXTERN void RefreshUninitMstxContextFuncPtrTest(int forceNull);
EXTERN int CheckFuncPointerAllNull();

void TestMultiThreadInit()
{
    mstxRangeId id = MstxRangeStartATest("test", NULL); // in this case, init mstx with range start
    MstxRangeEndTest(id);
}

TEST(CoreApi, test_init_with_multi_thread)
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

TEST(CoreApi, test_range_start_a_with_range_end_if_init_success_expect_success)
{
    MstxInit();
    if (GetInitResult() != 2) { // do this test if init success
        return;
    }
    mstxRangeId id = MstxRangeStartATest("test", nullptr);
    ASSERT_TRUE(id == TEST_RANGE_ID);
    GetRangeStateFunc stateFunc = (GetRangeStateFunc)dlsym(nullptr, "GetRangeState");
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 1);
    }
    MstxRangeEndTest(id);
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 0);
    }
}

TEST(CoreApi, test_range_start_a_with_range_end_if_init_failed_expect_invalid_id)
{
    MstxInit();
    if (GetInitResult() == 2) { // do this test if init failed
        return;
    }
    mstxRangeId id = MstxRangeStartATest("test", nullptr);
    ASSERT_TRUE(id == 0);
    MstxRangeEndTest(id);
}

TEST(CoreApi, test_get_tool_id_if_init_success_expect_success)
{
    uint64_t id = 0;

    MstxInit();
    if (GetInitResult() != 2) { // do this test if init success
        return;
    }
    MstxGetToolIdTest(&id);
    ASSERT_TRUE(id == TOOL_ID);
}

TEST(CoreApi, test_get_tool_id_if_init_failed_expect_default_id)
{
    uint64_t id = 7;

    MstxInit();
    if (GetInitResult() == 2) { // do this test if init success
        return;
    }
    MstxGetToolIdTest(&id);
    ASSERT_TRUE(id == 0);
}

TEST(CoreApi, test_domain_createA_if_init_fail_expect_nullptr)
{
    MstxInit();
    if (GetInitResult() == 2) { // do this test if init failed
        return;
    }
    auto handle = MstxDomainCreateATest("domain");
    ASSERT_TRUE(handle == nullptr);
}

TEST(CoreApi, test_init_with_range_end_func_expect_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxRangeEndTest(0);
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_domain_createA_if_init_succ_expect_valid_handle)
{
    MstxInit();
    if (GetInitResult() != 2) { // do this test if init success
        return;
    }
    auto handle = MstxDomainCreateATest("domain");
    ASSERT_TRUE(handle != nullptr);
}

TEST(CoreApi, test_domain_destroy_if_init_success_expect_destroy_handle)
{
    MstxInit();
    if (GetInitResult() != 2) { // do this test if init success
        return;
    }
    auto handle = MstxDomainCreateATest("domain");
    MstxDomainDestroyTest(handle);
    GetDomainStateFunc stateFunc = (GetDomainStateFunc)dlsym(nullptr, "GetDomainState");
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 0);
    }
}

TEST(CoreApi, test_domain_destroy_if_init_fail_expect_destroy_handle)
{
    MstxInit();
    if (GetInitResult() == 2) { // do this test if init failed
        return;
    }
    auto handle = MstxDomainCreateATest("domain");
    MstxDomainDestroyTest(handle);
}

TEST(CoreApi, test_domain_markA_with_input_if_init_fail_expect_not_save_mark_msg)
{
    MstxInit();
    if (GetInitResult() == 2) { // do this test if init failed
        return;
    }
    int msgLen = 4;
    std::string msg = "test";
    auto handle = MstxDomainCreateATest("domain");
    MstxDomainMarkATest(handle, msg.c_str(), nullptr);
    GetDomainMarkMessageFunc func = (GetDomainMarkMessageFunc)dlsym(nullptr, "GetDomainMarkMessage");
    if (func) {
        const char *domainMarkMsg = func();
        ASSERT_TRUE(domainMarkMsg == nullptr);
    }
}

TEST(CoreApi, test_domain_markA_with_input_if_init_success_expect_save_mark_msg)
{
    MstxInit();
    if (GetInitResult() != 2) { // do this test if init success
        return;
    }
    int msgLen = 4;
    std::string msg = "test";
    auto handle = MstxDomainCreateATest("domain");
    MstxDomainMarkATest(handle, msg.c_str(), nullptr);
    GetDomainMarkMessageFunc func = (GetDomainMarkMessageFunc)dlsym(nullptr, "GetDomainMarkMessage");
    if (func) {
        const char *domainMarkMsg = func();
        EXPECT_STREQ(domainMarkMsg, msg.c_str());
    }
}

TEST(CoreApi, test_domain_range_startA_with_input_if_init_fail_expect_zero_range_id)
{
    MstxInit();
    if (GetInitResult() == 2) { // do this test if init failed
        return;
    }
    int msgLen = 4;
    std::string msg = "test";
    auto handle = MstxDomainCreateATest("domain");
    auto id = MstxDomainRangeStartATest(handle, msg.c_str(), nullptr);
    ASSERT_TRUE(id == 0);
    GetRangeStateFunc stateFunc = (GetRangeStateFunc)dlsym(nullptr, "GetRangeState");
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 0);
    }
    MstxDomainRangeEndTest(handle, id);
}

TEST(CoreApi, test_domain_range_if_init_succ_expect_success)
{
    MstxInit();
    if (GetInitResult() != 2) { // do this test if init success
        return;
    }
    int msgLen = 4;
    std::string msg = "test";
    auto handle = MstxDomainCreateATest("domain");
    auto id = MstxDomainRangeStartATest(handle, msg.c_str(), nullptr);
    ASSERT_TRUE(id == TEST_RANGE_ID);
    GetRangeStateFunc stateFunc = (GetRangeStateFunc)dlsym(nullptr, "GetRangeState");
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 1);
    }
    MstxDomainRangeEndTest(handle, id);
    if (stateFunc) {
        ASSERT_TRUE(stateFunc() == 0);
    }
}

TEST(CoreApi, test_getFuncTable_with_invalid_input_expect_failed)
{
    mstxFuncTable outTable;
    unsigned int outSize;
    ASSERT_TRUE(MstxGetModuleFuncTableTest(MSTX_API_MODULE_INVALID, &outTable, &outSize) == MSTX_FAIL);
    ASSERT_TRUE(MstxGetModuleFuncTableTest(MSTX_API_MODULE_CORE, nullptr, &outSize) == MSTX_FAIL);
    ASSERT_TRUE(MstxGetModuleFuncTableTest(MSTX_API_MODULE_CORE, &outTable, nullptr) == MSTX_FAIL);
    ASSERT_TRUE(MstxGetModuleFuncTableTest(MSTX_API_MODULE_CORE_DOMAIN, &outTable, nullptr) == MSTX_FAIL);
}

TEST(CoreApi, test_double_init)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxRangeEndTest(0); // init with mstxRangeEnd func
    ASSERT_TRUE(GetInitResult() != 0);
    MstxInit();
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_refresh_unit_api_to_null)
{
    MstxDeInit();
    RefreshUninitMstxContextFuncPtrTest(0);
    ASSERT_TRUE(CheckFuncPointerAllNull() == 1);
    MstxDeInit();
    RefreshUninitMstxContextFuncPtrTest(1);
    ASSERT_TRUE(CheckFuncPointerAllNull() == 1);
    MstxInit();
    RefreshUninitMstxContextFuncPtrTest(1);
    ASSERT_TRUE(CheckFuncPointerAllNull() == 1);
}

TEST(CoreApi, test_init_with_mark_a)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxMarkATest("test", nullptr); // init with mstxMarkA func
    ASSERT_TRUE(GetInitResult() != 0);
    RefreshUninitMstxContextFuncPtrTest(1);
    MstxMarkATest("test", nullptr); // test run with null pointer
}

TEST(CoreApi, test_init_with_mem_heap_register_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxMemHeapRegisterTest(globalDomain, nullptr);
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_init_with_mem_heap_unregisterr_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxMemHeapUnregisterTest(globalDomain, {});
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_init_with_mem_regions_registerr_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxMemRegionsRegisterTest(globalDomain, nullptr);
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_init_with_mem_regions_unregisterr_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxMemRegionsUnregisterTest(globalDomain, nullptr);
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_init_with_domain_createA_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxDomainCreateATest("domain");
    ASSERT_TRUE(GetInitResult() != 0);
}
 
TEST(CoreApi, test_init_with_domain_destroy_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxDomainDestroyTest(nullptr);
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_init_with_domain_markA_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxDomainMarkATest(nullptr, "test", nullptr);
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_init_with_domain_range_startA_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxDomainRangeStartATest(nullptr, "test", nullptr);
    ASSERT_TRUE(GetInitResult() != 0);
}

TEST(CoreApi, test_init_with_domain_range_end_expect_init_success)
{
    MstxDeInit();
    ASSERT_TRUE(GetInitResult() == 0);
    MstxDomainRangeEndTest(nullptr, 0);
    ASSERT_TRUE(GetInitResult() != 0);
}