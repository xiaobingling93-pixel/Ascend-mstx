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

#include "mstx/ms_tools_ext.h"
#include "mstx_api.h"

class GILCtrl {
public:
    GILCtrl() : tstate(PyEval_SaveThread()) {} // 释放GIL
    ~GILCtrl()
    {
        PyEval_RestoreThread(tstate);   // 恢复GIL
    }
private:
    PyThreadState* tstate;
};

void ParseArgs(PyObject *args, PyObject *kwds, char *&message, aclrtStream &stream)
{
    message = nullptr;
    stream = nullptr;
    static char arg1[] = "message";
    static char arg2[] = "stream";
    static char *kwlist[] = {arg1, arg2, nullptr};
    PyArg_ParseTupleAndKeywords(args, kwds, "|sO", kwlist, &message, &stream);
}

PyObject *WrapMstxMarkA(PyObject *self, PyObject *args, PyObject *kwds)
{
    char *message;
    aclrtStream stream;
    ParseArgs(args, kwds, message, stream);

    // 检查stream对象是否为None
    if (stream == Py_None) {
        stream = nullptr;
    }
    {
        GILCtrl gilCtrl;
        mstxMarkA(message, stream);
    }
    Py_RETURN_NONE;
}

PyObject *WrapMstxRangeStartA(PyObject *self, PyObject *args, PyObject *kwds)
{
    char *message;
    aclrtStream stream;
    ParseArgs(args, kwds, message, stream);
    mstxRangeId ret;
    {
        GILCtrl gilCtrl;
        // 检查stream对象是否为None
        if (stream == Py_None) {
            stream = nullptr;
        }
        ret = mstxRangeStartA(message, stream);
    }
    return Py_BuildValue("I", ret);
}

PyObject *WrapMstxRangeEnd(PyObject *self, PyObject *args, PyObject *kwds)
{
    mstxRangeId rangeId = 0;
    static char arg1[] = "rangeId";
    static char *kwlist[] = {arg1, nullptr};
    PyArg_ParseTupleAndKeywords(args, kwds, "I", kwlist, &rangeId);
    {
        GILCtrl gilCtrl;
        mstxRangeEnd(rangeId);
    }
    Py_RETURN_NONE;
}

PyObject *WrapMstxGetToolId(PyObject *self, PyObject *args, PyObject *kwds)
{
    uint64_t id = 0;
    {
        GILCtrl gilCtrl;
        mstxGetToolId(&id);
    }
    return Py_BuildValue("K", id);
}