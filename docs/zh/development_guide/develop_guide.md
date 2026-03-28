# MindStudio Tools Extension Library 开发指南

<br>

## 1. 开发环境准备

请按照以下文档进行环境配置：[《算子工具开发环境安装指导》](https://gitcode.com/Ascend/msot/blob/master/docs/zh/common/dev_env_setup.md)。

## 2. 编译打包

```shell
python build.py
```

## 3. 执行UT测试

可以通过一键式脚本来执行C/CPP/PYTHON代码的UT测试：

```sh
python build.py test
```

输出类似如下跑的用例数和通过用例数相同即表示成功：

```text
[----------] 4 tests from CoreApi (8ms total) 
```

```text
============= 6 passed in 0.03s =============
```

注：需要环境中提前安装pytest以执行python测试
