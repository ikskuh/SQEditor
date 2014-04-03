#include <acknex.h>
#include <windows.h>
#include <AckPhysX.h>
#include <strio.c>
#define PRAGMA_PATH "Source"
#define PRAGMA_PATH "Images"
#define PRAGMA_PATH "Lightmaps"
#define PRAGMA_PATH "Models"
#define PRAGMA_PATH "Models\LevelEditor"
#define PRAGMA_PATH "Textures"
#define PRAGMA_PATH "Textures\NormalMaps"
#define PRAGMA_PATH "Textures\SpecularityMaps"
#define PRAGMA_PATH "Billboards"

#define PRAGMA_POINTER

typedef void *HMONITOR;

typedef struct tagMONITORINFO
{
	DWORD cbSize;
	RECT  rcMonitor;
	RECT  rcWork;
	DWORD dwFlags;
} MONITORINFO;

long WINAPI MonitorFromWindow(long hwnd,long dwFlags);
#define PRAGMA_API MonitorFromWindow;user32!MonitorFromWindow

BOOL WINAPI PathIsRelative(char *path);
#define PRAGMA_API PathIsRelative;Shlwapi!PathIsRelativeA 

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

#include "gui.c"

////////////////////////////////////////////////////////////////////////////
//			Skill Usage
////////////////////////////////////////////////////////////////////////////
/*
ENTITY
	string1		List with assigned actions
	string2		Filename of the assigned lightmap
	skill[80]	Light type
					0	Pointlight
					1	Spotlight
					2	Projection Light


*/

////////////////////////////////////////////////////////////////////////////
//			Editor Defines
////////////////////////////////////////////////////////////////////////////

#define EDIT_MAX_SELECTION 10

#define EDIT_MOVE 0
#define EDIT_ROTATE 1
#define EDIT_SCALE 2

#include "scheduler.c"

int level_index = 0;

var debug_panel_visible = 0;
function update_debug_panel();
function toggle_debug_panel();
function quit();

function diagt(STRING *str)
{
	diag("\n");
	diag(str_for_num(NULL, total_secs));
	diag(":\t");
	diag(str);
}

////////////////////////////////////////////////////////////////////////////
//			Game related
////////////////////////////////////////////////////////////////////////////

ENTITY *entSkycube;
BMAP *bmpSplashcreen = "splashscreen.png";

////////////////////////////////////////////////////////////////////////////
//			UI Windows
////////////////////////////////////////////////////////////////////////////
PANEL *editWindowObjectEditor = NULL;
PANEL *editWindowEditorSettings = NULL;
PANEL *editWindowActionEditor = NULL;

PANEL *editListboxFileList = NULL;
PANEL *editListboxActions = NULL;
PANEL *editListboxLightmaps = NULL;

PANEL *editPreviewLightmap = NULL;
VIEW *editPreviewModel = NULL;

PANEL *editMenuEntryFile = NULL;
PANEL *editMenuEntryEdit = NULL;
PANEL *editMenuEntryWindows = NULL;
PANEL *editMenuEntryTools = NULL;
PANEL *editMenuEntryAbout = NULL;

////////////////////////////////////////////////////////////////////////////
//			Editor Control Variables
////////////////////////////////////////////////////////////////////////////

var edit_loading = 1;
var loading_progress = 0;

TEXT *editTextFilelist = NULL;
TEXT *editTextActionlist = NULL;
TEXT *editTextLightmaplist = NULL;
ENTITY *editSelection[EDIT_MAX_SELECTION];
PANEL *editPanSelection[EDIT_MAX_SELECTION];

var editRawGridsize = 4;
var editGridsize;
var editRawRotosize = 6;
var editRotosize;
var editRawScalesize = 4;
var editScalesize;

var editGridVisible = 1;

ENTITY *entEditAxisX = NULL;
ENTITY *entEditAxisY = NULL;
ENTITY *entEditAxisZ = NULL;

var editMoveSpeed = 5;
var editGizmoMode = EDIT_MOVE;

STRING *editStrSelectionModel = "#64";
STRING *editStrSelectionX = "#8";
STRING *editStrSelectionY = "#8";
STRING *editStrSelectionZ = "#8";
STRING *editStrSelectionPan = "#8";
STRING *editStrSelectionTilt = "#8";
STRING *editStrSelectionRoll = "#8";
STRING *editStrSelectionScale = "#8";

var editLightrange = 0;

ENTITY* entEditPreviewModel = NULL;

STRING *editStrCurrentLevel = "#64";

#include "level.c"

int editActiveShadeQLevel = SHADEQ_LEVEL_EDITOR;

function a_rotator()
{
	my->pan += 2 * time_step;
}



function entity_init()
{
	my->string1 = list_create();
	my->string2 = str_create("#128");
	my->flags |= FLAG1;
	wait(1);
	ent_setminmax(me);
}
function txt_loaddir_fast(TEXT* text, STRING *filter)
{
	int counter = 0;
	WIN32_FIND_DATA finddata;
	HANDLE myHandle = FindFirstFile(_chr(filter), &finddata);
	if(myHandle == INVALID_HANDLE_VALUE)
		return 0;
	(text->pstring)[counter] = str_create(finddata.cFileName);
	counter += 1;
	while(1)
	{
		if(FindNextFile(myHandle, &finddata))
		{
			(text->pstring)[counter] = str_create(finddata.cFileName);
			counter += 1;
		}
		else
		{
			break;
		}
	}
	FindClose(myHandle);
	return counter;
}
function ent_drawbounds_line(VECTOR *from, VECTOR *to, ENTITY* ent)
{
	VECTOR f,t;
	vec_set(&f, from);
	vec_mul(&f, &ent->scale_x);
	vec_rotate(&f, &ent->pan);
	vec_add(&f, &ent->x);
	vec_set(&t, to);
	vec_mul(&t, &ent->scale_x);
	vec_rotate(&t, &ent->pan);
	vec_add(&t, &ent->x);
	
	draw_line3d(&f, NULL, 100);
	draw_line3d(&f, COLOR_BLUE, 100);
	draw_line3d(&t, COLOR_BLUE, 100);
}

