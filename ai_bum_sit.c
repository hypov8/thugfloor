#include "g_local.h"
#include "ai_bum_sit.h"
#include "voice_punk.h"

void bum_sit_talk (edict_t *self);
void bum_sit_end_sit (edict_t *self);
void bum_talk_think( edict_t *self );

#include "ai_bum_sit_tables.h"

//void ai_stand_onfire (edict_t *self, float dist);//FREDZ moved table and double this now

void ai_stand_onfire (edict_t *self, float dist)
{

	if (self->onfiretime <= 0)
	{	// stopping running around
		self->cast_info.currentmove = self->cast_info.move_stand;
		return;
	}

	if (!self->groundentity)
		return;

}

void bum_talk_think (edict_t *self)
{
	AI_TalkThink (self, true);
}

void bum_sit_end_sit (edict_t *self)
{
	int		rnd;

	if (self->cast_info.currentmove == &bum_sit_move_cough_l)
		rnd = rand() % 3;
	else
		rnd = rand() % 5;

	if (rnd < 1)
		self->cast_info.currentmove = &bum_sit_move_amb_sit;
	else if (rnd < 2)
		self->cast_info.currentmove = &bum_sit_move_leg_shuf;
	else if (rnd < 3)
		self->cast_info.currentmove = &bum_sit_move_swat_fly;
	else
	{
		if (self->name_index == NAME_RUMMY || self->name_index == NAME_NICK)
			self->cast_info.currentmove = &bum_sit_move_amb_sit;
		else
			self->cast_info.currentmove = &bum_sit_move_cough_l;
	}

}
void BumSitkilledmessage (edict_t *self, edict_t *inflictor, edict_t *attacker)//FREDZ
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
				gi.bprintf (PRINT_MEDIUM,"%s(Bum sit) %s.\n", self->name, message);
			else
				gi.bprintf (PRINT_MEDIUM,"Bum sit %s.\n", message);
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
			//attacker->client->resp.score++; //TF moved to TF_giveCashOnKill.

			TF_giveCashOnKill(BOT_BUM, self);//FREDZ give cash

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
				gi.bprintf (PRINT_MEDIUM,"%s(Bum sit) %s %s%s\n", self->name, message, attacker->client->pers.netname, message2);
			else
				gi.bprintf (PRINT_MEDIUM,"Bum sit %s %s%s\n", message, attacker->client->pers.netname, message2);
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
				gi.bprintf (PRINT_MEDIUM,"%s(Bum sit) %s %s(%s)%s\n", self->name, message, attacker->name, attacker->classname, message2);
			else if (attacker->name && !self->name)
				gi.bprintf (PRINT_MEDIUM,"Bum sit %s %s(%s)%s\n", message, attacker->name, attacker->classname, message2);
			else if (!attacker->name && self->name)
				gi.bprintf (PRINT_MEDIUM,"%s(Bum sit) %s %s%s\n", self->name, message, attacker->classname, message2);
			else
				gi.bprintf (PRINT_MEDIUM,"Bum sit %s %s%s\n", message, attacker->classname, message2);
			self->enemy = NULL;
			return;
		}
	}
	if (self->name)
		gi.bprintf (PRINT_MEDIUM,"%s(Bum sit) died.\n", self->name);
	else
		gi.bprintf (PRINT_MEDIUM,"Bum sit died.\n");
}//FREDZ end
void bum_sit_pain (edict_t *self, edict_t *other, float kick, int damage, int mdx_part, int mdx_subobject)
{

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3 + random();

	gi.sound ( self, CHAN_VOICE, gi.soundindex ("actors/bum/pain.wav"), 1, 3, 0);

	if (rand()%100 > 50)
		self->cast_info.currentmove = &bum_sit_move_pain_r;
	else
		self->cast_info.currentmove = &bum_sit_move_pain_l;

}

void bum_sit_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int mdx_part, int mdx_subobject)
{
//	self->s.modelindex2 = 0;

	self->takedamage = DAMAGE_YES;

    if (!self->deadflag)//FREDZ
	{
		BumSitkilledmessage (self, inflictor, attacker);
	}


	if (DeathByGib(self, inflictor, attacker, damage))
	{	// gib
		self->deadflag = DEAD_DEAD;
		GibEntity( self, inflictor, damage );
		return;
	}

	if (self->deadflag == DEAD_DEAD)
		return;

	self->deadflag = DEAD_DEAD;

	gi.sound ( self, CHAN_VOICE, gi.soundindex ("actors/bum/death.wav"), 1, 3, 0);

	if (rand()%100 > 50)
		self->cast_info.currentmove = &bum_sit_move_death_r;
	else
		self->cast_info.currentmove = &bum_sit_move_death_l;
}

