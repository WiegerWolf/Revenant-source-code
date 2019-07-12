// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 effect2.cpp - Second Effects module                   *
// *************************************************************************

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"

#include "revenant.h"
#include "effect.h"
#include "mappane.h"	   
#include "character.h"
#include "statusbar.h"
#include "MissileEffect.H"
#include "StripEffect.H"
#include "food.h"
#include "textbar.h"
#include "rules.h"



/****************************************************************/
/*                         Paralize                             */
/****************************************************************/

// ***************
// * Para Effect *
// ***************

#define MAX_NUM_PARA 15
#define PARA_STATE_GROW 1
#define PARA_STATE_MOVE 2
#define PARA_STATE_SHRINK 3
#define PARA_GRAVITY 1.2f
#define PARA_KNOT_SCALE 0.5f
#define PARA_BASE_VELOCITY 0.24f
#define PARA_MAX_NUM_FLARES 10
#define PARA_FLARE_SCALE 0.4f
#define PARA_MAX_FLARE_SCALE 10.0f
#define PARA_MAX_TARGETS 5

_CLASSDEF(TParaEffect)

class TParaEffect : public TEffect
{
protected:
	D3DVECTOR direction[MAX_NUM_PARA];		// velocity vector
	D3DVECTOR acc[MAX_NUM_PARA];
	float home_radius[MAX_NUM_PARA];
	float speed[MAX_NUM_PARA];
	float home_z[MAX_NUM_PARA];
	float max_knot_scale;
	float min_radius;
	float max_radius;
	int state;						// is the particle growing, shrinking, or moving
	PTCharacter target[PARA_MAX_TARGETS];
	int current_target[MAX_NUM_PARA];
	bool hit_flag;
public:
	int level;
	int start_time[MAX_NUM_PARA];
	int count;						// keeps track of when to kill the particle
	D3DVECTOR pos[MAX_NUM_PARA];
	float val;
	float knot_scale;
	float scale;
	S3DPoint target_position[PARA_MAX_TARGETS];
	int num_targets;
    TParaEffect(PTObjectImagery newim) : TEffect(newim) { }
    TParaEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
	void Initialize();
	void Pulse();
};

DEFINE_BUILDER("Paralize1", TParaEffect)
REGISTER_BUILDER(TParaEffect)

void TParaEffect::Initialize()
{
	// this whole thing finds a target to use
	int i;
	bool found = false;
	S3DPoint temp_point;
//	int itargets[MAXFOUNDOBJS];

/*
	if(strcmp(spell->VariantData()->name, "Paralize1") == 0)
	{
		level = 1;
		count = 200;
	}
	else if(strcmp(spell->VariantData()->name, "Paralize2") == 0)
	{
		level = 2;
		count = 300;
	}
	else if(strcmp(spell->VariantData()->name, "Paralize3") == 0)
	{
		level = 3;
		count = 300;
	}
	else if(strcmp(spell->VariantData()->name, "Paralize4") == 0)
	{
		level = 4;
		count = 200;
	}

	num_targets = 0;

	if(spell)
	{
		PTCharacter invoker = (PTCharacter)spell->GetInvoker();
		invoker->GetPos(temp_point);
		if(level == 3)
		{
			PTCharacter invoker = (PTCharacter)spell->GetInvoker();
			GetPos(temp_point);
			// figure out how many targets are in range
			int num = MapPane.FindObjectsInRange(temp_point, itargets, 5000, 0, OBJCLASS_CHARACTER);
			for (i = 0; i < num; i++) 
			{
				target[num_targets] = (PTCharacter)MapPane.GetInstance(itargets[i]);
				// check to see if the current target is groovy
				if(target[num_targets] && target[num_targets] != invoker && !((PTCharacter)target[num_targets])->IsDead())
				{
					target[num_targets]->GetPos(target_position[num_targets]);	// and use it
					target_position[num_targets].z += 40;
					target_position[num_targets].z = (int)FIX_Z_VALUE(target_position[num_targets].z);
					num_targets++;
					if(num_targets == 5)
						break;
				}
			}
			if(num_targets == 0)
			{
				target[0] = NULL;
				invoker->GetPos(target_position[0]);
				target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
			}

			invoker->GetPos(temp_point);

		}
		else
		{
			target[0] = (PTCharacter)invoker->Fighting();
			num_targets = 0;
			if(target[0])
			{
				target[0]->GetPos(target_position[0]);	// and use it
				target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
				num_targets = 1;
			}
			invoker->GetPos(temp_point);
		}
	}
	else
	{
		num_targets = 0;
		target[0] = NULL;
		GetPos(target_position[0]);
		GetPos(temp_point);
		target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
	}
*/

	num_targets = 0;
// we REALLY want a target on this spell!
	if(spell)
	{
		if(strcmp(spell->VariantData()->name, "Paralize1") == 0)
		{
			level = 1;
			count = 200;
		}
		else if(strcmp(spell->VariantData()->name, "Paralize2") == 0)
		{
			level = 2;
			count = 300;
		}
		else if(strcmp(spell->VariantData()->name, "Paralize3") == 0)
		{
			level = 3;
			count = 300;
		}
		else if(strcmp(spell->VariantData()->name, "Paralize4") == 0)
		{
			level = 4;
			count = 200;
		}
		PTCharacter invoker = (PTCharacter)spell->GetInvoker();
		if(invoker)
		{
			invoker->GetPos(temp_point);
			invoker->GetPos(target_position[0]);
			target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
			target[0] = (PTCharacter)invoker->Fighting();
			if(target[0])
			{
				target[0]->GetPos(target_position[0]);
				target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
				num_targets++;
			}
			S3DPoint temp_point1;
			S3DPoint temp_point2;
			invoker->GetPos(temp_point1);
			for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
			{
				PTCharacter chr = (PTCharacter)i.Item();

				if (chr == invoker)
					continue;

				if (chr->IsDead())
					continue;

				chr->GetPos(temp_point2);
				if (::Distance(temp_point1,temp_point2) > 400)
					continue;

				if (invoker && !invoker->IsEnemy(chr))
					continue;	// We can't hurt our friends
				
				target[num_targets] = chr;
				target[num_targets]->GetPos(target_position[num_targets]);
				target_position[num_targets].z = (int)FIX_Z_VALUE(target_position[num_targets].z);
				num_targets++;
	
				if(num_targets >= PARA_MAX_TARGETS || level != 3)
					break;
			}
		}
		else
		{
			target[0] = NULL;
			target_position[0].x = target_position[0].y = target_position[0].z = 0;
		}
	}
	else
	{
		target[0] = NULL;
		target_position[0].x = target_position[0].y = target_position[0].z = 0;
	}


// brought from animator

	D3DVECTOR point_in_relation_to_target;
	float initial_length;

	SetCommandDone( FALSE);

	GetPos(temp_point);
	
	temp_point.z = (int)FIX_Z_VALUE(temp_point.z);

	if(num_targets > 0)
	{
		min_radius = (float)((PTCharacter)target[0])->Radius();

		point_in_relation_to_target.x = (float)(temp_point.x - target_position[0].x);
		point_in_relation_to_target.y = (float)(temp_point.y - target_position[0].y);

		initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));
	}
	else
	{
		min_radius = 50;
		initial_length = 80;
		point_in_relation_to_target.x = 0;
		point_in_relation_to_target.y = 0;
	}
	max_knot_scale = min_radius * 0.4f;
	min_radius *= 1.5f;
	max_radius = min_radius + 1;

// initialize each of our particles to a random position relative
// to the casters position  and give them an initial random direction
	if(level == 4)
	{
		for(i = 0; i<MAX_NUM_PARA;i++)
		{
			pos[i].x =  temp_point.x + ((float)random(-15, 15) / 10);
			pos[i].y = temp_point.y + (float)random(-15, 15) / 10;
			pos[i].z = temp_point.z + 60 + ((float)random(-5, 5) / 10);
			direction[i].x = (float)random(-15,15) / 10;
			direction[i].y = (float)random(-15,15) / 10;
			direction[i].z = (float)random(-5,0) / 100;
			home_radius[i] = min_radius + ((float)random(0, 100 * (int)(max_radius-min_radius)) / 100);
			home_z[i] = 1;
			speed[i] = PARA_BASE_VELOCITY; //+ (((float)random(-10,10)) / 200);
			start_time[i] = count - (i*2);
			if(num_targets == 0)
				current_target[i] = 0;
			else
				current_target[i] = i%num_targets;
		}
	}
	else
	{
		for(i = 0; i<MAX_NUM_PARA;i++)
		{
			pos[i].x =  temp_point.x + ((float)random(-15, 15) / 10);
			pos[i].y = temp_point.y + (float)random(-15, 15) / 10;
			pos[i].z = temp_point.z + 60 + ((float)random(-5, 5) / 10);
			direction[i].x = -2 * (point_in_relation_to_target.x + ((float)random(-55,55))) / initial_length;
			direction[i].y = -2 * (point_in_relation_to_target.y + ((float)random(-55,55))) / initial_length;
			direction[i].z = (float)random(-5,0) / 100;
			home_radius[i] = min_radius + ((float)random(0, 100 * (int)(max_radius-min_radius)) / 100);
			home_z[i] = 1;
			speed[i] = PARA_BASE_VELOCITY; //+ (((float)random(-10,10)) / 200);
			start_time[i] = count - (i*2);
			if(num_targets == 0)
				current_target[i] = 0;
			else
				current_target[i] = i%num_targets;
		}
	}
	scale = 0.2f;	// start them out small
	knot_scale = 0.0f;

	val = 0.04f;

	state = PARA_STATE_GROW;	// and grow 'em

	hit_flag = true;
}

