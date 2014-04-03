#ifndef _EVENTS_C_
#define _EVENTS_C_

#define MAX_EVENTS 32

#include "list.c"

typedef struct tagEventEntry
{
	ListItem listItem;
	ENTITY *ent;
	void *eventHandlers[MAX_EVENTS];
} EventEntry;

List *eventList = NULL;

function event_set(ENTITY *ent, int eventType, void *eventHandler)
{
	EventEntry *entry = NULL;
	for(entry = list_first(eventList); entry != NULL; entry = list_next(eventList, entry))
	{
		if(entry->ent == ent)
		{
			entry->eventHandlers[eventType] = eventHandler;
			return;
		}
	}
	entry = sys_malloc(sizeof(EventEntry));
	entry->ent = ent;
	entry->eventHandlers[eventType] = eventHandler;
	list_insert_last(eventList, entry);
}

function event_call(ENTITY *ent, int eventType, int evParam)
{
	EventEntry *entry = NULL;
	for(entry = list_first(eventList); entry != NULL; entry = list_next(eventList, entry))
	{
		if(entry->ent == ent)
		{
			function evHandler(int evParam);
			evHandler = entry->eventHandlers[eventType];
			if(evHandler == NULL) return;
			
			me = ent;
			evHandler(evParam);
			me = NULL;			
			return;
		}
	}
}


function events_init()
{
	eventList = list_create();
}

#endif