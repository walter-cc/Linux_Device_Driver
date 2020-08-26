/*
# 此範例執行步驟

1. 新增hello.c

2. 新增Makefile

3. cc@kernel_module_sample$make

4. ls : 確認編好的檔案

5. 載入module 「hello」 : sudo insmod hello.ko ## 注意 : 是輸入檔案名稱(xxx.ko)

6. 查詢module 「hello」 : sudo lsmod | grep "hello"

7. 移除module 「hello」 : sudo rmmod hello ## 注意 : 是輸入模組名稱(xxx)

8. 查詢有無成功 : dmesg 指令來察看系統日誌

===============================
# 執行結果 : 

3) cc@kernel_module_sample$make
make -C /usr/src/linux-headers-4.13.0-39-generic/ M=/home/cc/githome/Linux_Device_Driver/kernel_module_sample  modules
make[1]: Entering directory '/usr/src/linux-headers-4.13.0-39-generic'
  CC [M]  /home/cc/githome/Linux_Device_Driver/kernel_module_sample/hello.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/cc/githome/Linux_Device_Driver/kernel_module_sample/hello.mod.o
  LD [M]  /home/cc/githome/Linux_Device_Driver/kernel_module_sample/hello.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.13.0-39-generic'

    # note : 
      可用「uname -a」查詢自己的 Linux 與 Kernel版本
    cc@kernel_module_sample$uname -a
    Linux cc-VirtualBox 4.13.0-39-generic #44~16.04.1-Ubuntu SMP Thu Apr 5 16:43:10 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux


4) ls -al : 確認編好的檔案
cc@kernel_module_sample$ls
hello.c  hello.ko  hello.mod.c  hello.mod.o  hello.o  Makefile  modules.order  Module.symvers


6) 查詢module 「hello.ko」 : sudo lsmod | grep "hello"
cc@kernel_module_sample$sudo lsmod | grep 'hello'
hello                  16384  0


8) 查詢有無成功 : dmesg 指令來察看系統日誌
   dmesg

...(略)   
[14162.950021] == Hello Walter ! ==
[14255.757208] == Bye Walter ! ==

===============================
# 參考文件 :
  # 如何寫一個 Linux Kernel Module
    https://jerrynest.io/how-to-write-a-linux-kernel-module/
*/

/*
# 此範例執行步驟

1. $make

2. ls : 確認編好的檔案

3. 載入 module 「mydemo」 : sudo insmod mydemo.ko       ## 注意 : 是輸入檔案名稱(xxx.ko)

4. 查詢 module 「mydemo」 : sudo lsmod | grep "mydemo"

5. 查詢有無成功 : dmesg 指令來察看系統日誌
[ 7938.150032] succeeded register char device: my_demo_dev
[ 7938.150033] Major number = 244, minor number = 0

6. cat /proc/devices
244 my_demo_dev

7. 手動生成「device node」: mknod "file name" "type" "major" "minor", c : character device
sudo mknod /dev/demo_drv c 244 0
# note : "file name" = /dev/demo_drv = 這個會用在 user process(test.c)裡，來被"open"這個system call用

8. ll /dev/
crw-r--r--   1 root    root    244,   0 十二 19 23:54 demo_drv

9. gcc test.c -o test; ./test; dmesg
[ 8255.113817] demodrv_open: major=244, minor=0
[ 8255.113820] demodrv_read enter

10. 手動移除「device node」 : sudo rm /dev/demo_drv

11. 移除 module 「mydemo」 : sudo rmmod mydemo      ## 注意 : 是輸入模組名稱(xxx)

======================================

# 新增module_param使用範例

1. $make

2. ls : 確認編好的檔案

3. 載入 module : sudo insmod hello.ko       ## 注意 : 是輸入檔案名稱(xxx.ko)

4. 查詢 module 「hello」 : sudo lsmod | grep "hello"

5. dmesg

[ 1705.671779] == Hello walter ! ==
[ 1705.671780] [walter]module parameter = 1


6. sudo rmmod hello

7. sudo insmod hello.ko waltertest=345

8. dmesg | grep walter

[ 2333.076037] == Hello walter ! ==
[ 2333.076038] [walter]module parameter = 1
[ 2475.039928] == Bye walter ! ==
[ 2477.813324] == Hello walter ! ==
[ 2477.813325] [walter]module parameter = 345


9. cc@kernel_module_sample$ll /sys/module/hello/parameters/

total 0
drwxr-xr-x 2 root root    0 十二 19 22:21 ./
drwxr-xr-x 6 root root    0 十二 19 22:21 ../
-rw-r--r-- 1 root root 4096 十二 19 22:21 waltertest

===============================
# 參考文件 : 張天飛，笨叔叔，奔跑吧內核
*/


#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("Hello_world");
MODULE_LICENSE("GPL");

static int waltertest = 1;
module_param(waltertest,int,0644);

static int hello_init(void)
{
    printk(KERN_INFO "== Hello walter ! ==\n");
    printk(KERN_INFO "[walter]module parameter = %d\n", waltertest);
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_INFO "== Bye walter ! ==\n");
}

module_init(hello_init);
module_exit(hello_exit);

