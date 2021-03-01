
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/delay.h>
#include<linux/workqueue.h>


#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/device.h>

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A Simple Blocking IO device RaspPi");

/* declare a wait queue*/
static wait_queue_head_t my_wait_queue;      // wait_queue_head_t : 等待隊列頭

struct work_struct workq;   // walter : 當成要排隊的人

static int condition = 0;

// the prototype of the function which is going to be the task
// typedef void (*work_func_t)(struct work_struct *work);  *work_func_t : 要延遲處理的工作
void my_workqueue_handler(struct work_struct *work)
{
    printk("[walter]200105 : %s: +++\n", __func__);

	msleep(5000);  /* sleep */

    printk("[walter]%s: time up MODULE !! wake up !!!!\n", __func__);
    wake_up_interruptible(&my_wait_queue);  // 喚醒正在等待狀態的指定 wait queue - my_wait_queue。
    condition = 1;  // if 0, the wait queue would not wake up

    printk("[walter]200105 : %s: ---\n", __func__);
}

static int my_init(void)  // entry point
{
    printk("[walter]200105 : %s: +++\n", __func__);
    
    /* 
    walter : 當初始化"排隊的人(= my_workqueue_handler)"，當系統排程至 workq 執行時會去呼叫該函式
    */
	INIT_WORK(&workq, my_workqueue_handler);  
    
    // 將其註冊進去核心的工作排程當中
	schedule_work(&workq); // 安排一個 work queue 去執行一個工作
    
    // -- initialize the WAIT QUEUE head
    init_waitqueue_head(&my_wait_queue);    // 動態初始化 等待隊列頭

    printk("[walter]%s: This moudle is goint to sleep....\n", __func__);
    wait_event_interruptible(my_wait_queue, condition); // my_wait_queue 睡著

    printk("[walter]200105 : %s: ---\n", __func__);
/*
===============================
# 螢幕輸出結果
[  151.414815] [walter]200105 : my_init: +++
[  151.414818] [walter]my_init: This moudle is goint to sleep....
[  151.414822] [walter]200105 : my_workqueue_handler: +++
[  156.568117] [walter]my_workqueue_handler: time up MODULE !! wake up !!!!
[  156.568124] [walter]200105 : my_workqueue_handler: ---
[  156.568133] [walter]200105 : my_init: ---
[  191.240729] [walter]200105 : my_exit: +++
[  191.240731] [walter]200105 : my_exit: ---
===============================
*/    
	return 0;
}

static void my_exit(void)   // exit point
{
    printk("[walter]200105 : %s: +++\n", __func__);
	flush_scheduled_work(); // 保證workqueue上的work都處理完了
    printk("[walter]200105 : %s: ---\n", __func__);
}

module_init(my_init);
module_exit(my_exit);

