
// g_utils.c -- misc utility functions for game module

#include "g_local.h"


void G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	result[0] = point[0] + forward[0] * distance[0] + right[0] * distance[1];
	result[1] = point[1] + forward[1] * distance[0] + right[1] * distance[1];
	result[2] = point[2] + forward[2] * distance[0] + right[2] * distance[1] + distance[2];
}


/*
=============
G_Find

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
edict_t *G_Find (edict_t *from, int fieldofs, char *match)
{
	char	*s;

	if (!from)
		from = g_edicts;
	else
		from++;

	for ( ; from < &g_edicts[globals.num_edicts] ; from++)
	{
		if (!from->inuse)
			continue;
		s = *(char **) ((byte *)from + fieldofs);
		if (!s)
			continue;
		if (!Q_stricmp (s, match))
			return from;
	}

	return NULL;
}


/*
=============
G_ClearUp
=============
*/
void G_ClearUp (edict_t *from, int fieldofs)
{
	char	*s;

	if (!from)
		from = g_edicts;
	else
		from++;

	for ( ; from < &g_edicts[globals.num_edicts] ; from++)
	{
		if (!from->inuse)
			continue;
		s = *(char **) ((byte *)from + fieldofs);
		if (!s)
			continue;
		if (from->nextthink)
            if (from->nextthink > level.time)
                from->nextthink = level.time;
	}

}


/*
=================
findradius

Returns entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
edict_t *findradius (edict_t *from, vec3_t org, float rad)
{
	vec3_t	eorg;
	int		j;

	if (!from)
		from = g_edicts;
	else
		from++;
	for ( ; from < &g_edicts[globals.num_edicts]; from++)
	{
		if (!from->inuse)
			continue;
		if (from->solid == SOLID_NOT)
			continue;
		for (j=0 ; j<3 ; j++)
			eorg[j] = org[j] - (from->s.origin[j] + (from->mins[j] + from->maxs[j])*0.5);
		if (VectorLength(eorg) > rad)
			continue;
		return from;
	}

	return NULL;
}


/*
=============
G_PickTarget

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
#define MAXCHOICES	8

edict_t *G_PickTarget (char *targetname)
{
	edict_t	*ent = NULL;
	int		num_choices = 0;
	edict_t	*choice[MAXCHOICES];

	if (!targetname)
	{
		gi.dprintf("G_PickTarget called with NULL targetname\n");
		return NULL;
	}

	while(1)
	{
		ent = G_Find (ent, FOFS(targetname), targetname);
		if (!ent)
			break;
		choice[num_choices++] = ent;
		if (num_choices == MAXCHOICES)
			break;
	}

	if (!num_choices)
	{
		gi.dprintf("G_PickTarget: target %s not found\n", targetname);
		return NULL;
	}

	return choice[rand() % num_choices];
}



void Think_Delay (edict_t *ent)
{
	G_UseTargets (ent, ent->activator);
	G_FreeEdict (ent);
}

/*
==============================
G_UseTargets

the global "activator" should be set to the entity that initiated the firing.

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Centerprints any self.message to the activator.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void G_UseTargets (edict_t *ent, edict_t *activator)
{
	edict_t		*t;

//
// check for a delay
//
	if (ent->delay && ent->think != Think_Delay)
	{
	// create a temp object to fire at a later time
		t = G_Spawn();
		t->classname = "DelayedUse";
		t->nextthink = level.time + ent->delay;
		t->think = Think_Delay;
		t->activator = activator;
		if (!activator)
			gi.dprintf ("Think_Delay with no activator\n");
		t->message = ent->message;
		t->target = ent->target;
		t->killtarget = ent->killtarget;
		// RAFAEL
		t->activate_flags = ent->activate_flags;
		return;
	}


//
// print the message
//
	if ((ent->message) && !(activator->svflags & SVF_MONSTER))
	{
		gi.centerprintf (activator, "%s", ent->message);
		if (ent->noise_index)
			gi.sound (activator, CHAN_AUTO, ent->noise_index, 1, ATTN_NORM, 0);
		// JOSEPH 29-MAR-99
		//else
		//	gi.sound (activator, CHAN_AUTO, gi.soundindex ("misc/talk1.wav"), 1, ATTN_NORM, 0);
		// END JOSEPH
	}

//
// kill killtargets
//
	// JOSEPH 4-MAY-99
	if (ent->killtarget)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->killtarget)))
		{
			G_FreeEdict (t);
			if (!ent->inuse)
			{
				gi.dprintf("entity was removed while using killtargets\n");
				return;
			}
		}

		t = NULL;
		while ((t = G_Find (t, FOFS(target2), ent->killtarget)))
		{
			G_FreeEdict (t);
			if (!ent->inuse)
			{
				gi.dprintf("entity was removed (target2 type) while using killtargets\n");
				return;
			}
		}
	}
	// END JOSEPH
//
// fire targets
//
	if (ent->target)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->target)))
		{
			// Rafael: hack to fix Entity used itself message
			// this was showing up when you leave a pawnomatic
			// exit button is probably tagged wrong
			if (t == ent)
				continue;

			// RAFAEL
			if (ent->activate_flags & ACTIVATE_AND_OPEN)
				t->activate_flags |= ACTIVATE_AND_OPEN;
			// JOSEPH 16-MAR-99
			//else if (!Q_stricmp (ent->classname, "trigger_once") && !Q_stricmp (t->classname, "func_door_rotating"))
			//	t->activate_flags |= ACTIVATE_AND_OPEN;
			//else if (!Q_stricmp (ent->classname, "func_door_rotating") && !Q_stricmp (t->classname, "func_door_rotating"))
			//	t->activate_flags |= ACTIVATE_AND_OPEN;
			else if (!Q_stricmp (t->classname, "func_door_rotating"))
				t->activate_flags |= ACTIVATE_AND_OPEN;
			// JOSEPH 23-APR-99
			else if (!Q_stricmp (t->classname, "func_door"))
				t->activate_flags |= ACTIVATE_AND_OPEN;
			// END JOSEPH
			// END JOSEPH
			// doors fire area portals in a specific way
			if (!Q_stricmp(t->classname, "func_areaportal") &&
				(!Q_stricmp(ent->classname, "func_door") || !Q_stricmp(ent->classname, "func_door_rotating")))
				continue;

			if (t == ent)
			{
				gi.dprintf ("WARNING: Entity used itself.\n");
			}
			else
			{
				if (t->use)
					t->use (t, ent, activator);
			}
			if (!ent->inuse)
			{
				// note to self: why is is happening
				// it may be possible that actors are getting removed
				// before they use thier targets

				// gi.dprintf("entity was removed while using targets\n");
				return;
			}
		}
	}
}


/*
=============
TempVector

This is just a convenience function
for making temporary vectors for function calls
=============
*/
float	*tv (float x, float y, float z)
{
	static	int		index;
	static	vec3_t	vecs[8];
	float	*v;

	// use an array so that multiple tempvectors won't collide
	// for a while
	v = vecs[index];
	index = (index + 1)&7;

	v[0] = x;
	v[1] = y;
	v[2] = z;

	return v;
}


