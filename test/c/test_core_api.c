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

#include <mstx/ms_tools_ext.h>

int GetInitResult(void)
{
    return g_mstxContext.initState;
}

int CheckFuncPointerAllNull(void)
{
    if (g_mstxContext.mstxRangeStartAPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxRangeEndPtr != NULL) {
        return 0;
    }    
    if (g_mstxContext.mstxGetToolIdPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxMemHeapRegisterPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxMemHeapUnregisterPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxMemRegionsRegisterPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxMemRegionsUnregisterPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxDomainCreateAPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxDomainDestroyPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxDomainMarkAPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxDomainRangeStartAPtr != NULL) {
        return 0;
    }
    if (g_mstxContext.mstxDomainRangeEndPtr != NULL) {
        return 0;
    }
    return 1;
}

mstxRangeId MstxRangeStartATest(const char *message, aclrtStream stream)
{
    return mstxRangeStartA(message, stream);
}

void MstxRangeEndTest(mstxRangeId id)
{
    return mstxRangeEnd(id);
}

void MstxMarkATest(const char *message, aclrtStream stream)
{
    return mstxMarkA(message, stream);
}

void MstxGetToolIdTest(uint64_t *id)
{
    return mstxGetToolId(id);
}

mstxMemHeapHandle_t MstxMemHeapRegisterTest(mstxDomainHandle_t domain, const mstxMemHeapDesc_t *desc)
{
    return mstxMemHeapRegister(domain, desc);
}

void MstxMemHeapUnregisterTest(mstxDomainHandle_t domain, mstxMemHeapHandle_t heap)
{
    return mstxMemHeapUnregister(domain, heap);
}

void MstxMemRegionsRegisterTest(mstxDomainHandle_t domain, const mstxMemRegionsRegisterBatch_t *desc)
{
    return mstxMemRegionsRegister(domain, desc);
}

void MstxMemRegionsUnregisterTest(mstxDomainHandle_t domain, const mstxMemRegionsUnregisterBatch_t *desc)
{
    return mstxMemRegionsUnregister(domain, desc);
}

mstxDomainHandle_t MstxDomainCreateATest(const char* name)
{
    return mstxDomainCreateA(name);
}

void MstxDomainDestroyTest(mstxDomainHandle_t handle)
{
    return mstxDomainDestroy(handle);
}

void MstxDomainMarkATest(mstxDomainHandle_t handle, const char *message, aclrtStream stream)
{
    return mstxDomainMarkA(handle, message, stream);
}

mstxRangeId MstxDomainRangeStartATest(mstxDomainHandle_t handle, const char *message, aclrtStream stream)
{
    return mstxDomainRangeStartA(handle, message, stream);
}

void MstxDomainRangeEndTest(mstxDomainHandle_t handle, mstxRangeId id)
{
    return mstxDomainRangeEnd(handle, id);
}

void MstxDeInit(void)
{
    g_mstxContext.initState = MSTX_INIT_STATE_UNINITED;
    g_mstxContext.mstxMarkAPtr = mstxMarkAInit;
    g_mstxContext.mstxRangeStartAPtr = mstxRangeStartAInit;
    g_mstxContext.mstxRangeEndPtr = mstxRangeEndInit;

    g_mstxContext.mstxMemHeapRegisterPtr = mstxMemHeapRegisterInit;
    g_mstxContext.mstxMemHeapUnregisterPtr = mstxMemHeapUnregisterInit;
    g_mstxContext.mstxMemRegionsRegisterPtr = mstxMemRegionsRegisterInit;
    g_mstxContext.mstxMemRegionsUnregisterPtr = mstxMemRegionsUnregisterInit;

    g_mstxContext.mstxDomainCreateAPtr = mstxDomainCreateAInit;
    g_mstxContext.mstxDomainDestroyPtr = mstxDomainDestroyInit;
    g_mstxContext.mstxDomainMarkAPtr = mstxDomainMarkAInit;
    g_mstxContext.mstxDomainRangeStartAPtr = mstxDomainRangeStartAInit;
    g_mstxContext.mstxDomainRangeEndPtr = mstxDomainRangeEndInit;

    g_mstxContext.mstxGetToolIdPtr = mstxGetToolIdInit;
}

void MstxInit(void)
{
    mstxInitOnce();
}

int MstxGetModuleFuncTableTest(mstxFuncModule module, mstxFuncTable* outTable, unsigned int* outSize)
{
    return g_mstxContext.getModuleFuncTable(module, outTable, outSize);
}

void RefreshUninitMstxContextFuncPtrTest(int forceNull)
{
    refreshUninitMstxContextFuncPtr(forceNull);
}
