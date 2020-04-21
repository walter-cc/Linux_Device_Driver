/*===============================

# 我們可以利用**gcc -E**來觀看程式碼被展開之後的樣子
gcc -E test2.c

# 新增編譯和執行步驟 :

cc@simple_File_IO$gcc bufferIO.c -o test
cc@simple_File_IO$./test

===============================
# 此範例可以看到 : 「習」

# 緩衝式I/O sample code :

  此程式會初始設定其中一個變數(blackbeard)，隨後把他寫入磁碟(也就是，經由輸出串流寫入檔案data)。
  然後此程式會經由不同的串流從data檔案把資料直接讀回 struct pirate的另一個實例(instance, p)。
  最後，此程式會把該結構的內容寫入標準輸出。

p76
檔案指標(File Pointer) : FILE *fopen()
標準I/O常式不會直接操作fd，而會使用他們自己的識別符(identifier)，稱為檔案指標(File Pointer)。

===============================
# 螢幕輸出結果

arguments numbers : 1
[0] ./test

name = "Walter Teach" booty = 950 beard_len = 48

===============================
# 參考文件 :

  - Linux 系統程式設計, 2/e(內容涵蓋 Linux Kernel 3.0)，Robert Love 著、蔣大偉 譯
    Linux System Programming: Talking Directly to the Kernel and C Library)
  - 深入淺出 C，David Griffiths、Dawn Griffiths，O'Reilly
*/

#include <stdio.h>      // standard I/O


/*
# argc : 所帶參數的數目
# argv : 一個陣列，它會分拆所帶的參數放到陣列內
*/
int main(int argc, char *argv[]) {

	int i;
	FILE *in, *out;             // *in, *out 為 FILE pointer
    struct pirate {
        char name[100];         // real name
        unsigned long booty;    // 英鎊為單位
        unsigned int beard_len; // 英吋為單位
    } p, blackbeard = {"Walter Teach", 950, 48};
    // 宣告2個變數p, blackbeard

    printf("arguments numbers : %d\n", argc);   // argc : 所帶參數的數目

    for (i = 0; i < argc; i++) {
        printf("[%d] %s\n", i, argv[i]);    // argv : 一個陣列，它會分拆所帶的參數放到陣列內
    }
    printf("\n");

    /*
    - data為路徑，程式碼會開啟"/data"已備「寫入」，以及把它關連到 out 這個 FILE pointer
    - 執行成功時，fopen()會傳回一個有效的 "FILE 指標"。且該目錄會長出一個"data"的正規檔案(regular file)。
    - 一個已經開啟的檔案"data"稱為串流(stream)
    */
    out = fopen("data", "w");
    if(!out){
        perror("fopen");
        return 1;
    }

    // 把"blackbeard"這個struct寫入磁碟(也就是，經由"out"(file pointer)指向的輸出串流(stream)"data")
    // 所寫入的bytes數為1(資料中共有1個元素，每個元素具有sizeof(struct pirate)個bytes)
    if(!fwrite(&blackbeard, sizeof(struct pirate), 1, out)){
        perror("fwrite");
        return 1;
    }

    // 關閉所指定的串流(stream) "out"
    if(fclose(out)){
        perror("fclose");
        return 1;
    }

    // data為路徑，程式碼會開啟"/data"已備「讀取」，以及把它關連到 in 這個 FILE pointer
    in = fopen("data", "r");
    if(!in){
        perror("fopen");
        return 1;
    }

    /*
    - 經由不同的串流從data檔案把資料直接讀回 struct pirate的另一個實例(instance, p)。
    - 從File Pointer "in"指向的stream "/data"把資料讀進"p"這個instance，
      所讀取的bytes數為1(資料中共有1個元素，每個元素具有sizeof(struct pirate)個bytes)
    */
    if(!fread(&p, sizeof(struct pirate), 1, in)){
        perror("fread");
        return 1;
    }

    // 關閉所指定的串流(stream) "in"
    if(fclose(in)){
        perror("fclose");
        return 1;
    }

    // 把該結構的內容寫入標準輸出。
    printf("name = \"%s\" booty = %lu beard_len = %u\n", p.name, p.booty, p.beard_len);

    return 0;
}





