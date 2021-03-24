// rat
#include "g_local.h"
#include "ai_rat.h"

void rat_end_stand (edict_t *self);
void rat_pounce (edict_t *self);
void rat_talk_think ( edict_t *self );
void rat_bite (edict_t *self);
void rat_respawn (edict_t *ent);

#define RAT_NOSHADOW	128

#include "ai_rat_tables.h"

void rat_talk_think ( edict_t *self )
{
	edict_t	*talk_ent;
	cast_memory_t	*mem;

	if (!(talk_ent = self->cast_info.talk_ent))
		return;

	if (VectorDistance( talk_ent->s.origin, self->s.origin ) > 600)
	{
		self->cast_info.talk_ent = NULL;
		return;
	}

	if (	(self->cast_info.talk_ent == &g_edicts[1])
		&&	(self->cast_info.talk_ent->last_talk_time < (level.time - TALK_OTHER_DELAY*2))
		&&	(self->last_talk_time > (level.time - TALK_SELF_DELAY)))
	{
		return;
	}

//	if (last_client_talk && last_client_talk > (level.time - TALK_OTHER_DELAY))
//		return;	// don't talk too much around the client

	if ((talk_ent->health <= 0) || !visible(self, talk_ent) || !infront(talk_ent, self))
	{
		self->cast_info.talk_ent = NULL;
		return;
	}

	mem = level.global_cast_memory[self->character_index][talk_ent->character_index];
	if (!mem || (mem->flags & MEMORY_NO_TALK))
		return;

	// say something!
	if (	(	(self->last_talk_time < (level.time - TALK_SELF_DELAY))			// we haven't spoken for a while
			 ||	(	(talk_ent->client || talk_ent->last_talk_time)		// if they haven't spoken yet, don't bother
				 &&	(talk_ent->last_talk_time > (level.time - TALK_OTHER_DELAY*1.5))	// or they've just said something, and we've allowed some time for them to finish saying it
				 &&	(talk_ent->cast_info.talk_ent == self)
				 &&	(self->last_talk_time < talk_ent->last_talk_time)
				 &&	(self->last_talk_time < (level.time - TALK_OTHER_DELAY))))
		&&	(talk_ent->last_talk_time < (level.time - TALK_OTHER_DELAY)))
	{
		if (talk_ent->client)
		{
			// should we be aggressive? or friendly?

			if (self->profanity_level)
			{	// attack!
				AI_MakeEnemy( self, talk_ent, 0 );
			}
			else	// ready to attack at any second
			{
				self->profanity_level = 3;
			}

			self->last_talk_time = level.time;

			if (!infront( self, talk_ent ))
			{
				self->cast_info.avoid( self, talk_ent, true );
			}
		}
	}
}

void rat_end_stand( edict_t *self )
{
	if (self->cast_info.move_stand_evade && (self->last_stand_evade > (level.time - 3)))
		return;


    self->cast_info.currentmove = self->cast_info.move_stand;

}

void rat_backoff( edict_t *self, edict_t *other )
{

}