void TParaEffect::Pulse()
{
	int i;
	D3DVECTOR point_on_circle;
	D3DVECTOR next_point_on_circle;
	D3DVECTOR point_in_relation_to_target;
	float initial_length;
	float ratio;
	float angle;
	float temp_angle;
	bool grow = false;

	for(i=0;i<num_targets;i++)	// if there is a target
	{
		target[i]->GetPos(target_position[i]);	// update our target position
		target_position[i].x += 20;
		target_position[i].y += 20;
		target_position[i].z = (int)FIX_Z_VALUE(target_position[i].z);
		((PTCharacter)target[i])->Disable();		// stop the bad man from moving
		((PTCharacter)target[i])->Stop();
		((PTCharacter)target[i])->SetParalize(TRUE);		// stop the bad man from moving
	}

	switch(state)
	{
		case PARA_STATE_GROW:
			scale += 0.2f;		// if we're in the grow state, grow the particles
			if(scale >= 2.0f)	// if the particles are adult, let them move
				state = PARA_STATE_MOVE;
			break;
		case PARA_STATE_SHRINK:
			scale -= 0.2f;		// if we're in the shrink state, shrink the particles
			knot_scale -= PARA_KNOT_SCALE;
			if(scale <= 0.0f)	// once there' to small to see, kill the spell
			{
				for(i = 0;i<num_targets;i++)
				{
					((PTCharacter)target[i])->SetParalize(FALSE);		// stop the bad man from moving
					target[i]->ClearFlag(OF_DISABLED);		// stop the bad man from moving
				}
				if(num_targets > 0 && level == 4 && spell)
					spell->Damage(target[0]);
				KillThisEffect();
			}
		case PARA_STATE_MOVE:
			if(level == 4)
			{
				if(count < start_time[0])
				{
					grow = true;
					point_in_relation_to_target.x = pos[0].x - target_position[current_target[0]].x;
					point_in_relation_to_target.y = pos[0].y - target_position[current_target[0]].y;

					initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

					acc[0].x = pos[0].x - target_position[0].x * PARA_GRAVITY * 0.01f;
					acc[0].y = pos[0].y - target_position[0].y * PARA_GRAVITY * 0.01f;
					acc[0].z = (pos[0].z - (target_position[0].z + 20) + random(-5,5)) * PARA_GRAVITY * 0.01f;

					// apply the acceleration to the velocity
					direction[0].x -= acc[0].x;
					direction[0].y -= acc[0].y;
					direction[0].z -= acc[0].z;

					// update the position based on the velocity
					pos[0].x += direction[0].x;
					pos[0].y += direction[0].y;
//					pos[0].z += direction[0].z;

					if(initial_length < 20 && !hit_flag)
					{
						if(spell)
						{
							if(target)
							{
								spell->Damage(target[current_target[0]]);
								hit_flag = true;
							}
						}
					}
					else if(initial_length > 20)
					{
						hit_flag = false;
					}
				}
				for(i = 1; i<MAX_NUM_PARA;i++)
				{
					if(count < start_time[i])
					{
						point_in_relation_to_target.x = pos[i].x - target_position[current_target[i]].x;
						point_in_relation_to_target.y = pos[i].y - target_position[current_target[i]].y;

						initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

						acc[i].x = ((pos[i].x - target_position[0].x) * PARA_GRAVITY * 0.01f);
						acc[i].y = ((pos[i].y - target_position[0].y) * PARA_GRAVITY * 0.01f);
						acc[i].z = (pos[i].z - target_position[0].z + random(-5,5) + 20) * PARA_GRAVITY * 0.01f;

						// apply the acceleration to the velocity
						direction[i].x -= acc[i].x;
						direction[i].y -= acc[i].y;
						direction[i].z -= acc[i].z;

						// update the position based on the velocity
						pos[i].x += direction[i].x;
						pos[i].y += direction[i].y;
//						pos[i].z += direction[i].z;
					}
				}
			}
			else
			{
				point_in_relation_to_target.x = pos[0].x - target_position[current_target[0]].x;
				point_in_relation_to_target.y = pos[0].y - target_position[current_target[0]].y;

				initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

				ratio = home_radius[0] / initial_length;

				point_on_circle.x = point_in_relation_to_target.x * ratio;
				point_on_circle.y = point_in_relation_to_target.y * ratio;

				angle = (float)atan2(point_on_circle.y, point_on_circle.x);

				angle += speed[0];

				for(i = 0; i<MAX_NUM_PARA;i++)
				{
//					if(count < start_time[i])
					{
						// calculate the circular velocity
						point_in_relation_to_target.x = pos[i].x - target_position[current_target[i]].x;
						point_in_relation_to_target.y = pos[i].y - target_position[current_target[i]].y;

						initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

						ratio = home_radius[i] / initial_length;

						point_on_circle.x = point_in_relation_to_target.x * ratio;
						point_on_circle.y = point_in_relation_to_target.y * ratio;

						temp_angle = (float)atan2(point_on_circle.y, point_on_circle.x);
						temp_angle += speed[i];

						if(ratio > 0.75f)
						{
							grow = true;
							angle -= (float)(6.28 / MAX_NUM_PARA);
							if(temp_angle > angle)
								angle = (float)(temp_angle + (1.57 / MAX_NUM_PARA));
						}
						else
						{
							angle = temp_angle;
						}


						next_point_on_circle.x = home_radius[i] * (float)cos(angle);
						next_point_on_circle.y = home_radius[i] * (float)sin(angle);

						next_point_on_circle.x = next_point_on_circle.x - point_on_circle.x;
						next_point_on_circle.y = next_point_on_circle.y - point_on_circle.y;

						// set the acceleration so that particles hone in on the target
						acc[i].x = (pos[i].x - target_position[current_target[i]].x + random(-25,25)) * PARA_GRAVITY / initial_length;
						acc[i].y = (pos[i].y - target_position[current_target[i]].y + random(-25,25)) * PARA_GRAVITY / initial_length;
						acc[i].z = (pos[i].z - (target_position[current_target[i]].z + home_z[i]) + random(-5,5)) * PARA_GRAVITY / initial_length;

						// apply the acceleration to the velocity
						ratio = home_radius[i] / initial_length;
						direction[i].x = (ratio * next_point_on_circle.x) + ((1-ratio) * (direction[i].x - acc[i].x));
						direction[i].y = (ratio * next_point_on_circle.y) + ((1-ratio) * (direction[i].y - acc[i].y));
						if(home_z[i] == 1)
							direction[i].z = -0.3f;
						else
							direction[i].z = 0.3f;

						// update the position based on the velocity
						pos[i].x += direction[i].x;
						pos[i].y += direction[i].y;
						pos[i].z += direction[i].z;
						if(pos[i].z < target_position[current_target[i]].z + 10)
							home_z[i] = -1;
						else if(pos[i].z > target_position[current_target[i]].z + 60)
							home_z[i] = 1;
					}
				}
			}
			count--;	// make our effect older
			//if the particles are ready to die, or we don'e have a target, shrink them
			if(count == 0 || num_targets == 0)
				state = PARA_STATE_SHRINK;
			if(grow == true || level == 3)
				if(knot_scale < max_knot_scale && state != PARA_STATE_SHRINK)
					knot_scale += PARA_KNOT_SCALE;
			break;
	}

	TEffect::Pulse();
}

// ************************
// *  Para Animator Code  *
// ************************

_CLASSDEF(TParaAnimator)
class TParaAnimator : public T3DAnimator
{
protected:
//  The first 3 vectors keep track of each particular particle
public:
	void Initialize();
	BOOL Render();
    TParaAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
	void RefreshZBuffer();
};

REGISTER_3DANIMATOR("Paralize1", TParaAnimator)

void TParaAnimator::Initialize()
{
	T3DAnimator::Initialize();

	((PTParaEffect)inst)->Initialize();

	PS3DAnimObj obj = GetObject(1);
    GetVerts(obj, D3DVT_LVERTEX);
	obj = GetObject(3);
    GetVerts(obj, D3DVT_LVERTEX);
	obj = GetObject(4);
    GetVerts(obj, D3DVT_LVERTEX);
}

