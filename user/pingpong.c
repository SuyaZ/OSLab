// //Lab01-pingpong


//#include "kernel/stat.h" //结构体stat描述文件的状态信息
#include "kernel/types.h" //基本类型重命名
#include "user/user.h"

int main(int arg, char** argv)
{
    int pp2c[2], pc2p[2]; //pp2c:process parent to child
    pipe(pp2c); //创建管道并初始化p[0](读端，从管道读数据),p[1](写端，向管道写数据)
    pipe(pc2p);

    if(fork() != 0) //父进程
    {
        //父进程向子进程发送数据
        write(pp2c[1], ".", 1);
        close(pp2c[1]); //及时关闭写端

        //子进程向父进程发数据
        char buff;
        read(pc2p[0], &buff, 1);
        printf("%d: received pong\n", getpid());
        wait(0);//等待子进程结束，不关心子进程的状态
    }
    else
    {
        char buff;
        read(pp2c[0], &buff, 1);
        printf("%d: received ping\n", getpid());
        write(pc2p[1], &buff, 1);
        close(pc2p[1]);
    }
    //关闭管道读端
    close(pp2c[0]);
    close(pc2p[0]);

    exit(0); //安全退出
}