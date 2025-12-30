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

import unittest
import mstx


class TestMstxWithoutInit(unittest.TestCase):
    @classmethod
    def tearDownClass(cls) -> None:
        pass

    @classmethod
    def setUpClass(cls) -> None:
        pass

    def setUp(self) -> None:
        pass

    def tearDown(self) -> None:
        pass

    def test_range_start_with_message(self):
        ret = mstx.range_start("test01")
        self.assertEqual(ret, 0)
        ret = mstx.range_start(message="test01")
        self.assertEqual(ret, 0)

    def test_range_start_with_stream(self):
        ret = mstx.range_start("test01", None)
        self.assertEqual(ret, 0)

        ret = mstx.range_start("test01", 1)
        self.assertEqual(ret, 0)

        ret = mstx.range_start("test01", stream=None)
        self.assertEqual(ret, 0)

        ret = mstx.range_start("test01", stream=1)
        self.assertEqual(ret, 0)

        ret = mstx.range_start(message="test01", stream=None)
        self.assertEqual(ret, 0)

        ret = mstx.range_start(message="test01", stream=1)
        self.assertEqual(ret, 0)

    def test_mark_with_message(self):
        ret = mstx.mark("test01")
        self.assertEqual(ret, None)

        ret = mstx.mark(message="test01")
        self.assertEqual(ret, None)

    def test_mark_with_stream(self):
        ret = mstx.mark("test01", None)
        self.assertEqual(ret, None)

        ret = mstx.mark("test01", 1)
        self.assertEqual(ret, None)

        ret = mstx.mark("test01", stream=None)
        self.assertEqual(ret, None)

        ret = mstx.mark("test01", stream=1)
        self.assertEqual(ret, None)

        ret = mstx.mark(message="test01", stream=None)
        self.assertEqual(ret, None)

        ret = mstx.mark(message="test01", stream=1)
        self.assertEqual(ret, None)

    def test_range_end(self):
        ret = mstx.range_end(1)
        self.assertEqual(ret, None)

        ret = mstx.range_end(rangeId=1)
        self.assertEqual(ret, None)

    def test_get_tool_id(self):
        id = mstx.get_tool_id()
        self.assertEqual(id, 0)

