//Lab01-primes
//多线程多管道筛质数
//由于内核的原因，差不多筛选[2,35]以内的质数就能导致文件描述符耗尽，因此在实现的过程中要仔细关闭不需要的文件描述符

#include "kernel/types.h"
#include "user/user.h"

void sieve(int pleft[2]) //传递管道作为参数
{
    int p; //从左邻居进程第一个读取的数,一定是质数。反证法
    read(pleft[0], &p, sizeof(p));
    if(p == -1) exit(0);
    printf("prime:%d\n", p);

    int pright[2];
    pipe(pright);
    if(fork() == 0) //右邻居进程
    {
        close(pleft[0]);
        close(pright[1]);
        sieve(pright);
        exit(0);
    }
    else
    {
        close(pright[0]);
        int buf;
        while(read(pleft[0], &buf, sizeof(buf)) && buf != -1)
        {
            if(buf % p != 0)
            {
                write(pright[1], &buf, sizeof(buf));
            }
        }
        buf = -1;
        write(pright[1], &buf, sizeof(buf));
        wait(0);
        exit(0);
    }
}

int main(int argc, char** argv)
{
    //创建管道初始进程
    int input_pipe[2];
    pipe(input_pipe);

    if(fork() == 0) //子进程，右邻居
    {
        close(input_pipe[1]);
        sieve(input_pipe);
        exit(0);
    }
    else
    {
        close(input_pipe[0]);

        int i;
        for(i = 2; i <= 35; i++)
        {
            write(input_pipe[1], &i, sizeof(i));
        }
        i = -1; //写入结束标志
        write(input_pipe[1], &i, sizeof(i));
    }

    wait(0); //等待main的子进程结束，注意这里无法等待子进程的子进程
    //只有调用sieve之后才能形成完整的等待链。
    exit(0);
}