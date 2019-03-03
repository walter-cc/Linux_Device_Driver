/*
# 此範例執行步驟
1) 新增hello.c
2) 新增Makefile
3) cc@kernel_module_sample$make
4) ls : 確認編好的檔案
5) 載入module 「hello」 : sudo insmod hello.ko           ## 注意 : 是輸入檔案名稱(xxx.ko)
6) 查詢module 「hello」 : sudo lsmod | grep "hello"
7) 移除module 「hello」 : sudo rmmod hello               ## 注意 : 是輸入模組名稱(xxx)
8) 查詢有無成功 : dmesg 指令來察看系統日誌
   dmesg
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



#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("Hello_world");
MODULE_LICENSE("GPL");

static int hello_init(void)
{
    printk(KERN_INFO "== Hello Walter ! ==\n");
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_INFO "== Bye Walter ! ==\n");
}

module_init(hello_init);
module_exit(hello_exit);