function ent_drawbounds(ENTITY *ent)
{
	ent_drawbounds_line(vector(ent->max_x,ent->max_y,ent->max_z), vector(ent->min_x,ent->max_y,ent->max_z), ent);
	ent_drawbounds_line(vector(ent->max_x,ent->max_y,ent->max_z), vector(ent->max_x,ent->min_y,ent->max_z), ent);
	ent_drawbounds_line(vector(ent->max_x,ent->max_y,ent->max_z), vector(ent->max_x,ent->max_y,ent->min_z), ent);
	ent_drawbounds_line(vector(ent->min_x,ent->min_y,ent->min_z), vector(ent->max_x,ent->min_y,ent->min_z), ent);
	ent_drawbounds_line(vector(ent->min_x,ent->min_y,ent->min_z), vector(ent->min_x,ent->max_y,ent->min_z), ent);
	ent_drawbounds_line(vector(ent->min_x,ent->min_y,ent->min_z), vector(ent->min_x,ent->min_y,ent->max_z), ent);
	
	ent_drawbounds_line(vector(ent->max_x,ent->min_y,ent->max_z), vector(ent->max_x,ent->min_y,ent->min_z), ent);
	ent_drawbounds_line(vector(ent->max_x,ent->max_y,ent->min_z), vector(ent->min_x,ent->max_y,ent->min_z), ent);
	ent_drawbounds_line(vector(ent->min_x,ent->max_y,ent->max_z), vector(ent->min_x,ent->max_y,ent->min_z), ent);
	ent_drawbounds_line(vector(ent->min_x,ent->max_y,ent->max_z), vector(ent->min_x,ent->min_y,ent->max_z), ent);
	ent_drawbounds_line(vector(ent->max_x,ent->min_y,ent->max_z), vector(ent->min_x,ent->min_y,ent->max_z), ent);
	ent_drawbounds_line(vector(ent->max_x,ent->min_y,ent->min_z), vector(ent->max_x,ent->max_y,ent->min_z), ent);	
}

function edit_recreate_level();

function edit_export_level()
{
	char* levelfile = file_dialog_save("Export Level","*.sqlvl");
	if (levelfile)
	{
		qlevel_export(levelfile);
		g_msgbox("Hotel California - Editor", "Level exportet!", GMB_OK);
	}
}

function edit_save_level_as()
{
	char* levelfile = file_dialog_save("Save Level","*.ini");
	if (levelfile)
	{
		str_cpy(editStrCurrentLevel, levelfile);
		qlevel_save(editStrCurrentLevel);
		g_msgbox("Hotel California - Editor", "Level saved!", GMB_OK);
	}
	
}

function edit_save_level()
{
	if(str_cmpi(editStrCurrentLevel, ""))
	{
		edit_save_level_as();
	}
	else
	{
		qlevel_save(editStrCurrentLevel);
		g_msgbox("Hotel California - Editor", "Level saved!", GMB_OK);
	}
}

function edit_quicksave()
{
	qlevel_save("Level\\quicksave.ini");
	g_msgbox("Hotel California - Editor", "Level saved!", GMB_OK);
}

function edit_open_level()
{
	g_msgbox("Hotel California - Editor", "Do you want to save the current level?", GMB_YES | GMB_NO);
	if(gMsgBoxResult == GMB_YES)
	{
		edit_save_level();
		while(proc_status(edit_save_level) > 0) wait(1);
	}
	char* levelfile = file_dialog("Open Level","*.ini");
	if (levelfile)
	{
		str_cpy(editStrCurrentLevel, levelfile);
		level_index++;
		wait(1);
		qlevel_load(editStrCurrentLevel);
		edit_recreate_level();
		g_msgbox("Hotel California - Editor", "Level loaded!", GMB_OK);
	}
}

function edit_quickload()
{
	level_index++;
	wait(1);
	qlevel_load("Level\\quicksave.ini");
	edit_recreate_level();
	g_msgbox("Hotel California - Editor", "Level loaded!", GMB_OK);
}

function edit_new_level()
{
	g_msgbox("Hotel California - Editor", "Do you want to create a new level?", GMB_YES | GMB_NO);
	if(gMsgBoxResult == GMB_YES)
	{
		wait(1);
		g_msgbox("Hotel California - Editor", "Do you want to save the current level?", GMB_YES | GMB_NO);
		if(gMsgBoxResult == GMB_YES)
		{
			edit_save_level();
			while(proc_status(edit_save_level) > 0) wait(1);
		}
		level_index++;
		wait(1);
		level_load(NULL);
		edit_recreate_level();
	}
}

function edit_select(ENTITY *ent)
{
	int i;
	editSelection[0] = ent;
	
	if(ent != NULL)
		editLightrange = ent->lightrange;
	
	for(i = 1; i < EDIT_MAX_SELECTION; i++)
	{
		editSelection[i] = NULL;
	}
}

int edit_selection_count()
{
	int i, count = 0;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] != NULL) count++;
	}
	return count;
}

function edit_toggle_selection(ENTITY *ent)
{
	if(ent == NULL) return;
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		// Check if entity is in selection then unselect it
		if(editSelection[i] == ent)
		{
			editSelection[i] = NULL;
			return;
		}
	}
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		// Check if entity is in selection then unselect it
		if(editSelection[i] == NULL)
		{
			editSelection[i] = ent;
			return;
		}
	}
}

function edit_cam_free()
{
	VECTOR force,speed,dist;
	ANGLE aforce,aspeed; 
	vec_zero(speed);
	vec_zero(aspeed);
	vec_zero(dist);
	while(1)
	{
		var speedfac = (1 + key_shift - 0.95 * key_ctrl);
		
		aforce.tilt = 5 * (mouse_right * mouse_force.y);
		aforce.pan = -5 * (mouse_right * mouse_force.x);
		aforce.roll = 0;
		vec_add(&camera->pan,vec_accelerate(&dist,&aspeed,&aforce,0.8));

		force.x = speedfac * 7 * (key_w - key_s);
		force.y = speedfac * 3 * (key_a - key_d);
		force.z = speedfac * 3 * (key_home - key_end);
		vec_accelerate(&dist,&speed,&force,0.5);
		vec_add(&camera->x,vec_rotate(&dist,&camera->pan));
		wait(1);
	}
}

function edit_create_model()
{
	STRING *file = (editTextFilelist->pstring)[g_listbox_getselection(editListboxFileList)];
	ENTITY *ent = ent_create(file, nullvector, entity_init);
	ent->group = GROUP_LEVEL;
	vec_fill(&ent->scale_x, 5);
	edit_select(ent);
}

