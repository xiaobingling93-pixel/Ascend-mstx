# MindStudio Tools Extension Library安装指南

## 1. 项目依赖

由于本项目中的代码依赖python3的头文件，因此需要编译环境中安装python3-dev包，可通过如下命令执行：

```sh
apt-get install python3-dev
```

## 2. 构建打包

```sh
python build.py
```

## 3. 安装whl包

```sh
cd output
pip3 install mstx-xxxxx.whl
```

## 4. 升级

如需使用whl包替换运行环境原有已安装的whl包，执行如下安装操作：

```sh
pip3 install mstx-xxxxx.whl --force-reinstall
```

安装过程中，若提示是否替换原有安装包：
输入"y"，则安装包会自动完成升级操作。

## 5. 卸载

卸载则通过如下命令卸载：

```sh
pip3 uninstall mstx-xxxxx.whl 
```
