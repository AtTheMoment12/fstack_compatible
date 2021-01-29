#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "msgList.h"

/*init list*/
void list_init(struct list *list)
{
	list->next = list;
	list->prev = list;
}
/*check list is empty or not*/
int list_empty(struct list *list)
{
	return list->next == list;
}

/*insert before link */
void list_insert(struct list *link, struct list *new_link)
{
	new_link->prev = link->prev;
	new_link->next = link;
	new_link->prev->next	= new_link;
	new_link->next->prev	= new_link;
}
/*insert new link to tail*/
void list_append(struct list *list, struct list *new_link)
{
	list_insert((struct list *)list, new_link);
}
/*insert new link to head after list*/
void list_prepend(struct list *list, struct list *new_link)
{
	list_insert(list->next, new_link);
}
/*delete this link*/
void list_remove(struct list *link)
{
	link->prev->next = link->next;
	link->next->prev = link->prev;
}