void rat_pain (edict_t *self, edict_t *other, float kick, int damage, int mdx_part, int mdx_subobject)
{
	AI_CheckMakeEnemy( self, other );

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3 + random();

}
void Ratkilledmessage (edict_t *self, edict_t *inflictor, edict_t *attacker)//FREDZ
{
	int			mod;
	char		*message;
	char		*message2;
	qboolean	ff;

	if (deathmatch->value || coop->value || !deathmatch->value)
	{
		ff = meansOfDeath & MOD_FRIENDLY_FIRE;
		mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
		message = NULL;
		message2 = "";

		switch (mod)
		{
		case MOD_SUICIDE:
			message = "suicides";
			break;
		case MOD_FALLING:
			message = "cratered";
			break;
		case MOD_CRUSH:
			message = "was squished";
			break;
		case MOD_WATER:
			message = "sank like a rock";
			break;
		case MOD_SLIME:
			message = "melted";
			break;
		case MOD_LAVA:
			message = "does a back flip into the lava";
			break;
/*		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "blew up";
			break;*/
		case MOD_EXIT:
			message = "found a way out";
			break;
		case MOD_TARGET_LASER:
			message = "saw the light";
			break;
		//FREDZ q2 stuff not used
/*		case MOD_TARGET_BLASTER:
			message = "got blasted";
			break;*/
		case MOD_TARGET_ROCKET:
			message = "got blew up";
			break;
		case MOD_BOMB:
		case MOD_SPLASH:
		case MOD_TRIGGER_HURT:
			message = "was in the wrong place";
			break;
		case MOD_HURT_ELECTRIC://FREDZ
			message = "electricuted himself";
			break;
		case MOD_HURT_FLAMES:
			message = "roasted himself";
			break;
    // RAFAEL
		case MOD_GEKK:
		case MOD_BRAINTENTACLE:
			message = "that's gotta hurt";
			break;
		case MOD_DOGBITE: //FREDZ
			message = "killed by a dog";
			break;
		case MOD_RATBITE: //FREDZ
			message = "killed by a rat";
			break;
		case MOD_FISHBITE: //FREDZ
			message = "killed by a shrimp";
			break;
		}
		if (attacker == self)//FREDZ should never happen
		{
			switch (mod)
			{
			case MOD_TRIGGER_HURT://FREDZ extra check
				message = "was in the wrong place";
				break;
			case MOD_HURT_ELECTRIC://FREDZ
				message = "electricuted himself";
				break;
			case MOD_HURT_FLAMES:
				message = "roasted himself";
				break;
			default:
				message = "killed himself";
				break;
			}
		}
		if (message)
		{
			if (self->name)
				gi.bprintf (PRINT_MEDIUM,"%s(Rat) %s.\n", self->name, message);
			else
				gi.bprintf (PRINT_MEDIUM,"Rat %s.\n", message);
			self->enemy = NULL;
			return;
		}
		self->enemy = attacker;
		if (attacker->client)
		{
			switch (mod)
			{
			case MOD_EXPLOSIVE:
			case MOD_BARREL:
				if(attacker!=self)
					message = "was blown up by";
				break;
			case MOD_BLACKJACK:
				message = "was mashed by";
				break;
			case MOD_CROWBAR:
				message = "was severely dented by";
				break;
			case MOD_PISTOL:
				message = "was busted by";
				message2 = "'s cap";
				break;
			case MOD_SILENCER:
				message = "was silenced by";
				break;
			case MOD_SHOTGUN:
				message = "accepted";
				message2 = "'s load";
				break;
			case MOD_MACHINEGUN:
				message = "bows to";
				message2 = "'s Tommygun";
				break;
			case MOD_FLAMETHROWER:
				message = "roasted in";
				message2 = "'s torch";
				break;
			case MOD_GRENADE:
				message = "fumbled";
				message2 = "'s grenade";
				break;
			case MOD_G_SPLASH:
				message = "was mortally wounded by";
				message2 = "'s shrapnel";
				break;
			case MOD_ROCKET:
				message = "was minced by";
				message2 = "'s rocket";
				break;
			case MOD_R_SPLASH:
				message = "couldn't escape";
				message2 = "'s blast";
				break;
			case MOD_TELEFRAG:
				message = "couldn't co-exist with";
				message2 = "";
				break;
			case MOD_BARMACHINEGUN:
				message = "was maimed by";
				message2 = "'s H.M.G.";
			}
		}
		if (message)
		{
			attacker->client->resp.score++;

			attacker->client->pers.currentcash += giveCashOnKill(BOT_RAT);//FREDZ give cash

			//FREDZ killstreak
/*			attacker->client->resp.killstreak++;
			if (attacker->client->resp.killstreak > attacker->client->resp.maxkillstreak)//fix
				attacker->client->resp.maxkillstreak = attacker->client->resp.killstreak;


			if ((int)teamplay->value == 4)//FREDZ
			{
				team_cash[attacker->client->pers.team]++;
				UPDATESCORE
			}*/

			if (self->name)
				gi.bprintf (PRINT_MEDIUM,"%s(Rat) %s %s%s\n", self->name, message, attacker->client->pers.netname, message2);
			else
				gi.bprintf (PRINT_MEDIUM,"Rat %s %s%s\n", message, attacker->client->pers.netname, message2);
			self->enemy = NULL;
			return;
		}
		//FREDZ cast
		if (attacker->svflags & SVF_MONSTER)
		{
			//Bitch
			if (!strcmp(attacker->classname, "cast_bitch"))
			{
				switch (mod)
				{
				case MOD_BLACKJACK:
					message = "was mashed by";
					break;
				case MOD_PISTOL:
					message = "was busted by";
					message2 = "'s cap";
					break;
				case MOD_SHOTGUN:
					message = "accepted";
					message2 = "'s load";
					break;
				case MOD_MACHINEGUN:
					message = "bows to";
					message2 = "'s Tommygun";
					break;
				case MOD_FLAMETHROWER:
					message = "roasted in";
					message2 = "'s torch";
					break;
				case MOD_G_SPLASH:
					message = "was mortally wounded by";
					message2 = "'s shrapnel";
					break;
				case MOD_ROCKET:
					message = "was minced by";
					message2 = "'s rocket";
					break;
				case MOD_BARMACHINEGUN:
					message = "was maimed by";
					message2 = "'s H.M.G.";
				}
			}
			//Runt
			else if (!strcmp(attacker->classname, "cast_runt"))
			{
				switch (mod)
				{
				case MOD_BLACKJACK:
					message = "was mashed by";
					break;
				case MOD_PISTOL:
					message = "was busted by";
					message2 = "'s cap";
					break;
				case MOD_SHOTGUN:
					message = "accepted";
					message2 = "'s load";
					break;
				case MOD_MACHINEGUN:
					message = "bows to";
					message2 = "'s Tommygun";
					break;
				case MOD_FLAMETHROWER:
					message = "roasted in";
					message2 = "'s torch";
					break;
				case MOD_G_SPLASH:
					message = "was mortally wounded by";
					message2 = "'s shrapnel";
					break;
				case MOD_ROCKET:
					message = "was minced by";
					message2 = "'s rocket";
					break;
				case MOD_BARMACHINEGUN:
					message = "was maimed by";
					message2 = "'s H.M.G.";
				}
			}
			//Shorty
			else if (!strcmp(attacker->classname, "cast_shorty"))
			{
				switch (mod)
				{
				case MOD_BLACKJACK:
					message = "was mashed by";
					break;
				case MOD_PISTOL:
					message = "was busted by";
					message2 = "'s cap";
					break;
				case MOD_SHOTGUN:
					message = "accepted";
					message2 = "'s load";
					break;
				case MOD_MACHINEGUN:
					message = "bows to";
					message2 = "'s Tommygun";
					break;
				case MOD_FLAMETHROWER:
					message = "roasted in";
					message2 = "'s torch";
					break;
				case MOD_G_SPLASH:
					message = "was mortally wounded by";
					message2 = "'s shrapnel";
					break;
				case MOD_ROCKET:
					message = "was minced by";
					message2 = "'s rocket";
					break;
				case MOD_BARMACHINEGUN:
					message = "was maimed by";
					message2 = "'s H.M.G.";
				}
			}
			//Thug
			else if (!strcmp(attacker->classname, "cast_thug"))
			{
				switch (mod)
				{
				case MOD_BLACKJACK:
					message = "was mashed by";
					break;
				case MOD_PISTOL:
					message = "was busted by";
					message2 = "'s cap";
					break;
				case MOD_SHOTGUN:
					message = "accepted";
					message2 = "'s load";
					break;
				case MOD_MACHINEGUN:
					message = "bows to";
					message2 = "'s Tommygun";
					break;
				case MOD_FLAMETHROWER:
					message = "roasted in";
					message2 = "'s torch";
					break;
				case MOD_G_SPLASH:
					message = "was mortally wounded by";
					message2 = "'s shrapnel";
					break;
				case MOD_ROCKET:
					message = "was minced by";
					message2 = "'s rocket";
					break;
				case MOD_BARMACHINEGUN:
					message = "was maimed by";
					message2 = "'s H.M.G.";
				}
			}
			//Punk
			else if (!strcmp(attacker->classname, "cast_punk"))
			{
				switch (mod)
				{
				case MOD_BLACKJACK:
					message = "was mashed by";
					break;
				case MOD_PISTOL:
					message = "was busted by";
					message2 = "'s cap";
					break;
				case MOD_SHOTGUN:
					message = "accepted";
					message2 = "'s load";
					break;
				case MOD_MACHINEGUN:
					message = "bows to";
					message2 = "'s Tommygun";
					break;
				case MOD_FLAMETHROWER:
					message = "roasted in";
					message2 = "'s torch";
					break;
				case MOD_G_SPLASH:
					message = "was mortally wounded by";
					message2 = "'s shrapnel";
					break;
				case MOD_ROCKET:
					message = "was minced by";
					message2 = "'s rocket";
					break;
				case MOD_BARMACHINEGUN:
					message = "was maimed by";
					message2 = "'s H.M.G.";
				}
			}
			//Whore
			else if (!strcmp(attacker->classname, "cast_whore"))
			{
				switch (mod)
				{
				case MOD_BLACKJACK:
					message = "was mashed by";
					break;
				case MOD_PISTOL:
					message = "was busted by";
					message2 = "'s cap";
					break;
				case MOD_SHOTGUN:
					message = "accepted";
					message2 = "'s load";
					break;
				case MOD_MACHINEGUN:
					message = "bows to";
					message2 = "'s Tommygun";
					break;
				case MOD_FLAMETHROWER:
					message = "roasted in";
					message2 = "'s torch";
					break;
				case MOD_G_SPLASH:
					message = "was mortally wounded by";
					message2 = "'s shrapnel";
					break;
				case MOD_ROCKET:
					message = "was minced by";
					message2 = "'s rocket";
					break;
				case MOD_BARMACHINEGUN:
					message = "was maimed by";
					message2 = "'s H.M.G.";
				}
			}
		}
		if (message)
		{
			if (attacker->name && self->name)
				gi.bprintf (PRINT_MEDIUM,"%s(Rat) %s %s(%s)%s\n", self->name, message, attacker->name, attacker->classname, message2);
			else if (attacker->name && !self->name)
				gi.bprintf (PRINT_MEDIUM,"Rat %s %s(%s)%s\n", message, attacker->name, attacker->classname, message2);
			else if (!attacker->name && self->name)
				gi.bprintf (PRINT_MEDIUM,"%s(Rat) %s %s%s\n", self->name, message, attacker->classname, message2);
			else
				gi.bprintf (PRINT_MEDIUM,"Rat %s %s%s\n", message, attacker->classname, message2);
			self->enemy = NULL;
			return;
		}
	}
	if (self->name)
		gi.bprintf (PRINT_MEDIUM,"%s(Rat) died.\n", self->name);
	else
		gi.bprintf (PRINT_MEDIUM,"Rat died.\n");
}//FREDZ end
void Rat_GibEntity( edict_t *self, edict_t *inflictor, float damage )//FREDZ less gibs
{
	vec3_t	dir, offset;

	VectorCopy(self->s.origin, offset);
	offset[2] += 8; //shift gibs out of floor

	// turn off flames
	if (self->onfiretime)
		self->onfiretime = 0;

	if (inflictor->client || VectorCompare( inflictor->velocity, vec3_origin) )
		VectorSubtract( self->s.origin, inflictor->s.origin, dir );
	else
		VectorCopy( inflictor->velocity, dir );

	VectorNormalize( dir );

	//FREDZ copied from rat_die and altered
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_GIBS);
	gi.WritePosition (offset);
	gi.WriteDir (dir);
	gi.WriteByte ( 1 );	// number of gibs
	gi.WriteByte ( 0 );	// scale of direction to add to velocity
	gi.WriteByte ( 0 );	// random offset scale
	gi.WriteByte ( 10 );	// random velocity scale
	gi.multicast (self->s.origin, MULTICAST_PVS);

	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_SPLASH);
	gi.WriteByte (50);
	gi.WritePosition (self->s.origin);
	gi.WriteDir (self->movedir);
	gi.WriteByte (6);
	gi.multicast (self->s.origin, MULTICAST_PVS);

	self->takedamage = DAMAGE_NO;
	self->think = G_FreeEdict;
	self->nextthink = level.time + FRAMETIME;
}
void rat_die_gib (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int mdx_part, int mdx_subobject)
{
	// regular death
	self->takedamage = DAMAGE_YES;

    if (!self->deadflag)//FREDZ
	{
		Ratkilledmessage (self, inflictor, attacker);
	}

	//if (DeathByGib(self, inflictor, attacker, damage))
	if (!cl_parental_lock->value) //hypov8 gib rats
	{	// gib
		self->deadflag = DEAD_DEAD;
		Rat_GibEntity( self, inflictor, damage );
		self->s.model_parts[PART_GUN].invisible_objects = (1<<0 | 1<<1);
		return;
	}
	else
	{
		//hypov8 todo: no death animations setup yet? item wont free
		self->think = G_FreeEdict;
		self->nextthink = level.time + 10;
	}

	if (self->deadflag == DEAD_DEAD)
		return;

	self->deadflag = DEAD_DEAD;

    //FREDZ need gib anyway
}