BOOL TParaAnimator::Render()
{
	int i;

	SaveBlendState();
	SetBlendState();		// we want a cool glow effect

	PS3DAnimObj obj = GetObject(0);

	if(((PTParaEffect)inst)->level == 2)
		obj = GetObject(2);
	if(((PTParaEffect)inst)->level == 4)
		obj = GetObject(5);

	for(i = 0;i<MAX_NUM_PARA;i++)
	{
		// update the object with each of our particles new position and scale
		obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS;
		obj->scl.x = obj->scl.y = obj->scl.z = D3DVAL(((PTParaEffect)inst)->scale);
		obj->pos.x = ((PTParaEffect)inst)->pos[i].x;
		obj->pos.y = ((PTParaEffect)inst)->pos[i].y;
		obj->pos.z = ((PTParaEffect)inst)->pos[i].z;
	
		RenderObject(obj);
		if(((PTParaEffect)inst)->level == 2)
		{
			obj->pos.z = (120 + ((PTParaEffect)inst)->target_position[0].z) - (((PTParaEffect)inst)->pos[i].z - ((PTParaEffect)inst)->target_position[0].z);
			RenderObject(obj);
		}
	}

	SetAddBlendState();

// Render the Egg
	if(((PTParaEffect)inst)->level == 2)
		obj = GetObject(3);
	else if(((PTParaEffect)inst)->level == 4)
		obj = GetObject(4);
	else
		obj = GetObject(1);

	obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS | OBJ3D_VERTS;

// Do a little texture mapping to animate it
	for(i=0;i<151;i++)
	{
		obj->lverts[i].tu -= ((PTParaEffect)inst)->val;
		obj->lverts[i].tv -= ((PTParaEffect)inst)->val;
	}

	obj->scl.x = obj->scl.y = obj->scl.z = D3DVAL(((PTParaEffect)inst)->knot_scale);
	for(i=0;i<((PTParaEffect)inst)->num_targets;i++)
	{
		obj->pos.x = (float)((PTParaEffect)inst)->target_position[i].x-10;
		obj->pos.y = (float)((PTParaEffect)inst)->target_position[i].y-20;
		obj->pos.z = (float)((PTParaEffect)inst)->target_position[i].z+20;
		RenderObject(obj);
	}
// actually we want to animate the texture a few times
	for(int j = 0;j<2;j++)
	{
		for(i=0;i<151;i++)
		{
			obj->lverts[i].tu -= (((PTParaEffect)inst)->val*40);
			obj->lverts[i].tv -= (((PTParaEffect)inst)->val*40);
		}
		for(i=0;i<((PTParaEffect)inst)->num_targets;i++)
		{
			obj->pos.x = (float)((PTParaEffect)inst)->target_position[i].x-10;
			obj->pos.y = (float)((PTParaEffect)inst)->target_position[i].y-20;
			obj->pos.z = (float)((PTParaEffect)inst)->target_position[i].z+20;
			RenderObject(obj);
		}
	}
	for(i=0;i<151;i++)
	{
		obj->lverts[i].tu += (((PTParaEffect)inst)->val*80);
		obj->lverts[i].tv += (((PTParaEffect)inst)->val*80);
	}


	RestoreBlendState();

	return TRUE;
}

void TParaAnimator::RefreshZBuffer()
{
	S3DPoint effect, screen, size;
	int i;

	size.x = 20;
	size.y = 20;

	if(((PTParaEffect)inst)->level != 3)
	{
		for(i = 0;i<MAX_NUM_PARA;i++)
		{
			effect.x = (int)((PTParaEffect)inst)->pos[i].x;
			effect.y = (int)((PTParaEffect)inst)->pos[i].y;
			effect.z = (int)((PTParaEffect)inst)->pos[i].z;
			WorldToScreen(effect, screen);
			RestoreZ(screen.x - (size.x / 2), screen.y - size.y / 2, size.x, size.y);
		}
	}

	for(i=0;i<((PTParaEffect)inst)->num_targets;i++)
	{
		size.x = 50;
		size.y = 50;
		effect.x = (int)((PTParaEffect)inst)->target_position[i].x;
		effect.y = (int)((PTParaEffect)inst)->target_position[i].y;
		effect.z = (int)((PTParaEffect)inst)->target_position[i].z;
		WorldToScreen(effect, screen);
		RestoreZ(screen.x - (size.x / 2), screen.y - size.y / 2, size.x, size.y);
	}
}


/****************************************************************/
/*                         Mana Drain                           */
/****************************************************************/

// ********************
// * ManaDrain Effect *
// ********************

_CLASSDEF(TManaDrainEffect)

class TManaDrainEffect : public TEffect
{
protected:
public:
	int life;
    TManaDrainEffect(PTObjectImagery newim) : TEffect(newim) { }
    TManaDrainEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
	void Initialize();
	void Pulse();
};

DEFINE_BUILDER("ManaDrain", TManaDrainEffect)
REGISTER_BUILDER(TManaDrainEffect)


void TManaDrainEffect::Initialize()
{
	SetCommandDone( FALSE);
	life = 0;			
}

void TManaDrainEffect::Pulse()
{
//	mystrip->ScrollTexture( D3DVALUE( -0.1));
//	life++;
	if(life >= 300)
		KillThisEffect();
}


// *****************************
// *  ManaDrain Animator Code  *
// *****************************

#define MD_MAX_NUM 40
#define MD_STATE_START 1
#define MD_STATE_OUTPUT 2
#define MD_STATE_TRANSFER 3
#define MD_STATE_TRANSFER2 4
#define MD_STATE_INTAKE 5
#define MD_STATE_END 6
#define MD_TYPE_MANA 1
#define MD_TYPE_HEALTH 2
#define MD_MAX_PARTICLE_SCALE 0.08f

_CLASSDEF(TManaDrainAnimator)
class TManaDrainAnimator : public T3DAnimator
{
protected:
	float floor_scale;
	int floor_scale_direction;
	int my_state[MD_MAX_NUM];
	D3DVECTOR position[MD_MAX_NUM];
	D3DVECTOR velocity[MD_MAX_NUM];
	S3DPoint target_position;
	S3DPoint invoker_position;
	PTCharacter target;
	PTCharacter invoker;
	int transfer_time[MD_MAX_NUM];
	int level;
	int type;
	float factor;
	float particle_scale[MD_MAX_NUM];
	float group_angle;
// these track the mana pile above each person
	float invoker_glow_scale;
	float target_glow_scale;
	float glow_rotation;
	float invoker_glow_z;
	float target_glow_z;
	int invoker_glow_z_direction;
	int target_glow_z_direction;
	int current_particle;
public:
	void Initialize();
	BOOL Render();
    TManaDrainAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
	void RefreshZBuffer();
	void Animate(BOOL);
};

REGISTER_3DANIMATOR("ManaDrain", TManaDrainAnimator)

void TManaDrainAnimator::Initialize()
{
	int i;

	T3DAnimator::Initialize();

	((PTManaDrainEffect)inst)->Initialize();

	PS3DAnimObj obj = GetObject(12);
    GetVerts(obj, D3DVT_LVERTEX);
	for(i=0;i<obj->numverts;i++)
	{
		obj->lverts[i].tv -= 0.05f;
	}

	obj = GetObject(3);
    GetVerts(obj, D3DVT_LVERTEX);
	for(i=0;i<obj->numverts;i++)
	{
		obj->lverts[i].tv -= 0.05f;
	}
	
	factor = 0.0f;

	floor_scale = 0.6f;
	floor_scale_direction = 1;

	target_glow_scale = 0.01f;
	invoker_glow_scale = 0.01f;
	glow_rotation = 0.0f;
	invoker_glow_z = (float)random(-30,30) / 10;
	target_glow_z = (float)random(-30,30) / 10;
	invoker_glow_z_direction = 1;
	target_glow_z_direction = 1;

// we REALLY want a target on this spell!
	PTSpell spell = ((PTManaDrainEffect)inst)->GetSpell();
	if(spell)
	{
	// set up the spell
		if(strcmp(spell->VariantData()->name, "ManaDrain1") == 0)
		{
			level = 1;
			type = MD_TYPE_MANA;
		}
		else if(strcmp(spell->VariantData()->name, "ManaDrain2") == 0)
		{
			level = 2;
			type = MD_TYPE_MANA;
		}
		else if(strcmp(spell->VariantData()->name, "LifeDrain1") == 0)
		{
			level = 1;
			type = MD_TYPE_HEALTH;
		}
		else if(strcmp(spell->VariantData()->name, "LifeDrain2") == 0)
		{
			level = 2;
			type = MD_TYPE_HEALTH;
		}
		invoker = (PTCharacter)spell->GetInvoker();
		if(invoker)
		{
			invoker->GetPos(invoker_position);
			target = (PTCharacter)invoker->Fighting();
			if(target)
			{
				target->GetPos(target_position);
			}
			else
			{
				S3DPoint temp_point1;
				S3DPoint temp_point2;
				invoker->GetPos(temp_point1);
				for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
				{
					PTCharacter chr = (PTCharacter)i.Item();

					if (chr == invoker)
						continue;

					if (chr->IsDead())
						continue;

					chr->GetPos(temp_point2);
					if (Distance(temp_point1,temp_point2) > 300)
						continue;

					if (invoker && !invoker->IsEnemy(chr))
						continue;	// We can't hurt our friends
					
					target = chr;
					break;
				}
				if(target)
				{
					target->GetPos(target_position);
				}
				else
				{
					invoker->GetPos(target_position);
					target = invoker;
				}
			}
		}
		else
		{
			target = NULL;
			target_position.x = target_position.y = target_position.z = 0;
			invoker_position.x = invoker_position.y = invoker_position.z = 0;
		}
	}
	else
	{
		target = NULL;
		target_position.x = target_position.y = target_position.z = 0;
		invoker_position.x = invoker_position.y = invoker_position.z = 0;
	}


	target_position.z = (int)FIX_Z_VALUE(target_position.z);
	invoker_position.z = (int)FIX_Z_VALUE(invoker_position.z);

	group_angle = (float)atan(max(1,(invoker_position.x - target_position.x)) / max(1,(invoker_position.y - target_position.y)));
	group_angle = group_angle / 6.28318f;

	for(i=0;i<MD_MAX_NUM;i++)
	{
		position[i].x = (float)target_position.x;// + (float)random(-200,200) / 10;
		position[i].y = (float)target_position.y;// + (float)random(-200,200) / 10;
		position[i].z = (float)(target_position.z + 50);// + ((float)random(-200,200) / 10);
		velocity[i].x = (float)random(-50,50) / 10;
		velocity[i].y = (float)random(-50,50) / 10;
		velocity[i].z = -0.8f;
		transfer_time[i] = random(i*2,(i+4)*2);
		particle_scale[i] = 0.0f;
		my_state[i] = MD_STATE_START;
	}
}