function edit_clone_model()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		if(editSelection[i]->group == GROUP_LEVEL)
		{
			diagt("Clone Level Model...");
			ENTITY *ent = ent_create(editSelection[i]->type, &editSelection[i]->x, entity_init);
			ent->group = GROUP_LEVEL;
			vec_set(&ent->pan, &editSelection[i]->pan);
			vec_set(&ent->scale_x, &editSelection[i]->scale_x);
			
			diagt("Clone Actions...");
			// Copy actions:
			ActionInformation *info = NULL;
			for(info = list_first(editSelection[i]->string1); info != NULL; info = list_next(editSelection[i]->string1, info))
			{
				diagt(info->actionname);
				actinfo_add(ent->string1, info->actionname);
				scheduler_add(action_getptr(info->actionname), ent);
			}
			diagt("Done!");
		}
	}
}

function edit_morph_model()
{
	STRING *file = (editTextFilelist->pstring)[g_listbox_getselection(editListboxFileList)];
	if(edit_selection_count() > 1)
	{
		g_msgbox("Hotel California - Editor", "Do you want to morph all selected models?", GMB_YES | GMB_NO);
		if(gMsgBoxResult != GMB_YES)
		{
			return;
		}
	}
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		if(editSelection[i]->group != GROUP_LEVEL) continue;
		ent_morph(editSelection[i], file);
	}
}

function edit_delete_model()
{
	if(edit_selection_count() > 1)
	{
		g_msgbox("Hotel California - Editor", "Do you want to delete all selected models?", GMB_YES | GMB_NO);
		if(gMsgBoxResult != GMB_YES)
		{
			return;
		}
	}
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		if(editSelection[i]->group == GROUP_LEVEL)
		{
			scheduler_remove_ent(editSelection[i]);
		}
		ptr_remove(editSelection[i]);
		editSelection[i] = NULL;
	}
}

function edit_addaction()
{	
	int selectedAct = g_listbox_getselection(editListboxActions);
	STRING *actName = (editTextActionlist->pstring)[selectedAct];
	void *fn = action_getptr(actName);
	
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		if(editSelection[i]->group != GROUP_LEVEL) continue;
		if(actinfo_add(editSelection[i]->string1, actName))
		{
			if(fn != NULL)
				scheduler_add(fn, editSelection[i]);
		}
	}
}

function edit_removeaction()
{
	int selectedAct = g_listbox_getselection(editListboxActions);
	
	STRING *actName = (editTextActionlist->pstring)[selectedAct];
	void *fn = action_getptr(actName);
	
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		if(editSelection[i]->group != GROUP_LEVEL) continue;
		actinfo_remove(editSelection[i]->string1, actName);
		if(fn != NULL)
			scheduler_remove_ent_func(editSelection[i], fn);
	}
}

function edit_clearaction()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		if(editSelection[i]->group != GROUP_LEVEL) continue;
		actinfo_clear(editSelection[i]->string1);
		scheduler_remove_ent(editSelection[i]);
	}
}

function edit_mouse_select()
{
	int i;
	if(mouse_panel) return;
	VECTOR from, to;
	vec_set(&from, &mouse_pos);
	vec_set(&to, &mouse_pos);
	from.z = 0;
	to.z = 100000;
	vec_for_screen(&from, camera);
	vec_for_screen(&to, camera);
	c_ignore(GROUP_FOG, GROUP_LEVEL, GROUP_LIGHT, GROUP_IGNORE_ALWAYS, 0);
	if(c_trace(&from, &to, IGNORE_CONTENT | USE_POLYGON))
	{
		// Check if we got an editor thingy
		
		// Do movement/rotation
		if(edit_selection_count() == 0) return;
		VECTOR movAxis;
		VECTOR rotAxis;
		vec_set(&movAxis, nullvector);
		vec_set(&rotAxis, nullvector);
		ENTITY *ent = you;
		if(ent == entEditAxisX)
		{
			vec_set(&movAxis, vector(1, 0, 0));
			if(editGizmoMode == EDIT_ROTATE)
				vec_set(&movAxis, vector(0, 0, 1));
		}
		if(ent == entEditAxisY)
		{
			vec_set(&movAxis, vector(0, 1, 0));
		}
		if(ent == entEditAxisZ)
		{
			vec_set(&movAxis, vector(0, 0, 1));
			if(editGizmoMode == EDIT_ROTATE)
				vec_set(&movAxis, vector(1, 0, 0));
		}
		
		var movesnap = editGridsize;
		switch(editGizmoMode)
		{
			case EDIT_MOVE:
				movesnap = editGridsize;
				break;
			case EDIT_SCALE:
				movesnap = editScalesize;
				break;
			case EDIT_ROTATE:
				movesnap = editRotosize;
				break;
		}
		
		VECTOR movDir;
		VECTOR movOffset;
		POINT pMouse;
		GetCursorPos(&pMouse);
		vec_fill(&movOffset, 0);
		mouse_pointer = 0;
		while(mouse_left)
		{
			SetCursorPos(pMouse.x, pMouse.y);
			
			var speedfac = (1 + key_shift - 0.95 * key_ctrl);
			movDir.x = 0;
			movDir.y = -0.025 * speedfac * editMoveSpeed * mickey.x;
			movDir.z = -0.025 * speedfac * editMoveSpeed * mickey.y;
			vec_rotate(&movDir, &camera->pan);
			vec_mul(&movDir, &movAxis);
			if(movesnap == 0)
			{
				switch(editGizmoMode)
				{
					case EDIT_MOVE:
						for(i = 0; i < EDIT_MAX_SELECTION; i++)
						{
							if(editSelection[i] == NULL) continue;
							vec_add(&editSelection[i]->x, &movDir);
						}
						break;
					case EDIT_SCALE:
						var move = vec_length(&movDir);
						if(sign(movDir.x)) move *= sign(movDir.x);
						if(sign(movDir.y)) move *= sign(movDir.y);
						if(sign(movDir.z)) move *= sign(movDir.z);
						
						for(i = 0; i < EDIT_MAX_SELECTION; i++)
						{
							if(editSelection[i] == NULL) continue;
							editSelection[i]->scale_x += move;
							editSelection[i]->scale_y += move;
							editSelection[i]->scale_z += move;
						}
						break;
					case EDIT_ROTATE:
						for(i = 0; i < EDIT_MAX_SELECTION; i++)
						{
							if(editSelection[i] == NULL) continue;
							vec_add(&editSelection[i]->pan, &movDir);
						}
						break;
				}
			}
			else
			{
				vec_add(&movOffset, &movDir);
				if(vec_length(&movOffset) > movesnap)
				{
					vec_normalize(&movOffset, movesnap);
					switch(editGizmoMode)
					{
						case EDIT_MOVE:
							for(i = 0; i < EDIT_MAX_SELECTION; i++)
							{
								if(editSelection[i] == NULL) continue;
								vec_add(&editSelection[i]->x, &movOffset);
							}
							break;
						case EDIT_SCALE:
							var move = vec_length(&movOffset);
							if(sign(movOffset.x)) move *= sign(movOffset.x);
							if(sign(movOffset.y)) move *= sign(movOffset.y);
							if(sign(movOffset.z)) move *= sign(movOffset.z);
							for(i = 0; i < EDIT_MAX_SELECTION; i++)
							{
								if(editSelection[i] == NULL) continue;
								editSelection[i]->scale_x += move;
								editSelection[i]->scale_y += move;
								editSelection[i]->scale_z += move;
							}
							break;
						case EDIT_ROTATE:
							for(i = 0; i < EDIT_MAX_SELECTION; i++)
							{
								if(editSelection[i] == NULL) continue;
								vec_add(&editSelection[i]->pan, &movOffset);
							}
							break;
					}
					vec_fill(&movOffset, 0);
				}
			}
			wait(1);
		}
		mouse_pointer = 1;
		return;
	}
	c_ignore(GROUP_EDITOR, GROUP_LIGHT, GROUP_IGNORE_ALWAYS, 0);
	if(c_trace(&from, &to, IGNORE_CONTENT | USE_POLYGON))
	{
		// Check if we got anything other
		if(key_shift)
			edit_toggle_selection(you);
		else
			edit_select(you);
		return;
	}
	if(!key_shift)
		edit_select(NULL);
}

