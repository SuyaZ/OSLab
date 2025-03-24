# OSLab
操作系统内核实践项目

## 项目环境搭建

本项目使用`WSL(Windows SubSystem for Linux)`。避免了虚拟机使用不了显卡的问题，避免了双系统切换麻烦的问题。

1. 安装`WSL`，参考[微软官方文档](https://docs.microsoft.com/zh-cn/windows/wsl/install)。
2. 视频教学参考[bilibili](https://www.bilibili.com/video/BV1Du4y1q7YJ/?spm_id_from=333.1007.0.0&vd_source=613a53a7f837964b5f35a97b7d255ad4)。

## WSL安装以及导出

1. [如何查看WSL默认安装位置以及挪动其到指定安装路径](https://blog.csdn.net/tassadar/article/details/142407262)
2. [如果wsl命令行安装非常缓慢，可以试试这个方法](https://blog.csdn.net/weixin_44301630/article/details/122390018)

## 如果安装linux内核更新包失败
我也不知道为什么，重启了很多次就是装不了内核更新包。

## 设置用户名和密码
ubuntu
123456

## ubuntu导入到d盘
```bash
PS C:\Users\JialinZhong> wsl -l- v
命令行参数“-l-”无效
请使用 'wsl.exe --help' 获取受支持的参数列表。
PS C:\Users\JialinZhong> wsl -l -v
  NAME            STATE           VERSION
* Ubuntu          Stopped         2
  Ubuntu-20.04    Running         2
PS C:\Users\JialinZhong> wsl --export Ubuntu Ubuntu.tar
正在导出，这可能需要几分钟时间。 (1106 MB)

操作成功完成。
PS C:\Users\JialinZhong> wsl --export Ubuntu-20.04 d:\wsl\wsl-ubuntu20.04.tar
正在导出，这可能需要几分钟时间。 (901 MB): ./var/snap/lxd/common/lxd/unix.socket: pax format cannot archiv (1253 MB)

操作成功完成。
PS C:\Users\JialinZhong> wsl --unregister Ubuntu
正在注销。
操作成功完成。

PS C:\Users\JialinZhong> wsl -l -v
  NAME            STATE           VERSION
* Ubuntu-20.04    Stopped         2
PS C:\Users\JialinZhong> wsl --unregister Ubuntu-20.04
正在注销。
操作成功完成。

PS C:\Users\JialinZhong> wsl --import Ubuntu-20.04 d:\wsl\wsl-ubuntu20.04 d:\wsl\wsl-ubuntu20.04.tar --version 2
操作成功完成。
PS C:\Users\JialinZhong> ubuntu2004 config --default-user Username
id: ‘Username’: no such user
Error: 0x80070057 ?????

PS C:\Users\JialinZhong> ubuntu2004 config --default-user ubuntu
PS C:\Users\JialinZhong> del d:\wsl\wsl-ubuntu20.04.tar
PS C:\Users\JialinZhong>
```

启用和关闭虚拟机：
```bash
PS C:\Users\JialinZhong> wsl -l -v
  NAME            STATE           VERSION
* Ubuntu-20.04    Stopped         2
PS C:\Users\JialinZhong> wsl -d Ubuntu-20.04
To run a command as administrator (user "root"), use "sudo <command>".
See "man sudo_root" for details.

ubuntu@LAPTOP-BLECCI6K:/mnt/c/Users/JialinZhong$ exit
logout
PS C:\Users\JialinZhong> wsl -l -v
  NAME            STATE           VERSION
* Ubuntu-20.04    Running         2
PS C:\Users\JialinZhong> wsl --terminate Ubuntu-20.04
操作成功完成。
```

https://www.cnblogs.com/gatsby123/p/9746193.html


## 启用hyper-v

安装编译链
```bash
sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
sudo apt-get remove qemu-system-misc
sudo apt-get install qemu-system-misc=1:4.2-3ubuntu6
```

在虚拟机中克隆仓库
```bash
git clone git://g.csail.mit.edu/xv6-labs-2020
```

仓库是空的也没关系，切换到不同的分支，对应每一章的内容。

由于是克隆的MIT的实验库，所以需要修改提交的仓库。