void TManaDrainAnimator::Animate(BOOL draw)
{
	int i;
	int count = 0;

	glow_rotation += 0.5f;
	if(target_glow_z_direction == 1)
	{
		target_glow_z += 0.5;
		if(target_glow_z > 5.0f)
		{
			target_glow_z = 5.0f;
			target_glow_z_direction = -1;
		}
	}
	else
	{
		target_glow_z -= 0.5;
		if(target_glow_z < -5.0f)
		{
			target_glow_z = -5.0f;
			target_glow_z_direction = 1;
		}
	}
	if(invoker_glow_z_direction == 1)
	{
		invoker_glow_z += 0.5;
		if(invoker_glow_z > 5.0f)
		{
			invoker_glow_z = 5.0f;
			invoker_glow_z_direction = -1;
		}
	}
	else
	{
		target_glow_z -= 0.5;
		if(target_glow_z < -5.0f)
		{
			target_glow_z = -5.0f;
			invoker_glow_z_direction = 1;
		}
	}


	if(floor_scale_direction == 1)
	{
		floor_scale += 0.01f;
		if(floor_scale >= 0.7f)
			floor_scale_direction = -1;
	}
	else
	{
		floor_scale -= 0.01f;
		if(floor_scale <= 0.5f)
			floor_scale_direction = 1;
	}		

	if(target)
	{
		target->Stop();
		target->GetPos(target_position);
		target_position.z = (int)FIX_Z_VALUE(target_position.z);
	}
	if(invoker)
	{
		invoker->GetPos(invoker_position);
		invoker_position.z = (int)FIX_Z_VALUE(invoker_position.z);
	}


	for(i=0;i<MD_MAX_NUM;i++)
	{
		switch(my_state[i])
		{
			case MD_STATE_START:
				position[i].x += velocity[i].x;
				position[i].y += velocity[i].y;
				position[i].z += velocity[i].z;
				velocity[i].x -= velocity[i].x * factor;
				velocity[i].y -= velocity[i].y * factor;
				if(i == 0)
					factor += 0.2f;
				if(position[i].z <= (target_position.z+20))
				{
					my_state[i] = MD_STATE_OUTPUT;
					position[i].x = (float)target_position.x + (float)random(-200,200) / 10;
					position[i].y = (float)target_position.y + (float)random(-200,200) / 10;
					position[i].z = target_position.z + 20.0f + ((float)random(0,200) / 10);
					velocity[i].x = ((target_position.x - position[i].x) / 8);
					velocity[i].y = ((target_position.y - position[i].y) / 8);
					velocity[i].z = 2.8f;
				}
				break;
			case MD_STATE_OUTPUT:
				position[i].x = (float)target_position.x + (float)(((position[i].x - target_position.x) * cos(0.2f)) - ((position[i].y - target_position.y) * sin(0.2f)));
				position[i].y = (float)target_position.y + (float)(((position[i].x - target_position.x) * sin(0.2f)) + ((position[i].y - target_position.y) * cos(0.2f)));
				position[i].z += velocity[i].z;
				if(position[i].z > (target_position.z + 100.0f))
				{
					position[i].z = target_position.z + 100.0f;
					if(particle_scale[i] > 0.0f)
					{
						particle_scale[i] -= 0.02f;
						target_glow_scale += 0.002f;
					}
					else
					{
						my_state[i] = MD_STATE_TRANSFER;
						float ratio = (float)random(50,100) / 100;
						if((i%5) < 2)
							particle_scale[i] = MD_MAX_PARTICLE_SCALE * 3;
						else
							particle_scale[i] = MD_MAX_PARTICLE_SCALE;
						velocity[i].x = ratio * ((position[i].x - invoker_position.x) / -8);// + (random(0,50) / 10);
						velocity[i].y = ratio * ((position[i].y - invoker_position.y) / -8);// + (random(0,50) / 10);
						
						transfer_time[i] = (int)(8 / ratio);
						current_particle = 0;
					}
				}
				else if(particle_scale[i] < MD_MAX_PARTICLE_SCALE)
					particle_scale[i] += 0.02f;
				break;
			case MD_STATE_TRANSFER:
				if(transfer_time[i] >= 0)
				{
					transfer_time[i]--;
					if(transfer_time[i] <= 3)
					{
						particle_scale[i] *= 0.75f;
						invoker_glow_scale += 0.0017f;
						target_glow_scale -= 0.0022f;
						if(target_glow_scale < 0.0f)
							target_glow_scale = 0.0f;
					}
					position[i].x += velocity[i].x;
					position[i].y += velocity[i].y;
				}
				
				if(transfer_time[i] <= 0)
				{
					my_state[i] = MD_STATE_INTAKE;
					particle_scale[i] = 0.0f;
					transfer_time[i] = i*2;
					if(i == 0)
					{
			// grab the juice
						if(invoker && target)
						{
							int amt = 20;
							if(((PTManaDrainEffect)inst)->GetSpell())
								amt = ((PTManaDrainEffect)inst)->GetSpell()->VariantData()->maxdamage;
							if(type == MD_TYPE_MANA)
							{
								if(amt > target->Mana())
									amt = target->Mana();
								if(invoker->Mana() + amt > invoker->MaxMana())
									amt = invoker->MaxMana() - invoker->Mana();
								if(amt < 0)
									amt = 0;
								target->SetMana(target->Mana() - amt);
								invoker->SetMana(invoker->Mana() + amt);
							}
							else if(type == MD_TYPE_HEALTH)
							{
								if(amt > target->Health())
									amt = target->Health();
								if(invoker->Health() + amt > invoker->MaxHealth())
									amt = invoker->MaxHealth() - invoker->Health();
								if(amt < 0)
									amt = 0;
								target->SetHealth(target->Health() - amt);
								invoker->SetHealth(invoker->Health() + amt);
							}
						}
					}
				}
				break;
			case MD_STATE_INTAKE:
				transfer_time[i]--;
				if(transfer_time[i] == 0)
				{
					particle_scale[i] = MD_MAX_PARTICLE_SCALE;
					invoker_glow_scale -= 0.007f;
					if(invoker_glow_scale < 0.0f)
						invoker_glow_scale = 0.0f;
				}
				if(transfer_time[i] < 0)
				{
					position[i].x = (float)invoker_position.x + (float)(((position[i].x - invoker_position.x) * cos(0.2f)) - ((position[i].y - invoker_position.y) * sin(0.2f))) - ((position[i].x - invoker_position.x) / 4);
					position[i].y = (float)invoker_position.y + (float)(((position[i].x - invoker_position.x) * sin(0.2f)) + ((position[i].y - invoker_position.y) * cos(0.2f))) - ((position[i].y - invoker_position.y) / 4);
					position[i].z -= 4.0f;
				}
				if(position[i].z < (invoker_position.z + 40.0f))
				{
					if(particle_scale[i] > 0.0f)
						particle_scale[i] -= 0.02f;
				}
				if(i == (MD_MAX_NUM - 1))
				{
					if(particle_scale[i] <= 0.0f && transfer_time[i] < 0)
					{
						my_state[i] = MD_STATE_END;
					}
				}
				break;
			case MD_STATE_END:
				if(i == (MD_MAX_NUM - 1))
					((PTManaDrainEffect)inst)->life = 300;
				break;
		}
	}

	T3DAnimator::Animate(draw);
}

