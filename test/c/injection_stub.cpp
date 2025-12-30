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

#define MSTX_NO_IMPL
#include <mstx/ms_tools_ext.h>
#include <cstdio>
int g_rangeState = 0;
char g_message[5] = {'\0'};
const int TEST_RANGE_ID = 123456;
const uint64_t TOOL_ID = 1234;

struct mstxDomainRegistration_st {};
int g_domainState = 0;
struct mstxDomainRegistration_st g_domain;
const char *g_domainMarkMessage = nullptr;

extern "C" int GetRangeState()
{
    return g_rangeState;
}

extern "C" char GetMarkMessage(int index)
{
    if (index >= sizeof(g_message) / sizeof(g_message[0])) {
        return '\0';
    }
    return g_message[index];
}

extern "C" int GetDomainState()
{
    return g_domainState;
}

extern "C" const char *GetDomainMarkMessage()
{
    return g_domainMarkMessage;
}

void mstxMarkA(const char* message, aclrtStream stream)
{
    g_message[0] = (*message);
}

mstxRangeId mstxRangeStartA(const char* message, aclrtStream stream)
{
    g_rangeState = 1;
    return TEST_RANGE_ID;
}

void mstxRangeEnd(mstxRangeId id)
{
    g_rangeState = 0;
}

void mstxRangeTest(mstxRangeId id)
{
}

mstxDomainHandle_t mstxDomainCreateA(const char* name)
{
    g_domainState = 1;
    return &g_domain;
}

void mstxDomainDestroy(mstxDomainHandle_t domain)
{
    g_domainState = 0;
}

void mstxDomainMarkA(mstxDomainHandle_t domain, const char* message, aclrtStream stream)
{
    g_domainMarkMessage = message;
}

void mstxGetToolId(uint64_t *id)
{
    *id = TOOL_ID;
}

mstxRangeId mstxDomainRangeStartA(mstxDomainHandle_t domain, const char* message, aclrtStream stream)
{
    g_rangeState = 1;
    return TEST_RANGE_ID;
}

void mstxDomainRangeEnd(mstxDomainHandle_t domain, mstxRangeId id)
{
    g_rangeState = 0;
}

void mstxDomainTest()
{
}

void mstxFoundationTest()
{
}

int getFuncTableForCore(mstxGetModuleFuncTableFunc getFuncTable)
{
    unsigned int outSize = 0;
    mstxFuncTable outTable;
    int ret = getFuncTable(MSTX_API_MODULE_CORE, &outTable, &outSize);
    if (ret == MSTX_FAIL) {
        return MSTX_FAIL;
    }
    mstxFuncPointer func_list[] = {
        (mstxFuncPointer)mstxMarkA,
        (mstxFuncPointer)mstxRangeStartA,
        (mstxFuncPointer)mstxRangeEnd,
        (mstxFuncPointer)mstxGetToolId,
        (mstxFuncPointer)mstxRangeTest,
    };
    for (size_t i = 0; i < sizeof(func_list)/sizeof(mstxFuncPointer); i++) {
        if (i >= outSize - 1) {
            break;
        }
        *(outTable[i + 1]) = func_list[i];
    }
    return MSTX_SUCCESS;
}

int getFuncTableForCore2(mstxGetModuleFuncTableFunc getFuncTable)
{
    unsigned int outSize = 0;
    mstxFuncTable outTable;
    int ret = getFuncTable(MSTX_API_MODULE_CORE_DOMAIN, &outTable, &outSize);
    if (ret == MSTX_FAIL) {
        return MSTX_FAIL;
    }
    mstxFuncPointer func_list[] = {
        (mstxFuncPointer)mstxDomainCreateA,
        (mstxFuncPointer)mstxDomainDestroy,
        (mstxFuncPointer)mstxDomainMarkA,
        (mstxFuncPointer)mstxDomainRangeStartA,
        (mstxFuncPointer)mstxDomainRangeEnd,
        (mstxFuncPointer)mstxDomainTest,
    };
    for (size_t i = 0; i < sizeof(func_list)/sizeof(mstxFuncPointer); i++) {
        if (i >= outSize - 1) {
            break;
        }
        *(outTable[i + 1]) = func_list[i];
    }
    return MSTX_SUCCESS;
}

extern "C" int InitInjectionMstx(mstxGetModuleFuncTableFunc getFuncTable)
{
    if (getFuncTableForCore(getFuncTable) != MSTX_SUCCESS || getFuncTableForCore2(getFuncTable) != MSTX_SUCCESS) {
        return MSTX_FAIL;
    }
    return MSTX_SUCCESS;
}
