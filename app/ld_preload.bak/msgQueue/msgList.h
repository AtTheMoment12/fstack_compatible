#ifndef __LIST_H__
#define __LIST_H__

struct list {
	struct list *next;
	struct list *prev;
};
struct list_x {
	struct list *next;
	struct list *prev;
};
/*init list*/
void list_init(struct list *list);
/*free list*/
int list_empty(struct list *list);

/*insert before link */
void list_insert(struct list *link, struct list *new_link);
/*insert before link */
void list_append(struct list *list, struct list *new_link);
/*insert after link*/
void list_prepend(struct list *list, struct list *new_link);
/*delete this link*/
void list_remove(struct list *link);

#define list_entry(link, type, member) \
	((type *)((char *)(link)-(unsigned long)(&((type *)0)->member)))

/*get list head*/
#define list_head(list, type, member)		\
	list_entry((list)->next, type, member)

#define list_tail(list, type, member)		\
	list_entry((list)->prev, type, member)

#define list_next(elm, member)					\
	list_entry((elm)->member.next, typeof(*elm), member)

#define list_for_each_entry(pos, list, member)			\
	for (pos = list_head(list, typeof(*pos), member);	\
	     &pos->member != (list);				\
	     pos = list_next(pos, member))

#endif