BOOL TManaDrainAnimator::Render()
{
	int i;

	SaveBlendState();
	SetBlendState();

	PS3DAnimObj obj;
// lower circle
	if(level == 2)
	{
		if(type == MD_TYPE_MANA)
			obj = GetObject(3);
		else
			obj = GetObject(12);

		obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_VERTS | OBJ3D_ABSPOS; //OBJ3D_POS3 | OBJ3D_ABSPOS | OBJ3D_VERTS;

		for(i = 0; i < obj->numverts; i++)
		{
			obj->lverts[i].tu += 0.05f;
//			obj->lverts[i].color = D3DRGBA(fade_amount,fade_amount,fade_amount,fade_amount);
		}
		obj->scl.x = 0.5f;
		obj->scl.y = 0.5f;
		obj->scl.z = floor_scale;

		obj->pos.x = (float)target_position.x;
		obj->pos.y = (float)target_position.y;
		obj->pos.z = (float)target_position.z;

		obj->rot.x = 0.0f;
		obj->rot.y = 0.0f;
		obj->rot.z = 0.0f;

		RenderObject( obj);
	}

/*
		PS3DAnimObj obj = GetObject(3);

		obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_VERTS | OBJ3D_ABSPOS; //OBJ3D_POS3 | OBJ3D_ABSPOS | OBJ3D_VERTS;

		for(i = 0; i < obj->numverts; i++)
		{
			obj->lverts[i].color = D3DRGBA(fade_amount,fade_amount,fade_amount,fade_amount);
		}
		obj->scl.x = 0.3f;
		obj->scl.y = 0.3f;
		obj->scl.z = 0;

		obj->pos.x = (float)target_position.x;
		obj->pos.y = (float)target_position.y;
		obj->pos.z = (float)target_position.z;

		obj->rot.x = 0.0f;
		obj->rot.y = 0.0f;
		obj->rot.z = rotation;

		RenderObject( obj);
*/


// Now we render the glow for the pile'o mana
	if(type == MD_TYPE_MANA)
		obj = GetObject(1);
	else
		obj = GetObject(10);

	obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;

	obj->pos.x = (float)target_position.x;
	obj->pos.y = (float)target_position.y;
	obj->pos.z = target_position.z + target_glow_z + 100;

	if(random(0,5) == 1)
		obj->scl.z = obj->scl.x = obj->scl.y = target_glow_scale * (1.0f + (float)random(-10,10) / 100.0f);
	else
		obj->scl.z = obj->scl.x = obj->scl.y = target_glow_scale;

	obj->rot.x = obj->rot.y = 0.0f;
	obj->rot.z = 0.0f;//glow_rotation;

	RenderObject(obj);

	if(type == MD_TYPE_MANA)
		obj = GetObject(1);
	else
		obj = GetObject(10);
	obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;

	obj->pos.x = (float)invoker_position.x;
	obj->pos.y = (float)invoker_position.y;
	obj->pos.z = invoker_position.z + invoker_glow_z + 100;

	if(random(0,5) == 1)
		obj->scl.z = obj->scl.x = obj->scl.y = invoker_glow_scale * (1.0f + (float)random(-10,10) / 100.0f);
	else
		obj->scl.z = obj->scl.x = obj->scl.y = invoker_glow_scale;

	obj->rot.x = obj->rot.y = 0.0f;
	obj->rot.z = 0.0f; //-glow_rotation;

	RenderObject(obj);

	for(i=0;i<MD_MAX_NUM;i++)
	{
		if(my_state[0] == MD_STATE_START)
		{
		// sparks
			if(type == MD_TYPE_MANA)
				obj = GetObject(0);
			else
				obj = GetObject(9);

			obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS; //OBJ3D_POS3 | OBJ3D_ABSPOS | OBJ3D_VERTS;

			if(random(0,1) == 0)
			{
				obj->pos.x = position[i].x;
				obj->pos.y = position[i].y;
				obj->pos.z = position[i].z;

				obj->scl.x = 0.10f;
				obj->scl.y = 0.10f;
				obj->scl.z = 0.10f;

				RenderObject(obj);
			}
		}
		else if(my_state[0] == MD_STATE_TRANSFER)
		{
		// mana glow

			if(transfer_time[i] > 0)
			{
				if(type == MD_TYPE_MANA)
					obj = GetObject(4 + i%5);
				else
					obj = GetObject(13 + i%5);


				obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS2 | OBJ3D_ABSPOS; //OBJ3D_POS3 | OBJ3D_ABSPOS | OBJ3D_VERTS;

				obj->pos.x = position[i].x;
				obj->pos.y = position[i].y;
				obj->pos.z = position[i].z;

				obj->rot.x = obj->rot.y = obj->rot.z = group_angle;

				obj->scl.x = particle_scale[i];
				obj->scl.y = particle_scale[i];
				obj->scl.z = particle_scale[i];

				RenderObject(obj);
			}
		}
		else if(my_state[0] == MD_STATE_OUTPUT)
		{
		// mana glow

			if(type == MD_TYPE_MANA)
				obj = GetObject(6 + i%3);
			else
				obj = GetObject(15 + i%3);

			obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS; //OBJ3D_POS3 | OBJ3D_ABSPOS | OBJ3D_VERTS;

			obj->pos.x = position[i].x;
			obj->pos.y = position[i].y;
			obj->pos.z = position[i].z;

			obj->scl.x = particle_scale[i];
			obj->scl.y = particle_scale[i];
			obj->scl.z = particle_scale[i];

			RenderObject(obj);
		}
		else if(my_state[0] == MD_STATE_INTAKE)
		{
		// mana glow

			if(type == MD_TYPE_MANA)
				obj = GetObject(6 + i%3);
			else
				obj = GetObject(15 + i%3);

			obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS; //OBJ3D_POS3 | OBJ3D_ABSPOS | OBJ3D_VERTS;

			obj->pos.x = position[i].x;
			obj->pos.y = position[i].y;
			obj->pos.z = position[i].z;

			obj->scl.x = particle_scale[i];
			obj->scl.y = particle_scale[i];
			obj->scl.z = particle_scale[i];

			RenderObject(obj);
		}
	}

	RestoreBlendState();
	return TRUE;
}