qboolean rat_attack (edict_t *self)
{
	vec3_t	vec;
	float dist;

	self->cast_info.aiflags |= AI_RUN_LIKE_HELL;	// rat runs really fast

	VectorSubtract( self->enemy->s.origin, self->s.origin, vec );
	dist = VectorNormalize( vec );

	// start panting now when we stop
	self->cast_info.move_stand = &rat_move_run;


	{
		qboolean attack=false;

		if (dist < 48 && VectorLength( self->enemy->velocity ) < 250)
		{
			attack = true;
		}
		else if (dist < 64)
		{
			attack = false;	// so we do a high attack
		}
		else	// are they running towards us?
		{
			VectorSubtract( self->enemy->s.origin, self->s.origin, vec );
			VectorMA( vec, 0.5, self->enemy->velocity, vec );
			dist = VectorNormalize( vec );

			if (dist < 96)
			{
				attack = true;
			}

		}

		if (attack)
		{

			self->ideal_yaw = vectoyaw(vec);

			M_ChangeYaw( self );

            self->cast_info.currentmove = &rat_move_sit_up;

			self->cast_info.aiflags &=~ AI_RUSH_THE_PLAYER;
			return true;
		}
		else if (dist < 180)	// FREDZ helps to move
		{
			self->ideal_yaw = vectoyaw(vec);
			M_ChangeYaw( self );

			self->cast_info.currentmove = &rat_move_sit_up;
			return true;
		}
		else if (dist < 400)
			self->cast_info.aiflags |= AI_RUSH_THE_PLAYER;

	}

	return false;
}


