#ifndef _MOVEMENT_C_
#define _MOVEMENT_C_

#include <ackPhysX.h>
#include "scheduler.c"

SOUND *sndFootstepsConcrete[4];

var movement_step_dist = 28;

function movement_update()
{
	VECTOR mov;
	mov.x = (4 * key_shift + 2.5) * (key_w - key_s);
	mov.y = 1.5 * (key_a - key_d);
	mov.z = 0;
	vec_rotate(&mov, vector(camera->pan, 0, 0));
	if(total_ticks > 8)
		mov.z = -4;
	vec_scale(&mov, time_step);
	
	pXent_move(me, nullvector, vector(0, 0, 2));
	pXent_move(me, nullvector, &mov);
	pXent_move(me, nullvector, vector(0, 0, -2));

	mov.z = 0;
	my->skill20 -= vec_length(&mov);
	if(my->skill20 <= 0)
	{
		int i = integer(random(4));
		if(sndFootstepsConcrete[i])
		{
			ent_playsound(me, sndFootstepsConcrete[i], 100);
		}
		my->skill20 = movement_step_dist;
	}

	//var result = c_trace(vector(my->x, my->y, my->z + 16), vector(my->x, my->y, my->z - 2048), IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | USE_POLYGON);
	//if(result)
	//{
	//	my->z = target.z + 32 * my->scale_z;
	//}
	
	camera->x = my->x;
	camera->y = my->y;
	camera->z = my->z + 28 * my->scale_z;
	
	camera->pan = camera->pan - 10 * mouse_force.x * time_step;
	camera->tilt = clamp(camera->tilt + 10 * mouse_force.y * time_step, -80, 80);
}


function movement_init()
{
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		if(sndFootstepsConcrete[i] != NULL) continue;
		STRING *str = "#64";
		str_cpy(str, "Footstep_Concrete_");
		str_cat(str, str_for_int(NULL, i + 1));
		str_cat(str, ".wav");
		diag("\nLoad sound: "); diag(str);
		sndFootstepsConcrete[i] = snd_create(str);
		if(sndFootstepsConcrete[i] == NULL) diag("\tNOT FOUND!");
	}
	
	
	my->skill20 = movement_step_dist;
	snd_pause(my->skill20);
	pXent_settype(me, PH_CHAR, PH_CAPSULE);
	scheduler_setnext(movement_update);
}

#endif