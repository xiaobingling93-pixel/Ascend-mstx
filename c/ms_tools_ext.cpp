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

#ifndef MSTX_TOOLS_EXT
#define MSTX_TOOLS_EXT

#define MSTX_DECLSPEC __attribute__((visibility("default")))

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define MSTX_SUCCESS 0
#define MSTX_FAIL 1

#define MSTX_TOOL_INVALID_ID 0x0
#define MSTX_TOOL_MSPROF_ID 0x1000
#define MSTX_TOOL_MSOPPROF_ID 0x1001
#define MSTX_TOOL_MSSANITIZER_ID 0x1002
#define MSTX_TOOL_MSLEAKS_ID 0x1003

typedef uint64_t mstxRangeId;
typedef void* aclrtStream;

struct mstxDomainRegistration_st;
typedef struct mstxDomainRegistration_st mstxDomainRegistration_t;
typedef mstxDomainRegistration_t* mstxDomainHandle_t;

struct mstxMemHeap_st;
typedef struct mstxMemHeap_st mstxMemHeap_t;
typedef mstxMemHeap_t* mstxMemHeapHandle_t;

struct mstxMemRegion_st;
typedef struct mstxMemRegion_st mstxMemRegion_t;
typedef mstxMemRegion_t* mstxMemRegionHandle_t;

typedef struct mstxMemVirtualRangeDesc_t {
    uint32_t deviceId;
    void const *ptr;
    uint64_t size;
} mstxMemVirtualRangeDesc_t;

typedef enum mstxMemHeapUsageType {
    MSTX_MEM_HEAP_USAGE_TYPE_SUB_ALLOCATOR = 0,
} mstxMemHeapUsageType;

typedef enum mstxMemType {
    MSTX_MEM_TYPE_VIRTUAL_ADDRESS = 0,
} mstxMemType;

typedef struct mstxMemHeapDesc_t {
    mstxMemHeapUsageType usage;
    mstxMemType type;
    void const *typeSpecificDesc;
} mstxMemHeapDesc_t;

typedef struct mstxMemRegionsRegisterBatch_t {
    mstxMemHeapHandle_t heap;
    mstxMemType regionType;
    size_t regionCount;
    void const *regionDescArray;
    mstxMemRegionHandle_t* regionHandleArrayOut;
} mstxMemRegionsRegisterBatch_t;

typedef enum mstxMemRegionRefType {
    MSTX_MEM_REGION_REF_TYPE_POINTER = 0,
    MSTX_MEM_REGION_REF_TYPE_HANDLE
} mstxMemRegionRefType;

typedef struct mstxMemRegionRef_t {
    mstxMemRegionRefType refType;
    union {
        void const *pointer;
        mstxMemRegionHandle_t handle;
    };
} mstxMemRegionRef_t;

typedef struct mstxMemRegionsUnregisterBatch_t {
    size_t refCount;
    mstxMemRegionRef_t const *refArray;
} mstxMemRegionsUnregisterBatch_t;

/**
 * @ingroup MSTX
 * @brief mstx mark
 *
 * @param message [IN]    message for mark, cannot be null
 * @param stream  [IN]    stream used by mark, which can be set to null if not needed.
 */
MSTX_DECLSPEC void mstxMarkA(const char *message, aclrtStream stream);

/**
 * @ingroup MSTX
 * @brief mstx range start
 *
 * @param message [IN]    message to mark range, cannot be null
 * @param stream  [IN]    stream used by range, which can be set to null if not needed.
 * @retval mstxRangeId  the range id used for mstxRangeEnd
 * @retval return 0 if range start failed
 */
MSTX_DECLSPEC mstxRangeId mstxRangeStartA(const char *message, aclrtStream stream);

/**
 * @ingroup MSTX
 * @brief mstx range end
 *
 * @param id [IN]    the range id return by range start api
 */
MSTX_DECLSPEC void mstxRangeEnd(mstxRangeId id);

