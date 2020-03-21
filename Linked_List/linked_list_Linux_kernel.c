/*===============================
1. 我們可以利用**gcc -E**來觀看程式碼被展開之後的樣子
gcc -E test2.c
 
2. 執行結果
[mtk18157@mtkslt207 testhome]$gcc test2.c -o test
[mtk18157@mtkslt207 testhome]$./test
DEBUG: hello
[mtk18157@mtkslt207 testhome]$gcc test2.c -o test
[mtk18157@mtkslt207 testhome]$./test
arguments numbers : 1
[0] ./test
 
[mtk18157@mtkslt207 testhome]$./test walter haha
arguments numbers : 3
[0] ./test
[1] walter
[2] haha
[mtk18157@mtkslt207 testhome]$
===============================
# 參考文件 :
  - 資料結構初學指引 ─ 入門精要版, 3/e，陳錦輝
  - Linux Kernel: 強大又好用的list_head結構
    http://adrianhuang.blogspot.com/2007/10/linux-kernel-listhead.html
  - Linux鏈結串列struct list_head 研究
    https://myao0730.blogspot.com/2016/12/linux.html
  - Linux內核鍊錶——看這一篇文章就夠了
    https://www.cnblogs.com/yangguang-it/p/11667772.html
 
 
# Node = Data field + link field
 
# Linked List 可充分而有效的利用可用記憶體空間。我們不該限制使用者在程式執行過程中使用的節點數量，也不該配置過多不會用到的記憶體。我們必須藉由C語言的動態記憶體配置函式「malloc()」來產生新節點。
 
# Linux kernel定義一通用結構 (struct list_head，kernel\v4.19\include\linux\types.h)，用以實作雙向鏈結串列 (Doubly Linked List)
 
struct list_head {
    struct list_head *next, *prev;
};
 
所以
struct student {
    char name[16];
    int id;
    struct student *next, *prev;
};
變成
struct student
{
    char name[16];
    int id;
    struct list_head list;  // 只要在自定義的結構中加入struct list_head結構體就可以使用Linux一系列的link-list函數而不用自行撰寫。
};
 
# list_head相關Marco與Function 在 kernel\v4.19\include\linux\list.h，譬如下面的define
 
    - #define LIST_HEAD_INIT(name) { &(name), &(name) }
 
    - #define LIST_HEAD(name) \
        struct list_head name = LIST_HEAD_INIT(name)
 
# 內核鍊錶的結構是個雙向循環鍊錶，只有「指針域」，數據域根據使用鍊錶的人的具體需求而定。
 
# 內核鍊錶設計哲學 ： 既然鍊錶不能包含萬事萬物，那麼就讓「萬事萬物來包含鍊錶」。
 
「正」
 
===============================*/
 
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
 
#define EXAMPLE 5
/*
1 : e.g. > User space 雙向鏈結串列 (Doubly Linked List) 完整sample code
2 : e.g. > Linux kernel 雙向鏈結串列 (Doubly Linked List) 完整sample code : 第1種形式
3 : e.g. > Linux kernel 雙向鏈結串列 (Doubly Linked List) 完整sample code : 第2種形式
4 : e.g. > Linux kernel 雙向鏈結串列 (Doubly Linked List) 完整sample code : 第3種形式
5 : e.g. > Linux kernel 雙向鏈結串列 (Doubly Linked List) 完整sample code : 混和形式
*/
 
 
//=========== list_head 相關Marco與Function ===========================
 