/*
=============
VectorToString

This is just a convenience function
for printing vectors
=============
*/
char	*vtos (vec3_t v)
{
	static	int		index;
	static	char	str[8][32];
	char	*s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = (index + 1)&7;

	Com_sprintf (s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2]);

	return s;
}


vec3_t VEC_UP		= {0, -1, 0};
vec3_t MOVEDIR_UP	= {0, 0, 1};
vec3_t VEC_DOWN		= {0, -2, 0};
vec3_t MOVEDIR_DOWN	= {0, 0, -1};

void G_SetMovedir (vec3_t angles, vec3_t movedir)
{
	if (VectorCompare (angles, VEC_UP))
	{
		VectorCopy (MOVEDIR_UP, movedir);
	}
	else if (VectorCompare (angles, VEC_DOWN))
	{
		VectorCopy (MOVEDIR_DOWN, movedir);
	}
	else
	{
		AngleVectors (angles, movedir, NULL, NULL);
	}

	VectorClear (angles);
}


float vectoyaw (vec3_t vec)
{
	float	yaw;

	if (vec[YAW] == 0 && vec[PITCH] == 0)
		yaw = 0;
	else
	{
		yaw = (int) (atan2(vec[YAW], vec[PITCH]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;
	}

	return yaw;
}

float	entyaw( edict_t *self, edict_t *other )
{
	vec3_t vec;

	VectorSubtract( other->s.origin, self->s.origin, vec );
	VectorNormalize( vec );

	return vectoyaw( vec );
}

// Ridah, deleted vectoangles, moved to q_shared.c

char *G_CopyString (char *in)
{
	char	*out;

	out = gi.TagMalloc (strlen(in)+1, TAG_LEVEL);
	strcpy (out, in);
	return out;
}


void G_InitEdict (edict_t *e)
{
	e->inuse = true;
	e->classname = "noclass";
	e->gravity = 1.0;
	e->s.number = e - g_edicts;

// BEGIN:	Xatrix/Ridah/Navigator/19-mar-1998
	e->nav_data.cache_node = -1;
	e->active_node_data = level.node_data;
// END:		Xatrix/Ridah/Navigator/19-mar-1998

}

/*
=================
G_Spawn

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
edict_t *G_Spawn (void)
{
	int			i;
	edict_t		*e;

	e = &g_edicts[(int)maxclients->value+1];
	for ( i=maxclients->value+1 ; i<globals.num_edicts ; i++, e++)
	{
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!e->inuse && ( e->freetime < 2 || level.time - e->freetime > 0.5 ) )
		{
			G_InitEdict (e);
			return e;
		}
	}

	if (i == game.maxentities)
		gi.error ("ED_Alloc: no free edicts");

	globals.num_edicts++;
	G_InitEdict (e);
	return e;
}

/*
=================
G_FreeEdict

Marks the edict as free
=================
*/
void G_FreeEdict (edict_t *ed)
{
#if 0 //def BETADEBUG
	char freeName[256]; //hypov8 debug: catched freed items that bots try to use
	strcpy(freeName, "free_");
	strcat(freeName, ed->classname);
#endif

	// Ridah, fixes Rockets crashing SR1
	if (ed->character_index)
	{
		level.characters[ed->character_index] = NULL;
		level.num_characters--; //hypov8 todo: fix this?
		ed->character_index = 0;
		level.waveEnemyCount_cur--;
		level.waveEnemyCount_total--;
	}


	gi.unlinkentity (ed);		// unlink from world

	if ((ed - g_edicts) <= (maxclients->value + BODY_QUEUE_SIZE))
	{
//		gi.dprintf("tried to free special edict\n");
		return;
	}


	memset (ed, 0, sizeof(*ed));
	ed->classname = "freed";
	ed->freetime = level.time;
	ed->inuse = false;
#if 0 //def BETADEBUG
	ed->classname = strcpy(gi.TagMalloc(sizeof(freeName)+1, TAG_LEVEL), freeName);
#endif
}


/*
============
G_TouchTriggers

============
*/
void	G_TouchTriggers (edict_t *ent)
{
	int			i, num;
	edict_t		*touch[MAX_EDICTS], *hit;

	// dead things don't activate triggers!
	if ((ent->client || (ent->svflags & SVF_MONSTER)) && (ent->health <= 0))
		return;

	// JOSEPH 25-APR-99
	// props don't activate triggers!
    if (ent->svflags & SVF_PROP)
		return;
	// END JOSEPH

	num = gi.BoxEdicts (ent->absmin, ent->absmax, touch , MAX_EDICTS, AREA_TRIGGERS);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (i=0 ; i<num ; i++)
	{
		hit = touch[i];
		if (!hit->inuse)
			continue;
		if (!hit->touch)
			continue;
		hit->touch (hit, ent, NULL, NULL);
	}
}

/*
============
G_TouchSolids

Call after linking a new trigger in during gameplay
to force all entities it covers to immediately touch it
============
*/
void	G_TouchSolids (edict_t *ent)
{
	int			i, num;
	edict_t		*touch[MAX_EDICTS], *hit;

	num = gi.BoxEdicts (ent->absmin, ent->absmax, touch
		, MAX_EDICTS, AREA_SOLID);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (i=0 ; i<num ; i++)
	{
		hit = touch[i];
		if (!hit->inuse)
			continue;
		if (ent->touch)
			ent->touch (hit, ent, NULL, NULL);
		if (!ent->inuse)
			break;
	}
}




/*
==============================================================================

Kill box

==============================================================================
*/

/*
=================
KillBox

Kills all entities that would touch the proposed new positioning
of ent.  Ent should be unlinked before calling this!
=================
*/
qboolean KillBox (edict_t *ent)
{
	trace_t		tr;
	int			mask = MASK_PLAYERSOLID; // MH: what to look for

	while (1)
	{
		tr = gi.trace (ent->s.origin, ent->mins, ent->maxs, ent->s.origin, NULL, mask);
		if (!tr.ent || !tr.startsolid) // MH: done if no hit
			break;

		// nail it
		T_Damage (tr.ent, ent, ent, vec3_origin, ent->s.origin, vec3_origin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG); // MH: back to MOD_TELEFRAG

		// if we didn't kill it, fail
		if (tr.ent->solid)
		{
			// MH: recheck if necessary to make sure a monster/player there is killed
			if (!(tr.contents & CONTENTS_MONSTER) && mask != CONTENTS_MONSTER)
			{
				mask = CONTENTS_MONSTER;
				continue;
			}
			return false;
		}
	}

	return true;		// all clear
}


/*
SurfaceSpriteEffect

  Determines whether or not a surface sprite effect can be placed in this location, and
  if so, spawns the requested sprite.

  RETURNS true if a sprite was spawned
*/
qboolean	SurfaceSpriteEffect(	byte surf_sfx, byte width, byte height,
									edict_t *impact_ent, vec3_t pos, vec3_t normal )
{
	#define	EXTRUDE_DIST	0.5		// come this far out from wall for trace test

	vec3_t	angles;
	vec3_t	right, up;
	vec3_t	this_pos, x_pos, center_pos, end_pos;
	float	x, y;
	float	fwidth, fheight;
	trace_t	tr;

	if (impact_ent != world)
		return false;

	fwidth = (float) width;
	fheight = (float) height;

	vectoangles( normal, angles );
	AngleVectors( angles, NULL, right, up );

	VectorMA( pos, EXTRUDE_DIST, normal, center_pos );

	if (!(surf_sfx == SFX_SPRITE_SURF_RIPPLE) && (width > 2 && height > 2))
	{
		// check all corners of sprite on wall
		for (x= -fwidth/2; x<=fwidth/2; x+= fwidth)
		{
			VectorMA( center_pos, x, right, x_pos );

			for (y= -fheight/2; y<= fheight/2; y+= fheight)
			{
				VectorMA( x_pos, y, up, this_pos );

				VectorMA( this_pos, -EXTRUDE_DIST*2, normal, end_pos );

				// do a trace
				tr = gi.trace( this_pos, NULL, NULL, end_pos, NULL, CONTENTS_SOLID );

				if ( ! (!tr.startsolid && tr.fraction < 1 && tr.ent == impact_ent))
				{	// no good
					return false;
				}
			}
		}
	}

	// succesful, so spawn the sprite
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_SURFACE_SPRITE_ENTITY);
	gi.WriteByte (surf_sfx);
	gi.WriteByte (width);
	gi.WriteByte (height);
	gi.WritePosition (center_pos);
	gi.WriteAngle (angles[0]);
	gi.WriteAngle (angles[1]);	// don't need ROLL

	if (deathmatch->value)
		gi.multicast (center_pos, MULTICAST_PVS);
	else
		gi.multicast (center_pos, MULTICAST_ALL);

	return true;
}


qboolean	SurfaceSpriteEffectRipple(	byte surf_sfx, byte width, byte height,
									edict_t *impact_ent, vec3_t pos, vec3_t normal )
{
	#define	EXTRUDE_DIST	0.5		// come this far out from wall for trace test

	vec3_t	angles;
	vec3_t	right, up;
	vec3_t	this_pos, x_pos, center_pos, end_pos;
	float	x, y;
	float	fwidth, fheight;
	trace_t	tr;

	//if (impact_ent != world)
	//	return false;

	fwidth = (float) width;
	fheight = (float) height;

	vectoangles( normal, angles );
	AngleVectors( angles, NULL, right, up );

	VectorMA( pos, EXTRUDE_DIST, normal, center_pos );

	if (!(surf_sfx == SFX_SPRITE_SURF_RIPPLE) && (width > 2 && height > 2))
	{
		// check all corners of sprite on wall
		for (x= -fwidth/2; x<fwidth; x+= fwidth)
		{
			VectorMA( center_pos, x, right, x_pos );

			for (y= -fheight/2; y< fheight; y+= fheight)
			{
				VectorMA( x_pos, y, up, this_pos );

				VectorMA( this_pos, -EXTRUDE_DIST*2, normal, end_pos );

				// do a trace
				tr = gi.trace( this_pos, NULL, NULL, end_pos, NULL, CONTENTS_SOLID );

				if ( ! (!tr.startsolid && tr.fraction < 1 && tr.ent == impact_ent))
				{	// no good
					return false;
				}
			}
		}
	}

	// succesful, so spawn the sprite
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_SURFACE_SPRITE_ENTITY);
	gi.WriteByte (surf_sfx);
	gi.WriteByte (width);
	gi.WriteByte (height);
	gi.WritePosition (center_pos);
	gi.WriteAngle (angles[0]);
	gi.WriteAngle (angles[1]);	// don't need ROLL

	if (deathmatch->value)
		gi.multicast (center_pos, MULTICAST_PVS);
	else
		gi.multicast (center_pos, MULTICAST_ALL);

	return true;
}

