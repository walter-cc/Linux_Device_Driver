#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEMO_DEV_NAME "/dev/my_demo_dev"    // 手動生成「device node」: the name =  demo_drv in the command "sudo mknod /dev/demo_drv c 244 0"

/*
- argc : argument count(參數總和)的縮寫，代表包括指令本身的參數個數。
- argv :argument value 的縮寫。 一個陣列，它會分拆所帶的參數放到陣列內
*/
int main(int argc, char *argv[])
{
	int fd;     // file descriptor
	int ret;
	size_t len;
	char message[80] = "walter testing the virtual FIFO device";
	char *read_buffer;

	len = sizeof(message);

	read_buffer = malloc(2*len);
	memset(read_buffer, 0, 2*len);

	fd = open(DEMO_DEV_NAME, O_RDWR | O_NONBLOCK);  // open the device node "DEMO_DEV_NAME"
	if (fd < 0) {
		printf("[walter]open device %s failded\n", DEMO_DEV_NAME);
		return -1;
	}

	/*1: read it first*/
	ret = read(fd, read_buffer, sizeof(read_buffer));   // 從fd 讀取data並放入"read_buffer"裡面
	printf("[walter]read %d bytes\n", ret);
	printf("[walter]read buffer=%s\n", read_buffer);

/*
===============================
# 螢幕輸出結果
[walter]read -1 bytes
[walter]read buffer=
===============================
*/
	/*2. write the message to device*/
	ret = write(fd, message, len);
	if (ret != len)
		printf("[walter]have write %d bytes\n", ret);
    // 螢幕輸出結果 >  [walter]have write 64 bytes

	/*3. write again*/
	ret = write(fd, message, len);
	if (ret != len)
		printf("[walter]have write %d bytes\n", ret);
    // 螢幕輸出結果 > [walter]have write -1 bytes
    
	ret = read(fd, read_buffer, 2*len);
	printf("[walter]read %d bytes\n", ret);
	printf("[walter]read buffer=%s\n", read_buffer);
/*
===============================
# 螢幕輸出結果
[walter]read 64 bytes
[walter]read buffer=walter testing the virtual FIFO device
===============================
*/

	close(fd);

	return 0;
}


/*
===============================
printf("\n\033[1;35m[walter]+++++++++++++++++++++++++[%s][%d]\n\n\033[0m",__FUNCTION__,__LINE__);
printf("\n\033[1;35m[walter]-------------------------[%s][%d]\n\n\033[0m",__FUNCTION__,__LINE__);
===============================

int main(int argc, char *argv[]) {
    
    printf("arguments numbers : %d\n", argc);   // argc : 所帶參數的數目

    for (i = 0; i < argc; i++) {
        printf("[%d] %s\n", i, argv[i]);    // argv : 一個陣列，它會分拆所帶的參數放到陣列內
    }
    printf("\n");
    return 0;
}

# 螢幕輸出結果

cc@myfirst$gcc Temp_C_Code.c -o test
cc@myfirst$./test

arguments numbers : 1
[0] ./test



cc@myfirst$./test walter

arguments numbers : 2
[0] ./test
[1] walter



cc@myfirst$./test w a l t e r

arguments numbers : 7
[0] ./test
[1] w
[2] a
[3] l
[4] t
[5] e
[6] r



cc@myfirst$./test 1 2 3 4

arguments numbers : 5
[0] ./test
[1] 1
[2] 2
[3] 3
[4] 4

===============================
# 參考文件 :

C/C++ 程式設計教學：main 函數讀取命令列參數，argc 與 argv 用法
https://blog.gtwang.org/programming/c-cpp-tutorial-argc-argv-read-command-line-arguments/


MAIN(int argc, char *argv[ ]) 之用法
https://blog.xuite.net/tzeng015/twblog/113273095

===============================
*/
