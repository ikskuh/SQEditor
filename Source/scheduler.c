#ifndef _SCHEDULER_C_
#define _SCHEDULER_C_

#include "list.c"

typedef struct tagSchedulerEntry
{
	ListItem list;
	ENTITY *ent;
	void *func;
	int enabled;
} SchedulerEntry;

List *schedulerList = NULL;
SchedulerEntry *schedCurrentEntry = NULL;

SchedulerEntry *scheduler_add(void *func, ENTITY *ent)
{
	SchedulerEntry *entry = sys_malloc(sizeof(SchedulerEntry));
	entry->func = func;
	entry->ent = ent;
	entry->list.next = NULL;
	entry->list.previous = NULL;
	entry->enabled = 1;
	
	list_insert_last(schedulerList, entry);
	
	return entry;
}

SchedulerEntry *scheduler_remove(SchedulerEntry *entry)
{
	return list_remove(schedulerList, entry);
}

void scheduler_remove_func(void *func)
{
	for(schedCurrentEntry = list_first(schedulerList); schedCurrentEntry != NULL; )
	{
		if(schedCurrentEntry->func == func)
		{
			schedCurrentEntry = scheduler_remove(schedCurrentEntry);
		}
		else
		{
			schedCurrentEntry = list_next(schedulerList, schedCurrentEntry);
		}
	}
	schedCurrentEntry = NULL;
}

void scheduler_remove_ent(ENTITY *ent)
{
	for(schedCurrentEntry = list_first(schedulerList); schedCurrentEntry != NULL; )
	{
		if(schedCurrentEntry->ent == ent)
		{
			schedCurrentEntry = scheduler_remove(schedCurrentEntry);
		}
		else
		{
			schedCurrentEntry = list_next(schedulerList, schedCurrentEntry);
		}
	}
	schedCurrentEntry = NULL;
}

void scheduler_remove_ent_func(ENTITY *ent, void *func)
{
	for(schedCurrentEntry = list_first(schedulerList); schedCurrentEntry != NULL; )
	{
		if(schedCurrentEntry->ent == ent && schedCurrentEntry->func == func)
		{
			schedCurrentEntry = scheduler_remove(schedCurrentEntry);
		}
		else
		{
			schedCurrentEntry = list_next(schedulerList, schedCurrentEntry);
		}
	}
	diag("g");
	schedCurrentEntry = NULL;
}

function scheduler_init()
{
	schedulerList = list_create();
}

function scheduler_run()
{
	schedCurrentEntry = list_first(schedulerList);
	while(schedCurrentEntry)
	{
		if(schedCurrentEntry->func && schedCurrentEntry->enabled)
		{
			me = schedCurrentEntry->ent;
			my = schedCurrentEntry->ent;
			function sched_action();
			sched_action = schedCurrentEntry->func;
			sched_action();
			me = NULL;
			my = NULL;
		}
		// Remove entry
		if(schedCurrentEntry->func == NULL)
		{
			schedCurrentEntry = scheduler_remove(schedCurrentEntry);
		}
		else
		{
			schedCurrentEntry = list_next(schedulerList, schedCurrentEntry);
		}
	}
	schedCurrentEntry = NULL;
}

function scheduler_setnext(void *func)
{
	if(schedCurrentEntry == NULL) return;
	schedCurrentEntry->func = func;
}

function scheduler_setme(ENTITY *ent)
{
	if(schedCurrentEntry == NULL) return;
	schedCurrentEntry->ent = ent;
}

#endif