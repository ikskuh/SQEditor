#ifndef _PROPS_C_
#define _PROPS_C_

SOUND *sndPropReceptionBell = "receptionBell.wav";

action a_prop_reception_bell_use()
{
	if(my->skill1 > total_secs) return;
	ent_playsound(me, sndPropReceptionBell, 100);
	my->skill1 = total_secs + 2;
	dialog_say(0);
}

action a_prop_reception_bell()
{
	event_set(me, EV_USE, a_prop_reception_bell_use);
	my->skill1 = total_secs;
	my->ENT_HUD_MODE = HUD_USE;
	scheduler_setnext(NULL);
}

ENTITY *entReceptionPhone = NULL;
SOUND *sndPropReceptionPhone = "phoneRing.wav";
var phone_triggered = 0;

action a_prop_reception_phone()
{
	entReceptionPhone = me;
	entReceptionPhone->skill1 = 0;
	entReceptionPhone->skill2 = 0;
	scheduler_setnext(NULL);
}

action a_prop_reception_phone_trigger_on()
{
	set(me, INVISIBLE | PASSABLE);
	if(player == NULL) return;
	if(phone_triggered)
	{
		scheduler_setnext(NULL);
		return;
	}
	VECTOR tmp;
	vec_set(&tmp, &player->x);
	vec_sub(&tmp, &my->x);
	vec_rotateback(&tmp, &my->pan);
	if(tmp.x > my->max_x * my->scale_x) return;
	if(tmp.x < my->min_x * my->scale_x) return;
	if(tmp.y > my->max_y * my->scale_y) return;
	if(tmp.y < my->min_y * my->scale_y) return;
	if(tmp.z > my->max_z * my->scale_z) return;
	if(tmp.z < my->min_z * my->scale_z) return;
	
	if(entReceptionPhone)
	{
		entReceptionPhone->skill1 = ent_playloop(entReceptionPhone, sndPropReceptionPhone, 250);
		phone_triggered = 1;
		scheduler_setnext(NULL);
	}
}

action a_prop_reception_phone_trigger_off()
{
	set(me, INVISIBLE | PASSABLE);
	if(player == NULL) return;
	if(phone_triggered == 2)
	{
		scheduler_setnext(NULL);
		return;
	}
	VECTOR tmp;
	vec_set(&tmp, &player->x);
	vec_sub(&tmp, &my->x);
	vec_rotateback(&tmp, &my->pan);
	if(tmp.x > my->max_x * my->scale_x) return;
	if(tmp.x < my->min_x * my->scale_x) return;
	if(tmp.y > my->max_y * my->scale_y) return;
	if(tmp.y < my->min_y * my->scale_y) return;
	if(tmp.z > my->max_z * my->scale_z) return;
	if(tmp.z < my->min_z * my->scale_z) return;
	
	if(entReceptionPhone)
	{
		if(entReceptionPhone->skill1 == NULL) return;
		snd_stop(entReceptionPhone->skill1);
		phone_triggered = 2;
		scheduler_setnext(NULL);
	}
}


function a_px_door_locked_use(int evParam)
{
	if(my->skill1 > total_secs) return;
	my->skill1 = total_secs + 2;
	ent_playsound(me, sndDoorlocked, 100);
	dialog_say(1);
}

function a_px_door_locked()
{
	pXent_settype(me, PH_STATIC, PH_BOX);
	my->ENT_HUD_MODE = HUD_USE;
	event_set(me, EV_USE, a_px_door_locked_use);
	scheduler_setnext(NULL);
}

var px_door_speed = 5.7;

function a_px_door_update()
{
	my->skill1 = clamp(my->skill1 + px_door_speed * (2 * my->skill4 - 1) * time_step, 0, 110);
	
	VECTOR pos;
	pos.x = 0;
	pos.y = -3 * my->scale_x;
	pos.z = 0;
	vec_rotate(&pos, vector(my->skill5 + my->skill6 * my->skill1, 0, 0));
	pos.x += my->skill2;
	pos.y += my->skill3;
	pos.z = my->z;
	
	pXent_setposition(me, &pos);
	pXent_rotate(me, nullvector, vector(my->skill5 + my->skill6 * my->skill1, 0, 0));
}

function a_px_door_use(int evParam)
{
	my->skill4 = !my->skill4;
	if(my->skill4)
		ent_playsound(me, sndDoorOpen, 100);
	else
		ent_playsound(me, sndDoorClose, 100);
}

function a_px_door()
{
	pXent_settype(me, PH_RIGID, PH_BOX);
	pXent_kinematic(me, 1);
	my->skill1 = 0;
	
	VECTOR pos;
	pos.x = 0;
	pos.y = 3 * my->scale_x;
	pos.z = 0;
	vec_rotate(&pos, vector(my->pan, 0, 0));
	
	my->skill2 = my->x + pos.x;
	my->skill3 = my->y + pos.y;
	my->skill4 = 0;
	my->skill5 = my->pan;
	my->ENT_HUD_MODE = HUD_USE;
	event_set(me, EV_USE, a_px_door_use);
	scheduler_setnext(a_px_door_update);
}

function a_px_door_right()
{
	a_px_door();
	my->skill6 = 1;
}

function a_px_door_left()
{
	a_px_door();
	my->skill6 = -1;
}

#endif