function edit_mouse_togglemode()
{
	editGizmoMode = cycle(editGizmoMode + 1, 0, 3);
	if(key_1) editGizmoMode = EDIT_MOVE;
	if(key_2) editGizmoMode = EDIT_ROTATE;
	if(key_3) editGizmoMode = EDIT_SCALE;
	switch(editGizmoMode)
	{
		case EDIT_MOVE:
			ent_morph(entEditAxisX, "Axis_Move_X.mdl");
			ent_morph(entEditAxisY, "Axis_Move_Y.mdl");
			ent_morph(entEditAxisZ, "Axis_Move_Z.mdl");
			break;
		case EDIT_SCALE:
			ent_morph(entEditAxisX, "Axis_Scale_X.mdl");
			ent_morph(entEditAxisY, "Axis_Scale_Y.mdl");
			ent_morph(entEditAxisZ, "Axis_Scale_Z.mdl");
			break;
		case EDIT_ROTATE:
			ent_morph(entEditAxisX, "Axis_Rotate_Roll.mdl");
			ent_morph(entEditAxisY, "Axis_Rotate_Tilt.mdl");
			ent_morph(entEditAxisZ, "Axis_Rotate_Pan.mdl");
			break;
	}
}
function edit_recreate_level()
{	
	// Setup Axis
	entEditAxisX = ent_create("Axis_Move_X.mdl", nullvector, NULL);
	entEditAxisX->group = GROUP_EDITOR;
	entEditAxisX->flags |= ZNEAR;
	
	entEditAxisY = ent_create("Axis_Move_Y.mdl", nullvector, NULL);
	entEditAxisY->group = GROUP_EDITOR;
	entEditAxisY->flags |= ZNEAR;
	
	entEditAxisZ = ent_create("Axis_Move_Z.mdl", nullvector, NULL);
	entEditAxisZ->group = GROUP_EDITOR;
	entEditAxisZ->flags |= ZNEAR;
	
	entEditPreviewModel = ent_create("", vector(0, 0, -10000), NULL);
}

function edit_load_settings()
{
	STRING *file = "#256";
	str_cpy(file, work_dir);
	str_cat(file, "\\editor.ini");
	
	editRawGridsize = ini_read_float(file, "Grid", "SnapMove", editRawGridsize);
	editRawRotosize = ini_read_float(file, "Grid", "SnapRotate", editRawRotosize);
	editRawScalesize = ini_read_float(file, "Grid", "SnapScale", editRawScalesize);
	
	editGridVisible = ini_read_int(file, "Level", "GridVisible", editGridVisible);
}

function edit_save_settings()
{
	STRING *file = "#256";
	str_cpy(file, work_dir);
	str_cat(file, "\\editor.ini");
	
	ini_write_float(file, "Grid", "SnapMove", editRawGridsize);
	ini_write_float(file, "Grid", "SnapRotate", editRawRotosize);
	ini_write_float(file, "Grid", "SnapMove", editRawScalesize);
	
	ini_write_int(file, "Level", "GridVisible", editGridVisible);
}

function edit_save_viewport(int viewport)
{
	STRING *file = "#256";
	str_cpy(file, work_dir);
	str_cat(file, "\\editor.ini");
	
	STRING *section = "#64";
	str_cpy(section, "Viewport ");
	str_cat(section, str_for_int(NULL, viewport));
	
	ini_write_float(file, section, "X", camera->x);
	ini_write_float(file, section, "Y", camera->y);
	ini_write_float(file, section, "Z", camera->z);
	
	ini_write_float(file, section, "Pan", camera->pan);
	ini_write_float(file, section, "Tilt", camera->tilt);
	ini_write_float(file, section, "Roll", camera->roll);
}

function edit_load_viewport(int viewport)
{
	STRING *file = "#256";
	str_cpy(file, work_dir);
	str_cat(file, "\\editor.ini");
	
	STRING *section = "#64";
	str_cpy(section, "Viewport ");
	str_cat(section, str_for_int(NULL, viewport));
	
	camera->x = ini_read_float(file, section, "X", 0);
	camera->y = ini_read_float(file, section, "Y", 0);
	camera->z = ini_read_float(file, section, "Z", 0);
	
	camera->pan = ini_read_float(file, section, "Pan", 0);
	camera->tilt = ini_read_float(file, section, "Tilt", 0);
	camera->roll = ini_read_float(file, section, "Roll", 0);
}

function edit_take_screenshot()
{
	STRING *filename = "#128";
	str_cpy(filename, "Screenshots\\edit_");
	str_cat(filename, str_for_num(NULL, sys_year));
	str_cat(filename, "_");
	str_cat(filename, str_for_num(NULL, sys_month));
	str_cat(filename, "_");
	str_cat(filename, str_for_num(NULL, sys_day));
	str_cat(filename, "_");
	str_cat(filename, str_for_num(NULL, sys_hours));
	str_cat(filename, "_");
	str_cat(filename, str_for_num(NULL, sys_minutes));
	str_cat(filename, "_");
	str_cat(filename, ".png");
	
	file_for_screen(filename,sys_seconds);
}

