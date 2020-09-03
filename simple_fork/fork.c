/*===============================

# 我們可以利用**gcc -E**來觀看程式碼被展開之後的樣子
gcc -E test2.c

# 新增編譯和執行步驟 :
gcc fork.c -o test
./test
===============================
# 此範例可以看到 : 「習」



===============================
# 螢幕輸出結果

cc@simple_fork$./test 

***********************

========================
++++ parent process ++++
I am parent ! my pid is 21625, the pid of my child is 21626 
before ++, a is 88 
after ++, a is 89 
---- parent process ----
cc@simple_fork$
========================
++++ child process ++++
I am child ! my pid is 21626 
a is 88 
---- child process ----
^C
cc@simple_fork$

===============================
# 參考文件 :

圖解Linux核心工作原理：透過實作與圖解學習OS與硬體的基礎知識，武內覺

*/

#include <stdio.h>      // printf()
#include <unistd.h>     // read(), write(), fork()


int main(void) {

    pid_t pid;
    int a = 88;     // 為了驗證 母行程和子行程是跑在不同的記憶體

    printf("\n***********************\n");
    
    /*
        1. 新增子行程，複製母行程的記憶體給子行程。 此時 a = 88 這個變數的值也被複製到子行程了
        2. 從呼叫 fork() 開始，母行程與子行程為了要執行不同程式碼而分支。
    */
    pid = fork();   // pid = 子行程的id，子行程從fork()的地方開始執行，且pid = 0

    if(pid == -1)
        printf("fork() failed\n");

    printf("\n========================\n");

    if(pid == 0) {
        // child process came here because fork() returns 0 for child process
        // 此時是子行程囉 !!

        printf("++++ child process ++++\n");

        printf("I am child ! my pid is %d \n",getpid());
        printf("a is %d \n",a);     // 驗證，若a = 89，則母子行程的記憶體共用，顯然不是 !!

        printf("---- child process ----\n");    // 當執行到這時，子行程已經結束
    }
    else {
        // parent process came here because fork() returns the pid of newly created child process (>1)
        // 此時是母行程唷 !!

        printf("++++ parent process ++++\n");

        printf("I am parent ! my pid is %d, the pid of my child is %d \n",getpid(), pid);

        printf("before ++, a is %d \n",a);
        ++a;
        printf("after ++, a is %d \n",a);

        printf("---- parent process ----\n");  // 當執行到這時，母行程已經結束
    }

    return 0;
}