/**
 * @ingroup MSTX
 * @brief Register a memory heap as a memory pool
 *
 * @param domain [IN]    domain the memory pool belongs to
 * @param desc   [IN]    memory range description for memory pool
 * @retval mstxMemHeapHandle_t Return memory pool
 */
MSTX_DECLSPEC mstxMemHeapHandle_t mstxMemHeapRegister(mstxDomainHandle_t domain, mstxMemHeapDesc_t const *desc);

/**
 * @ingroup MSTX
 * @brief Unregister a memory pool
 *
 * @param domain [IN]    domain the memory pool belongs to
 * @param heap   [IN]    memory pool to unregister
 */
MSTX_DECLSPEC void mstxMemHeapUnregister(mstxDomainHandle_t domain, mstxMemHeapHandle_t heap);

/**
 * @ingroup MSTX
 * @brief Register suballocations within a pool
 *
 * @param domain - domain the memory pool belongs to
 * @param desc - description of a batch of memory regions
 */
MSTX_DECLSPEC void mstxMemRegionsRegister(mstxDomainHandle_t domain, mstxMemRegionsRegisterBatch_t const *desc);

/**
 * @ingroup MSTX
 * @brief Unregister suballocations within a pool
 * @param domain - domain the suballocations belongs to
 * @param desc - description of a batch of region references
 */
MSTX_DECLSPEC void mstxMemRegionsUnregister(mstxDomainHandle_t domain, mstxMemRegionsUnregisterBatch_t const *desc);

/**
 * @brief mstx create a domain
 *
 * @param name [IN]    a unique string representing the domain.
 * @retval mstxDomainHandle_t  a handle representing the domain.
*/
MSTX_DECLSPEC mstxDomainHandle_t mstxDomainCreateA(const char *name);

/**
 * @ingroup MSTX
 * @brief mstx destroy a domain
 *
 * @param mstxDomainHandle_t [IN]    the domain handle to be destroyed.
*/
MSTX_DECLSPEC void mstxDomainDestroy(mstxDomainHandle_t domain);

/**
 * @ingroup MSTX
 * @brief mstx mark for specific domain
 *
 * @param domain  [IN]    the domain of scoping the category
 * @param message [IN]    message for mark, cannot be null
 * @param stream  [IN]    stream used by mark, which can be set to null if not needed.
 */
MSTX_DECLSPEC void mstxDomainMarkA(mstxDomainHandle_t domain, const char *message, aclrtStream stream);

/**
 * @ingroup MSTX
 * @brief mstx range start for specific domain
 *
 * @param domain  [IN]    the domain of scoping the category
 * @param message [IN]    message to mark range, cannot be null
 * @param stream  [IN]    stream used by range, which can be set to null if not needed.
 * @retval mstxRangeId  the range id used for mstxRangeEnd
 * @retval return 0 if range start failed or input invalid domain handle
 */
MSTX_DECLSPEC mstxRangeId mstxDomainRangeStartA(mstxDomainHandle_t domain, const char *message, aclrtStream stream);

/**
 * @ingroup MSTX
 * @brief mstx range end for specific domain
 *
 * @param domain  [IN]    the domain of scoping the category
 * @param id [IN]    the range id return by range start api
 */
MSTX_DECLSPEC void mstxDomainRangeEnd(mstxDomainHandle_t domain, mstxRangeId id);

/**
 * @ingroup MSTX
 * @brief get id of the registered tool
 * @param id [OUT] Tool registration identifier. If the tool is not registered
 *                 or this function is not registered by tool, `id` will be set to MSTX_TOOL_INVALID_ID.
 */
MSTX_DECLSPEC void mstxGetToolId(uint64_t *id);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#define MSTX_IMPL_GUARD // Ensure other headers cannot include directly

#include "mstx/mstx_detail/mstx_types.h"

#ifndef MSTX_NO_IMPL
#include "mstx/mstx_detail/mstx_impl.h"
#endif // MSTX_NO_IMPL

#undef MSTX_IMPL_GUARD
#endif // MSTX_TOOLS_EXT
