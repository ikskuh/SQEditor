#ifndef _DIALOGS_C_
#define _DIALOGS_C_

#include "ini.c"

#define MAX_DIALOG_ENTRIES 1024

typedef struct tagDialogEntry
{
	int initialized;
	STRING *text;
	SOUND *sound;
} DialogEntry;

DialogEntry dialogEntries[MAX_DIALOG_ENTRIES];

TEXT *dialogText = NULL;
FONT *dialogFont = "Arial#32b";

function dialog_say(int dialog)
{
	if(dialogEntries[dialog].initialized == NULL) return;
	proc_kill(4);
	
	var soundID = -1;
	if(dialogEntries[dialog].sound != NULL)
	{
		soundID = snd_play(dialogEntries[dialog].sound, 100, 0);
	}
	set(dialogText, SHOW);
	(dialogText->pstring)[0] = dialogEntries[dialog].text;
	dialogText->alpha = 0;
	var timer = 32;
	while(timer > 0 || snd_playing(soundID))
	{
		dialogText->pos_x = 0.5 * screen_size.x;
		dialogText->pos_y = screen_size.y - 64;
		
		if(timer > 8)
		{
			dialogText->alpha = minv(100, dialogText->alpha + 12 * time_step);
		}
		else
		{
			dialogText->alpha = maxv(0, dialogText->alpha - 12 * time_step);
		}
		
		if(!snd_playing(soundID))
			timer -= time_step;
		
		wait(1);
	}
	reset(dialogText, SHOW);
	dialogText->alpha = 0;
}

function dialogs_init()
{
	STRING *file = "#256";
	make_path(file, "messages.ini");
	int i;
	for(i = 0; i < MAX_DIALOG_ENTRIES; i++)
	{
		/*
		[Message 0]
		text=Oh man, nice bell!
		sound=just_stupid_sound.wav
		*/
		STRING *section = "#64";
		str_cpy(section, "Message ");
		str_cat(section, str_for_int(NULL, i));
		char buffer[256];
		ini_read_buffer(file, section, "text", "FIXIT: You forgot a message here!", buffer, 256);
		
		if(strlen(buffer) > 0)
		{
			dialogEntries[i].initialized = 1;
			dialogEntries[i].text = str_create(buffer);
			
			ini_read_buffer(file, section, "sound", "", buffer, 256);
			if(strlen(buffer) > 0)
				dialogEntries[i].sound = snd_create(buffer);
			else
				dialogEntries[i].sound = NULL;
		}
	}
	
	dialogText = txt_create(1, 20);
	dialogText->flags |= CENTER_X | TRANSLUCENT | OUTLINE;
	dialogText->font = dialogFont;
}

#endif