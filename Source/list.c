#ifndef _LIST_C_
#define _LIST_C_

typedef struct tagListItem
{
	struct tagListItem *previous;
	struct tagListItem *next;
} ListItem;

typedef struct tagList
{
	struct tagListItem *first;
	struct tagListItem *last;
} List;

List *list_create()
{
	List *list = sys_malloc(sizeof(List));
	list->first = NULL;
	list->last = NULL;	
	return list;
}

ListItem *list_remove(List *list, ListItem *entry)
{
	if(entry->previous)
	{
		entry->previous->next = entry->next;
	}
	if(entry->next)
	{
		entry->next->previous = entry->previous;
	}
	if(entry == list->last)
	{
		list->last = entry->previous;
	}
	if(entry == list->first)
	{
		list->first = entry->next;
	}
	ListItem *next = entry->next;
	sys_free(entry);
	return next;	
}

void list_free(List *list)
{
	if(list == NULL) return;
	ListItem *entry = list->first;
	while(entry)
	{
		ListItem *e = entry;
		entry = entry->next;
		sys_free(e);
	}
	sys_free(list);
}

void list_insert_last(List *list, ListItem *entry)
{
	if(list == NULL) return;
	if(entry == NULL) return;
	
	if(list->last)
	{
		list->last->next = entry;
		entry->previous = list->last;
	}
	else
	{
		list->first = entry;
	}
	list->last = entry;
}

void list_insert_first(List *list, ListItem *entry)
{
	if(list == NULL) return;
	if(entry == NULL) return;
	if(list->first)
	{
		list->first->previous = entry;
		entry->next = list->first;
	}
	else
	{
		list->last = entry;
	}
	list->first = entry;
}

ListItem *list_next(List *list, ListItem *entry)
{
	if(list == NULL) return NULL;
	if(entry == NULL) return NULL;
	return entry->next;
}

ListItem *list_previous(List *list, ListItem *entry)
{
	if(list == NULL) return NULL;
	if(entry == NULL) return NULL;
	return entry->previous;
}

ListItem *list_first(List *list)
{
	if(list == NULL) return NULL;
	return list->first;
}

ListItem *list_last(List *list)
{
	if(list == NULL) return NULL;
	return list->last;
}

#endif