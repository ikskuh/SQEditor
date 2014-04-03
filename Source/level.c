#ifndef _LEVEL_C_
#define _LEVEL_C_

#include <windows.h>
#include "scheduler.c"
#include "ini.c"

#define GROUP_LEVEL 1
#define GROUP_LIGHT 2
#define GROUP_FOG 3
#define GROUP_EDITOR 4
#define GROUP_IGNORE_ALWAYS 5

#define LIGHTTYPE skill[80]
	#define LIGHT_POINT			0
	#define LIGHT_SPOT			1
	#define LIGHT_PROJECTION	2

#define SHADEQ_LEVEL_EDITOR -1


typedef struct tagActionInformation
{
	ListItem list;
	char actionname[64];
} ActionInformation;

typedef struct tagRegAction
{
	ListItem list;
	char actionname[64];
	void *ptr;
} RegAction;

List *registeredActions = NULL;

RegAction *action_register(STRING *name, void *ptr)
{
	if(registeredActions == NULL) registeredActions = list_create();
	
	RegAction *info = NULL;
	for(info = list_first(registeredActions); info != NULL; info = list_next(registeredActions, info))
	{
		if(strcmp(info->actionname, _chr(name)) == 0)
			return NULL;
	}
	info = sys_malloc(sizeof(RegAction));
	strcpy(info->actionname, _chr(name));
	info->ptr = ptr;
	list_insert_last(registeredActions, info);
	return info;
}

void *action_getptr(STRING *name)
{
	if(registeredActions == NULL) registeredActions = list_create();
	
	RegAction *info = NULL;
	for(info = list_first(registeredActions); info != NULL; info = list_next(registeredActions, info))
	{
		if(strcmp(info->actionname, _chr(name)) == 0)
			return info->ptr;
	}
	return NULL;
}

ActionInformation *actinfo_add(List *list, STRING *name)
{
	ActionInformation *info = NULL;
	for(info = list_first(list); info != NULL; info = list_next(list, info))
	{
		if(strcmp(info->actionname, _chr(name)) == 0)		
			return NULL;
	}
	info = sys_malloc(sizeof(ActionInformation));
	strcpy(info->actionname, _chr(name));
	list_insert_last(list, info);
	return info;
}

void *actinfo_remove(List *list, STRING *name)
{
	ActionInformation *info = NULL;
	for(info = list_first(list); info != NULL; )
	{
		if(strcmp(info->actionname, _chr(name)) == 0)
			info = list_remove(list, info);
		else
			info = list_next(list, info);
	}
}

void *actinfo_clear(List *list)
{
	ActionInformation *info = NULL;
	for(info = list_first(list); info != NULL; )
	{
		info = list_remove(list, info);
	}
}

void *qlevel_entity_init = NULL;
void *qlevel_pointlight_init = NULL;
void *qlevel_spotlight_init = NULL;
void *qlevel_projlight_init = NULL;

function ent_setminmax(ENTITY *ent)
{
	VECTOR pos;
	vec_set(&ent->max_x, vector(-9999999, -9999999, -9999999));
	vec_set(&ent->min_x, vector(9999999, 9999999, 9999999));
	int i;
	CONTACT c;
	for(i = 1; i <= ent_status(ent, 0); i++)
	{
		VECTOR pos;
		ent_getvertex(ent, &c, i);
		pos.x = c.v->x;
		pos.y = c.v->z;
		pos.z = c.v->y;
		//vec_mul(&pos, &ent->scale_x);
		
		ent->max_x = maxv(pos.x, ent->max_x);
		ent->max_y = maxv(pos.y, ent->max_y);
		ent->max_z = maxv(pos.z, ent->max_z);
		ent->min_x = minv(pos.x, ent->min_x);
		ent->min_y = minv(pos.y, ent->min_y);
		ent->min_z = minv(pos.z, ent->min_z);
	}
}