/*
- Linux kernel定義一通用結構 (struct list_head，kernel\v4.19\include\linux\types.h)，用以實作雙向鏈結串列 (Doubly Linked List)。
- 只要在自定義的結構中加入struct list_head結構體就可以使用Linux一系列的link-list函數而不用自行撰寫。
*/
struct list_head {
    struct list_head *next, *prev;
};
/*
宣告兩個指標 *next, *prev 指向自己
*/
 
 
// 宣告一個 struct list_head 的結構變數並作初始化，也就是將perv與next指標指向自己，意味著此list為空的。
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)
/*
#define LIST_HEAD(name) \
    struct list_head name = { &(name), &(name) }
 
宣告一個變數 name，其元素 *next, *prev 的初始值為自己的位址，也就是將perv與next指標指向自己
*/
 
 
// 也可以使用 INIT_LIST_HEAD 這個函式來初始化頭節點，但是這樣的作法要先宣告一 struct list_head變數 然後再餵給INIT_LIST_HEAD做初始化。project use this
void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}
/*
e.g. :
struct student
{
    char name[16];
    int id;
    struct list_head list;
};
struct student stu;
INIT_LIST_HEAD(&stu.list);
*/
 
 
// 檢查這個串列是否為空。
int list_empty(const struct list_head *head)
{
    return head->next == head;
}
/*
head->next == head : next指標指向自己，表示此List為空。
*/
 
 
// __list_add 這個函式會將一個新的節點加入一對已知的前(prev)/後(next)端節點之間，也就是說原本相連的2個端點中間會插入一個新節點。
static  void __list_add(struct list_head *new_lst,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = new_lst;
    new_lst->next = next;
    new_lst->prev = prev;
    prev->next = new_lst;
}
/*
struct list_head *new_lst   : 指向 新增的node 的 pointer
struct list_head *prev      : 指向 前一個node 的 pointer
struct list_head *next      : 指向 後一個node 的 pointer
 
 
before : prev node <-> next node
after  : prev node <-> new node <-> next node
 
1. next->prev = new_lst; : new node <- next node。
   新增節點的位址 (new_lst) 被 後一個節點的prev指標 (next->prev) 指
2. new_lst->next = next; : new node <-> next node
   後一個節點的位址 (next) 被 新增節點的next指標 (new_lst->next) 指
3. new_lst->prev = prev; : prev node <- new node <-> next node
   前一個節點的位址 (prev) 被 新增節點的prev指標 (new_lst->prev) 指
4. prev->next = new_lst; : prev node <-> new node <-> next node
   新增節點的位址 (new_lst) 被 前一個節點的next指標 (prev->next) 指
*/
 
 
/*
- 將__list_add重新包裝，讓一個新的節點加入串列的開頭(將資料加入至doubly linked list最前端)，新節點會變成HEAD指向的next，新加入的節點等於是向前端插入。
- PS: 在LDD這書中把這樣的操作比喻為Stack(LIFO)。
*/
void list_add(struct list_head *new_lst,
              struct list_head *head)
{
    __list_add(new_lst, head, head->next);
}
/*
# __list_add(new_lst, head, head->next);
 
- new_lst       : 指向 新增的node 的 pointer
- head          : 指向 前一個node 的 pointer。在這裡表示頭節點
- head->next    : 指向 後一個node 的 pointer。在這裡表示頭節點的下一個節點 (1st node)
 
before : head node <-> 1st node
after  : head node <-> new node <-> 1st node
*/
 
 
/*
- 將__list_add重新包裝，讓一個新的節點加入串列末端中，新節點會變成HEAD指向的prev，新加入的節點等於是向尾端插入。
- PS: 在LDD這書中把這樣的操作比喻為Queue(FIFO)，事實上它還真的能拿來實作Queue。
*/
void list_add_tail(struct list_head *new_lst, struct list_head *head)
{
    __list_add(new_lst, head->prev, head);
}
/*
# __list_add(new_lst, head->prev, head);
 
- new_lst       : 指向 新增的node 的 pointer
- head->prev    : 指向 前一個node 的 pointer。在這裡表示頭節點的下一個節點 (1st node)
- head          : 指向 後一個node 的 pointer。在這裡表示頭節點
 
before : head node <-> 1st node
after  : head node <-> 1st node <-> new node
*/
 
 
// @ kernel\v4.19\include\linux\poison.h
/*
* These are non-NULL pointers that will result in page faults
* under normal circumstances, used to verify that nobody uses
* non-initialized list entries.
*/
// LIST_POISON1與LIST_POISON2用來標記那些沒有被初始化以及沒有在鍊表中的list項，讓它們不可被訪問。
#define POISON_POINTER_DELTA 0
#define LIST_POISON1  ((void *) 0x100 + POISON_POINTER_DELTA)
#define LIST_POISON2  ((void *) 0x200 + POISON_POINTER_DELTA)
 
 
/*
- __list_del這個函式會刪除某一個節點 entry，這需要先知道entry的前(prev)/後(next)端節點。
- entry原本的前端節點的next指標會指向entry原本的後端節點，
  而後端節點的prev指標會指向entry的前端節點，這樣一來就沒有其它節點的指標會指向entry，達到刪除的效果。
*/
static  void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}
/*
- prev  : 指向 前一個node 的 pointer。
- next  : 指向 後一個node 的 pointer。
 
before : prev node <-> useless node <-> next node
after  : prev node <-> next node
 
1. next->prev = prev; : prev node <- next node。
   前一個節點的位址 (prev) 被 後一個節點的prev指標 (next->prev) 指
2. prev->next = next; : new node <-> next node。
   後一個節點的位址 (next) 被 前一個節點的next指標 (prev->next) 指
*/
 
 
// 而list_del除了呼叫__list_del之外，還會將entry的prev/next指標指向LIST_POISON1與LIST_POISON2這兩個特別位址。
void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
//    entry->next = LIST_POISON1;
//    entry->prev = LIST_POISON2;
}
/*
- entry->prev  : 指向 前一個node 的 pointer。
- entry->next  : 指向 後一個node 的 pointer。
 
- 值得注意的是，刪除串列項的時候會使用到list_del函式，在這裡暫時將 entry->next = LIST_POISON1 和 entry->prev = LIST_POISON2 這2行註解掉，
  因為我們不是在寫驅動程式而是在user space使用，可以把entry->{prev/next}設為NULL，
  代表該指標沒有指向任何東西，或者也可以把它重新初始化，使用INIT_LIST_HEAD(entry)。
*/
 
 
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/*
TYPE : structure name
MEMBER : structure member
 
1. ((TYPE *)0) : 轉型成 "TYPE指標" 型別
2. &((TYPE *)0)->MEMBER : 0 被當作該TYPE的起始地址，然後再指向某成員
3. &((TYPE *)0)->MEMBER) : 得到MEMBER的位址，因為起始位址等於 0，所以MEMBER的位址也就等於MEMBER與起始位址  0 的偏移(offset)。
4. offsetof(TYPE, MEMBER) : 用來計算某一個struct結構的成員相對於該結構起始位址的偏移量( offset )
*/
 
