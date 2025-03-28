# OSLab
操作系统内核实践项目

## 项目环境搭建

本项目使用`WSL(Windows SubSystem for Linux)`。避免了虚拟机使用不了显卡的问题，避免了双系统切换麻烦的问题。

1. 安装`WSL`，参考[微软官方文档](https://docs.microsoft.com/zh-cn/windows/wsl/install)。
2. 视频教学参考[bilibili](https://www.bilibili.com/video/BV1Du4y1q7YJ/?spm_id_from=333.1007.0.0&vd_source=613a53a7f837964b5f35a97b7d255ad4)。

## WSL安装以及导出

1. [如何查看WSL默认安装位置以及挪动其到指定安装路径](https://blog.csdn.net/tassadar/article/details/142407262)
2. [如果wsl命令行安装非常缓慢，可以试试这个方法](https://blog.csdn.net/weixin_44301630/article/details/122390018)

## 项目架构

main分支是项目主分支，里面什么都没有。
每一次实验都有一个小分支。每一个实验都会学习对应的内核源码然后完善这个xv6操作系统。

比如第一次实验主要是熟悉系统调用，因此切换到`util`分支，然后在`user/`下面写`.c`文件。

相关笔记存放在`~/note/`下面，如果有图片的话，存在`~/note/img/`下。