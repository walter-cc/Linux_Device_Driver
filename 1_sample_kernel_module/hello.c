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

3. cc@kernel_module_sample$make
-----------------------------------------------------------
make -C /usr/src/linux-headers-4.13.0-39-generic/ M=/home/cc/githome/Linux_Device_Driver/kernel_module_sample  modules
make[1]: Entering directory '/usr/src/linux-headers-4.13.0-39-generic'
  CC [M]  /home/cc/githome/Linux_Device_Driver/kernel_module_sample/hello.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/cc/githome/Linux_Device_Driver/kernel_module_sample/hello.mod.o
  LD [M]  /home/cc/githome/Linux_Device_Driver/kernel_module_sample/hello.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.13.0-39-generic'
-----------------------------------------------------------
    # note : 
      可用「uname -a」查詢自己的 Linux 與 Kernel版本
    cc@kernel_module_sample$uname -a
    Linux cc-VirtualBox 4.13.0-39-generic #44~16.04.1-Ubuntu SMP Thu Apr 5 16:43:10 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux


4. ls -al : 確認編好的檔案
cc@kernel_module_sample$ls
-----------------------------------------------------------
hello.c  hello.ko  hello.mod.c  hello.mod.o  hello.o  Makefile  modules.order  Module.symvers
-----------------------------------------------------------


6. 查詢module 「hello.ko」 : sudo lsmod | grep "hello"
cc@kernel_module_sample$sudo lsmod | grep 'hello'
-----------------------------------------------------------
hello                  16384  0
-----------------------------------------------------------


8. 查詢有無成功 : dmesg 指令來察看系統日誌
   dmesg
-----------------------------------------------------------
...(略)   
[ 1825.351715] == Hello walter ! ==
[ 1825.351716] [walter]module parameter = 1
[ 1853.267947] == Bye walter ! ==
-----------------------------------------------------------

===============================

# 新增module_param使用範例

1. $make

2. ls : 確認編好的檔案

3. 載入 module : sudo insmod hello.ko       ## 注意 : 是輸入檔案名稱(xxx.ko)

4. 查詢 module 「hello」 : sudo lsmod | grep "hello"

5. dmesg
-----------------------------------------------------------
[ 1705.671779] == Hello walter ! ==
[ 1705.671780] [walter]module parameter = 1
-----------------------------------------------------------

6. sudo rmmod hello

7. sudo insmod hello.ko waltertest=345

8. dmesg | grep walter

# 螢幕輸出結果
===============================
[ 1825.351715] == Hello walter ! ==
[ 1825.351716] [walter]module parameter = 1
[ 1853.267947] == Bye walter ! ==
[ 1938.586564] == Hello walter ! ==
[ 1938.586565] [walter]module parameter = 345
===============================


9. cc@kernel_module_sample$ll /sys/module/hello/parameters/
-----------------------------------------------------------
total 0
drwxr-xr-x 2 root root    0 十二 19 22:21 ./
drwxr-xr-x 6 root root    0 十二 19 22:21 ../
-rw-r--r-- 1 root root 4096 十二 19 22:21 waltertest
-----------------------------------------------------------

===============================
# 參考文件 : 
張天飛，笨叔叔，奔跑吧內核
如何寫一個 Linux Kernel Module
 https://jerrynest.io/how-to-write-a-linux-kernel-module/
 ======================================
*/


#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("Hello_world");
MODULE_LICENSE("GPL");

static int waltertest = 1;
module_param(waltertest, int, 0644);

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

