
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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


struct list_head {
	struct list_head *next, *prev;
};

// 靜態初始化
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
			struct list_head name = LIST_HEAD_INIT(name)		// 宣告一個struct list_head的結構變數並作初始化，將 next 與 prev 指到自己，意味著此list為空的。
// LIST_HEAD(name) : struct list_head name = { &(name), &(name) }，讓 name addr = next 和 prev


// 動態初始化
void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

// 檢查這個串列是否為空
int list_empty(const struct list_head *head)
{
	return head->next == head;		// 跟 LIST_HEAD(name) 情況一樣，name addr = next 表示此list為空的。
}

// 將一個新的節點加入一對已知的前(prev)/後(next)端節點之間，也就是說原本相連的2個端點中間會插入一個新節點。
static  void __list_add(struct list_head *new_lst,
						struct list_head *prev,
						struct list_head *next)
{
	next->prev = new_lst;
	new_lst->next = next;
	new_lst->prev = prev;
	prev->next = new_lst;
}

// 讓一個新的節點加入串列的開頭，Stack(LIFO)
void list_add(struct list_head *new_lst,
              struct list_head *head)
{
	__list_add(new_lst, head, head->next);
}

// 讓一個新的節點加入串列末端，Queue(FIFO)
void list_add_tail(struct list_head *new_lst, struct list_head *head)
{
	__list_add(new_lst, head->prev, head);
}

// 走訪整個鏈結串列，pos為一個指標並指向第一個項目(head->next)，以此為起點一路訪問下去，終止條件是當pos指向head時，代表已經走訪完一圈了。
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

// 獲取節點本身的數據結構
#define list_entry(ptr, type, member)\
    container_of(ptr, type, member)

/*
// 刪除某一個節點entry，這需要先知道entry的前(prev)/後(next)端節點。
static void __list_del(struct list_head * prev,
						struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

// 定義在linux/poison.h中
#define LIST_POISON1  NULL	// ((void *) 0x00100100 + POISON_POINTER_DELTA)
#define LIST_POISON2  NULL	// ((void *) 0x00200200 + POISON_POINTER_DELTA)

void list_del(struct list_head * entry)
{
	__list_del(entry->prev,entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

struct MY_DATA {
   int Data_A;		// 數據域(int Data_A, Data_B, Data_C)根據使用鍊錶的人的具體需求而定
   int Data_B;
   int Data_C;
   struct list_head list;	// Linux kernel 的 Linked list 指定義"指標域(struct list_head list;)"
};

// 訪問整個串列並印出串列項的address和data內容。
void print_List(struct list_head *head){

	struct list_head *listptr;
	struct My_DATA *entry;

	printf("\n*********************************************************************************\n");
	printf("(HEAD addr =  %p, next = %p, prev = %p)\n",head, head->next, head->prev);
	list_for_each(listptr, head) {
		entry = list_entry(listptr, struct My_DATA, list);
		printf("entry->Data_1= %d | list addr = %p | next = %p | prev = %p\n",
				entry->Data_1,
				&entry->list,
				entry->list.next,
				entry->list.prev  );
	}
	printf("*********************************************************************************\n");
}

// 從尾端加入一個新的串列項
struct My_DATA *add_Node_tail(struct list_head *head){
	struct My_DATA *entry;
	entry=(struct My_DATA*)malloc(sizeof(struct My_DATA));
	list_add_tail( &entry->list,head);
	return entry;
}

// 從前端加入一個新的串列項
struct My_DATA *add_Node(struct list_head *head){
	struct My_DATA *entry;
	entry = (struct My_DATA*)malloc(sizeof(struct My_DATA));
	list_add( &entry->list,head);
	return entry;
}

// 刪除單一個串列項以及銷毀整個串列：
void remove_Node(struct My_DATA *entry){
    printf("\nremove: Data_1=%d (list %p, next %p, prev %p)\n",
            entry->Data_1,
            &entry->list,
            entry->list.next,
            entry->list.prev);

    list_del(&entry->list);
    free(entry);
}

void free_List(struct list_head *head)
{
	struct list_head *listptr;
	struct My_DATA *entry;

	list_for_each(listptr, head) {
		entry = list_entry(listptr, struct My_DATA, list);
		printf("\nFree: entry->Data_1 = %d | list addr = %p | next = %p | prev = %p\n",
		entry->Data_1,
		&entry->list,
		entry->list.next,
		entry->list.prev);

	free(entry);
	entry=NULL;
	}
}
*/

struct person
{
    int age;
    struct list_head list;
};

int main(void){

	int i;
	struct person *p1, *p2, *pos;
    struct list_head *pos_head;

    // static initialize
    static LIST_HEAD(list_head1);
    static LIST_HEAD(list_head2);

    printf("\nbegin, list_empty, p1 = %d, p2 = %d\n", list_empty(&list_head1), list_empty(&list_head2));

    printf("\n=== list_add : add to head ===\n");
	for (i = 0; i < 5; i++) {
		p1 = (struct person *)malloc(sizeof(struct person));
		p1->age = i*10;
		list_add(&p1->list, &list_head1);
	}

    printf("\np1 age = ");
    list_for_each(pos_head, &list_head1){
        pos = list_entry(pos_head, struct person, list);
        printf("%d -> ",pos->age);
    }
    printf("\n");

    printf("\n=== list_add_tail : add to tail ===\n");
    for (i = 0; i < 5; i++) {
        p2 = (struct person *)malloc(sizeof(struct person));
		p2->age = i*3;
		list_add_tail(&p2->list,&list_head2);
	}

    printf("\np2 age = ");
    list_for_each(pos_head, &list_head2){
        pos = list_entry(pos_head, struct person, list);
        printf("%d -> ",pos->age);
    }
    printf("\n");

    printf("\nafter, list_empty, p1 = %d, p2 = %d\n", list_empty(&list_head1), list_empty(&list_head2));


/*
	LIST_HEAD(HEAD);

	struct My_DATA *itemPtr_1 = add_Node(&HEAD);
	itemPtr_1->Data_1 = 100 ;

	struct My_DATA *itemPtr_2 = add_Node(&HEAD);
	itemPtr_2->Data_1 = 200 ;

	struct My_DATA *itemPtr_3 = add_Node(&HEAD);
	itemPtr_3->Data_1 = 300 ;

	struct My_DATA *itemPtr_4 = add_Node(&HEAD);
	itemPtr_4->Data_1 = 400 ;

	print_List(&HEAD);

	remove_Node(itemPtr_3);
	print_List(&HEAD);
	free_List(&HEAD);
*/
	return 0;
}

