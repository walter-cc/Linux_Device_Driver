
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/*
TYPE : structure name
MEMBER : structure member

1. ((TYPE *)0) : �૬�� "TYPE����" ���O
2. &((TYPE *)0)->MEMBER : 0 �Q��@��TYPE���_�l�a�}�A�M��A���V�Y����
3. &((TYPE *)0)->MEMBER) : �o��MEMBER����}�A�]���_�l��}���� 0�A�ҥHMEMBER����}�]�N����MEMBER�P�_�l��}  0 ������(offset)�C
4. offsetof(TYPE, MEMBER) : �Ψӭp��Y�@��struct���c�������۹��ӵ��c�_�l��}�������q( offset )
*/

#define container_of(PTR, TYPE, MEMBER) ({\
        const typeof( ((TYPE *)0)->MEMBER ) *__mptr = (PTR);\
              (TYPE *)( (char *)__mptr - offsetof(TYPE,MEMBER) );})
/*
PTR : ���V�ӵ��c���������O������
TYPE :  structure name
MEMBER :  structure member

1. typeof( ((TYPE *)0)->MEMBER ) : ���XMEMBER����ƫ��O
2. typeof( ((TYPE *)0)->MEMBER ) *__mptr : �w�q�@�ӫ��V MEMBER ���O������ __mptr
3. const typeof( ((TYPE *)0)->MEMBER ) *__mptr = (PTR); : �N����PTR�ǵ�__mptr
4. (TYPE *)( (char *)__mptr - offsetof(TYPE,MEMBER) ); : �N__mptr����} � MEMBER�P���c�_�l�I������(�Q��offsetof) = �ӵ��c���_�l�I��}�C
*/


struct list_head {
	struct list_head *next, *prev;
};

// �R�A��l��
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
			struct list_head name = LIST_HEAD_INIT(name)		// �ŧi�@��struct list_head�����c�ܼƨç@��l�ơA�N next �P prev ����ۤv�A�N���ۦ�list���Ū��C
// LIST_HEAD(name) : struct list_head name = { &(name), &(name) }�A�� name addr = next �M prev


// �ʺA��l��
void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

// �ˬd�o�Ӧ�C�O�_����
int list_empty(const struct list_head *head)
{
	return head->next == head;		// �� LIST_HEAD(name) ���p�@�ˡAname addr = next ��ܦ�list���Ū��C
}

// �N�@�ӷs���`�I�[�J�@��w�����e(prev)/��(next)�ݸ`�I�����A�]�N�O���쥻�۳s��2�Ӻ��I�����|���J�@�ӷs�`�I�C
static  void __list_add(struct list_head *new_lst,
						struct list_head *prev,
						struct list_head *next)
{
	next->prev = new_lst;
	new_lst->next = next;
	new_lst->prev = prev;
	prev->next = new_lst;
}

// ���@�ӷs���`�I�[�J��C���}�Y�AStack(LIFO)
void list_add(struct list_head *new_lst,
              struct list_head *head)
{
	__list_add(new_lst, head, head->next);
}

// ���@�ӷs���`�I�[�J��C���ݡAQueue(FIFO)
void list_add_tail(struct list_head *new_lst, struct list_head *head)
{
	__list_add(new_lst, head->prev, head);
}

// ���X����쵲��C�Apos���@�ӫ��Шë��V�Ĥ@�Ӷ���(head->next)�A�H�����_�I�@���X�ݤU�h�A�פ����O��pos���Vhead�ɡA�N��w�g���X���@��F�C
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

// ����`�I�������ƾڵ��c
#define list_entry(ptr, type, member)\
    container_of(ptr, type, member)

/*
// �R���Y�@�Ӹ`�Ientry�A�o�ݭn�����Dentry���e(prev)/��(next)�ݸ`�I�C
static void __list_del(struct list_head * prev,
						struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

// �w�q�blinux/poison.h��
#define LIST_POISON1  NULL	// ((void *) 0x00100100 + POISON_POINTER_DELTA)
#define LIST_POISON2  NULL	// ((void *) 0x00200200 + POISON_POINTER_DELTA)

void list_del(struct list_head * entry)
{
	__list_del(entry->prev,entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

struct MY_DATA {
   int Data_A;		// �ƾڰ�(int Data_A, Data_B, Data_C)�ھڨϥ�������H������ݨD�өw
   int Data_B;
   int Data_C;
   struct list_head list;	// Linux kernel �� Linked list ���w�q"���а�(struct list_head list;)"
};

// �X�ݾ�Ӧ�C�æL�X��C����address�Mdata���e�C
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

// �q���ݥ[�J�@�ӷs����C��
struct My_DATA *add_Node_tail(struct list_head *head){
	struct My_DATA *entry;
	entry=(struct My_DATA*)malloc(sizeof(struct My_DATA));
	list_add_tail( &entry->list,head);
	return entry;
}

// �q�e�ݥ[�J�@�ӷs����C��
struct My_DATA *add_Node(struct list_head *head){
	struct My_DATA *entry;
	entry = (struct My_DATA*)malloc(sizeof(struct My_DATA));
	list_add( &entry->list,head);
	return entry;
}

// �R����@�Ӧ�C���H�ξP����Ӧ�C�G
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