void qlevel_load(STRING *filename)
{
	STRING *file = "#256";
	make_path(file, filename);
	if(!file_exists(file))
	{
		error("FIXIT! qlevel_load: File does not exist!");
		return;
	}
		
	level_load(NULL);
	
	int countEntity = ini_read_int(file, "Level Information", "EntityCount", 0);
	int countLight = ini_read_int(file, "Level Information", "LightCount", 0);
	int countFog = ini_read_int(file, "Level Information", "FogCount", 0);
	
	STRING *section = "#64";
	
	int i;
	char buffer[256];
	STRING *strTemp = "#128";
	for(i = 0; i < countEntity; i++)
	{
		diag("\nLoad :");
		str_cpy(section, "Entity ");
		str_cat(section, str_for_int(NULL, i));
		diag(section);
		
		ini_read_buffer(file, section, "Model", "error.mdl", buffer, 256);
		ENTITY *ent = ent_create(buffer, nullvector, qlevel_entity_init);
		ent->group = GROUP_LEVEL;
		ent->x = ini_read_float(file, section, "X", 0);
		ent->y = ini_read_float(file, section, "Y", 0);
		ent->z = ini_read_float(file, section, "Z", 0);
		
		ent->pan = ini_read_float(file, section, "Pan", 0);
		ent->tilt = ini_read_float(file, section, "Tilt", 0);
		ent->roll = ini_read_float(file, section, "Roll", 0);
		
		ent->scale_x = ini_read_float(file, section, "ScaleX", 0);
		ent->scale_y = ini_read_float(file, section, "ScaleY", 0);
		ent->scale_z = ini_read_float(file, section, "ScaleZ", 0);
		
		ent->lightrange = ini_read_float(file, section, "Lightrange", 0);
		
		int iActionCount = ini_read_int(file, section, "ActionCount", 0);
		int iA = 0;
		STRING *actionName = "#64";
		for(iA = 0; iA < iActionCount; iA++)
		{
			str_cpy(actionName, "Action");
			str_cat(actionName, str_for_int(NULL, iA));
			ini_read_buffer(file, section, actionName, "", buffer, 256);
			str_cpy(strTemp, buffer);
			diag("\nTry adding action '");
			diag(strTemp);
			diag("'...");
			if(actinfo_add(ent->string1, strTemp))
			{
				diag(" SUCCESS!\n\tTry getting function pointer...");
				void *fn = action_getptr(strTemp);
				if(fn != NULL)
				{
					diag(" SUCCESS!");
					scheduler_add(fn, ent);
				}
				else
				{
					diag(" FAILED!");				
				}
			}
			else
			{
				diag(" FAILED!");
			}
		}
		diag("\nEntity loaded!");
	}
}

void qlevel_save(STRING *filename)
{
	STRING *file = "#256";
	make_path(file, filename);
	
	STRING *backup = "#256";
	str_cpy(backup, file);
	str_cat(backup, ".bak");
	file_delete(backup);
	file_rename(file, backup);
	
	ini_write_int(file, "Level Information", "EntityCount", 0);
	ini_write_int(file, "Level Information", "LightCount", 0);
	ini_write_int(file, "Level Information", "FogCount", 0);
	
	int countEntity = 0;
	int countLight = 0;
	int countFog = 0;
	
	STRING *section = "#64";
	for(you = ent_next(NULL); you != NULL; you = ent_next(you))
	{
		switch(you->group)
		{
			case GROUP_LEVEL:
				str_cpy(section, "Entity ");
				str_cat(section, str_for_int(NULL, countEntity));
				
				ini_write_string(file, section, "Model", you->type);
				ini_write_int(file, section, "ActionCount", 0);
				int actionCount = 0;
				STRING *actionName = "#64";
				ActionInformation *entry;
				for(entry = list_first(you->string1); entry != NULL; entry = list_next(you->string1, entry))
				{
					str_cpy(actionName, "Action");
					str_cat(actionName, str_for_int(NULL, actionCount));
					ini_write_string(file, section, actionName, _str(entry->actionname));
					actionCount++;
				}
				ini_write_int(file, section, "ActionCount", actionCount);
				
				ini_write_float(file, section, "X", you->x);
				ini_write_float(file, section, "Y", you->y);
				ini_write_float(file, section, "Z", you->z);
				ini_write_float(file, section, "Pan", cycle(you->pan, 0, 360));
				ini_write_float(file, section, "Tilt", cycle(you->tilt, 0, 360));
				ini_write_float(file, section, "Roll", cycle(you->roll, 0, 360));
				ini_write_float(file, section, "ScaleX", you->scale_x);
				ini_write_float(file, section, "ScaleY", you->scale_y);
				ini_write_float(file, section, "ScaleZ", you->scale_z);
				ini_write_float(file, section, "Lightrange", you->lightrange);

				countEntity++;
				break;
		}
	}
	
	ini_write_int(file, "Level Information", "EntityCount", countEntity);
	ini_write_int(file, "Level Information", "LightCount", countLight);
	ini_write_int(file, "Level Information", "FogCount", countFog);
}

