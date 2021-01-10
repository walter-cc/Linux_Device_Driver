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


p404 C程式設計策略－入門篇，林振輝
#include <stdio.h>      // standard I/O
FILE *in;             // *in, *out 為 FILE pointer
in = fopen("data", "r");
fscanf(inf, "%s", name);
fprintf(outf, "%d", ar[i]);
fclose(inf);


p425 C程式設計策略－入門篇，林振輝
#include <stdio.h>      // standard I/O
fwrite(輸出變數的指標，資料的大小，次數，輸出檔的指標);


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
  - C程式設計策略－入門篇，林振輝
*/

#include <stdio.h>      // standard I/O


/*
# argc : 所帶參數的數目
# argv : 一個陣列，它會分拆所帶的參數放到陣列內
*/
int main(int argc, char *argv[]) {

printf("\n\033[1;35m[walter]++++++++++ example 1 +++++++++++++++[%s][%d]\n\n\033[0m",__FUNCTION__,__LINE__);
{
    int i;
    FILE *inf, *outf;
    char name[20];
    int ar[6] = {1,2,3,4,5,6};
    
    inf = fopen("in.txt", "r"); // 開啟檔案"in.txt"，開檔模式"r"-read only，指向檔案的檔案指標為 inf
    
    for(i = 0; i < 6; i++){
        fscanf(inf, "%s", name);    // 從inf所指的地方，以字串"%s"的形式讀取，並存到"name"
        printf("%s\n",name);
    }
    
    outf = fopen("out.txt", "w"); // 開啟檔案"out.txt"，開檔模式"w"-write only，指向檔案的檔案指標為 outf
    
    for(i = 0; i < 6; i++){
        fprintf(outf, "%d", ar[i]);    // 將ar所指的地方，以int "%d"的形式寫到"outf"所指的地方
    }
    /*
        walter
        tiana
        chen
        amigo
        haha
        ccc
    */
    fclose(inf);    // 關閉檔案
}

printf("\n\033[1;35m[walter]++++++++++ example 2 +++++++++++++++[%s][%d]\n\n\033[0m",__FUNCTION__,__LINE__);
{
    int i = 0;
    FILE *fPtr;
    
    unsigned int table[] = {
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
        0x12345678,
    };

    // 開啟模式為"可讓程式寫入"，若原本有此檔案，則會覆寫，若無此檔案，則會自行建立
    fPtr = fopen( "example2.txt","w" );     // 第一個參數為檔案名稱，第二個參數為開啟模式。

    if( NULL == fPtr )
        printf( "Open fail...\n" );
    
    for( i = 0; i < (sizeof(table) / 4); i++) {

        // fprintf(檔案指標, 格式字串, 輸出表列);
        fprintf(fPtr, "0x%x, ", table[i]);  // 將 table的每個元素寫到檔案"example2.txt"

        if( (i+1) % 8 == 0)         // 每8個元素一行
            fprintf(fPtr, "\n");    // 空一行
    }
    printf("\n");
    fclose(fPtr);    
}

printf("\n\033[1;35m[walter]++++++++++ example 3 +++++++++++++++[%s][%d]\n\n\033[0m",__FUNCTION__,__LINE__);
{
    int i;
    FILE *inf, *outf;
    int ar[] = {1, 2, 3, 4, 5}; 
    
    i = 654321;
    
    // 將整數i的十進位數值(4321)轉換成對應的ASCII code後，寫到文字檔"out2.txt"，所以"out2.txt"的檔案大小是6 (因為"out2.txt"裡面是用"6","5","4","3","2","1"的方式來顯示，每個字元=1個byte)。
    outf = fopen("out2.txt", "w");
    fprintf(outf, "%d", i);
    fclose(outf); 
    
    // 將整數i的記憶體內容直接寫到檔案"binary.txt"，所以"binary.txt"的檔案大小是4 (因為sizeof(int)[4])。
    outf = fopen("binary.txt", "wb"); // binary 模式
    fwrite(&i, sizeof(i), 1, outf);  // fwrite(輸出變數的指標，資料型別大小(bytes)，次數，輸出檔的指標);
    fclose(outf);    // 關閉檔案
    
    printf("sizeof(int)[%d]\n", (int)sizeof(int));
    // sizeof(int)[4]

    // 將整數i的記憶體內容直接寫到檔案"binary.txt"，所以"binary.txt"的檔案大小是4 (因為sizeof(int)[4])。
    inf = fopen("binary.txt", "rb"); // binary 模式
    fread(&i, sizeof(i), 1, inf);  
    fclose(outf);    // 關閉檔案
    
    printf("i[%d]\n", i);
    // i[654321]
    
    
    outf = fopen("binary2.txt", "wb"); // binary 模式
    fwrite(&i, sizeof(int) * 5, 1, outf);  // fwrite(輸出變數的指標，資料型別大小(bytes)，次數，輸出檔的指標);

    inf = fopen("binary2.txt", "rb"); // binary 模式
    fread(&i, sizeof(int) * 5, 1, inf);  // ar 是 int, 共有5個元素
    fclose(outf);    // 關閉檔案
    
    for(i = 0; i < 5; i++) {
        printf("%d ", ar[i]); 
        // 1 2 3 4 5 
    }
    printf("\n"); 
}

printf("\n\033[1;35m[walter]++++++++++ example 4 +++++++++++++++[%s][%d]\n\n\033[0m",__FUNCTION__,__LINE__);
{
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
        - data為路徑，程式碼會開啟"/data"已被「寫入」，以及把它關連到 out 這個 FILE pointer
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
}
    printf("\n\033[1;35m[walter]-------------------------[%s][%d]\n\n\033[0m",__FUNCTION__,__LINE__);
    return 0;
}