void bum_avoid (edict_t *self, edict_t *other, qboolean face)
{
	self->cast_info.currentmove = &bum_sit_move_swat_fly;
}

void bum_backoff (edict_t *self, edict_t *other )
{
	Voice_Random (self, other, rummy, 0);
}

void bum_catch_on_fire (edict_t *self, edict_t *other)
{
	self->enemy = NULL;		// stop attacking
	self->cast_info.currentmove = &bum_sit_move_swat_fly_onfire;
}


/*QUAKED cast_bum_sit (1 .5 0) (-24 -24 -24) (24 24 48) x TRIGGERED_START x x x
cast_group 0	neutral
They should only be neutrals
model="models\actors\bum_sit\"
*/
void SP_cast_bum_sit (edict_t *self)
{
	int i;
	char	*head_skin, *body_skin, *legs_skin;
	int	skin;


/*	if (deathmatch->value)
	{
		G_FreeEdict (self);
		return;
	}*/

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	VectorSet (self->mins, -24, -24, -24);
	VectorSet (self->maxs, 24, 24, 48);

	self->s.skinnum = (self->skin-1) * 3;

    if (!self->art_skins)
	{	// use default skins
		self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "001 001 001"); // MH: crash fix
	}

	if (self->art_skins)
	{
		// convert spaces to NULL's
		for (i=0; i<11; i++)
			if (self->art_skins[i] == ' ')
				self->art_skins[i] = '\0';

		head_skin = &self->art_skins[0];
		body_skin = &self->art_skins[4];
		legs_skin = &self->art_skins[8];
	}
	else
	{
		head_skin = body_skin = legs_skin = NULL;
	}

	// ------------------------------------------------------------------------
	// initialize all model_part data
	memset(&(self->s.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);

	self->s.num_parts++;
	switch (self->head)//FREDZ new load more type of heads
	{
		case 1:
			self->s.model_parts[PART_HEAD].modelindex = gi.modelindex("models/actors/bum_sit/bald_head.mdx");
			break;
		case 2:
			self->s.model_parts[PART_HEAD].modelindex = gi.modelindex("models/actors/bum_sit/weld_head.mdx");
			break;
		default:
			self->s.model_parts[PART_HEAD].modelindex = gi.modelindex("models/actors/bum_sit/head.mdx");
			break;
	}
	if (head_skin)
	{
		skin = gi.skinindex( self->s.model_parts[PART_HEAD].modelindex, head_skin );
	}
	else
		skin = self->s.skinnum;
	for (i=0; i<MAX_MODELPART_OBJECTS; i++)
		self->s.model_parts[PART_HEAD].baseskin = self->s.model_parts[PART_HEAD].skinnum[i] = skin;
	gi.GetObjectBounds( "models/actors/bum_sit/head.mdx", &self->s.model_parts[PART_HEAD] );

	self->s.num_parts++;
	self->s.model_parts[PART_LEGS].modelindex = gi.modelindex("models/actors/bum_sit/legs.mdx");
	if (head_skin)
		skin = gi.skinindex( self->s.model_parts[PART_LEGS].modelindex, legs_skin );
	else
		skin = self->s.skinnum;
	for (i=0; i<MAX_MODELPART_OBJECTS; i++)
		self->s.model_parts[PART_LEGS].baseskin = self->s.model_parts[PART_LEGS].skinnum[i] = skin;
	gi.GetObjectBounds( "models/actors/bum_sit/legs.mdx", &self->s.model_parts[PART_LEGS] );

	self->s.num_parts++;
	self->s.model_parts[PART_BODY].modelindex = gi.modelindex("models/actors/bum_sit/body.mdx");
	if (head_skin)
		skin = gi.skinindex( self->s.model_parts[PART_BODY].modelindex, body_skin );
	else
		skin = self->s.skinnum;
	for (i=0; i<MAX_MODELPART_OBJECTS; i++)
		self->s.model_parts[PART_BODY].baseskin = self->s.model_parts[PART_BODY].skinnum[i] = skin;
	gi.GetObjectBounds( "models/actors/bum_sit/body.mdx", &self->s.model_parts[PART_BODY] );

	if (self->count & 1) // cigar
	{
		self->s.num_parts++;
		self->s.model_parts[PART_CIGAR].modelindex = gi.modelindex("models/actors/bum_sit/cigar.mdx");
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			self->s.model_parts[PART_CIGAR].baseskin = self->s.model_parts[PART_CIGAR].skinnum[i] = 0;
		gi.GetObjectBounds( "models/actors/runt/cigar.mdx", &self->s.model_parts[PART_CIGAR] );
	}

	if (self->count & 2) // fedora hat
	{
		self->s.num_parts++;
		self->s.model_parts[PART_HAT].modelindex = gi.modelindex("models/actors/bum_sit/fedora.mdx");
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			self->s.model_parts[PART_HAT].baseskin = self->s.model_parts[PART_HAT].skinnum[i] = 0;
		gi.GetObjectBounds( "models/actors/bum_sit/fedora.mdx", &self->s.model_parts[PART_HAT] );
	}
	else if (self->count & 4) // stetson hat
	{
		self->s.num_parts++;
		self->s.model_parts[PART_HAT].modelindex = gi.modelindex("models/actors/bum_sit/stetson.mdx");
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			self->s.model_parts[PART_HAT].baseskin = self->s.model_parts[PART_HAT].skinnum[i] = 0;
		gi.GetObjectBounds( "models/actors/bum_sit/stetson.mdx", &self->s.model_parts[PART_HAT] );
	}
	else if (self->count & 8) // cap (hat)
	{
		self->s.num_parts++;
		self->s.model_parts[PART_HAT].modelindex = gi.modelindex("models/actors/bum_sit/cap.mdx");
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			self->s.model_parts[PART_HAT].baseskin = self->s.model_parts[PART_HAT].skinnum[i] = 0;
		gi.GetObjectBounds( "models/actors/bum_sit/cap.mdx", &self->s.model_parts[PART_HAT] );
	}

	// remove NULL's
	if (self->art_skins)
		self->art_skins[3] = self->art_skins[7] = ' ';

	// ------------------------------------------------------------------------

	if (!(self->health))
	self->health = 100;

	self->gib_health = -200;
	self->mass = 200;

	self->gender = GENDER_MALE;

	self->pain = bum_sit_pain;
	self->die = bum_sit_die;

	self->cast_info.checkattack = NULL;

	self->cast_info.attack = NULL;
	self->cast_info.long_attack = NULL;
	self->cast_info.talk = bum_sit_end_sit;
	self->cast_info.avoid = bum_avoid;
	self->cast_info.backoff = bum_backoff;

	self->cast_info.catch_fire = bum_catch_on_fire;

	self->cast_info.max_attack_distance = 64;

	self->cast_info.move_stand = &bum_sit_move_amb_sit;
	self->cast_info.move_crstand = NULL;

	self->cast_info.move_run = NULL;
	self->cast_info.move_runwalk = NULL;
	self->cast_info.move_crwalk = NULL;

	self->cast_info.move_jump = NULL;

	self->cast_info.move_avoid_walk = NULL;
	self->cast_info.move_avoid_run = NULL;
	self->cast_info.move_avoid_reverse_walk = NULL;
	self->cast_info.move_avoid_reverse_run = NULL;
	self->cast_info.move_avoid_crwalk = NULL;

	self->cast_info.move_crouch_down = NULL;
	self->cast_info.move_stand_up = NULL;
	self->cast_info.move_lside_step = NULL;
	self->cast_info.move_rside_step = NULL;
	self->cast_info.move_start_climb = NULL;
	self->cast_info.move_end_climb = NULL;
	self->cast_info.move_evade = NULL;
	self->cast_info.move_stand_evade = NULL;

	gi.linkentity (self);

	self->cast_info.currentmove = self->cast_info.move_stand;


	if (!self->cast_info.scale)
		self->cast_info.scale = MODEL_SCALE;

	self->s.scale = self->cast_info.scale - 1.0;

	// talk by default
// 	self->cast_info.aiflags |= AI_NO_TALK;
	self->cast_info.aiflags |= (AI_TALK | AI_NOWALK_FACE);

	walking_cast_start (self);

}
