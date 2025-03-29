## 实验目的

熟悉系统调用的流程，然后为xv6增加新的系统调用。

在操作系统中，系统调用是用户程序与内核交互的关键机制。
用户程序无法直接操作硬件，也不能直接访问内核提供的资源。
这时候，系统充当桥梁，允许用户程序通过受控的接口向内核请求服务。

添加新的系统调用涉及到的文件如下（仅包括添加系统调用、不包括系统调用的实现）：
```cpp
user/user.h:    用户程序调用跳板函数

user/usys.pl:   跳板函数使用CPU提供的ecall指令，切换到内核态。

kernel/syscall.c:   内核态下调用系统封调用处理函数syscall()，所有系统调用都会跳到这里来处理

kernel/syscall.h:   设定系统调用号，syscall()根据跳板传进来的系统调用编号，查询syscalls[]表，找到对应的内核函数并调用。
```


## System call tracing

实现trace功能，跟踪用户程序中的系统调用，并将调用的相关信息打印出来。用户态的trace函数是已经写好了的（`user/trace.c`），但是实际的trace系统调用还没有实现。

比如输入一段命令行：
```bash
$ trace 32 grep hello README
```
就会打印跟踪信息：
```bash
3: syscall read -> 1023
3: syscall read -> 966
3: syscall read -> 70
3: syscall read -> 0
```
`(32)D == (100000)B`
`trace 32` 表示启用了跟踪功能，`32`表示调用号掩码为`5`的系统调用`(1<<5 == 32)` 。在`kernel/syscall.h`中可以看到`#define SYS_read    5`，所以这里表示要跟踪`read`的系统调用。

然后执行`grep hello README`命令。
即，用`grep`搜索`README`文件中的`hello`。


再举一个例子：
```bash
$ trace 2147483647 grep hello README
4: syscall trace -> 0
4: syscall exec -> 3  //运行grep的进程id
4: syscall open -> 3
4: syscall read -> 1023
4: syscall read -> 966
4: syscall read -> 70
4: syscall read -> 0
4: syscall close -> 0
```
在这个例子中，`2147483647` 的二进制中所有低 `31` 位都为 `1`，表示跟踪`所有`系统调用。因此在输出中可以看到，执行 `grep hello README` 命令涉及到的所有系统调用都会被打印出来。

关于操作系统掩码，32位掩码表示可以同时跟踪32个系统调用。
在二进制中，掩码为`i`的表示（1<<i）,表示第`i`为的状态是1。
第`i`位状态为`1`表示跟踪系统调用编号为`i`的系统调用。
虽然掩码的长度限制了可以同时跟踪的系统调用的个数，但并不限制系统调用的总数。
如果系统调用数量超过掩码的位数，可以通过扩展掩码位数或分组掩码来解决。

---------------------------------------------------------------

现在开始修改代码来实现这个功能。

### 按照系统调用的执行顺序，先从用户空间开始修改代码

首先在用户空间的头文件`user/user.h`加入函数声明，让用户态程序可以找到这个跳板入口函数。

```cpp
// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
......
int sleep(int);
int uptime(void);
int trace(int);  //这里添加，这时候函数知识空有一个声明还找不到定义
```

在`user/usys.pl`中加入用户态到内核态的跳板函数。这是一个Perl脚本，用于生成名为`usys.S`的汇编文件。`usys.S`文件的作用是为系统调用`(syscalls)`生成存根代码(stubs)，这些存根代码用于在用户态程序中调用内核提供的系统调用。

在文末添加系统调用：
```cpp
entry("trace");
```

[代码解释]
该脚本通过`entry`函数，为每个系统调用生成一个汇编函数。
生成的汇编函数会加载`系统调用编号`到寄存器`a7`。
然后通过`ecall`指令触发系统调用。
```perl
sub entry {
    my $name = shift;
    print ".global $name\n";
    print "${name}:\n";
    print " li a7, SYS_${name}\n";
    print " ecall\n";
    print " ret\n";
}
```

因为`user/trace.c`实现的是一个用户空间的函数(性质和上一个实验实现的sleep、pingpong一样)，需要在`Makefile`中添加：
```makefile
UPROGS=\
	$U/_cat\
	......
	$U/_zombie\
	$U/_trace\
```

以上是用户态的修改，接下来是内核态：

首先在`kernel/syscall.h`中添加系统调用号(掩码):
```cpp
#define SYS_trace  22
```

在`kernel/syscall.c`中全局声明`trace`系统调用处理函数，并且把系统调用号与处理函数关联。
```cpp
extern uint64 sys_trace(void);   //全局声明系统调用处理函数
//extern关键字的主要作用是允许在多个源文件之间共享变量或函数
//extern告诉编译器这个变量在其他文件中被定义

[SYS_trace]   sys_trace;  //将系统调用处理函数和系统调用编号关联起来
```

