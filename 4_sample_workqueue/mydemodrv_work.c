#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("simple work queue sample");

struct work_struct workq;   // walter : 當成要排隊的人

// the prototype of the function which is going to be the task
// typedef void (*work_func_t)(struct work_struct *work);  *work_func_t : 要延遲處理的工作
void my_workqueue_handler(struct work_struct *work)
{
    printk("[walter]200105 work queue : %s: +++\n", __func__);

	msleep(5000);  /* sleep */

    printk("[walter]200105 work queue : %s: ---\n", __func__);
}

static int my_init(void)  // entry point
{
    printk("[walter]200105 work queue : %s: +++\n", __func__);
    
    /* 
    walter : 初始化"排隊的人(= my_workqueue_handler)"，當系統排程至 workq 執行時會去呼叫該函式
    */
	INIT_WORK(&workq, my_workqueue_handler);  
    
    // 將其註冊進去核心的工作排程當中
	schedule_work(&workq);
    
    printk("[walter]200105 work queue : %s: ---\n", __func__);
	return 0;
}

static void my_exit(void)   // exit point
{
    printk("[walter]200105 work queue : %s: +++\n", __func__);
	flush_scheduled_work();         // 保證workqueue上的work都處理完了
    printk("[walter]200105 work queue : %s: ---\n", __func__);
}

/*
===============================
# 螢幕輸出結果
[ 2938.784923] [walter]200105 work queue : my_init: +++
[ 2938.784926] [walter]200105 work queue : my_init: ---
[ 2938.784930] [walter]200105 work queue : my_workqueue_handler: +++
[ 2944.038182] [walter]200105 work queue : my_workqueue_handler: ---
[ 2980.187105] [walter]200105 work queue : my_exit: +++
[ 2980.187107] [walter]200105 work queue : my_exit: ---
===============================
*/

module_init(my_init);
module_exit(my_exit);