#define container_of(PTR, TYPE, MEMBER) ({\
        const typeof( ((TYPE *)0)->MEMBER ) *__mptr = (PTR);\
              (TYPE *)( (char *)__mptr - offsetof(TYPE,MEMBER) );})
/*
PTR : 指向該結構成員的型別的指標
TYPE :  structure name
MEMBER :  structure member
 
1. typeof( ((TYPE *)0)->MEMBER ) : 取出MEMBER的資料型別
2. typeof( ((TYPE *)0)->MEMBER ) *__mptr : 定義一個指向 MEMBER 型別的指標 __mptr
3. const typeof( ((TYPE *)0)->MEMBER ) *__mptr = (PTR); : 將指標PTR傳給__mptr
4. (TYPE *)( (char *)__mptr - offsetof(TYPE,MEMBER) ); : 將__mptr的位址 減掉 MEMBER與結構起始點的偏移(利用offsetof) = 該結構的起始點位址。
*/
 
/*
- 取得該doubly linked list所有資料。
- 由定義可以知道是在走訪整個鏈結串列，pos為一個工作指標並指向第一個項目(head->next)，以此為起點一路訪問下去，終止條件是當pos指向head時，代表已經走訪完一圈了。
*/
#define list_for_each(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)
 
/*
- 透過此函式便能算出結構的起始位址，並做結構轉型便能取得結構的資料
- 所以，list_for_each 和 ist_entry 這兩個巨集搭配使用就可以存取整個鏈結串列。
*/
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)
 
 
/**
* list_first_entry - get the first element from a list
* @ptr:    the list head to take the element from.
* @type:    the type of the struct this is embedded in.
* @member:    the name of the list_head within the struct.
*
* Note, that list is expected to be not empty.
*/
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)
 
/**
* list_next_entry - get the next element in list
* @pos:    the type * to cursor
* @member:    the name of the list_head within the struct.
*/
#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)
    
