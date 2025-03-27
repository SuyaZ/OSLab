//从标准输入读取参数，并将这些参数传递给指定的参数运行
//echo "file1 file2 file3" | xargs rm:
//相当于实现xargs将"file1 file2 file3"这些字符串参数传递给rm，相当于执行了：
//rm file1 file2 file3


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void run (char* program, char* *args)
{
    if(fork() == 0)
    {
        exec(program, args);
        exit(0);
    }
    return;
}

void main(int argc ,char* argv[])
{
    char buf[2048];
    char* p = buf, *last_p = buf;
    char* argsbuf[128]; //初始化一个数组，每一个数组元素都是字符指针，每一个字符指针都指向一个字符串的首地址
    char* *args = argsbuf;  //定义一个指针args指向字符指针数组首地址argsbuf，指针会在数组中移动，从而达到遍历数组的目的

    //首先将xargs的参数复制到argsbuf中
    for(int i = 1; i < argc; i++)
    {
        *args = argv[i]; //将argv[i]的值赋值给指针args指向的内存
        args++; //指针向后移动
    }

    //记录当前参数的位置
    char* *pa = args;
    // 从标准输入读取数据，存储在缓冲区 buf 中
        while (read(0, p, 1) != 0) 
        {
        // 使用指针 p 遍历缓冲区，遇到空格或换行符时，将其替换为字符串结束符 \0
        if (*p == ' ' || *p == '\n') 
        {
            *p = '\0';

            // 将参数添加到参数缓冲区 argsbuf 中
            *(pa++) = last_p;
            last_p = p + 1;

            // 每当遇到换行符 \n 时，表示一组参数读取完毕，调用 run 函数执行程序，传递参数
            if (*p == '\n') 
            {
                *pa = 0;
                run(argv[1], argsbuf);
                pa = args;
            }
        }
        // 继续读取数据
        p++;
    }

        // 如果最后一行不是空行，同样的逻辑再处理一次
    if (pa != args) 
    {
        *p = '\0';
        *(pa++) = last_p;
        *pa = 0;

        run(argv[1], argsbuf);
    }

    // 等待所有子进程结束
    while (wait(0) != -1) {};
    exit(0);

}