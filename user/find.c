// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/fs.h"

// //user.h必须要在"kernel/types.h"后面引入

// // 递归查找函数，查找路径为 path 的目录下是否有目标文件 target
// void find(char* path, char* target) {
//     char buf[512], *p;
//     int fd;
//     struct dirent de;
//     struct stat st;
//     // 打开目录
//     if ((fd = open(path, 0)) < 0) {
//         fprintf(2, "find: cannot open %s\n", path);
//         return;
//     }
//     // 获取目录的状态信息
//     if (fstat(fd, &st) < 0) {
//         fprintf(2, "find: cannot stat %s\n", path);
//         close(fd);
//         return;
//     }
//     // 文件、目录分别处理
//     switch (st.type) {
//         // 如果是文件，检查文件名是否与目标文件名匹配
//         case T_FILE:
//             if (strcmp(path + strlen(path) - strlen(target), target) == 0) {
//                 printf("%s\n", path);
//             }
//             break;
//         // 如果是目录
//         case T_DIR:
//             // 检查路径长度是否超出缓冲区大小
//             if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
//                 printf("find: path too long\n");
//                 break;
//             }
//             strcpy(buf, path);
//             p = buf + strlen(buf);
//             *p++ = '/';
//             *p = 0;
//             // 读取目录项
//             while (read(fd, &de, sizeof(de)) == sizeof(de)) {
//                 if (de.inum == 0)
//                     continue;
//                 memmove(p, de.name, DIRSIZ);
//                 p[DIRSIZ] = 0;
//                 // 获取目录项的状态信息
//                 if (stat(buf, &st) < 0) {
//                     printf("find: cannot stat %s\n", buf);
//                     continue;
//                 }
//                 // 排除 "." 和 ".." 目录
//                 if (strcmp(buf + strlen(buf) - 2, "/.") != 0 && strcmp(buf + strlen(buf) - 3, "/..") != 0)
//                     find(buf, target); // 递归查找子目录
//             }
//             break;
//     }
//     close(fd); // 关闭目录
// }

// //argc表示参数的个数：argc至少为1，应为第一个参数总是程序名
// //argv[0] 总是程序名；[1]是命令的第一个参数；[2]是命令的第二个参数
// //argv指向字符指针类型的数组首地址;
// int main(int argc, char* argv[])
// {
//     if(argc < 3) {exit(0);}

//     char target[512];
//     target[0] = '/';  //为查找文件名在开头添加‘/’
//     strcpy(target+1, argv[1]);
//     find(argv[1], target);

//     exit(0);
// }

#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *path, const char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot fstat %s\n", path);
    close(fd);
    return;
  }

  //参数错误，find的第一个参数必须是目录
  if (st.type != T_DIR) {
    fprintf(2, "usage: find <DIRECTORY> <filename>\n");
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
    fprintf(2, "find: path too long\n");
    return;
  }
  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/'; //p指向最后一个'/'之后
  while (read(fd, &de, sizeof de) == sizeof de) {
    if (de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ); //添加路径名称
    p[DIRSIZ] = 0;               //字符串结束标志
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    //不要在“.”和“..”目录中递归
    if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
      find(buf, filename);
    } else if (strcmp(filename, p) == 0)
      printf("%s\n", buf);
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "usage: find <directory> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
