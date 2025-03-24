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

## 解决Ubuntu网络连接错误Failed to connect to github.com port 443: Connection refused

[Ubuntu开启ssh](https://blog.csdn.net/liubang00001/article/details/141334472?ops_request_misc=%257B%2522request%255Fid%2522%253A%252201607453a840905ad4732d8a46dddf98%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=01607453a840905ad4732d8a46dddf98&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v31_ecpm-2-141334472-null-null.142^v102^pc_search_result_base6&utm_term=Ubuntu%20%EF%BC%9A%20Failed%20to%20connect%20to%20github.com%20port%20443%3A%20Connection%20refused&spm=1018.2226.3001.4187)


输入
```bash
sudo nano /etc/hosts
```

添加github.com的ip地址，然后保存退出。
ctrl+o保存 enter确定文件名 ctrl+x退出
```bash
# This file was automatically generated by WSL. To stop automatic generation of this file, add the following entry to /># [network]
# generateHosts = false
127.0.0.1       localhost
127.0.1.1       LAPTOP-BLECCI6K.        LAPTOP-BLECCI6K
140.82.113.3    github.com  # 加在这里

# The following lines are desirable for IPv6 capable hosts
::1     ip6-localhost ip6-loopback
fe00::0 ip6-localnet
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters
```

调试记录：
```bash
ubuntu@LAPTOP-BLECCI6K:~$ git clone https://github.com/DamionGans/ubuntu-wsl2-systemd-script.git
Cloning into 'ubuntu-wsl2-systemd-script'...
fatal: unable to access 'https://github.com/DamionGans/ubuntu-wsl2-systemd-script.git/': Failed to connect to github.com port 443: Connection refused
ubuntu@LAPTOP-BLECCI6K:~$ sudo apt update
[sudo] password for ubuntu:
Get:1 http://security.ubuntu.com/ubuntu focal-security InRelease [128 kB]
Hit:2 http://archive.ubuntu.com/ubuntu focal InRelease
Get:3 http://archive.ubuntu.com/ubuntu focal-updates InRelease [128 kB]
Get:4 http://archive.ubuntu.com/ubuntu focal-backports InRelease [128 kB]
Fetched 383 kB in 4s (99.6 kB/s)
Reading package lists... Done
Building dependency tree
Reading state information... Done
5 packages can be upgraded. Run 'apt list --upgradable' to see them.
ubuntu@LAPTOP-BLECCI6K:~$ sudo apt install openssh-server
Reading package lists... Done
Building dependency tree
Reading state information... Done
openssh-server is already the newest version (1:8.2p1-4ubuntu0.12).
The following packages were automatically installed and are no longer required:
  adwaita-icon-theme at-spi2-core gtk-update-icon-cache hicolor-icon-theme humanity-icon-theme libatk-bridge2.0-0
  libatk1.0-0 libatk1.0-data libatspi2.0-0 libavahi-client3 libavahi-common-data libavahi-common3 libcolord2 libcups2
  libgtk-3-0 libgtk-3-bin libgtk-3-common liblcms2-2 librest-0.7-0 librsvg2-2 librsvg2-common libsoup-gnome2.4-1
  libvte-2.91-0 libvte-2.91-common libwayland-cursor0 libwayland-egl1 libxcursor1 libxkbcommon0 qemu-system-gui
  ubuntu-mono
Use 'sudo apt autoremove' to remove them.
0 upgraded, 0 newly installed, 0 to remove and 5 not upgraded.
ubuntu@LAPTOP-BLECCI6K:~$ sudo systemctl status ssh
● ssh.service - OpenBSD Secure Shell server
     Loaded: loaded (/lib/systemd/system/ssh.service; disabled; vendor preset: enabled)
     Active: inactive (dead)
       Docs: man:sshd(8)
             man:sshd_config(5)
ubuntu@LAPTOP-BLECCI6K:~$  git clone https://github.com/DamionGans/ubuntu-wsl2-systemd-script.git
Cloning into 'ubuntu-wsl2-systemd-script'...
fatal: unable to access 'https://github.com/DamionGans/ubuntu-wsl2-systemd-script.git/': Failed to connect to github.com port 443: Connection refused
ubuntu@LAPTOP-BLECCI6K:~$ sudo systemctl start ssh
ubuntu@LAPTOP-BLECCI6K:~$ sudo systemctl status ssh
● ssh.service - OpenBSD Secure Shell server
     Loaded: loaded (/lib/systemd/system/ssh.service; disabled; vendor preset: enabled)
     Active: active (running) since Mon 2025-03-24 20:30:23 CST; 5s ago
       Docs: man:sshd(8)
             man:sshd_config(5)
    Process: 43875 ExecStartPre=/usr/sbin/sshd -t (code=exited, status=0/SUCCESS)
   Main PID: 43876 (sshd)
      Tasks: 1 (limit: 9044)
     Memory: 2.1M
     CGroup: /system.slice/ssh.service
             └─43876 sshd: /usr/sbin/sshd -D [listener] 0 of 10-100 startups

Mar 24 20:30:23 LAPTOP-BLECCI6K systemd[1]: Starting OpenBSD Secure Shell server...
Mar 24 20:30:23 LAPTOP-BLECCI6K sshd[43876]: Server listening on 0.0.0.0 port 22.
Mar 24 20:30:23 LAPTOP-BLECCI6K sshd[43876]: Server listening on :: port 22.
Mar 24 20:30:23 LAPTOP-BLECCI6K systemd[1]: Started OpenBSD Secure Shell server.
ubuntu@LAPTOP-BLECCI6K:~$ git clone https://github.com/DamionGans/ubuntu-wsl2-systemd-script.git
Cloning into 'ubuntu-wsl2-systemd-script'...
fatal: unable to access 'https://github.com/DamionGans/ubuntu-wsl2-systemd-script.git/': Failed to connect to github.com port 443: Connection refused
ubuntu@LAPTOP-BLECCI6K:~$ ping github.com
PING github.com (127.0.0.1) 56(84) bytes of data.
64 bytes from localhost (127.0.0.1): icmp_seq=1 ttl=64 time=0.104 ms
64 bytes from localhost (127.0.0.1): icmp_seq=2 ttl=64 time=0.037 ms
64 bytes from localhost (127.0.0.1): icmp_seq=3 ttl=64 time=0.030 ms
64 bytes from localhost (127.0.0.1): icmp_seq=4 ttl=64 time=0.021 ms
^C
--- github.com ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3090ms
rtt min/avg/max/mdev = 0.021/0.048/0.104/0.032 ms
ubuntu@LAPTOP-BLECCI6K:~$ sudo vi /etc/hosts
ubuntu@LAPTOP-BLECCI6K:~$ ping github.com
PING github.com (127.0.0.1) 56(84) bytes of data.
64 bytes from localhost (127.0.0.1): icmp_seq=1 ttl=64 time=0.028 ms
64 bytes from localhost (127.0.0.1): icmp_seq=2 ttl=64 time=0.031 ms
64 bytes from localhost (127.0.0.1): icmp_seq=3 ttl=64 time=0.033 ms
^C
--- github.com ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2066ms
rtt min/avg/max/mdev = 0.028/0.030/0.033/0.002 ms
ubuntu@LAPTOP-BLECCI6K:~$ ^C
ubuntu@LAPTOP-BLECCI6K:~$ nslookup github.com
;; Got recursion not available from 10.255.255.254
Server:         10.255.255.254
Address:        10.255.255.254#53

Name:   github.com
Address: 127.0.0.1
;; Got recursion not available from 10.255.255.254

ubuntu@LAPTOP-BLECCI6K:~$ sudo nano /etc/hosts
ubuntu@LAPTOP-BLECCI6K:~$ sudo nano /etc/hosts
ubuntu@LAPTOP-BLECCI6K:~$ ping github.com
PING github.com (140.82.113.3) 56(84) bytes of data.
64 bytes from github.com (140.82.113.3): icmp_seq=1 ttl=45 time=274 ms
64 bytes from github.com (140.82.113.3): icmp_seq=2 ttl=45 time=295 ms
64 bytes from github.com (140.82.113.3): icmp_seq=3 ttl=45 time=317 ms
64 bytes from github.com (140.82.113.3): icmp_seq=4 ttl=45 time=340 ms
^C
--- github.com ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3003ms
rtt min/avg/max/mdev = 274.054/306.408/340.104/24.669 ms
ubuntu@LAPTOP-BLECCI6K:~$ ^C
ubuntu@LAPTOP-BLECCI6K:~$ git clone https://github.com/DamionGans/ubuntu-wsl2-systemd-script.git
Cloning into 'ubuntu-wsl2-systemd-script'...
remote: Enumerating objects: 121, done.
remote: Counting objects: 100% (39/39), done.
remote: Compressing objects: 100% (11/11), done.
remote: Total 121 (delta 33), reused 29 (delta 28), pack-reused 82 (from 1)
Receiving objects: 100% (121/121), 33.51 KiB | 147.00 KiB/s, done.
Resolving deltas: 100% (63/63), done.
ubuntu@LAPTOP-BLECCI6K:~$ sudo nano /etc/hosts
ubuntu@LAPTOP-BLECCI6K:~$
```

