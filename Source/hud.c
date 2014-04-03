#ifndef _HUD_C_
#define _HUD_C_

#define HUD_DEFAULT 0
#define HUD_USE 1
#define HUD_LOOK 2

BMAP *hudCrosshairDefault = "crosshair.tga";
BMAP *hudCrosshairHand = "hand_grab.tga";
BMAP *hudCrosshairEye = "eye.tga";
BMAP *hudCrosshairHealth = "crosshair_life.tga";

PANEL *panCrosshair = 
{
	bmap = hudCrosshairDefault;
	flags = SHOW;
	layer = 1;
}
PANEL *panCrosshairHealth = 
{
	bmap = hudCrosshairHealth;
	flags = SHOW | LIGHT;
	red = 255;
	layer = 2;
}

var hud_health = 0;

var hud_mode = 0;

function hud_init()
{
	while(1)
	{
		panCrosshair->pos_x = 0.5 * screen_size.x - 16;
		panCrosshair->pos_y = 0.5 * screen_size.y - 16;
		panCrosshairHealth->pos_x = 0.5 * screen_size.x - 16;
		panCrosshairHealth->pos_y = 0.5 * screen_size.y - 16;
		vec_lerp(&panCrosshairHealth->blue, vector(0, 0, 255), vector(0, 255, 0), 0.01 * hud_health);
		
		reset(panCrosshairHealth, SHOW);
		if(hud_mode == HUD_DEFAULT)
		{
			panCrosshair->bmap = hudCrosshairDefault;
			set(panCrosshairHealth, SHOW);
		}
		else if(hud_mode == HUD_USE)
		{
			panCrosshair->bmap = hudCrosshairHand;
		}
		else if(hud_mode == HUD_LOOK)
		{
			panCrosshair->bmap = hudCrosshairEye;
		}
		
		wait(1);
	}
}

#endif