void rat_bite (edict_t *self)
{
	vec3_t	start;//, offset;
	vec3_t	forward, right;

	float damage = 1;//FREDZ to high was 8. Rat is always 1 damage?

	if (self->cast_info.currentmove == &rat_move_sit_up)
		damage *= 2;		// double handed attack


	AngleVectors (self->s.angles, forward, right, NULL);


	damage *= 0.5;

	// fire_dogbite (self, start, forward, damage, 10, MOD_RATBITE);
	if (self->enemy)
	{
//		trace_t		tr;
		vec3_t		aimdir, dang, end;

		VectorSubtract (self->enemy->s.origin, self->s.origin, aimdir);
		vectoangles (aimdir, dang);
		AngleVectors (dang, forward, NULL, NULL);
		VectorMA (self->s.origin, 16, forward, start);
		VectorMA (start, 32, forward, end);

		// ok it seems to line up with the head ok now
		// NAV_DrawLine (start, end);
		fire_blackjack( self, start, forward, damage, 0, MOD_RATBITE );
		/*

		if (VectorLength (aimdir) < 96)
		{
			tr = gi.trace (start, NULL, NULL, end, self, MASK_SHOT );

			if (tr.ent->takedamage)
				T_Damage (tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, 0, DAMAGE_BULLET, MOD_RATBITE);
		}
		// ok lets see why this isnt working
		else if (self->enemy->groundentity == self)
			T_Damage (self->enemy, self, self, aimdir, self->enemy->s.origin, vec3_origin, damage, 0, DAMAGE_BULLET, MOD_RATBITE);
		*/
	}
}