/**
* list_for_each_entry    -    iterate over list of given type
* @pos:    the type * to use as a loop cursor.
* @head:    the head for your list.
* @member:    the name of the list_head within the struct.
 
list_for_each_entry，通過其名字我們也能猜測其功能，list_for_each是遍歷鍊錶，
增加entry後綴，表示遍歷的時候，還要獲取entry（條目），即獲取鍊錶容器結構的地址。
*/
#define list_for_each_entry(pos, head, member)                \
    for (pos = list_first_entry(head, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = list_next_entry(pos, member))
         
/**
* list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
* @pos:    the type * to use as a loop cursor.
* @n:        another type * to use as temporary storage
* @head:    the head for your list.
* @member:    the name of the list_head within the struct.
*/
#define list_for_each_entry_safe(pos, n, head, member)            \
    for (pos = list_first_entry(head, typeof(*pos), member),    \
        n = list_next_entry(pos, member);            \
         &pos->member != (head);                     \
         pos = n, n = list_next_entry(n, member))     
 
static inline void __list_splice(const struct list_head *list,
                 struct list_head *prev,
                 struct list_head *next)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
 
    first->prev = prev;
    prev->next = first;
 
    last->next = next;
    next->prev = last;
}
 
/**
* list_splice - join two lists, this is designed for stacks
* @list: the new list to add.
* @head: the place to add it in the first list.
*/
static inline void list_splice(const struct list_head *list,
                struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head, head->next);
}
 
/**
* list_splice_tail - join two lists, each list being a queue
* @list: the new list to add.
* @head: the place to add it in the first list.
*/
static inline void list_splice_tail(struct list_head *list,
                struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head->prev, head);
}         
//======================================================================
 
#if (EXAMPLE == 1)
 
struct My_DATA {
    int Data_1;
    int Data_2;
    int Data_3;
    struct list_head list;
};
 
// 訪問整個串列並印出串列項的address和data內容。
void print_List(struct list_head *head){
 
    struct list_head *pos;  // 工作指標
    struct My_DATA *entry;
 
    printf("\n*********************************************************************************\n");
    printf("(HEAD addr = %p, next = %p, prev = %p)\n", head, head->next, head->prev);
 
    list_for_each(pos, head) {
 
        entry = list_entry(pos, struct My_DATA, list);  // 取得該節點結構的起始點位址。
 
        printf("entry->Data_1 = %d | list addr = %p | next = %p | prev = %p\n",
                entry->Data_1, &entry->list, entry->list.next, entry->list.prev);
    }
    printf("*********************************************************************************\n");
 
}
 
// 從串列尾端加入一個新的節點
struct My_DATA *add_Node_tail(struct list_head *head){
    struct My_DATA *entry;
    entry = (struct My_DATA*)malloc(sizeof(struct My_DATA));
    list_add_tail(&entry->list, head);
    return entry;
}
 
// 從串列前端加入一個新的節點
struct My_DATA *add_Node(struct list_head *head){
    struct My_DATA *entry;
    entry = (struct My_DATA*)malloc(sizeof(struct My_DATA));
    list_add(&entry->list, head);
    return entry;
}
 
/*
- 刪除單一個節點
- 值得注意的是，刪除串列項的時候會使用到list_del函式，在這裡暫時將 entry->next = LIST_POISON1 和 entry->prev = LIST_POISON2 這2行註解掉，
  因為我們不是在寫驅動程式而是在user space使用，可以把entry->{prev/next}設為NULL，
  代表該指標沒有指向任何東西，或者也可以把它重新初始化，使用INIT_LIST_HEAD(entry)。
*/
void remove_Node(struct My_DATA *entry){
 
    printf("\nremove: Data_1 = %d (list %p, next %p, prev %p)\n",
            entry->Data_1, &entry->list, entry->list.next, entry->list.prev);
 
    list_del(&entry->list);
    free(entry);
    entry = NULL;
}
 
// 銷毀整個串列
void free_List(struct list_head *head)
{
    struct list_head *pos;      // 工作指標
    struct My_DATA *entry;
 
    list_for_each(pos, head) {
 
        entry = list_entry(pos, struct My_DATA, list);
 
        printf("\nFree: entry->Data_1 = %d | list addr = %p | next = %p | prev = %p\n",
                entry->Data_1, &entry->list, entry->list.next, entry->list.prev);
 
        free(entry);
        entry = NULL;
    }
}
 
