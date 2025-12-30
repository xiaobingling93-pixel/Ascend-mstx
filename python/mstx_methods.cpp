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

#include "mstx_api.h"

namespace {
    PyMethodDef g_methodsMstx[] = {
        {"range_start", reinterpret_cast<PyCFunction>(WrapMstxRangeStartA), METH_VARARGS | METH_KEYWORDS, "mark start"},
        {"range_end", reinterpret_cast<PyCFunction>(WrapMstxRangeEnd), METH_VARARGS | METH_KEYWORDS, "mark end"},
        {"mark", reinterpret_cast<PyCFunction>(WrapMstxMarkA),  METH_VARARGS | METH_KEYWORDS, "mark a position"},
        {"get_tool_id", reinterpret_cast<PyCFunction>(WrapMstxGetToolId),  METH_NOARGS, "get id of registered tool"},
        {nullptr, nullptr, 0, nullptr}
    };
}

PyMethodDef *GetMstxMethods(void)
{
    return g_methodsMstx;
}