void edit_randomize_pan()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		editSelection[i]->pan = random(360);
	}
}

void edit_randomize_tilt()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		editSelection[i]->tilt = random(360);
	}
}

void edit_randomize_roll()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		editSelection[i]->roll = random(360);
	}
}

void edit_subpan_90()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		editSelection[i]->pan -= 90;
	}
}

void edit_addpan_180()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		editSelection[i]->pan += 180;
	}
}

void edit_addpan_90()
{
	int i;
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		if(editSelection[i] == NULL) continue;
		editSelection[i]->pan += 90;
	}
}

function edit_init()
{
	diagt("Initializing Editor");
	
	diagt("Load model file list");
	editTextFilelist = txt_create(99999, -1);
	int fileCnt = txt_loaddir_fast(editTextFilelist, "Models\\*.mdl");
	
	
	diagt("Load lightmap file list");
	editTextLightmaplist = txt_create(99999, -1);
	int lightmapCnt = txt_loaddir_fast(editTextLightmaplist, "Lightmaps\\*.tga");
	
	
	diagt("Load action list");
	editTextActionlist = txt_create(99999, -1);
	int actionCnt = txt_load(editTextActionlist, "actions.list");
	
	STRING *actionIni = "#256";
	str_cpy(actionIni, work_dir);
	str_cat(actionIni, "\\actions.ini");
	
	diagt("Load last level");
	qlevel_load("Level\\quicksave.ini");
	edit_recreate_level();
	
	diagt("Load Editor-Settings");
	edit_load_settings();
	
	loading_progress++; wait(1);
	
	diagt("Create Main Window");
	
	editWindowEditorSettings = g_window_create(0, 0, 256, "Editor Settings", GWINDOW_MOVE | GWINDOW_HIDE); 
	
	g_window_addtext(editWindowEditorSettings, "Model Preview:");
	
	editPreviewModel = view_create(1);
	editPreviewModel->flags |= SHOW | NOFLAG1;
	editPreviewModel->pos_x = 0;
	editPreviewModel->pos_y = 0;
	editPreviewModel->size_x = 256;
	editPreviewModel->size_y = 192;
	editPreviewModel->z = entEditPreviewModel->z;
	editPreviewModel->x = -100;
	editPreviewModel->clip_near = 0;
	
	g_window_addview(editWindowEditorSettings, editPreviewModel, 192);
	
	g_window_addtext(editWindowEditorSettings, "Model Files:");
	editListboxFileList = g_window_addlist(editWindowEditorSettings, 384);
	int i;
	for(i = 0; i < fileCnt; i++)
	{
		g_listbox_add(editListboxFileList, (editTextFilelist->pstring)[i]);
	}
	gWriteY = 0;	// Prevent buttons from writing to y information
	g_window_addbutton(editWindowEditorSettings, "Create", 64, GALIGN_LEFT, 0, edit_create_model);
	g_window_addbutton(editWindowEditorSettings, "Morph", 64, GALIGN_LEFT, 64, edit_morph_model);
	g_window_addbutton(editWindowEditorSettings, "Clone", 64, GALIGN_RIGHT, -64, edit_clone_model);
	gWriteY = 1;	// Enable it for the last button again
	g_window_addbutton(editWindowEditorSettings, "Delete", 64, GALIGN_RIGHT, 0, edit_delete_model);
	g_window_addblank(editWindowEditorSettings);
	
	g_window_adddigits(editWindowEditorSettings, "Grid Snap: %3.0f", &editGridsize);
	g_window_addslider(editWindowEditorSettings, &editRawGridsize, 0, 7);
	g_window_adddigits(editWindowEditorSettings, "Rotation Snap: %3.1f", &editRotosize);
	g_window_addslider(editWindowEditorSettings, &editRawRotosize, 0, 6);
	g_window_adddigits(editWindowEditorSettings, "Scale Snap: %4.3f", &editScalesize);
	g_window_addslider(editWindowEditorSettings, &editRawScalesize, 0, 5);
	g_window_addtoggle(editWindowEditorSettings, "Grid Visible", &editGridVisible);
	g_window_addblank(editWindowEditorSettings);
	gWriteY = 0;
	g_window_addbutton(editWindowEditorSettings, "Quicksave", 96, GALIGN_LEFT, 0, edit_quicksave);
	gWriteY = 1;
	g_window_addbutton(editWindowEditorSettings, "Quickload", 96, GALIGN_RIGHT, 0, edit_quickload);
	
	g_window_dock(editWindowEditorSettings, GDOCK_LEFT | GDOCK_TOP);
	
	loading_progress++; wait(1);
	
	diagt("Create Object Editor");
	
	editWindowObjectEditor = g_window_create(0, 0, 256, "Object Editor", GWINDOW_MOVE | GWINDOW_HIDE);
	g_window_addtext(editWindowObjectEditor, "Model File:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionModel);
	g_window_addtext(editWindowObjectEditor, "Position.X:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionX);
	g_window_addtext(editWindowObjectEditor, "Position.Y:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionY);
	g_window_addtext(editWindowObjectEditor, "Position.Z:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionZ);
	g_window_addtext(editWindowObjectEditor, "Rotation.Pan:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionPan);
	g_window_addtext(editWindowObjectEditor, "Rotation.Tilt:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionTilt);
	g_window_addtext(editWindowObjectEditor, "Rotation.Roll:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionRoll);
	g_window_addtext(editWindowObjectEditor, "Scale.XYZ:");
	g_window_addstring(editWindowObjectEditor, editStrSelectionScale);
	g_window_addblank(editWindowObjectEditor);
	g_window_adddigits(editWindowObjectEditor, "Lightrange: %3.1f", &editLightrange);
	g_window_addslider(editWindowObjectEditor, &editLightrange, 0, 500);
	g_window_addblank(editWindowObjectEditor);
	gWriteY = 0;
	g_window_addtext(editWindowObjectEditor, "Randomize");
	g_window_addbutton(editWindowObjectEditor, "Pan", 48, GALIGN_RIGHT, -96, edit_randomize_pan);
	g_window_addbutton(editWindowObjectEditor, "Tilt", 48, GALIGN_RIGHT, -48, edit_randomize_tilt);
	gWriteY = 1;
	g_window_addbutton(editWindowObjectEditor, "Roll", 48, GALIGN_RIGHT, 0, edit_randomize_roll);
	g_window_addblank(editWindowObjectEditor);
	gWriteY = 0;
	g_window_addtext(editWindowObjectEditor, "Add Pan");
	g_window_addbutton(editWindowObjectEditor, "-90", 48, GALIGN_RIGHT, -96, edit_subpan_90);
	g_window_addbutton(editWindowObjectEditor, "180", 48, GALIGN_RIGHT, -48, edit_addpan_180);
	gWriteY = 1;
	g_window_addbutton(editWindowObjectEditor, "90", 48, GALIGN_RIGHT, 0, edit_addpan_90);
	
	g_window_dock(editWindowObjectEditor, GDOCK_RIGHT | GDOCK_TOP);
	
	loading_progress++; wait(1);

	diagt("Create Action Editor");

	editWindowActionEditor = g_window_create(0, 0, 256, "Action Editor", GWINDOW_MOVE | GWINDOW_HIDE);
	
	g_window_addtext(editWindowActionEditor, "Avialable Actions:");
	editListboxActions = g_window_addlist(editWindowActionEditor, 128);
	
	int i;
	for(i = 0; i < actionCnt; i++)
	{
		g_listbox_add(editListboxActions, (editTextActionlist->pstring)[i]);
	}
	
	gWriteY = 0;
	g_window_addbutton(editWindowActionEditor, "Add", 64, GALIGN_LEFT, 0, edit_addaction);
	g_window_addbutton(editWindowActionEditor, "Remove", 64, GALIGN_LEFT, 64, edit_removeaction);
	gWriteY = 1;
	g_window_addbutton(editWindowActionEditor, "Clear", 64, GALIGN_RIGHT, 0, edit_clearaction);
	
	g_window_dock(editWindowActionEditor, GDOCK_LEFT | GDOCK_BOTTOM);
	
	loading_progress++; wait(1);
	
	editMenuEntryFile = g_menu_addentry("File");
	
	g_window_addbutton(editMenuEntryFile, "New Level", 192, GALIGN_LEFT, 0, edit_new_level);
	g_window_addbutton(editMenuEntryFile, "Open Level", 192, GALIGN_LEFT, 0, edit_open_level);
	g_window_addbutton(editMenuEntryFile, "Save Level", 192, GALIGN_LEFT, 0, edit_save_level);
	g_window_addbutton(editMenuEntryFile, "Save Level As", 192, GALIGN_LEFT, 0, edit_save_level_as);
	g_window_addseparator(editMenuEntryFile);
	g_window_addbutton(editMenuEntryFile, "Export Level", 192, GALIGN_LEFT, 0, edit_export_level);
	g_window_addseparator(editMenuEntryFile);
	g_window_addbutton(editMenuEntryFile, "Exit", 192, GALIGN_LEFT, 0, quit);
	
	//editMenuEntryEdit = g_menu_addentry("Edit");
	//editMenuEntryWindows = g_menu_addentry("Windows");
	editMenuEntryTools = g_menu_addentry("Tools");
	g_window_addbutton(editMenuEntryTools, "Take Screenshot", 192, GALIGN_LEFT, 0, edit_take_screenshot);
	//editMenuEntryAbout = g_menu_addentry("About");	
	
	// Setup Events
	on_mouse_left = edit_mouse_select;
	on_mouse_middle = edit_mouse_togglemode;
	on_1 = edit_mouse_togglemode;
	on_2 = edit_mouse_togglemode;
	on_3 = edit_mouse_togglemode;
	on_del = edit_delete_model;
	on_e = edit_morph_model;
	on_q = edit_clone_model;
	on_f6 = edit_take_screenshot;
	
	edit_cam_free();
	
	diagt("Initialize selection panels");
	
	for(i = 0; i < EDIT_MAX_SELECTION; i++)
	{
		editPanSelection[i] = pan_create("", 5);
		editPanSelection[i]->bmap = bmap_createblack(128, 512, 888);
		editPanSelection[i]->flags = TRANSLUCENT | UNTOUCHABLE;
		editPanSelection[i]->alpha = 0;
	}
	
	loading_progress++; wait(1);
	diagt("Editor fully initialized!");
	edit_loading = 0;
	
	int i;
	while(1)
	{
		var gridval = integer(editRawGridsize + 0.5);
		if(gridval > 0)
		{
			if(gridval < 7)
				editGridsize = pow(2, gridval - 1);	
			else
				editGridsize = 100;
		}
		else
			editGridsize = 0;
		var rotoval = integer(editRawRotosize + 0.5);
		editRotosize = 7.5 * rotoval;
		
		var scaleval = integer(editRawScalesize + 0.5);
		if(scaleval > 0)
			editScalesize = pow(2, scaleval - 4);
		else
			editScalesize = 0;
		
		draw_textmode("Arial", 0, 12, 100);
		int count = 0;
		int containsLevel = 0;
		int containsLight = 0;
		int containsProjection = 0;
		VECTOR pos;
		for(i = 0; i < EDIT_MAX_SELECTION; i++)
		{
			if(editSelection[i] == NULL)
			{
				editPanSelection[i]->flags &= ~SHOW;
				continue;
			}
			ent_drawbounds(editSelection[i]);
			
			bmap_rendertarget(editPanSelection[i]->bmap, 0, 0);
			
			draw_quad(NULL, vector(0, 0, 0), NULL, vector(128, 512, 0), NULL, vector(64, 64, 64), 100, 0);
			
			editPanSelection[i]->size_y = 4;
			ActionInformation *info = NULL;
			for(info = list_first(editSelection[i]->string1); info != NULL; info = list_next(editSelection[i]->string1, info))
			{
				draw_text(info->actionname, 4, editPanSelection[i]->size_y, COLOR_WHITE);
				editPanSelection[i]->size_y += 12;
			}
			editPanSelection[i]->size_y += 4;
			
			bmap_rendertarget(NULL, 0, 0);
			
			if(editPanSelection[i]->size_y > 8)
			{
				VECTOR tmp;
				vec_set(&tmp, &editSelection[i]->x);
				vec_to_screen(&tmp, camera);
				editPanSelection[i]->flags |= SHOW;
				editPanSelection[i]->pos_x = tmp.x - 64;
				editPanSelection[i]->pos_y = tmp.y - 0.5 * editPanSelection[i]->size_y;
				
				var alphaX = clamp(2 * (abs(mouse_pos.x - editPanSelection[i]->pos_x - 0.5 * editPanSelection[i]->size_x) - 0.5 * editPanSelection[i]->size_x), 0, 100);
				var alphaY = clamp(2 * (abs(mouse_pos.y - editPanSelection[i]->pos_y - 0.5 * editPanSelection[i]->size_y) - 0.5 * editPanSelection[i]->size_y), 0, 100);
				
				editPanSelection[i]->alpha = maxv(alphaX, alphaY);
			}
			else
			{
				editPanSelection[i]->flags &= ~SHOW;
			}
			
			if(editSelection[i]->group == GROUP_LEVEL)
			{
				containsLevel += 1;
			}
			
			if(count == 0)
			{
				vec_set(&pos, &editSelection[i]->x);
				str_cpy(editStrSelectionModel, editSelection[i]->type);
				str_for_num(editStrSelectionPan, cycle(editSelection[i]->pan, 0, 360));
				str_for_num(editStrSelectionTilt, cycle(editSelection[i]->tilt, 0, 360));
				str_for_num(editStrSelectionRoll, cycle(editSelection[i]->roll, 0, 360));
				str_for_num(editStrSelectionScale, editSelection[i]->scale_x);
				
				editSelection[i]->lightrange = editLightrange;
			}
			else
			{
				vec_add(&pos, &editSelection[i]->x);
				str_cpy(editStrSelectionModel, "-");
				str_cpy(editStrSelectionPan, "-");
				str_cpy(editStrSelectionTilt, "-");
				str_cpy(editStrSelectionRoll, "-");
				str_cpy(editStrSelectionScale, "-");
			}
			count++;
		}
		if(count > 0)
		{
			vec_scale(&pos, 1.0 / (var)count);
			
			vec_set(&entEditAxisX->x, &pos);
			vec_set(&entEditAxisY->x, &pos);
			vec_set(&entEditAxisZ->x, &pos);
			
			str_for_num(editStrSelectionX, pos.x);
			str_for_num(editStrSelectionY, pos.y);
			str_for_num(editStrSelectionZ, pos.z);
			
			vec_to_screen(&pos, camera);
			var scale = 0.02 * pos.z;
			
			vec_fill(&entEditAxisX->scale_x, scale);
			vec_fill(&entEditAxisY->scale_x, scale);
			vec_fill(&entEditAxisZ->scale_x, scale);
			
			reset(entEditAxisX, INVISIBLE);
			reset(entEditAxisY, INVISIBLE);
			reset(entEditAxisZ, INVISIBLE);
			
			g_window_open(editWindowObjectEditor);
			if(containsLevel > 0)
				g_window_open(editWindowActionEditor);
		}
		else
		{
			set(entEditAxisX, INVISIBLE);
			set(entEditAxisY, INVISIBLE);
			set(entEditAxisZ, INVISIBLE);
			
			g_window_close(editWindowObjectEditor);
			g_window_close(editWindowActionEditor);
		}
		
		int selectedModel = g_listbox_getselection(editListboxFileList);
		if(selectedModel < editTextFilelist->strings && editPreviewModel != NULL)
		{
			entEditPreviewModel->pan += 5 * time_step;
			ent_morph(entEditPreviewModel, (editTextFilelist->pstring)[selectedModel]);
			
			var zoomX = maxv(abs(entEditPreviewModel->max_x), abs(entEditPreviewModel->min_x));
			var zoomY = maxv(abs(entEditPreviewModel->max_y), abs(entEditPreviewModel->min_y));
			var zoomFac = maxv(zoomX, zoomY);
			
			editPreviewModel->x = -zoomFac * 2;
			editPreviewModel->z = entEditPreviewModel->z + 0.5 + (entEditPreviewModel->max_z + entEditPreviewModel->min_z);
		}
		
		if(editGridVisible)
		{
			int size = 25;
			for(i = -size; i < size; i++)
			{
				COLOR col;
				vec_set(&col, COLOR_WHITE);
				if(i == 0) vec_set(&col, COLOR_RED);
				draw_line3d(vector(16 * i, -size * 16, 0), NULL, 100);
				draw_line3d(vector(16 * i, -size * 16, 0), &col, 100);
				draw_line3d(vector(16 * i,  size * 16, 0), &col, 100);
				
				vec_set(&col, COLOR_WHITE);
				if(i == 0) vec_set(&col, COLOR_GREEN);
				draw_line3d(vector(-size * 16, i * 16, 0), NULL, 100);
				draw_line3d(vector(-size * 16, i * 16, 0), &col, 100);
				draw_line3d(vector( size * 16, i * 16, 0), &col, 100);
			}
		}
		
		wait(1);
	}
}

var scheduler_running = 1;

function quit()
{
	g_msgbox("Hotel California - Editor", "Do you really want to quit?", GMB_YES | GMB_NO);
	if(gMsgBoxResult == GMB_YES)
	{
		scheduler_running = 0;
		wait(1);
		edit_save_settings();
		edit_quicksave();
		sys_exit(NULL);
	}
}

function initialize_actions()
{
	action_register("Simple Rotator", NULL);
}

function splashscreen()
{
	edit_loading = 1;
	while(edit_loading)
	{
		draw_quad(
			bmpSplashcreen,
			vector(0, 0, 0),
			NULL,
			vector(800, 600, 0),
			vector(screen_size.x / 800, screen_size.y / 600, 0),
			NULL,
			100,
			0);
		wait(1);
	}
}

function main()
{	
	wait(1);
	SetWindowText(hWnd, "Hotel California - Edit");
	video_window(NULL, NULL, 1, NULL);
	video_set(800, 600, 32, 2);
	
	
	splashscreen();
	wait(5);
	scheduler_init();
	initialize_actions();
	physX_open();
	
	qlevel_entity_init = entity_init;
	gEventClose = quit;
	
	MONITORINFO info;
	memset(&info, 0, sizeof(MONITORINFO));
	info.cbSize = sizeof(MONITORINFO);
	HMONITOR primary = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
	GetMonitorInfo(primary, &info);
	video_set(info.rcWork.right - info.rcWork.left, info.rcWork.bottom - info.rcWork.top, 32, 2);
	SetWindowPos(hWnd, HWND_TOP, info.rcWork.left, info.rcWork.top, info.rcWork.right - info.rcWork.left, info.rcWork.bottom - info.rcWork.top, SWP_SHOWWINDOW);
	
	g_init();
	level_load(NULL);
	camera->clip_near = 0;
	edit_init();
	
	mouse_mode = 1;
	sky_color.red = 227;
	sky_color.green = 112;
	sky_color.blue = 0;
	//entSkycube = ent_createlayer("skyrain+6.tga", SKY | CUBE, 2);
	//entSkycube->SHADEQ_TYPE = SHADEQ_GEOMETRY;
	//entSkycube->material = sqMatGeometry->base;
	
	while(edit_loading)
	{
		wait(1);
	}
	
	on_esc = quit;
	on_close = quit;
	on_f11 = toggle_debug_panel;
	
	str_cpy(editStrCurrentLevel, "");
	
	while(1)
	{
		proc_mode = PROC_LATE;
		vec_set(&mouse_pos, &mouse_cursor);
		if(scheduler_running)
			scheduler_run();
		if(debug_panel_visible)
		{
			update_debug_panel();
		}
		wait(1);
	}
}



////////////////////////////////////////////////////////////////////////////
//			Debug Panel
////////////////////////////////////////////////////////////////////////////

var def_dfps,def_dtps,def_dtlv,def_dtcs,def_dtac,def_dtrf,def_dsnd;
ANGLE def_cang;

PANEL *def_debug_panbg = 
{
	layer = 1;
	
	flags = LIGHT;
	size_x = 317;
	size_y = 106;
	red = 64;
	green = 64;
	blue = 64;
}

PANEL *def_debug_pan = 
{
	layer = 2;
	
	digits(0,2,"%4.0fx",*,1,screen_size.x);
	digits(30,2,"%.0f",*,1,screen_size.y);
	digits(0,12,"fps%5.0f",*,16,def_dfps);
	digits(0,22,"pps%5.0fk",*,0.016,def_dtps);
	digits(0,32,"x%7.0f",*,1,camera.x);
	digits(0,42,"y%7.0f",*,1,camera.y);
	digits(0,52,"z%7.0f",*,1,camera.z);
	digits(0,62,"pan%5.0f",*,1,def_cang.pan);
	digits(0,72,"til%5.0f",*,1,def_cang.tilt);
	digits(0,82,"rol%5.0f",*,1,def_cang.roll);
	//    digits(0,92,"ml %5.f",*,1,mouse_right);

	digits(65,2,"ms/frame",*,0,0);
	digits(65,12,"geo%5.1f",*,1,def_dtlv);
	digits(65,22,"ent%5.1f",*,1,time_entities);
	digits(65,32,"par%5.1f",*,1,time_effects);
	digits(65,42,"drw%5.1f",*,1,time_draw);
	digits(65,52,"pan%5.1f",*,1,time_panels);
	digits(65,62,"ref%5.1f",*,1,def_dtrf);
	digits(65,72,"srv%5.1f",*,1,def_dtcs);
	digits(65,82,"fnc%5.1f",*,1,def_dtac);
	//digits(65,92,"phy%5.1f",*,1,time_physics);
	//digits(65,106,"sco%5.1f",*,1,time_scope);

	digits(130,2,"count of",*,0,0);
	digits(130,12,"ent%5.0f",*,1,num_entities);
	digits(130,22,"vis%5.0f",*,1,num_visents);
	digits(130,32,"tri%5.0fk",*,0.001,num_vistriangles);
	digits(130,42,"par%5.0f",*,1,num_particles);
	digits(130,52,"lit%5.0f",*,1,num_lights);
	digits(130,62,"snd%5.0f",*,1,def_dsnd);
	digits(130,72,"fnc%5.0f",*,1,num_actions);
	digits(130,82,"bdy%5.0f",*,1,num_bodies);

	digits(200,2,"memory MB",*,0,0);
	digits(200,12,"nex%5.0f",*,1,nexus);
	digits(200,22,"mem%5.0f",*,1,sys_memory);
	digits(200,32,"geo%5.0f",*,1,d3d_texsurfs);
	digits(200,42,"shd%5.0f",*,1,d3d_texsmaps);
	digits(200,52,"ent%5.0f",*,1,d3d_texskins);
	digits(200,62,"fre%5.0f",*,1,d3d_texfree);
	//digits(200,82,"uu1%6.0f",*,1,uu1);
	//digits(200,92,"uu2%6.0f",*,1,uu2);

	digits(270,2,"network",*,0,0);
	digits(270,12,"lat%4.0f",*,1,dplay_latency);
	digits(270,22,"bps%4.0f",*,1,dplay_bps);
	digits(270,32,"rel%4.0f",*,1,dplay_bpsreliable);
	digits(270,42,"unr%4.0f",*,1,dplay_bpsunreliable);
	digits(270,52,"com%4.0f",*,1,dplay_compression);
	digits(270,62,"num%4.0f",*,1,num_clients);
}

void def_box(var x1,var y1,var x2,var y2,VECTOR* color)
{
    draw_line(vector(x1,y1,0),NULL,100);
    draw_line(vector(x2,y1,0),color,100);
    draw_line(vector(x2,y2,0),color,100);
    draw_line(vector(x1,y2,0),color,100);
    draw_line(vector(x1,y1,0),color,100);
}

function update_debug_panel()
{
	def_dfps = 0.9 * def_dfps + 0.1 / time_frame;
	def_dtps = 0.9 * def_dtps + 0.1 * num_vistriangles / time_frame;
	def_dtlv = time_level + time_sky;
	def_dtcs = time_client + time_server;
	def_dtrf = time_update + time_idle;
	def_dtac = time_actions + time_pfuncs;
	def_dsnd = num_mediastreams + num_sounds;
	def_cang.pan = cycle(camera->pan,0,360); 
	def_cang.tilt = cycle(camera->tilt,-180,180); 
	def_cang.roll = cycle(camera->roll,-180,180); 
	def_box(
		def_debug_pan->pos_x - 3,
		def_debug_pan->pos_y - 2,
		def_debug_pan->pos_x + 314,
		def_debug_pan->pos_y + 103,
		vector(255,255,255));
	def_debug_pan->pos_x = screen_size.x - 317;
	def_debug_pan->pos_y = screen_size.y - 106;
	def_debug_panbg->pos_x = def_debug_pan->pos_x - 2;
	def_debug_panbg->pos_y = def_debug_pan->pos_y - 2;
}

function toggle_debug_panel()
{
	toggle(def_debug_pan, SHOW);
	debug_panel_visible = is(def_debug_pan, SHOW);
	if(debug_panel_visible)
		set(def_debug_panbg, SHOW);
	else
		reset(def_debug_panbg, SHOW);
}