#endif  // #if (EXAMPLE == 1)
 
 
#if (EXAMPLE == 2)
struct person
{
    struct list_head list;
    int age;
};
#endif // #if (EXAMPLE == 2)
 
#if (EXAMPLE == 3)
/*
內核鍊錶是有頭節點的，一般而言頭節點的數據域我們不使用，但也有使用頭節點數據域記錄鍊錶長度的實現方法。
頭節點其實不是必需的，但作為學習，我們可以實現一下
*/
struct person_head  // 新增頭節點
{
    int len;
    struct list_head list;
};
    
struct person
{
    int age;
    struct list_head list;
};
#endif // #if (EXAMPLE == 3)
 
#if (EXAMPLE == 4)
struct person
{
    int age;
    struct list_head list;
};
#endif // #if (EXAMPLE == 4)
  
#if (EXAMPLE == 5)
struct person
{
    int age;
    struct list_head list;
};
struct My_DATA {
    int Data_1;
    int Data_2;
    int Data_3;
    struct list_head my_data_list;
};
#endif // #if (EXAMPLE == 5)
    
int main(int argc, char *argv[]) {
 
#if (EXAMPLE == 1)
    int i;
 
    printf("arguments numbers : %d\n", argc);
 
    for (i = 0; i < argc; i++)
        printf("[%d] %s\n", i, argv[i]);
 
    LIST_HEAD(HEAD);    // 初始化一個List
 
    struct My_DATA *itemPtr_1 = add_Node(&HEAD);    // 從串列前端加入一個新的節點，itemPtr_1 : 新節點的位址
    itemPtr_1->Data_1 = 100 ;
 
    struct My_DATA *itemPtr_2 = add_Node(&HEAD);
    itemPtr_2->Data_1 = 200 ;
 
    struct My_DATA *itemPtr_3 = add_Node(&HEAD);
    itemPtr_3->Data_1 = 300 ;
 
    struct My_DATA *itemPtr_4 = add_Node(&HEAD);
    itemPtr_4->Data_1 = 400 ;
 
    print_List(&HEAD);      // 訪問整個串列並印出串列項的address和data內容。
 
    remove_Node(itemPtr_3); // 刪除單一個節點
    print_List(&HEAD);
 
    free_List(&HEAD);       // 銷毀整個串列
 
/*
 
===============================
# 執行結果 : 
 
從結果可以看出串列項是向前端加入，在印出串列時從head->next開始印出，當然銷毀串列時也是一樣從head->next開始，這樣的運作模式就如同LDD書中所講的LIFO(後進先出)模式。
 
[mtk18157@mtkslt207 testhome]$gcc list_linux_kernel_sample.c -o test
[mtk18157@mtkslt207 testhome]$./test
arguments numbers : 1
[0] ./test
 
*********************************************************************************
(HEAD addr = 0x7fffd15cf4a0, next = 0x1d740b0, prev = 0x1d74020)
entry->Data_1 = 400 | list addr = 0x1d740b0 | next = 0x1d74080 | prev = 0x7fffd15cf4a0
entry->Data_1 = 300 | list addr = 0x1d74080 | next = 0x1d74050 | prev = 0x1d740b0
entry->Data_1 = 200 | list addr = 0x1d74050 | next = 0x1d74020 | prev = 0x1d74080
entry->Data_1 = 100 | list addr = 0x1d74020 | next = 0x7fffd15cf4a0 | prev = 0x1d74050
*********************************************************************************
 
remove: Data_1 = 300 (list 0x1d74080, next 0x1d74050, prev 0x1d740b0)
 
*********************************************************************************
(HEAD addr = 0x7fffd15cf4a0, next = 0x1d740b0, prev = 0x1d74020)
entry->Data_1 = 400 | list addr = 0x1d740b0 | next = 0x1d74050 | prev = 0x7fffd15cf4a0
entry->Data_1 = 200 | list addr = 0x1d74050 | next = 0x1d74020 | prev = 0x1d740b0
entry->Data_1 = 100 | list addr = 0x1d74020 | next = 0x7fffd15cf4a0 | prev = 0x1d74050
*********************************************************************************
 
Free: entry->Data_1 = 400 | list addr = 0x1d740b0 | next = 0x1d74050 | prev = 0x7fffd15cf4a0
 
Free: entry->Data_1 = 200 | list addr = 0x1d74050 | next = 0x1d74020 | prev = 0x1d740b0
 
Free: entry->Data_1 = 100 | list addr = 0x1d74020 | next = 0x7fffd15cf4a0 | prev = 0x1d74050
===============================
*/
 
#endif  // #if (EXAMPLE == 1)
 
#if (EXAMPLE == 2)
    
    int i;
    struct person *p;
    struct person person1;  // 頭節點
    struct list_head *pos;  // 工作指標
    
    /*
    - 初始化一個鍊錶
    - 需要先定義出鍊錶的實體，如前面的person1一樣，然後將person1的地址傳遞給初始化函數即可完成鍊錶的初始化。
    - 頭結點的數據域不重要，可以根據需要利用頭結點的數據域，一般而言，頭結點數據域不使用，在使用頭結點數據域的情況下，一般也僅僅記錄鍊錶的長度信息
    */
    INIT_LIST_HEAD(&person1.list);
    
    for(i = 0; i < 5; i++){
        p = (struct person *)malloc(sizeof(struct person));
        p->age = i * 10;
 
        // 頭插法，即每次插入的節點都位於上一個節點之前，比如上一個節點是head->1->head，本次使用頭插法插入之後，鍊錶結構變成了head->2->1->head。
        list_add(&p->list, &person1.list);  
    }
 
    // 遍歷鍊錶，訪問其數據域的內容，直到出現pos == head時，循環鍊錶就遍歷完畢了。
    list_for_each(pos, &person1.list){
        /*
        struct person
        {
            struct list_head list;
            int age;
        };
        由於我們將鍊錶放在結構體的首地址處，那麼此時鍊錶list的地址，和struct person 的地址是一致的，
        所以通過pos的地址，將其強制轉換成struct person *就可以訪問age元素了。
        */
        printf("age = %d\n",((struct person *)pos)->age);
        /*
        struct person
        {
            int age;
            struct list_head list;
        };
        - 如果改成這樣，printf("age = %d\n",((struct person *)pos)->age); ，這段就不能正常工作了，
          因為此時強制類型轉換得到地址不再是struct person結構的首地址，進行->age操作時，指針偏移不正確。
          所以，也就出現了container_of，offsetof這兩個廣為人知的宏。
        - 試想，如果我們能夠通過將pos指針傳遞給某個宏或者函數，該函數或者宏能夠通過pos返回包含pos容器這個結構體的地址，
          那麼我們不就可以正常訪問age了嗎。很顯然， container_of宏，就是這個作用啊，在內核中，將其又封裝成了 list_entry宏
        */
    };
/*
===============================
# 執行結果 : 
 
[mtk18157@mtkslt207 testhome]$gcc list_linux_kernel_sample.c -o test
[mtk18157@mtkslt207 testhome]$./test
age = 40
age = 30
age = 20
age = 10
age = 0
===============================
*/
 
#endif  // #if (EXAMPLE == 2)
 
#if (EXAMPLE == 3)
    
    int i;
    struct person *p;
    struct person_head head;    // 頭節點
    struct list_head *pos;      // 工作指標
    
    /*
    - 初始化一個鍊錶
    - 需要先定義出鍊錶的實體，如前面的person1一樣，然後將person1的地址傳遞給初始化函數即可完成鍊錶的初始化。
    - 頭結點的數據域不重要，可以根據需要利用頭結點的數據域，一般而言，頭結點數據域不使用，在使用頭結點數據域的情況下，一般也僅僅記錄鍊錶的長度信息
    */
    INIT_LIST_HEAD(&head.list);
    head.len = 0;
    
    for(i = 0; i < 5; i++){
        p = (struct person *)malloc(sizeof(struct person));
        p->age = i * 10;
 
        // 頭插法，即每次插入的節點都位於上一個節點之前，比如上一個節點是head->1->head，本次使用頭插法插入之後，鍊錶結構變成了head->2->1->head。
        list_add(&p->list, &head.list);  
    }
 
    // 遍歷鍊錶，訪問其數據域的內容，直到出現pos == head時，循環鍊錶就遍歷完畢了。
    list_for_each(pos, &head.list){
        p = list_entry(pos, struct person, list);   // 取得該節點結構的起始點位址。
        printf("age = %d\n", p->age);
        head.len++;
    };
    printf("list length = %d\n",head.len);
/*
===============================
# 執行結果 : 
 
[mtk18157@mtkslt207 testhome]$gcc list_linux_kernel_sample.c -o test
[mtk18157@mtkslt207 testhome]$./test
age = 40
age = 30
age = 20
age = 10
age = 0
list length = 5
===============================
*/
 
#endif  // #if (EXAMPLE == 3)
 
#if (EXAMPLE == 4)
    
    int i;
    struct person *p;
    struct person head;     // 頭節點
    struct person *pos;     // 工作指標
    struct person *n;       // 緩存指標
    
    /*
    - 初始化一個鍊錶
    - 需要先定義出鍊錶的實體，如前面的person1一樣，然後將person1的地址傳遞給初始化函數即可完成鍊錶的初始化。
    - 頭結點的數據域不重要，可以根據需要利用頭結點的數據域，一般而言，頭結點數據域不使用，在使用頭結點數據域的情況下，一般也僅僅記錄鍊錶的長度信息
    */
    INIT_LIST_HEAD(&head.list);
    
    for(i = 0; i < 5; i++){
        p = (struct person *)malloc(sizeof(struct person));
        p->age = i * 10;
 
        // 頭插法，即每次插入的節點都位於上一個節點之前，比如上一個節點是head->1->head，本次使用頭插法插入之後，鍊錶結構變成了head->2->1->head。
        list_add(&p->list, &head.list);  
    }
    
    list_for_each_entry(pos, &head.list, list){
        if(pos->age == 30){
            list_del(&pos->list);
            break;      // 如果不使用break；會發生異常。
        }
    };
    
    /*
    - 使用這個宏，可以在遍歷鍊錶時安全地執行刪除操作
    - 其原理就是先把後一個節點取出來使用n作為緩存，這樣在還沒刪除節點時，就得到了要刪除節點的下一個節點的地址，從而避免了程序出錯。
      它使用了一個中間變量緩存的方法，實現更為安全的變量鍊錶方法
    */
    list_for_each_entry_safe(pos, n, &head.list, list){
        if(pos->age == 20){
            list_del(&pos->list);
            //break;
        }
    };
    
    /*
    - 遍歷鍊錶並獲取entry(即獲取鍊錶容器結構的地址)。
    - 該宏中的pos類型為容器結構類型的指針，這與前面list_for_each中的使用的類型不再相同
    */
    list_for_each_entry(pos, &head.list, list){
        printf("age = %d\n", pos->age);
    };
/*
===============================
# 執行結果 : 
 
[mtk18157@mtkslt207 testhome]$gcc list_linux_kernel_sample.c -o test
[mtk18157@mtkslt207 testhome]$./test
age = 40
age = 10
age = 0
===============================
*/
 
#endif  // #if (EXAMPLE == 4)
 
#if (EXAMPLE == 5)
    
    int i;
    struct person *p1, *p2;
    struct My_DATA head1, head2;    // 頭節點
    struct person *pos;             // 工作指標
    struct person *tmp_pos;         // 緩存指標
 
    /*
    - 初始化一個鍊錶
    - 需要先定義出鍊錶的實體，如前面的person1一樣，然後將person1的地址傳遞給初始化函數即可完成鍊錶的初始化。
    - 頭結點的數據域不重要，可以根據需要利用頭結點的數據域，一般而言，頭結點數據域不使用，在使用頭結點數據域的情況下，一般也僅僅記錄鍊錶的長度信息
    */
    INIT_LIST_HEAD(&head1.my_data_list);
    INIT_LIST_HEAD(&head2.my_data_list);
 
    printf("begin, p1 return of list_empty = %d\n", list_empty(&head1.my_data_list));
    // begin, p1 return of list_empty = 1
 
    for (i = 1; i < 6; i++) {
        p1 = (struct person *)malloc(sizeof(struct person));
        p1->age = i * 10;
 
        // 頭插法，比如上一個節點是head->1->head，本次使用頭插法插入之後，鍊錶結構變成了head->2->1->head。
        list_add(&p1->list, &head1.my_data_list);   // 頭節點 : struct My_DATA，節點 : struct person
 
        p2 = (struct person *)malloc(sizeof(struct person));
        p2->age = i * 3;
        
        // 尾插法，如果原來是head->1->head,尾插法一個節點之後變成了head->1->2->head。
        list_add_tail(&p2->list,&head2.my_data_list);   // 頭節點 : struct My_DATA，節點 : struct person
    }
 
    list_for_each_entry_safe(pos, tmp_pos, &head1.my_data_list, list) {
        printf("before del, p1 age = %d\n", pos->age);
        /*
        before del, p1 age = 50
        before del, p1 age = 40
        before del, p1 age = 30
        before del, p1 age = 20
        before del, p1 age = 10
        */
    }
 
    list_for_each_entry_safe(pos, tmp_pos, &head2.my_data_list, list) {
        printf("p2 age = %d\n", pos->age);
        /*
        p2 age = 3
        p2 age = 6
        p2 age = 9
        p2 age = 12
        p2 age = 15
        */
    }
 
    list_for_each_entry_safe(pos, tmp_pos, &head1.my_data_list, list) {
        if (pos->age == 30)
            list_del(&pos->list);
    }
 
    list_for_each_entry_safe(pos, tmp_pos, &head1.my_data_list, list) {
        printf("after del, p1 age = %d\n", pos->age);
        /*
        after del, p1 age = 50
        after del, p1 age = 40
        after del, p1 age = 20
        after del, p1 age = 10
        */
    }
 
    printf("end, p1 return of list_empty = %d\n", list_empty(&head1.my_data_list));
    // end, p1 return of list_empty = 0
    
    // get the first element from a list
    tmp_pos = list_first_entry(&head1.my_data_list, struct person, list);
 
    printf("p1 get from list_first_entry, age = %d\n", tmp_pos->age);
    // p1 get from list_first_entry, age = 50
    
    // join two lists, each list being a queue
    list_splice_tail(&head2.my_data_list, &head1.my_data_list);
 
    list_for_each_entry_safe(pos, tmp_pos, &head1.my_data_list, list) {
        printf("after list_splice_tail, p1 age = %d\n", pos->age);
        /*
        after list_splice_tail, p1 age = 50
        after list_splice_tail, p1 age = 40
        after list_splice_tail, p1 age = 20
        after list_splice_tail, p1 age = 10
        after list_splice_tail, p1 age = 3
        after list_splice_tail, p1 age = 6
        after list_splice_tail, p1 age = 9
        after list_splice_tail, p1 age = 12
        after list_splice_tail, p1 age = 15
        */
    }
/*
===============================
# 執行結果 : 
 
[mtk18157@mtkslt207 testhome]$gcc list_linux_kernel_sample.c -o test
[mtk18157@mtkslt207 testhome]$./test
begin, p1 return of list_empty = 1
before del, p1 age = 50
before del, p1 age = 40
before del, p1 age = 30
before del, p1 age = 20
before del, p1 age = 10
p2 age = 3
p2 age = 6
p2 age = 9
p2 age = 12
p2 age = 15
after del, p1 age = 50
after del, p1 age = 40
after del, p1 age = 20
after del, p1 age = 10
end, p1 return of list_empty = 0
p1 get from list_first_entry, age = 50
after list_splice_tail, p1 age = 50
after list_splice_tail, p1 age = 40
after list_splice_tail, p1 age = 20
after list_splice_tail, p1 age = 10
after list_splice_tail, p1 age = 3
after list_splice_tail, p1 age = 6
after list_splice_tail, p1 age = 9
after list_splice_tail, p1 age = 12
after list_splice_tail, p1 age = 15
===============================
*/
 
#endif  // #if (EXAMPLE == 5)
    return 0;
}
 
 

