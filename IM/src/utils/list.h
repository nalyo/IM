#pragma once
struct list_head
{
	struct list_head* next, * prev;
};

#define LIST_POSITION ((void *) 0x0)

#define LIST_HEAD_INIT(name)  { &(name), &(name)}
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)
#define INIT_LIST_HEAD(ptr) \
	do{(ptr)->next=(ptr);(ptr)->prev=(ptr); \
} while(0)

#define list_for_each(pos, head) \
	for(pos = (head)->next; pos != (head); pos = pos->next)
/* 安全遍历，适合在循环里删除节点 */
#define list_for_each_safe(pos, tmp, head) \
    for (pos = (head)->next, tmp = pos->next; pos != (head); \
         pos = tmp, tmp = pos->next)

#define list_for_each_prev(pos, head) \
	for(pos = (head)->prev; pos != (head); pos = pos->prev)

//#define offsetof(t, m) \
//	((size_t)(&((t *)0) -> m))

#define container_of(ptr, type, member) \
	(((char *)(ptr)) - offsetof(type, member))

#define list_entry(ptr, type, member) \
	(type*)container_of(ptr, type, member)

void __list_add(struct list_head* node, struct list_head* prev, struct list_head* next);
void __list_del(struct list_head* prev, struct list_head* next);
void list_add(struct list_head* node, struct list_head* head);
void list_del(struct list_head* entry);
void list_del_init(struct list_head* entry);
void list_add_tail(struct list_head* node, struct list_head* head);
int list_empty(const struct list_head* head);