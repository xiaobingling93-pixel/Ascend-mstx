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

#include "mstx_methods.h"

#ifdef __cplusplus
extern "C" {
#endif

static struct PyModuleDef g_mstxMethods = {
    PyModuleDef_HEAD_INIT, "mstx", // name of module
    "", // module documentation, may be nullptr
    -1, // size of per-interpreter state of the module, or -1 if the module keeps state in global variables.
    GetMstxMethods()
};
__attribute__((visibility("default"))) PyObject *PyInit_mstx(void)
{
    PyObject *m = PyModule_Create(&g_mstxMethods);
    return m;
}

#ifdef __cplusplus
}
#endif