void TManaDrainAnimator::RefreshZBuffer()
{
	S3DPoint effect, screen, size;

	size.x = 400;
	size.y = 400;

	((PTEffect)inst)->GetPos(effect);

	WorldToScreen(effect, screen);
	RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

/****************************************************************/
/*                       Wraith Shriek                          */
/****************************************************************/

// ********************
// *   Shriek Effect  *
// ********************

#define SHRIEK_MAX_SHRIEKS 8
#define SHRIEK_MAX_SHRIEK_SCALE 2.0f

_CLASSDEF(TShriekEffect)

class TShriekEffect : public TEffect
{
protected:
public:
    TShriekEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TShriekEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
	void Initialize();
	void Pulse();
};

DEFINE_BUILDER("Shriek", TShriekEffect)
REGISTER_BUILDER(TShriekEffect)


void TShriekEffect::Initialize()
{
	SetCommandDone( FALSE);
}

void TShriekEffect::Pulse()
{
	TEffect::Pulse();
}


// **************************
// *  Shriek Animator Code  *
// **************************

_CLASSDEF(TShriekAnimator)
class TShriekAnimator : public T3DAnimator
{
protected:
	float shriek_scale[SHRIEK_MAX_SHRIEKS];
	int start_time[SHRIEK_MAX_SHRIEKS];
	float shriek_pos[SHRIEK_MAX_SHRIEKS];
	float shriek_fade[SHRIEK_MAX_SHRIEKS];
	int life;
	float anglec;
	float angles;
public:
	void Initialize();
	BOOL Render();
	void Animate(BOOL);
    TShriekAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
	void RefreshZBuffer();
};

REGISTER_3DANIMATOR("Shriek", TShriekAnimator)

void TShriekAnimator::Initialize()
{
	int i;
	PS3DAnimObj obj;

	T3DAnimator::Initialize();

	for(i=0;i<SHRIEK_MAX_SHRIEKS;i++)
	{
		shriek_scale[i] = 0.0f;
		start_time[i] = random(0,SHRIEK_MAX_SHRIEKS*4);
		shriek_pos[i] = 0.0f;
		shriek_fade[i] = 1.0f;
	}

	life = 0;

	for(i=0;i<4;i++)
	{
		obj = GetObject(i);
	    GetVerts(obj, D3DVT_LVERTEX);
	}

	if(((PTShriekEffect)inst)->GetSpell())
	{
		if(((PTShriekEffect)inst)->GetSpell()->GetInvoker())
		{
			anglec = (float)cos(((PTShriekEffect)inst)->GetSpell()->GetInvoker()->GetFace() * (6.28318 / 256));
			angles = (float)sin(((PTShriekEffect)inst)->GetSpell()->GetInvoker()->GetFace() * (6.28318 / 256));
		}
	}
}

void TShriekAnimator::Animate(BOOL draw)
{
	int i;

	life++;

	for(i=0;i<SHRIEK_MAX_SHRIEKS;i++)
	{
		if(start_time[i] > 0)
			start_time[i]--;
		else if(shriek_scale[i] != 0.0f || life < 50)
		{
			shriek_scale[i] += 0.12f;
			shriek_pos[i] += 1.0f;
			if(shriek_scale[i] > SHRIEK_MAX_SHRIEK_SCALE)
			{
				shriek_scale[i] = 0.0f;
				shriek_pos[i] = 0.0f;
				shriek_fade[i] = 1.0f;
			}
			else if(shriek_scale[i] > (SHRIEK_MAX_SHRIEK_SCALE - .48f))
			{
				shriek_fade[i] -= 0.25f;
			}
		}
	}

	PTSpell spell = ((PTShriekEffect)inst)->GetSpell();
	PTCharacter invoker;
	S3DPoint position;
	int min = 0,max = 0;
	int dtype;
	if(spell)
	{
		invoker = (PTCharacter)spell->GetInvoker();
		min = spell->VariantData()->mindamage;
		max = spell->VariantData()->maxdamage;
		dtype = spell->SpellData()->damagetype;
	}
	inst->GetPos(position);
	
	position.x -= (int)angles * (int)shriek_scale[0] * 8;
	position.y -= (int)anglec * (int)shriek_scale[0] * 8;
	position.y -= (int)shriek_pos[i];

	BlastCharactersInRange(invoker, position, (int)(shriek_scale[0] * 60), min, max, dtype);

	T3DAnimator::Animate(draw);

	if(life > 40)
		((PTShriekEffect)inst)->KillThisEffect();
}
BOOL TShriekAnimator::Render()
{
	int i;
//	int j;

	SaveBlendState();
	SetBlendState();

	PS3DAnimObj obj;

	for(i=0;i<SHRIEK_MAX_SHRIEKS;i++)
	{
		obj = GetObject( i%4);

		obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;// | OBJ3D_VERTS;// | OBJ3D_ABSPOS; // | OBJ3D_VERTS;// | OBJ3D_ABSPOS | OBJ3D_VERTS;

		obj->scl.x = obj->scl.y = obj->scl.z = shriek_scale[i];

		obj->pos.x = obj->pos.y = shriek_scale[i] * -4;
		obj->pos.y -= shriek_pos[i];
		obj->pos.z = FIX_Z_VALUE(54.0f);

//		for(j = 0; j < obj->numverts; j++)
//		{
//			obj->lverts[j].color = D3DRGBA(shriek_fade[i],shriek_fade[i],shriek_fade[i],shriek_fade[i]);
//		}

		RenderObject( obj);
	}

	RestoreBlendState();
	return TRUE;
}

void TShriekAnimator::RefreshZBuffer()
{
	S3DPoint effect, screen, size;

	size.x = 50;
	size.y = 50;

	((PTEffect)inst)->GetPos(effect);

	WorldToScreen(effect, screen);
	RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}






/****************************************************************/
/*                         Zombie Puke                          */
/****************************************************************/

// ********************
// *    Puke Effect   *
// ********************

#define PUKE_MAX_NUM 25

_CLASSDEF(TPukeEffect)

class TPukeEffect : public TEffect
{
protected:
public:
    TPukeEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TPukeEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
	void Initialize();
	void Pulse();
};

DEFINE_BUILDER("Puke", TPukeEffect)
REGISTER_BUILDER(TPukeEffect)


void TPukeEffect::Initialize()
{
	SetCommandDone( FALSE);
}

void TPukeEffect::Pulse()
{
	TEffect::Pulse();
}


// **************************
// *   Puke Animator Code   *
// **************************

_CLASSDEF(TPukeAnimator)
class TPukeAnimator : public T3DAnimator
{
protected:
	D3DVECTOR position[PUKE_MAX_NUM];
	D3DVECTOR velocity[PUKE_MAX_NUM];
	int start_time[PUKE_MAX_NUM];
	float scale;
	int life;
	BOOL firsttime;
public:
	void Initialize();
	BOOL Render();
    TPukeAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
	void RefreshZBuffer();
	void Animate(BOOL);
};

REGISTER_3DANIMATOR("Puke", TPukeAnimator)

void TPukeAnimator::Initialize()
{
//	int i;

	T3DAnimator::Initialize();

	life = 50;
	scale = 0.1f;

//	PS3DAnimObj obj = GetObject(1);
//    GetVerts(obj, D3DVT_LVERTEX);

	firsttime = TRUE;
}

void TPukeAnimator::Animate(BOOL draw)
{
	int i;

	life--;

	if(life == 30)
	{
	  // If no source, use character's hand as the source
		S3DPoint sourcepos;
		sourcepos.x = sourcepos.y = 0;
		sourcepos.z = 50;
		PTSpell spell = ((PTPukeEffect)inst)->GetSpell();
		if(spell)
		{
			PTCharacter invoker = (PTCharacter)spell->GetInvoker();
			if(invoker)
			{
				if (invoker->GetAnimator() /*&& (invoker->GetImagery()->ImageryId() != OBJIMAGE_MESH3D)*/)
				{
					if (((PT3DAnimator)invoker->GetAnimator())->GetObjectMapPos("warrior_he", sourcepos))
					{
						int z = sourcepos.z;
						ConvertToVector(invoker->GetFace(), Distance(sourcepos), sourcepos);
						sourcepos.z = z + 50;
					}
					else
					{
						sourcepos.x = sourcepos.y = 0;
						sourcepos.z = 50;
					}
				}
			}
		}
		for(i=0;i<PUKE_MAX_NUM;i++)
		{
			position[i].x = (float)sourcepos.x;
			position[i].y = (float)sourcepos.y;
			position[i].z = (float)sourcepos.z;

			velocity[i].x = (float)random(-5,5) / 10;
			velocity[i].y = -(float)random(35,45) / 10;
			velocity[i].z = -(float)random(20,50) / 30;
			start_time[i] = (int) i >> 4;
		}
	}
	else if(life < 30)
	{
		PTSpell spell = ((PTPukeEffect)inst)->GetSpell();

		for(i=0;i<PUKE_MAX_NUM;i++)
		{
			if(start_time[i] <= 0)
			{
				position[i].x += velocity[i].x;
				position[i].y += velocity[i].y;
				position[i].z += velocity[i].z;
				velocity[i].z -= 0.5f;
				velocity[i].y -= 0.2f;
				if(position[i].z < 10.0f && life > 10)
				{
					life = 10;
				}
			}
			else
				start_time[i]--;
		}

		if(firsttime)
		{
			if(spell)
			{
				PTCharacter invoker = (PTCharacter)spell->GetInvoker();
				if(invoker)
				{
					S3DPoint temp_point1, temp_point2;
					float ang = (float)((6.283f * (float)invoker->GetFace()) / 256);
					invoker->GetPos(temp_point1);
					temp_point1.x += (int)((position[0].x * cos(ang)) - (position[0].y * sin(ang)));
					temp_point1.y += (int)((position[0].x * sin(ang)) + (position[0].y * cos(ang)));
					temp_point1.z = (int)(position[0].z);
					for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
					{
						PTCharacter chr = (PTCharacter)i.Item();

						if (chr == invoker)
							continue;

						if (chr->IsDead())
							continue;

						chr->GetPos(temp_point2);
						if (Distance(temp_point1,temp_point2) > 16)
							continue;

						if (invoker && !invoker->IsEnemy(chr))
							continue;	// We can't hurt our friends
						
						spell->Damage(chr);
						firsttime = FALSE;
						break;
					}
				}
			}
		}

		if(life < 10)
		{
			scale -= 0.01f;
			if(scale < 0.0f)
				scale = 0.0f;
		}
		if(life <= 0)
		{
			((PTEffect)inst)->KillThisEffect();
		}
	}

	T3DAnimator::Animate(draw);
}

BOOL TPukeAnimator::Render()
{
	int i;
	PS3DAnimObj obj = GetObject(0);

	if(life < 30)
	{
		SaveBlendState();
		SetBlendState();

		obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;

		obj->scl.x = obj->scl.y = obj->scl.z = scale;

		for(i=0;i<PUKE_MAX_NUM;i+=3)
		{
			if(start_time[i] <= 0)
			{
				obj->pos.x = position[i].x;
				obj->pos.y = position[i].y;
				obj->pos.z = position[i].z;

				RenderObject(obj);
			}
		}

		obj = GetObject(1);
		obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;

		obj->scl.x = obj->scl.y = obj->scl.z = scale;

		for(i=1;i<PUKE_MAX_NUM;i+=3)
		{
			if(start_time[i] <= 0)
			{
				obj->pos.x = position[i].x;
				obj->pos.y = position[i].y;
				obj->pos.z = position[i].z;

				RenderObject(obj);
			}
		}


		obj = GetObject(2);
		obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;

		obj->scl.x = obj->scl.y = obj->scl.z = scale;

		for(i=2;i<PUKE_MAX_NUM;i+=3)
		{
			if(start_time[i] <= 0)
			{
				obj->pos.x = position[i].x;
				obj->pos.y = position[i].y;
				obj->pos.z = position[i].z;

				RenderObject(obj);
			}
		}

		RestoreBlendState();
	}

	return TRUE;
}

void TPukeAnimator::RefreshZBuffer()
{
	S3DPoint effect, screen, size;

	size.x = 50;
	size.y = 50;

	((PTEffect)inst)->GetPos(effect);

	WorldToScreen(effect, screen);
	RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x + (size.x / 2), size.y + (size.y / 2));
}












#if 0

/****************************************************************/
/*                        Tornado Helper                        */
/****************************************************************/

_STRUCTDEF(STornadoDust)

struct STornadoDust
{
	S3DPoint position;
	D3DVECTOR velocity;
	D3DVECTOR acceleration;
	float fade;
	float scale;
	BOOL used;
};

#define TORNADO_NUM_DUST 20

_CLASSDEF(TTornadoSystem)

class TTornadoSystem
{
private:
	PT3DAnimator animator;				// pointer to the animator used
	PS3DAnimObj object;					// pointer to the object for the funnel
	PS3DAnimObj dust_object;			// pointer to the object for the dust
	int num_sides;						// how many sides
	int num_rings;						// how many verticle segments
	S3DPoint center;					// center of the tornado
	float height;						// tornado height
	float magnitude;					// oscillation magnitude
	D3DVECTOR velocity;					// velocity of tornado
	float min_radius;
	float max_radius;
	LPD3DVECTOR ring_velocity;
	S3DPoint target_position;
	int *direction;
	float *change;
	int *ring_magnitude;
	int *ring_start_time;
	PSTornadoDust dust;
	int life;
	int vert_position;
public:
	TTornadoSystem(){}
	~TTornadoSystem();
	PS3DAnimObj GetTornadoObj(){return object;}
	BOOL Init(PT3DAnimator,PS3DAnimObj,PS3DAnimObj,int,int,S3DPoint,float,float,D3DVECTOR,float,float,S3DPoint);
	void Pulse();
	BOOL Render();
};


