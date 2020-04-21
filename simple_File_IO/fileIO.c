/*===============================
    新增編譯和執行步驟 :
    gcc fileIO.c -o test
    ./test
===============================
# 此範例可以看到 : 「習」

fd(file descriptor)、open()、close()、read()、write() 系統呼叫(system call)

# open() 系統呼叫
```
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *name, int flags);
int open(const char *name, int flags, mode_t mode);
```

===============================
# 螢幕輸出結果

cc@simple_File_IO$gcc fileIO.c -o test
cc@simple_File_IO$./test
Walter Programmer!
STDIN_FILENO :0, STDOUT_FILENO :1, STDERR_FILENO :2
Walter haha
Walter error

===============================
# 參考文件 :
  
  - Linux 系統程式設計, 2/e(內容涵蓋 Linux Kernel 3.0)，Robert Love 著、蔣大偉 譯
    Linux System Programming: Talking Directly to the Kernel and C Library)
  - Linux 文件描述符简介(file descriptor)
    https://blog.csdn.net/Artprog/article/details/60601253
  - Linux 系統程式設計 - fd 及 open()、close() 系統呼叫
    https://blog.jaycetyle.com/2018/12/linux-fd-open-close/

*/

#include <stdio.h>      // printf()
#include <string.h>     // strlen()
#include <unistd.h>     // read(), write()
#include <sys/types.h>  // open()
#include <sys/stat.h>   // open()
#include <fcntl.h>      // open()

int main(void) {
    int fd, size;
    char s[] = "Walter Programmer!\n", buffer[30];
    
    // open後，該路徑底下會多一個"temp"檔案
    fd = open("/home/cc/githome/Linux_Device_Driver/simple_File_IO/temp", O_WRONLY|O_CREAT);
    // 將s的字串寫入"temp"檔案
    write(fd, s, sizeof(s));
    close(fd);
    
    fd = open("/home/cc/githome/Linux_Device_Driver/simple_File_IO/temp", O_RDONLY);
    // 從fd( = "temp"檔案)讀取data並放入"buffer"裡面
    size = read(fd, buffer, sizeof(buffer));
    close(fd);
    printf("%s", buffer);
/*
    printf("STDIN_FILENO :%d, STDOUT_FILENO :%d, STDERR_FILENO :%d\n", STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
    // STDIN_FILENO :0, STDOUT_FILENO :1, STDERR_FILENO :2

    write(1, "Walter haha\n", 13);
    write(2, "Walter error\n", 14);
*/  

    return 0;
}





