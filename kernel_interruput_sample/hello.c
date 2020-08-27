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

9. cat /proc/interrupts 可以看到IRQ的狀態，最左邊的數值就是IRQ編號。

===============================
# 執行結果 : 

3.
cc@kernel_interruput_sample$make
make -C /usr/src/linux-headers-4.15.0-112-generic/ M=/home/cc/githome/Linux_Device_Driver/kernel_interruput_sample  modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-112-generic'
  CC [M]  /home/cc/githome/Linux_Device_Driver/kernel_interruput_sample/hello.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/cc/githome/Linux_Device_Driver/kernel_interruput_sample/hello.mod.o
  LD [M]  /home/cc/githome/Linux_Device_Driver/kernel_interruput_sample/hello.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-112-generic'

    # note : 
      可用「uname -a」查詢自己的 Linux 與 Kernel版本
    cc@kernel_module_sample$uname -a
    Linux cc-VirtualBox 4.13.0-39-generic #44~16.04.1-Ubuntu SMP Thu Apr 5 16:43:10 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux


4. ls : 確認編好的檔案
cc@kernel_interruput_sample$ls
hello.c  hello.ko  hello.mod.c  hello.mod.o  hello.o  Makefile  modules.order  Module.symvers



6. 查詢module 「hello」 : sudo lsmod | grep "hello"
cc@kernel_interruput_sample$sudo lsmod | grep "hello"
hello                  16384  0


8. 查詢有無成功 : dmesg 指令來察看系統日誌
   dmesg

...(略)   
[ 4797.450448] == Hello walter ! ==
[ 4798.129862] sample_isr : irq 16 dev_instance 000000000b049215
[ 4800.146715] sample_isr : irq 16 dev_instance 000000000b049215
[ 4800.387496] sample_isr : irq 16 dev_instance 000000000b049215
...(略)

9. cat /proc/interrupts 可以看到IRQ的狀態，最左邊的數值就是IRQ編號。

cc@kernel_interruput_sample$cat /proc/interrupts 
           CPU0       
  0:         41   IO-APIC   2-edge      timer
  1:        439   IO-APIC   1-edge      i8042
  8:          0   IO-APIC   8-edge      rtc0
  9:          0   IO-APIC   9-fasteoi   acpi
 12:        516   IO-APIC  12-edge      i8042
 14:          0   IO-APIC  14-edge      ata_piix
 15:       5642   IO-APIC  15-edge      ata_piix
 16:      11993   IO-APIC  16-fasteoi   enp0s8, hello



===============================
# 參考文件 : 
張天飛，笨叔叔，奔跑吧內核
平田豐，Linux Device Driver Programming 驅動程式設計
 ======================================
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_DESCRIPTION("Hello_world");
MODULE_LICENSE("GPL");

const int IRQ_NUM = 16;
void *irq_dev_id = (void *)&IRQ_NUM;

// sample_isr : 中斷處理程序 ( Interrupt Handler )
// irq : IRQ Line ( 中斷訊號、IRQ編號 )， 裝置 -> 中斷控制器 之間的線路
// dev_instance : dev_id的內容
irqreturn_t sample_isr(int irq, void *dev_instance){
    
    if(printk_ratelimit()) {
        printk(KERN_INFO "%s : irq %d dev_instance %p\n", __func__, irq, dev_instance);
    }
    return IRQ_NONE;
}

// 這個驅動程式enp0s8 & hello 共享IRQ，接下enp0s8的中斷訊號，但驅動程式本身不做進一步處理。
static int hello_init(void)
{
    int ret;
    
    printk(KERN_INFO "== Hello walter ! ==\n");
    
    /*
     - 登記中斷處理程序 ( IRQ, Interrupt )用的是request_irq()這個kernel函式。
     - IRQ_NUM : Linux Kernel 軟件中斷訊號、IRQ中斷號、IRQ編號、IRQ Number，這是一個Linux kernel管理的虛擬中斷號，並不是指硬件的中斷號。
     - sample_isr : 中斷處理程序 ( Interrupt Handler )
     - IRQF_SHARED : 共享IRQ。 log :  16: ...  enp0s8, hello -> enp0s8 & hello 共享IRQ
     - "hello" : 要在 cat /proc/interrupts 顯示的名字
     - irq_dev_id : 在共享IRQ的時候讓kernel區分中斷處理程序用的。傳入驅動程式的私有位址即可。
    */
    ret = request_irq(IRQ_NUM, sample_isr, IRQF_SHARED, "hello", irq_dev_id);
    
    if(ret) {
        printk(KERN_INFO "request_irq fail (%d)\n", ret);
        return ret;
    }
    
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_INFO "== Bye walter ! ==\n");
    
    free_irq(IRQ_NUM, irq_dev_id);
}

module_init(hello_init);
module_exit(hello_exit);