BOOL TTornadoSystem::Init(PT3DAnimator ani,PS3DAnimObj o,PS3DAnimObj o2,int ns,int nr,S3DPoint c,float h,float m,D3DVECTOR v,float minr,float maxr,S3DPoint tp)
{
	int i,j,val;

	animator = ani;
	object = o;
	dust_object = o2;
	num_sides = ns;
	num_rings = nr;
	center = c;
	height = h;
	magnitude = m;
	velocity = v;
	min_radius = minr;
	max_radius = maxr;
	target_position = tp;

	life = 0;

	animator->GetVerts(dust_object, D3DVT_LVERTEX);

// Build the actual tornado model
	// Paraphrased GetVerts
	object->numverts = (num_sides * (num_rings + 1));
	object->lverts = new D3DLVERTEX[object->numverts];
	object->verttype = D3DVT_LVERTEX;

	// Paraphrased GetFaces
	object->numfaces = (num_sides * 2 * num_rings);
	if (!object->faces)
		object->faces = new S3DFace[object->numfaces];
	object->texfaces[0] = 0;
	object->texfaces[1] = 0;
	object->numtexfaces[0] = 0;
	object->numtexfaces[1] = object->numfaces;

	ring_velocity = new D3DVECTOR[num_rings+1];
	direction = new int[num_rings+1];
	change = new float[num_rings+1];
	ring_magnitude = new int[num_rings+1];
	ring_start_time = new int[num_rings+1];

	j = 0;
	// Build Vertex index for each Face
	for (i = 0; i < object->numfaces; i++)
	{
		switch(i%2)
		{
			case 0:
				if((j%num_sides) == (num_sides - 1))
				{
					object->faces[i].v1 = (j%8) + (num_sides * (int)(j / 8));
					object->faces[i].v2 = object->faces[i].v1 + 1 - num_sides;
					object->faces[i].v3 = object->faces[i].v1 + num_sides;
				}
				else
				{
					object->faces[i].v1 = (j%8) + (num_sides * (int)(j / 8));
					object->faces[i].v2 = object->faces[i].v1 + 1;
					object->faces[i].v3 = object->faces[i].v1 + num_sides;
				}
				j++;
				break;
			case 1:
				if(((j-1)%num_sides) == (num_sides - 1))
				{
					object->faces[i].v1 = object->faces[i-1].v1 + 1 - num_sides;
					object->faces[i].v2 = object->faces[i-1].v3 + 1 - num_sides;
					object->faces[i].v3 = object->faces[i-1].v3;
				}
				else
				{
					object->faces[i].v1 = object->faces[i-1].v1 + 1;
					object->faces[i].v2 = object->faces[i-1].v3 + 1;
					object->faces[i].v3 = object->faces[i-1].v3;
				}
				break;
		}
	}

	// Build the Verex list
	float ang, radius,height_val;
	for(i=0;i<object->numverts;i++)
	{
		ang = (float)(i%num_sides);
		ang = (float)ang / num_sides;
		ang = ang * 6.28318f;
		radius = max_radius - min_radius;
		radius = radius * (i / num_sides);
		radius = radius / num_rings;
		radius = radius + min_radius;
		height_val = height;
		height_val = height_val * i;
		height_val = (float)(height_val / num_sides);
		height_val = height_val / num_rings;
		object->verts[i].x = center.x + (float)(radius * cos(ang));
		object->verts[i].y = center.y + (float)(radius * sin(ang));
		object->verts[i].z = center.z + (float)height_val;
		object->lverts[i].color = D3DRGBA(1.0f,1.0f,1.0f,0.8f);
//		object->lverts[i].color = D3DRGBA(0.7f,0.5f,0.2f,0.8f);
		object->lverts[i].tu = ((float)(i % num_sides) / (float)num_sides);
		object->lverts[i].tv = ((float)((int)(i / num_sides)) / (float)(num_rings+1));
	}		

	for(i=0;i<(num_rings+1);i++)
	{
		val = i*6;
		change[i] = (float)val;
		if(val > 0)
			direction[i] = -1;
		else
			direction[i] = 1;
		for(j=0;j<num_sides;j++)
		{
			object->verts[(i*num_sides)+j].x += val;
			object->verts[(i*num_sides)+j].y += val;
		}
		ring_start_time[i] = i * 5;
	}

	for(i=0;i<(int)(num_rings+1)/2;i++)
	{
		ring_magnitude[i] = (int)(magnitude * (int)(i)) / num_rings;
	}

	for(i=(int)(num_rings+1)/2;i<(int)(num_rings+1);i++)
	{
		ring_magnitude[i] = (int)(magnitude * (int)((num_rings + 1 - i))) / num_rings;
	}

	object->flags = OBJ3D_MATRIX | OBJ3D_ROT1 | OBJ3D_VERTS | OBJ3D_FACES | OBJ3D_OWNSVERTS | OBJ3D_OWNSFACES;
    D3DMATRIXClear( &object->matrix);


// Initialize the tornados dust
	dust = new STornadoDust[TORNADO_NUM_DUST];

	for(i=0;i<TORNADO_NUM_DUST;i++)
	{
		dust[i].used = FALSE;
	}

	return TRUE;
}

void TTornadoSystem::Pulse()
{
	int i,j,k;
	float speed;
	BOOL start_dust = FALSE;

	life++;

	k=0;
	for(i=0;i<object->numverts;i+=num_sides)
	{
//		valx = random(-3,3);
//		valy = random(-3,3);
		speed = 0.8f;
		if(direction[k] == -1)
		{
			change[k] -= speed;
			if(change[k] < -ring_magnitude[k])
			{
				direction[k] = 1;
			}
			for(j=0;j<num_sides;j++)
			{
				object->verts[i+ j].x -= speed;
				object->verts[i+ j].y -= speed;
	//			object->verts[i+ j].z += velocity.z;
				object->lverts[i + j].tu += 0.06f;
			}
		}
		else
		{
			change[k] += speed;
			if(change[k] > ring_magnitude[k])
			{
				direction[k] = -1;
			}
			for(j=0;j<num_sides;j++)
			{
				object->verts[i+ j].x += speed;
				object->verts[i+ j].y += speed;
	//			object->verts[i+ j].z += velocity.z;
				object->lverts[i + j].tu += 0.06f;
			}
		}
		if(ring_start_time[k] <= 0)
		{
			for(j=0;j<num_sides;j++)
			{
				object->verts[i + j].x += velocity.x;
				object->verts[i + j].y += velocity.y;
			}
		}
		else
			ring_start_time[k]--;
		k++;
	}

// handle the dust

	if(life < 5)
	{
		start_dust = TRUE;
	}
	else
	{
		life = 0;
	}
	for(i=0;i<TORNADO_NUM_DUST;i++)
	{
		if(dust[i].used == TRUE)
		{
			dust[i].position.x += (int)dust[i].velocity.x;
			dust[i].position.y += (int)dust[i].velocity.y;
			dust[i].position.z += (int)dust[i].velocity.z;

			if(dust[i].velocity.x != 0.0f)
				dust[i].velocity.x += dust[i].acceleration.x;
			if(dust[i].velocity.x != 0.0f)
				dust[i].velocity.y += dust[i].acceleration.y;
			dust[i].velocity.z += dust[i].acceleration.z;

			dust[i].scale += 0.02f;
			dust[i].fade -= 0.025f;

			if(dust[i].fade <= 0.0f)
			{
				dust[i].used = FALSE;
			}
		}
		else if(start_dust)
		{
			start_dust = FALSE;

			dust[i].fade = 0.5f;
			dust[i].scale = 0.1f;
			dust[i].used = TRUE;

			dust[i].position.x = (int)object->verts[0].x + random(-1,1);
			dust[i].position.y = (int)object->verts[0].y + random(-1,1);
			dust[i].position.z = (int)object->verts[0].z;

			if(random(0,1) == 1)
			{
				dust[i].velocity.x = 0.0f;
				dust[i].velocity.y = 0.0f;
				dust[i].velocity.z = 1.5f;

				dust[i].acceleration.x = 0.0f;
				dust[i].acceleration.y = 0.0f;
				dust[i].acceleration.z = 0.1f;
			}
			else
			{
				dust[i].velocity.x = 0.0f;
				dust[i].velocity.y = 0.0f;
				dust[i].velocity.z = 1.5f;

				dust[i].acceleration.x = 0.0f;
				dust[i].acceleration.y = 0.0f;
				dust[i].acceleration.z = 0.1f;
			}
		}
	}
}


TTornadoSystem::~TTornadoSystem()
{
	delete ring_velocity ;
	delete direction;
	delete change;
	delete ring_magnitude;
	delete ring_start_time;
	delete dust;
}

BOOL TTornadoSystem::Render()
{
	int i,j;

	SaveBlendState();
	SetBlendState();

	DWORD oldcullmode;
	Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullmode);
	Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);


	animator->RenderObject(object);