void rat_long_attack (edict_t *self)
{
}

void rat_pounce (edict_t *self)
{
	if (!self->enemy)
		return;

	self->ideal_yaw = entyaw( self, self->enemy );
	M_ChangeYaw( self );

	// leap if on ground
	if (self->groundentity)
	{
		AngleVectors( self->s.angles, self->velocity, NULL, NULL );
		VectorScale( self->velocity, 500, self->velocity );
		self->velocity[2] = 200;
		self->groundentity = NULL;
	}

}


void rat_avoid ( edict_t *self, edict_t *other, qboolean face )
{
	vec3_t	vec;

	if (self->health <= 0)
		return;

	if (!self->groundentity)
		return;

	self->cast_info.last_avoid = level.time;

	if (face)
	{	// turn to face them
		VectorSubtract( other->s.origin, self->s.origin, vec );
		self->cast_info.avoid_ent = other;
	}
	else
	{	// turn to face away from them
		VectorSubtract( self->s.origin, other->s.origin, vec );
		self->cast_info.avoid_ent = NULL;
	}
	VectorNormalize( vec );

	self->ideal_yaw = vectoyaw( vec );

	if (!AI_SideTrace( self, -64, 0, 1))
		self->cast_info.currentmove = &rat_move_reverse_run;
	else
		self->cast_info.currentmove = &rat_move_run;
}