在`kernel/proc.h`中，在进程类结构体中，添加syscall_trace属性，用掩码的方式记录要跟踪的系统调用：
```cpp
struct proc {
  ......

  // these are private to the process, so p->lock need not be held.
  ......
  uint64 syscall_trace;        //存储进程的系统调用掩码，用于记录哪些进程需要被跟踪
};
```

在``kernel/proc.c`中：
在结构体中声明了跟踪标志`syscall_trace`，那么就要在使用的时候初始化。
```cpp
static struct proc*
allocproc(void)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->state == UNUSED) {
      goto found;
    } else {
      release(&p->lock);
    }
  }
  return 0;

found:
  p->pid = allocpid();

  // Allocate a trapframe page.
  if((p->trapframe = (struct trapframe *)kalloc()) == 0){
    release(&p->lock);
    return 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  if(p->pagetable == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  p->syscall_trace = 0;  //创建新进程的时候，记录标志默认设置为0

  return p;
}
```
[代码解释]
这个函数 allocproc 是一个用于分配进程（process）的函数，通常在操作系统中用于创建新进程。它从一个进程数组中找到一个未使用的进程结构，并为其分配必要的资源。

遍历进程数组：函数首先遍历一个名为`proc`的数组，该数组包含多个`struct proc`类型的元素，每个元素代表一个可能的进程。

尝试获取该结构的锁：如果进程结构的状态是未使用，则找到一个可以分配的进程结构，并跳转到`found`标签处。如果该进程结构已经被使用，则释放锁并继续查找。

分配资源：分配进程ID：调用 allocpid 函数为新进程分配一个唯一的进程ID。
分配陷阱帧页面：使用 kalloc 函数为新进程分配一个陷阱帧（trapframe）页面。陷阱帧用于保存进程在用户态执行时的寄存器状态。
设置用户页表：调用 proc_pagetable 函数为新进程设置一个空的用户页表。
初始化上下文：设置新进程的上下文，包括返回地址（ra）和栈指针（sp），以便新进程在调度时能够正确地开始执行。
创建新进程的时候，记录标志默认设置为0。

在``kernel/proc.c`中的`fork`函数中子进程也能击沉父进程的跟踪状态。
我觉得，这里再释放锁之前设置标志位更好，这样可以确保子线程被其他线程访问之前，子进程的状态已经正确修改。
```cpp
int
fork(void)
{
  .......

  np -> syscall_trace = p -> syscall_trace;  //子进程继承父进程的标志位

  release(&np->lock);

  //np -> syscall_trace = p -> syscall_trace;  //子进程继承父进程的标志位

  return pid;
}
```

因为是在一个进程中跟踪系统调用，所以在`kernel/sysproc.c`中实现这个系统调用。
其实定义的位置并不重要，重要的是已经在引用`sys_trace`系统调用的文件中声明了，编译器会自动去外部寻找相关的函数。
```cpp
uint64
sys_trace(void)
{
    int mask;

    //获取用户程序传入的数据，如果argint()<0, 说明获取参数失败
    if(argint(0, &mask) < 0) return -1;

    //设置系统调用掩码为mask
    myproc()->syscall_trace = mask;

    return 0;
}
```
[代码解释]
`myproc()`返回当前cpu进程的指针。


在`kernel/syscall.c`中，为了方便打印，建立一个字符串数组映射，快速获取系统调用号对应的系统调用函数名。
```cpp
// kernel/syscall.c
// 定义系统调用名称的字符串数组
const char* syscall_names[] = {
    [SYS_fork]    "fork",
    [SYS_exit]    "exit",
    [SYS_wait]    "wait",
    [SYS_pipe]    "pipe",
    [SYS_read]    "read",
    [SYS_kill]    "kill",
    [SYS_exec]    "exec",
    [SYS_fstat]   "fstat",
    [SYS_chdir]   "chdir",
    [SYS_dup]     "dup",
    [SYS_getpid]  "getpid",
    [SYS_sbrk]    "sbrk",
    [SYS_sleep]   "sleep",
    [SYS_uptime]  "uptime",
    [SYS_open]    "open",
    [SYS_write]   "write",
    [SYS_mknod]   "mknod",
    [SYS_unlink]  "unlink",
    [SYS_link]    "link",
    [SYS_mkdir]   "mkdir",
    [SYS_close]   "close",
    [SYS_trace]   "trace",
};
```
然后在同一个文件下为`syscall`函数设置打印信息。
```cpp
void
syscall(void)
{
  int num;
  struct proc *p = myproc();

  //获取系统调用编号
  num = p->trapframe->a7;
  //如果系统调用编号有效（大于0且小于syscalls数组的长度，并且对应的处理函数存在）
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) 
  {
    //调用对应的处理函数，并且返回值存储在a0寄存器中
    p->trapframe->a0 = syscalls[num]();

    //如果当前进程用了trace跟踪，则按照题设来打印信息
    if((p->syscall_trace >> num) & 1)
    {
      printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num], p->trapframe->a0);
    }
  } 
  else 
  {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}
```
调用结果：
```shell
init: starting sh
$ trace 32 grep hello README
3: syscall read -> 1023
3: syscall read -> 966
3: syscall read -> 70
3: syscall read -> 0
$ trace 2147483647 grep hello README
4: syscall trace -> 0
4: syscall exec -> 3
4: syscall open -> 3
4: syscall read -> 1023
4: syscall read -> 966
4: syscall read -> 70
4: syscall read -> 0
4: syscall close -> 0
```
`syscall read`表示发生了read的系统调用，`syscall exec`等同理，`->`后面是系统调用的返回值，`->1023`表示读取了1023个字节；`->0`表示来到文件末尾。
程序使用了一个1024字节的缓冲区（常见配置）
第一次读取时获得1023字节（可能因对齐或文件系统块大小少1字节）
第二次读取966字节（此时文件剩余2059-1023=1036字节，但实际读取966说明可能中间存在换行符截断）
最后读取剩余70字节
最终EOF返回0


## Sysinfo

该实验添加一个系统调用，`Sysinfo`，用力啊获取空闲的内存，已创建的进程数量。
在`kernel/sysinfo.h`中已经声明好了`freemen`空闲存储属性和`nproc`进程数量属性。
需要把获取的数据赋值在其中。

在用户空间相关文件中添加代码：
```cpp
//user/user.h
struct sysinfo;        //声明sysinfo结构体，使用户程序可以使用这个结构体
int sysinfo(struct sysinfo*);  //用户程序可以找到sysinfo系统调用的跳板入口函数

//user/usys.pl
entry("sysinfo");

//Makefile
UPROGS=\
	...
	$U/_sysinfo\
```

在内核态添加`sysinfo`系统调用编号：
```cpp
//kernel/syscall.h
#define SYS_sysinfo 23
```

添加系统调用映射：
```cpp
//kernel/syscall.c
extern uint64 sys_sysinfo(void);

static uint64 (*syscalls[])(void) ={
    ......
    [SYS_sysinfo] sys_sysinfo,
}

const char* syscall_names[] ={
    ......
    [SYS_sysinfo] "sysinfo",
}
```

首先实现获取空闲内存的`freebytes`函数。因为和内存相关，所以在`kernel.kalloc.c`中实现。
`xv6`采用了一种简单的空闲链表机制来记录空闲的物理内存页，空闲页自身作为链表节点，指向下一个空闲页。
```cpp
void
freebytes(uint64* dst)
{
  *dst = 0;
  struct run* p = kmem.freelist;  //指向空闲物理内存页

  acquire(&kmem.lock); //枷锁保证线程安全
  while(p) //有空闲页
  {
    *dst += PGSIZE;
    p = p->next;
  }
  release(&kmem.lock);
}
```

实现统计进程数量的函数`procnum`。该函数在进程相关的`kernel/proc.c`中实现，在这个文件中有一个进程表`struct proc proc[NPROC]`,记录了所有进程，而每一个进程都有一个state属性，表示该进程是否在使用。因此可以得出思路：遍历`proc`进程表，判断当前进程是否在使用，统计数量。
代码：
```cpp
//kernel/proc.c
//统计处于活动状态的进程
void
procnum(uint64* dst)
{
    *dst = 0;
    struct proc* p;
    for(p = proc; p < &proc[NPROC]; p++)
    {
      if(p->state != UNUSED)
        (*dst)++;
    }
}
```


接下来实现`sysinfo`函数，这里先宏观的实现sysinfo的功能，然后再分解实现获取空闲内存和获取进程数的功能。
这里同样在`kernel/sysproc.c`中实现：
```cpp
#include "sysinfo.h"

uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  freebytes(&info.freemem); //获取空闲内存
  procnum(&info.nproc);  //获取进程数量

  //获取用户虚拟地址
  uint64 dstaddr;
  argaddr(0, &dstaddr);

  //从内核空间拷贝数据到用户空间
  if(copyout(myproc()->pagetable, dstaddr, (char*)&info, sizeof info) < 0)
  {
    return -1;
  }

  return 0;
}
```
在 `user/sysinfotest.c `中可以看到，用户程序调用 `sysinfo` 系统调用的时候，传入的参数是 `sysinfo` 结构体。因此在上面的代码中，获取到用户空间的 `sysinfo` 结构体地址后，要把内核空间的 `sysinfo` 结构体的数据复制过去，这样用户程序才能拿到数据。

因为`procnum`和`freebytes`不像系统调用那样用了`extern`全局声明，`kernel/sysproc.c` 中的 `sys_sysinfo` 是找不到这两个函数的。这里要在内核头文件 `kernel/defs.h` 声明这两个函数。
```cpp
// kalloc.c
...
void            freebytes(uint64* dst);

// proc.c
...
void            procnum(uint64* dst);
```


测试
```shell
$ sysinfotest
sysinfotest: start
sysinfotest: OK
```