//	GetExtents(&extents);
//	AddUpdateRect(&extents, UPDATE_RESTORE);
//	UpdateBoundingRect(&extents);

	Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);

	for(i=0;i<TORNADO_NUM_DUST;i++)
	{
		if(dust[i].used)
		{
			dust_object->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_VERTS;
			dust_object->pos.x = (float)dust[i].position.x;
			dust_object->pos.y = (float)dust[i].position.y;
			dust_object->pos.z = (float)dust[i].position.z;

			dust_object->scl.x = dust[i].scale;
			dust_object->scl.y = dust[i].scale;
			dust_object->scl.z = dust[i].scale;

			for(j=0;j<dust_object->numverts;j++)
			{
				dust_object->lverts[j].color = D3DRGBA(0.5f,0.4f,0.4f,dust[i].fade);
			}
		
			animator->RenderObject(dust_object);
		}
	}

	RestoreBlendState();

	return TRUE;
}


#endif




/****************************************************************/
/*                       Invisible Spell                        */
/****************************************************************/

// *************************
// *    Invisible Effect   *
// *************************

#define INVIS_MAX_LIFE 300
#define INVIS_RING_MAX_LIFE 30
#define INVIS_RING_MAX_NUM 6
#define INVIS_STATE_HIDE 1
#define INVIS_STATE_WAIT 2
#define INVIS_STATE_SHOW 3

_CLASSDEF(TInvisibleEffect)

class TInvisibleEffect : public TEffect
{
protected:
public:
	int life;
    TInvisibleEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TInvisibleEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
	void Initialize();
	void Pulse();
	void OffScreen();
};

DEFINE_BUILDER("Invisible", TInvisibleEffect)
REGISTER_BUILDER(TInvisibleEffect)


void TInvisibleEffect::Initialize()
{
	SetCommandDone( FALSE);
	life = 0;
}

void TInvisibleEffect::Pulse()
{
	TEffect::Pulse();
	life++;
	if(life >= INVIS_MAX_LIFE)
		KillThisEffect();
}

void TInvisibleEffect::OffScreen()
{
}
#if 0

// *******************************
// *   Invisible Animator Code   *
// *******************************

_CLASSDEF(TInvisibleAnimator)
class TInvisibleAnimator : public T3DAnimator
{
protected:
	D3DVECTOR position[INVIS_RING_MAX_NUM];
	float scale[INVIS_RING_MAX_NUM];
	int start_time[INVIS_RING_MAX_NUM];
	int my_state;
	PTCharacter invoker;
	PTTornadoSystem my_tornado;
	float rotation;
public:
	void Initialize();
	BOOL Render();
    TInvisibleAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
	void RefreshZBuffer();
	void Animate(BOOL);
};

REGISTER_3DANIMATOR("Invisible", TInvisibleAnimator)

void TInvisibleAnimator::Initialize()
{
	int i;
	PS3DAnimObj obj;
    PS3DAnimObj obj2;

	T3DAnimator::Initialize();

	my_tornado = new TTornadoSystem;


	S3DPoint c;
	S3DPoint t;
	D3DVECTOR v;
	v.x = v.y = 1.3f;
	v.z = 0.0f;
	c.x = c.y = c.z = 0;
	t.x = t.y = 22;
	t.z = 0;
	obj = GetObject(0);
	obj2 = GetObject(1);
	my_tornado->Init(this,obj,obj2,8,5,c,150,25,v,5,55,t);


	obj = GetObject(3);
	GetVerts(obj, D3DVT_LVERTEX);

	rotation = 0;

	for(i=0;i<INVIS_RING_MAX_NUM;i++)
	{
		start_time[i] = (int)(i / 2) * 8;
		position[i].x = position[i].y = 0;
		scale[i] = 0.0f;
		if((i%2) == 0)
			position[i].z = 0.0f;
		else
			position[i].z = 80.0f;
	}

	my_state = INVIS_STATE_HIDE;

	PTSpell spell = ((PTInvisibleEffect)inst)->GetSpell();
	if(spell)
	{
		invoker = (PTCharacter)spell->GetInvoker();
		invoker->SetFade(100,5,30);
	}

}

void TInvisibleAnimator::Animate(BOOL draw)
{
	int i;

	if(invoker)
		invoker->UpdateFade();


	my_tornado->Pulse();

	rotation += 0.2f;

	switch(my_state)
	{
		case INVIS_STATE_HIDE:
			for(i=0;i<INVIS_RING_MAX_NUM;i+=2)
			{
				start_time[i]--;
				if(start_time[i] <= 0)
				{
					if(position[i].z < 20.0f)
					{
						position[i].z += 2.0f;
						scale[i] += 0.05f;
						if(scale[i] > 0.8f)
							scale[i] = 0.8f;
					}
					else if(position[i].z < 40.0f)
					{
						position[i].z += 2.0f;
						scale[i] -= 0.05f;
						if(scale[i] < 0.0f)
							scale[i] = 0.0f;
					}
					else
						position[i].z = 40.0f;
				}
			}
			for(i=1;i<INVIS_RING_MAX_NUM;i+=2)
			{
				start_time[i]--;
				if(start_time[i] <= 0)
				{
					if(position[i].z > 60.0f)
					{
						position[i].z -= 2.0f;
						scale[i] += 0.05f;
						if(scale[i] > 0.8f)
							scale[i] = 0.8f;
					}
					else if(position[i].z > 40.0f)
					{
						position[i].z -= 2.0f;
						scale[i] -= 0.05f;
						if(scale[i] < 0.0f)
							scale[i] = 0.0f;
					}
					else
						position[i].z = 40.0f;
				}
			}
			if(position[INVIS_RING_MAX_NUM - 1].z == 40.0f)
			{
				my_state = INVIS_STATE_WAIT;
				for(i=0;i<INVIS_RING_MAX_NUM;i++)
				{
					start_time[i] = (int)(i / 2) * 8;
				}
			}
			break;
		case INVIS_STATE_WAIT:
			if(((PTInvisibleEffect)inst)->life > (INVIS_MAX_LIFE - 40))
			{
				my_state = INVIS_STATE_SHOW;
			}
			break;
		case INVIS_STATE_SHOW:
			for(i=0;i<INVIS_RING_MAX_NUM;i+=2)
			{
				start_time[i]--;
				if(start_time[i] <= 0)
				{
					if(position[i].z > 20.0f)
					{
						position[i].z -= 2.0f;
						scale[i] += 0.05f;
						if(scale[i] > 0.8f)
							scale[i] = 0.8f;
					}
					else if(position[i].z > 0.0f)
					{
						position[i].z -= 2.0f;
						scale[i] -= 0.05f;
						if(scale[i] < 0.0f)
							scale[i] = 0.0f;
					}
					else
						position[i].z = 0.0f;
				}
			}
			for(i=1;i<INVIS_RING_MAX_NUM;i+=2)
			{
				start_time[i]--;
				if(start_time[i] <= 0)
				{
					if(position[i].z < 60.0f)
					{
						position[i].z += 2.0f;
						scale[i] += 0.05f;
						if(scale[i] > 0.8f)
							scale[i] = 0.8f;
					}
					else if(position[i].z < 80.0f)
					{
						position[i].z += 2.0f;
						scale[i] -= 0.05f;
						if(scale[i] < 0.0f)
							scale[i] = 0.0f;
					}
					else
						position[i].z = 80.0f;
				}
			}
			if(((PTInvisibleEffect)inst)->life <= (INVIS_MAX_LIFE - 20))
			{
				if(invoker)
					invoker->SetFade(30,-5);
			}
			break;
	}

	T3DAnimator::Animate(draw);
}

BOOL TInvisibleAnimator::Render()
{
	int i;
	PS3DAnimObj obj = GetObject(3);
	SaveBlendState();
	SetBlendState();

	if(my_state == INVIS_STATE_HIDE || my_state == INVIS_STATE_SHOW)
	{

		if(invoker)
		{
			S3DPoint invoker_position;
			invoker->GetPos(invoker_position);

			obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;// | OBJ3D_VERTS;

			for(i=0;i<INVIS_RING_MAX_NUM;i++)
			{
				obj->scl.x = obj->scl.y = obj->scl.z = scale[i];
				
				obj->rot.x = obj->rot.y = 0.0f;
				obj->rot.z = rotation;

				obj->pos.x = position[i].x + invoker_position.x;
				obj->pos.y = position[i].y + invoker_position.y;
				obj->pos.z = position[i].z + invoker_position.z;

				RenderObject(obj);
			}
		}
		else
		{
			obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3;// | OBJ3D_VERTS;

			for(i=0;i<INVIS_RING_MAX_NUM;i++)
			{
				obj->scl.x = obj->scl.y = obj->scl.z = scale[i];

				obj->rot.x = obj->rot.y = 0.0f;
				obj->rot.z = rotation;

				obj->pos.x = position[i].x;
				obj->pos.y = position[i].y;
				obj->pos.z = position[i].z;

				RenderObject(obj);
			}
		}

	}

	my_tornado->Render();

	RestoreBlendState();

	return TRUE;
}

void TInvisibleAnimator::RefreshZBuffer()
{
	S3DPoint effect, screen, size;

	size.x = 400;
	size.y = 400;

	((PTEffect)inst)->GetPos(effect);

	WorldToScreen(effect, screen);
	RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

#endif