void rat_catch_fire( edict_t *self, edict_t *other )
{
	self->enemy = NULL;		// stop attacking
	self->cast_info.currentmove = &rat_move_run;
}

/*QUAKED cast_rat (1 .5 0) (-8 -8 0) (8 8 10) x TRIGGERED_START x x x
rat
cast_group defines which group the character is a member of
default cast_group is 0, which is neutral (won't help others out)
player's cast_group is 1 (friendly characters)
model="models\actors\rat\"
*/
void SP_cast_rat (edict_t *self)
{
	int i;

//	return;

/*	if (deathmatch->value)
	{
		G_FreeEdict (self);
		return;
	}*/

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

 //   if (!self->cast_group)//FREDZ doesn't work
//	    self->cast_group = 2;

//	VectorSet (self->mins, -8, -8, 0);
//	VectorSet (self->maxs, 8, 8, 10);
	VectorSet (self->mins, -16, -8, -5);
	VectorSet (self->maxs, 8, 8, 5);

	self->s.skinnum = self->skin;
 	memset(&(self->s.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);
	self->s.num_parts++;
	self->s.model_parts[0].modelindex = gi.modelindex("models/actors/rat/rat.mdx");
	for (i=0; i<MAX_MODELPART_OBJECTS; i++)
		self->s.model_parts[0].skinnum[i] = self->s.skinnum;
	gi.GetObjectBounds("models/actors/rat/rat.mdx", &self->s.model_parts[0]);

	if (!self->health)
		self->health = 10;//FREDZ should be 10

	self->gib_health = -10;
	self->mass = 5;//FREDZ 100 on dog

	self->gender = GENDER_NONE;

	self->yaw_speed = 20;

	self->pain = rat_pain;
	self->die = rat_die_gib;

	self->cast_info.checkattack = AI_CheckAttack;
	self->cast_info.attack = rat_attack;
	self->cast_info.long_attack = rat_long_attack;
	self->cast_info.talk = rat_end_stand;
	self->cast_info.avoid = rat_avoid;
	self->cast_info.backoff = rat_backoff;
	self->cast_info.catch_fire = rat_catch_fire;
	self->cast_info.max_attack_distance = 128;

	self->cast_info.move_stand = &rat_move_stand;
	self->cast_info.move_run = &rat_move_run;
	self->cast_info.move_runwalk = &rat_move_run;//FREDZ maybe need to be slower?
//	self->cast_info.move_jump = &rat_move_jump;		// Jumping animation screws up jump attacking, so don't do one

	self->cast_info.move_avoid_walk = &rat_move_run;//FREDZ maybe need to be slower?
	self->cast_info.move_avoid_run = &rat_move_run;
	self->cast_info.move_avoid_reverse_walk = &rat_move_reverse_run;//FREDZ maybe need to be slower?
	self->cast_info.move_avoid_reverse_run = &rat_move_reverse_run;

	self->cast_info.move_evade = &rat_move_reverse_run;//FREDZ needed?

	self->cast_info.currentmove = self->cast_info.move_stand;

	self->cast_info.aiflags |= AI_MELEE;
	self->cast_info.aiflags |= AI_NO_TALK;
	self->cast_info.aiflags |= AI_ADJUSTPITCH;

//	self->gravity = 0.7;

// Temp fix for Dog in SR2 that follows lamont
//if (self->leader_target)
//	self->target = NULL;

	//im not sure what this is ment to do.. but this is doing some weird shit to rat!
	//It's scale rat model see cast_TF_rat
	if (!self->cast_info.scale)
		self->cast_info.scale = MODEL_SCALE;

	self->s.scale = self->cast_info.scale - 1.0;

	// we're a dog, therefore we don't hide
	self->cast_info.aiflags |= AI_NO_TAKE_COVER;

	gi.linkentity (self);

	walking_cast_start (self);

	if (self->spawnflags & RAT_NOSHADOW)//FREDZ
		self->s.renderfx2 |= RF2_NOSHADOW;
}