#define QLEVEL_VERSION	0x01

typedef struct
{
	int a, b, c;
	int fileversion;
} TQLevelHeader;

typedef struct
{
	int type;
	int length;
} TQLevelItemHeader;

typedef struct
{
	char filename[128];
	VECTOR pos;
	VECTOR angle;
	VECTOR scale;
	int actioncount;
} TQLevelEntity;

typedef struct 
{
	char actionname[64];
} TQLevelAction;

typedef struct
{
	VECTOR pos;
	VECTOR angle;
	VECTOR scale;
	COLOR color;
	var range;
	var type;
	var spotlightCone;
	var spotlightFalloff;
	char lightmap[64];
} TQLevelLight;

void qlevel_export(char *filename)
{
	FILE *f = fopen(filename, "wb+");
	
	TQLevelHeader header;
	header.a = 42;
	header.b = 1337;
	header.c = -55;
	fwrite(&header, sizeof(TQLevelHeader), 1, f);
	fflush(f);
	
	TQLevelItemHeader item;
	for(you = ent_next(NULL); you != NULL; you = ent_next(you))
	{
		switch(you->group)
		{
			case GROUP_LEVEL:
				item.type = GROUP_LEVEL;
				item.length = sizeof(TQLevelEntity);
				fwrite(&item, sizeof(TQLevelItemHeader), 1, f);
				
				TQLevelEntity entityEntry;
				strcpy(entityEntry.filename, _chr(you->type));
				vec_set(&entityEntry.pos, &you->x);
				vec_set(&entityEntry.angle, &you->pan);
				vec_set(&entityEntry.scale, &you->scale_x);
				entityEntry.actioncount = 0;
				ActionInformation *entry;
				for(entry = list_first(you->string1); entry != NULL; entry = list_next(you->string1, entry))
				{
					entityEntry.actioncount++;
				}
				fwrite(&entityEntry, sizeof(TQLevelEntity), 1, f);
				
				ActionInformation *entry;
				for(entry = list_first(you->string1); entry != NULL; entry = list_next(you->string1, entry))
				{
					TQLevelAction entryAction;
					strcpy(entryAction.actionname, entry->actionname);
					fwrite(&entryAction, sizeof(TQLevelAction), 1, f);					
				}
				break;
			case GROUP_LIGHT:
				item.type = GROUP_LIGHT;
				item.length = sizeof(TQLevelLight);
				fwrite(&item, sizeof(TQLevelItemHeader), 1, f);
				
				TQLevelLight lightEntry;
				
				vec_set(&lightEntry.pos, &you->x);
				vec_set(&lightEntry.angle, &you->pan);
				vec_set(&lightEntry.color, &you->blue);
				lightEntry.range = you->lightrange;
				
				if(you->string2 != NULL)
				{
					strcpy(lightEntry.lightmap, _chr(you->string2));
				}
				
				fwrite(&lightEntry, sizeof(TQLevelLight), 1, f);	
				break;
		}
	}
	
	item.type = -1;
	item.length = 0;
	fwrite(&item, sizeof(TQLevelItemHeader), 1, f);
	
	fclose(f);
}

#endif