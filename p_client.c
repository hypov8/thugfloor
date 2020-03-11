
#include "g_local.h"
#include "m_player.h"

#include "voice_bitch.h"
#include "voice_punk.h"

#include <stddef.h> // MH: needed for offsetof

#define NAME_CLASH_STR    "<Name Clash>"
#define NAME_BLANK_STR    "<No Name>"

void ClientUserinfoChanged (edict_t *ent, char *userinfo);
static int CheckClientRejoin(edict_t *ent);

// MH: cloud spawning functions
void think_new_first_raincloud_client (edict_t *self, edict_t *clent);
void think_new_first_snowcloud_client (edict_t *self, edict_t *clent);

#if 1 //mm 2.0
static void playerskin(int playernum, char *s)
{
	// only update player's skin config if it has changed (saves a bit of bandwidth)
	if (strcmp(level.playerskins[playernum], s))
	{
		strncpy(level.playerskins[playernum], s, sizeof(level.playerskins[playernum]) - 1);
		gi.configstring(CS_PLAYERSKINS + playernum, s);
	}
}
#endif

extern void DrawPreviewLaserBBox(edict_t *ent, int laser_color, int laser_size);
void SP_info_player_show(edict_t *self)//FREDZ
{
	int i;
//	char	*head_skin, *body_skin, *legs_skin;
//	int	skin;

	self->solid = SOLID_NOT;
	self->movetype = MOVETYPE_NONE;
	VectorSet (self->mins, -16, -16, -24);
	VectorSet (self->maxs, 16, 16, 48);
	self->s.skinnum = (self->skin-1) * 3;
//	self->art_skins = "009 019 017";
	memset(&(self->s.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);

	self->s.num_parts++;
	self->s.model_parts[PART_HEAD].modelindex = gi.modelindex("models/actors/thug/head.mdx");
//	skin = gi.skinindex( self->s.model_parts[PART_HEAD].modelindex, "009" );//FREDZ fix fox linux
	for (i=0; i<MAX_MODELPART_OBJECTS; i++)
		self->s.model_parts[PART_HEAD].baseskin = self->s.model_parts[PART_HEAD].skinnum[i] = gi.skinindex( self->s.model_parts[PART_HEAD].modelindex, "009" );
	gi.GetObjectBounds( "models/actors/thug/head.mdx", &self->s.model_parts[PART_HEAD] );

	self->s.num_parts++;
	self->s.model_parts[PART_LEGS].modelindex = gi.modelindex("models/actors/thug/legs.mdx");
//	skin = gi.skinindex( self->s.model_parts[PART_HEAD].modelindex, "017" );//FREDZ fix fox linux
	for (i=0; i<MAX_MODELPART_OBJECTS; i++)
		self->s.model_parts[PART_LEGS].baseskin = self->s.model_parts[PART_LEGS].skinnum[i] = gi.skinindex( self->s.model_parts[PART_HEAD].modelindex, "017" );
	gi.GetObjectBounds( "models/actors/thug/legs.mdx", &self->s.model_parts[PART_LEGS] );

	self->s.num_parts++;
	self->s.model_parts[PART_BODY].modelindex = gi.modelindex("models/actors/thug/body.mdx");
//	skin = gi.skinindex( self->s.model_parts[PART_HEAD].modelindex, "019" );//FREDZ fix fox linux
	for (i=0; i<MAX_MODELPART_OBJECTS; i++)
		self->s.model_parts[PART_BODY].baseskin = self->s.model_parts[PART_BODY].skinnum[i] = gi.skinindex( self->s.model_parts[PART_HEAD].modelindex, "019" );
	gi.GetObjectBounds( "models/actors/thug/body.mdx", &self->s.model_parts[PART_BODY] );

	gi.linkentity (self);
	self->cast_info.scale = MODEL_SCALE;
	self->s.scale = self->cast_info.scale - 1.0;
	self->s.renderfx2 |= RF2_NOSHADOW;
}

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 48)
The normal starting point for a level.
*/
void SP_info_player_start(edict_t *self)
{
	extern void Show_Help (void);

	if (!(deathmatch->value))
	{
		if (	!strcmp(level.mapname, "sr1")
			||	!strcmp(level.mapname, "pv_h")
			||	!strcmp(level.mapname, "sy_h")
			||	!strcmp(level.mapname, "steel1")
			||	!strcmp(level.mapname, "ty1")
			||	!strcmp(level.mapname, "rc1") )
		Show_Help ();
	}

    #ifdef BETADEBUG
    self->s.renderfx = RF_SHELL_RED;
    SP_info_player_show(self);
    #endif
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 48)
potential spawning position for deathmatch games

  style - team # for Teamplay (1 or 2)
*/
void SP_info_player_deathmatch(edict_t *self)
{
	if (!deathmatch->value)
	{
		G_FreeEdict (self);
		return;
	}
    #ifdef BETADEBUG
    self->s.renderfx = RF_SHELL_BLUE;
    SP_info_player_show(self);
    #endif
}

/*QUAKED info_player_coop (1 0 1) (-16 -16 -24) (16 16 48)
potential spawning position for coop games
*/
void SP_info_player_coop(edict_t *self)
{

    #ifdef BETADEBUG
    self->s.renderfx = RF_SHELL_GREEN;
    SP_info_player_show(self);
/*//Not working :/
    int		i;
    for (i = 0; i < 3; i++)
	{
		self->mins[i] -= 1;
		self->maxs[i] += 1;
	}

    DrawPreviewLaserBBox (self, 0xf2f2f0f0, 2);
    gi.linkentity(self);*/
    #endif


/*
	if (!coop->value)
	{
		G_FreeEdict (self);
		return;
	}

*/
}


/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The deathmatch intermission point will be at one of these
Use 'angles' instead of 'angle', so you can set pitch or roll as well as yaw.  'pitch yaw roll'
*/
void SP_info_player_intermission(void)
{
}

void SP_info_box_intermission(void)
{
}


//=======================================================================


void player_pain (edict_t *self, edict_t *other, float kick, int damage, int mdx_part, int mdx_subobject)
{
	// player pain is handled at the end of the frame in P_DamageFeedback
}


qboolean IsFemale (edict_t *ent)
{
//	char		*info;

	if (!ent->client)
		return false;

	if (ent->gender == GENDER_FEMALE)
		return true;
/*
	info = Info_ValueForKey (ent->client->pers.userinfo, "gender");
	if (info[0] == 'f' || info[0] == 'F')
		return true;
*/
	return false;
}

qboolean IsNeutral (edict_t *ent)
{
//	char		*info;

	if (!ent->client)
		return false;

	if (ent->gender == GENDER_NONE)
		return true;
/*
	info = Info_ValueForKey (ent->client->pers.userinfo, "gender");
	if (info[0] != 'f' && info[0] != 'F' && info[0] != 'm' && info[0] != 'M')
		return true;
*/
	return false;
}

void ClientObituary (edict_t *self, edict_t *inflictor, edict_t *attacker)
{
	int			mod;
	char		*message;
	char		*message2;
	qboolean	ff;

	if (coop->value && attacker->client)
		meansOfDeath |= MOD_FRIENDLY_FIRE;

	if (deathmatch->value || coop->value)
	{
		ff = meansOfDeath & MOD_FRIENDLY_FIRE;
		mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
		message = NULL;
		message2 = "";

		// in realmode, track deaths
/*		if (dm_realmode->value && !teamplay->value)
			self->client->resp.deposited++;*/
		if (mod!=MOD_RESTART && mod!=MOD_TELEFRAG)// && teamplay->value!=1) // MH: fixed/simplified
			self->client->resp.deposited++;

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
 /*           if(self->client->pers.fakeThief>0)
            {
                self->client->resp.stole-=self->client->pers.fakeThief; // MH: changed from acchit
                if(self->client->pers.team==1)
                    team_cash[2]+=self->client->pers.fakeThief;
                else if(self->client->pers.team==2)
                    team_cash[1]+=self->client->pers.fakeThief;
            }*/
			break;
		/*case MOD_EXPLOSIVE:
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
/*			if(self->client->pers.fakeThief>0)
            {
                self->client->resp.acchit-=self->client->pers.fakeThief;
                if(self->client->pers.team==1)
					team_cash[2]+=self->client->pers.fakeThief;
                else if(self->client->pers.team==2)
					team_cash[1]+=self->client->pers.fakeThief;
            }*/
			break;
		case MOD_HURT_ELECTRIC://FREDZ
			if (IsNeutral(self))
				message = "electricuted itself";
			else if (IsFemale(self))
				message = "electricuted herself";
			else
				message = "electricuted himself";
/*			if(self->client->pers.fakeThief>0)
            {
                self->client->resp.acchit-=self->client->pers.fakeThief;
                if(self->client->pers.team==1)
					team_cash[2]+=self->client->pers.fakeThief;
                else if(self->client->pers.team==2)
					team_cash[1]+=self->client->pers.fakeThief;
            }*/
			break;
		case MOD_HURT_FLAMES://FREDZ fix bagman maps now
			if (IsNeutral(self))
				message = "roasted itself";
			else if (IsFemale(self))
				message = "roasted herself";
			else
				message = "roasted himself";
/*			if(self->client->pers.fakeThief>0)
            {
                self->client->resp.acchit-=self->client->pers.fakeThief;
                if(self->client->pers.team==1)
					team_cash[2]+=self->client->pers.fakeThief;
                else if(self->client->pers.team==2)
					team_cash[1]+=self->client->pers.fakeThief;
            }*/
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
		if (attacker == self)
		{
          switch (mod)
            {
                case MOD_HELD_GRENADE:
                    message = "tried to put the pin back in";
                    break;
                case MOD_HG_SPLASH:
                case MOD_G_SPLASH:
                    if (IsNeutral(self))
                        message = "tripped on its own grenade";
                    else if (IsFemale(self))
                        message = "tripped on her own grenade";
                    else
                        message = "tripped on his own grenade";
                    break;
                case MOD_R_SPLASH:
                case MOD_EXPLOSIVE:
                case MOD_BARREL:
                    if (IsNeutral(self))
                        message = "blew itself up";
                    else if (IsFemale(self))
                        message = "blew herself up";
                    else
                        message = "blew himself up";
                    break;
                //FREDZ q2 stuff not used
    /*			case MOD_BFG_BLAST:
                    message = "should have used a smaller gun";
                    break;*/
                // RAFAEL 03-MAY-98
                case MOD_TRAP:
                    message = "sucked into his own trap";
                    break;
                case MOD_FLAMETHROWER:
                    if (IsNeutral(self))
                        message = "roasted itself";
                    else if (IsFemale(self))
                        message = "roasted herself";
                    else
                        message = "roasted himself";
                    break;
                case MOD_TRIGGER_HURT://FREDZ extra check
                    message = "was in the wrong place";
                    if(self->client->pers.fakeThief>0)
    /*				{
                        self->client->resp.acchit-=self->client->pers.fakeThief;
                        if(self->client->pers.team==1)
                            team_cash[2]+=self->client->pers.fakeThief;
                        else if(self->client->pers.team==2)
                            team_cash[1]+=self->client->pers.fakeThief;
                    }*/
                    break;
                case MOD_HURT_ELECTRIC://FREDZ
                    if (IsNeutral(self))
                        message = "electricuted itself";
                    else if (IsFemale(self))
                        message = "electricuted herself";
                    else
                        message = "electricuted himself";
    /*				if(self->client->pers.fakeThief>0)
                    {
                        self->client->resp.acchit-=self->client->pers.fakeThief;
                        if(self->client->pers.team==1)
                            team_cash[2]+=self->client->pers.fakeThief;
                        else if(self->client->pers.team==2)
                            team_cash[1]+=self->client->pers.fakeThief;
                    }*/
                    break;
                case MOD_HURT_FLAMES://FREDZ
                    if (IsNeutral(self))
                        message = "roasted itself";
                    else if (IsFemale(self))
                        message = "roasted herself";
                    else
                        message = "roasted himself";
    /*				if(self->client->pers.fakeThief>0)
                    {
                        self->client->resp.acchit-=self->client->pers.fakeThief;
                        if(self->client->pers.team==1)
                            team_cash[2]+=self->client->pers.fakeThief;
                        else if(self->client->pers.team==2)
                            team_cash[1]+=self->client->pers.fakeThief;
                    }*/
                    break;
                default:
                    if (IsNeutral(self))
                        message = "killed itself";
                    else if (IsFemale(self))
                        message = "killed herself";
                    else
                        message = "killed himself";
                    break;
			}
		}
		if (message)
		{
			gi.bprintf (PRINT_MEDIUM, "%s %s.\n", self->client->pers.netname, message);
			if ((deathmatch->value) && (mod != MOD_RESTART))
			{
//				self->client->resp.score--; //hypov8 dont realy need this...

                if(enable_killerhealth)
                {
                    if ((self->client->pers.netname) && ((mod == MOD_DOGBITE) || (mod == MOD_RATBITE) || (mod == MOD_FISHBITE)))//It player?
                    {
                        if (attacker->name)//FREDZ
                            gi.cprintf (self, PRINT_HIGH,"%s(%s) had %i health!\n", attacker->name, attacker->classname, attacker->health);
                        else
                            gi.cprintf (self, PRINT_HIGH,"%s had %i health!\n", attacker->classname, attacker->health);
                    }

                }

				//FREDZ killstreak
				// Because we killed ourselves we want to end our kill streak so
				// we have to check if we had a better streak than before.
/*				if (self->client->resp.killstreak > self->client->resp.maxkillstreak)
					self->client->resp.maxkillstreak = self->client->resp.killstreak;
				// Reset the kill streak.
				self->client->resp.killstreak = 0;

				if ((int)teamplay->value == 4)//FREDZ 4 was TM_GANGBANG
				{
					team_cash[self->client->pers.team]--;
					UPDATESCORE
				}*/
			}
			self->enemy = NULL;
			return;
		}

		self->enemy = attacker;
		if (attacker && attacker->client)
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
			// JOSEPH 16-APR-99
			case MOD_BARMACHINEGUN:
				message = "was maimed by";
				message2 = "'s H.M.G.";
			// END JOSEPH
			}
		}
			if (message)
			{
				gi.bprintf (PRINT_MEDIUM,"%s %s %s%s\n", self->client->pers.netname, message, attacker->client->pers.netname, message2);
                if(enable_killerhealth)
                {
                   gi.cprintf (self, PRINT_HIGH,"%s had %i health!\n", attacker->client->pers.netname, attacker->health);
                }
				if ((deathmatch->value) && (mod != MOD_RESTART) && (mod != MOD_TELEFRAG))
				{
					if (ff)
					{
//						attacker->client->resp.score--; //hypov8 dont realy need this...

						// Oh dear we were just killed so that ends our kill streak.
/*						if (self->client->resp.killstreak > self->client->resp.maxkillstreak)//FREDZ dunno this correct
							self->client->resp.maxkillstreak = self->client->resp.killstreak;
						self->client->resp.killstreak = 0;
						//FREDZ end killstreak

						if ((int)teamplay->value == 4)//FREDZ 4 was TM_GANGBANG
						{
							team_cash[attacker->client->pers.team]--;
							UPDATESCORE
						}*/
					}
					else
					{
						attacker->client->resp.score++;

						//FREDZ killstreak
/*						attacker->client->resp.killstreak++;
						if (attacker->client->resp.killstreak > attacker->client->resp.maxkillstreak)//fix
							attacker->client->resp.maxkillstreak = attacker->client->resp.killstreak;


						// Oh dear we were just killed so that ends our kill streak.
						if (self->client->resp.killstreak > self->client->resp.maxkillstreak)
							self->client->resp.maxkillstreak = self->client->resp.killstreak;
						self->client->resp.killstreak = 0;
						//FREDZ end killstreak


						if ((int)teamplay->value == 4)//FREDZ 4 was TM_GANGBANG
						{
							team_cash[attacker->client->pers.team]++;
							UPDATESCORE
						}*/
					}
				}
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
			//FREDZ not working :-(
			//Dog
/*			else if (strcmp(attacker->classname, "cast_dog"))
			{
				switch (mod)
				{
				case MOD_DOGBITE: //FREDZ
					message = "killed by a";
					break;
				}
			}*/
		}
		if (message)
		{
			if (attacker->name)
				gi.bprintf (PRINT_MEDIUM,"%s %s %s(%s)%s\n", self->client->pers.netname, message, attacker->name, attacker->classname, message2);
			else
				gi.bprintf (PRINT_MEDIUM,"%s %s %s%s\n", self->client->pers.netname, message, attacker->classname, message2);

            if(enable_killerhealth)
            {
                if (self->client->pers.netname)//It player?
                {
                    if (attacker->name)//FREDZ
                        gi.cprintf (self, PRINT_HIGH,"%s(%s) had %i health!\n", attacker->name, attacker->classname, attacker->health);
                    else
                        gi.cprintf (self, PRINT_HIGH,"%s had %i health!\n", attacker->classname, attacker->health);
                }

            }

			//FREDZ killstreak
			// Because we killed ourselves we want to end our kill streak so
			// we have to check if we had a better streak than before.
/*			if (self->client->resp.killstreak > self->client->resp.maxkillstreak)
				self->client->resp.maxkillstreak = self->client->resp.killstreak;
			// Reset the kill streak.
			self->client->resp.killstreak = 0;

			if ((int)teamplay->value == 4)//FREDZ
			{
				team_cash[self->client->pers.team]--;
				UPDATESCORE
			}*/

//			self->client->resp.score--; //hypov8 dont realy need this...
			self->enemy = NULL;
			return;
		}//FREDZ end
	}

	gi.bprintf (PRINT_MEDIUM,"%s died.\n", self->client->pers.netname);
	if ((deathmatch->value) && (mod != MOD_RESTART))
	{
//		self->client->resp.score--; //hypov8 dont realy need this...

/*		if ((int)teamplay->value == 4)//FREDZ 4 was TM_GANGBANG
		{
			team_cash[self->client->pers.team]--;
			UPDATESCORE
		}*/
	}
}


void Touch_Item (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);

void TossClientWeapon (edict_t *self)
{
	gitem_t		*item;
	edict_t		*drop;
//	qboolean	quad;
	// RAFAEL
//	qboolean	quadfire;
	float		spread;

	if (!deathmatch->value)
		return;

	item = self->client->pers.weapon;
	if (! self->client->pers.inventory[self->client->ammo_index] )
		item = NULL;
//	if (item && (strcmp (item->pickup_name, "Blaster") == 0))//Q2
//		item = NULL;

//	if (!((int)(dmflags->value) & DF_QUAD_DROP))
//		quad = false;
//	else
//		quad = (self->client->quad_framenum > (level.framenum + 10));

	// RAFAEL
//	if (!((int)(dmflags->value) & DF_QUADFIRE_DROP))
//		quadfire = false;
//	else
//		quadfire = (self->client->quadfire_framenum > (level.framenum + 10));

/*
	if (item && quad)
		spread = 22.5;
	else if (item && quadfire)
		spread = 12.5;
	else*/
		spread = 0.0;

	if (item)
	{
		self->client->v_angle[YAW] -= spread;
		drop = Drop_Item (self, item);
		self->client->v_angle[YAW] += spread;
		drop->spawnflags = DROPPED_PLAYER_ITEM;
	}
/*
	if (quad)//Q2
	{
		self->client->v_angle[YAW] += spread;
		drop = Drop_Item (self, FindItemByClassname ("item_quad"));
		self->client->v_angle[YAW] -= spread;
		drop->spawnflags |= DROPPED_PLAYER_ITEM;

		drop->touch = Touch_Item;
		drop->nextthink = level.time + (self->client->quad_framenum - level.framenum) * FRAMETIME;
		drop->think = G_FreeEdict;
	}

	// RAFAEL
	if (quadfire)//Q2 Xatrix
	{
		self->client->v_angle[YAW] += spread;
		drop = Drop_Item (self, FindItemByClassname ("item_quadfire"));
		self->client->v_angle[YAW] -= spread;
		drop->spawnflags |= DROPPED_PLAYER_ITEM;

		drop->touch = Touch_Item;
		drop->nextthink = level.time + (self->client->quadfire_framenum - level.framenum) * FRAMETIME;
		drop->think = G_FreeEdict;
	}*/
}


/*
==================
LookAtKiller
==================
*/
void LookAtKiller (edict_t *self, edict_t *inflictor, edict_t *attacker)
{
	vec3_t		dir;

	if (attacker && attacker != world && attacker != self)
	{
		VectorSubtract (attacker->s.origin, self->s.origin, dir);
	}
	else if (inflictor && inflictor != world && inflictor != self)
	{
		VectorSubtract (inflictor->s.origin, self->s.origin, dir);
	}
	else
	{
		self->client->killer_yaw = self->s.angles[YAW];
		return;
	}

	self->client->killer_yaw = 180/M_PI*atan2(dir[1], dir[0]);
}

/*
==================
player_die
==================
*/
extern void VelocityForDamage (int damage, vec3_t v);

void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int mdx_part, int mdx_subobject)
{
	int		n;//, health;

	// MH: make sure the body shows up in the client's current position
	G_UnTimeShiftClient( self );

	//self->client->pers.spectator == PLAYER_READY
	VectorClear (self->avelocity);

	self->takedamage = DAMAGE_YES;
	self->movetype = MOVETYPE_TOSS;

//	self->s.modelindex2 = 0;	// remove linked weapon model
    //tical
//    self->s.modelindex3 = 0;

	self->s.model_parts[PART_GUN].modelindex = 0;

	self->s.renderfx2 &= ~RF2_FLAMETHROWER;
	self->s.renderfx2 &= ~RF2_MONEYBAG;

	self->s.angles[0] = 0;
	self->s.angles[2] = 0;

	self->s.sound = 0;
	self->client->weapon_sound = 0;

	self->maxs[2] = -8;

//	self->solid = SOLID_NOT;
	self->svflags |= SVF_DEADMONSTER;

	if (!self->deadflag && (self->health + damage > 0))
	{
		self->client->respawn_time = level.time + 1.0;
		LookAtKiller (self, inflictor, attacker);

		self->client->ps.pmove.pm_type = PM_DEAD;
		ClientObituary (self, inflictor, attacker);

/*		if (enable_killerhealth)
		{
			if(attacker->client && attacker!=self)
			{
				health = attacker->health;
				if(((unsigned int)attacker->health>100)||((unsigned int)attacker->health<0)) health=0;
				cprintf(self,PRINT_CHAT,"%s had %u health!\n", attacker->client->pers.netname, health);
			}
		}*/
//		sl_WriteStdLogDeath( &gi, level, self, inflictor, attacker);	// Standard Logging

//		if (meansOfDeath!=MOD_RESTART)//People keep weapon anyway
//			TossClientWeapon (self);
		if (deathmatch->value && meansOfDeath!=MOD_RESTART &&
			(!self->client->showscores || self->client->showscores == SCORE_TF_HUD)) // MH: only if not already showing scores
			Cmd_Help_f (self, 0);		// show scores

		// clear inventory
		// this is kind of ugly, but it's how we want to handle keys in coop
		for (n = 0; n < game.num_items; n++)
		{
			if (coop->value && itemlist[n].flags & IT_KEY)
				self->client->resp.coop_respawn.inventory[n] = self->client->pers.inventory[n];
			self->client->pers.inventory[n] = 0;
		}

		// yell at us?
		if (rand()%6 == 0 && attacker->last_talk_time < (level.time - TALK_FIGHTING_DELAY))
		{
			if (attacker->gender == GENDER_MALE)
				Voice_Random(attacker, self, fightsounds, NUM_FIGHTING);
			else if (attacker->gender == GENDER_FEMALE)
				Voice_Random(attacker, self, f_fightsounds, F_NUM_FIGHTING);
		}

		//TF: store current wep
		self->client->pers.weaponStore = self->client->pers.weapon;

		// drop cash if we have some
		// MH: copied from MM2
/*		if ((int)teamplay->value == 1)
		{
			// always drop at least 10 bucks to reward the killer (if on other team)
			if ((attacker->client) && (attacker->client->pers.team != self->client->pers.team))
			{
				self->client->pers.currentcash += 10;
				if (self->client->pers.currentcash > MAX_CASH_PLAYER)
					self->client->pers.currentcash = MAX_CASH_PLAYER;

				if (self->client->pers.bagcash < MAX_BAGCASH_PLAYER)
				{
					// if they were killed in the enemy base, reward them with some extra cash
					edict_t	*safes[2] = { NULL, NULL };
					edict_t *cash = NULL;
					while (cash = G_Find( cash, FOFS(classname), "dm_safebag" ))
						safes[cash->style == self->client->pers.team] = cash;
					if (safes[0] && safes[1])
					{
						float dist = VectorDistance(safes[0]->s.origin, self->s.origin);
						if (dist < VectorDistance(safes[1]->s.origin, self->s.origin) / 2)
						{
							if (dist < 512 || gi.inPHS( safes[0]->s.origin, self->s.origin ))
							{
								self->client->pers.bagcash += 30;
								if (self->client->pers.bagcash > MAX_BAGCASH_PLAYER)
									self->client->pers.bagcash = MAX_BAGCASH_PLAYER;
							}
						}
					}
				}
			}

			DropCash(self);
		}*/
	}

	// remove powerups
	self->client->quad_framenum = 0;
	self->client->invincible_framenum = 0;
	self->client->breather_framenum = 0;
	self->client->enviro_framenum = 0;
	self->flags &= ~FL_POWER_ARMOR;

	// RAFAEL
	self->client->quadfire_framenum = 0;

// Ridah
	self->moveout_ent = NULL;
// done.

	self->s.renderfx2 = 0;

	if (damage >= 10)// && self->health < -30 && !inflictor->client)//FREDZ was (damage >= 50 && self->health < -30 && !inflictor->client)/
	{	// gib
		GibEntity( self, inflictor, damage );
		self->s.renderfx2 |= RF2_ONLY_PARENTAL_LOCKED;
	}

	{	// normal death
		if (!self->deadflag)
		{
			static int i;

			i = (i+1)%4;
			// start a death animation
			self->client->anim_priority = ANIM_DEATH;
			if (self->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				self->s.frame = FRAME_crouch_death_01-1;
				self->client->anim_end = FRAME_crouch_death_12;
			}
			else switch (i)
			{
			case 0:
				self->s.frame = FRAME_death1_01-1;
				self->client->anim_end = FRAME_death1_19;
				break;
			case 1:
				self->s.frame = FRAME_death2_01-1;
				self->client->anim_end = FRAME_death2_16;
				break;
			case 2:
				self->s.frame = FRAME_death3_01-1;
				self->client->anim_end = FRAME_death3_28;
				break;
			default:
				self->s.frame = FRAME_death4_01-1;
				self->client->anim_end = FRAME_death4_13;
				break;
			}
			if (meansOfDeath!=MOD_RESTART)
				gi.sound (self, CHAN_VOICE, gi.soundindex(va("*death%i.wav", (rand()%4)+1)), 1, ATTN_NORM, 0);
		}
	}

	self->deadflag = DEAD_DEAD;

	gi.linkentity (self);
}

//=======================================================================

/*
==============
InitClientPersistant

This is only called when the game first initializes in single player,
but is called after each death and level change in deathmatch
==============
*/
extern void AutoLoadWeapon( gclient_t *client, gitem_t *weapon, gitem_t *ammo );

void InitClientPersistant (gclient_t *client)
{
	gitem_t		*item, *ammo;

	// MH: not clearing stuff that should persist at respawn (instead of copying it back and forth)
	memset(&client->pers, 0, offsetof(client_persistant_t, version));

	// JOSEPH 5-FEB-99-B
	item = FindItem("Pipe");
	// END JOSEPH
	client->pers.selected_item = ITEM_INDEX(item);
	client->pers.inventory[client->pers.selected_item] = 1;

	// Ridah, start with Pistol in deathmatch
	if (deathmatch->value)
	{
			item = FindItem("pistol");
			client->pers.selected_item = ITEM_INDEX(item);
			client->pers.inventory[client->pers.selected_item] = 1;
			client->ammo_index = ITEM_INDEX(FindItem(item->ammo));
			client->pers.inventory[client->ammo_index] = 50;
			client->pers.weapon = item;
			ammo = FindItem (item->ammo);

			if ((level.waveNum >= 2) && (timelimit->value)) //todo. players in wave dont get this!!!!
			{
				item = FindItem("Shotgun");
				client->pers.selected_item = ITEM_INDEX(item);
				client->pers.inventory[client->pers.selected_item] = 1;
				client->ammo_index = ITEM_INDEX(FindItem(item->ammo));
				client->pers.inventory[client->ammo_index] = 50;
				client->pers.weapon = item;
				ammo = FindItem(item->ammo);
			}

			if ((level.waveNum >= 4) && (timelimit->value)) //todo. players in wave dont get this!!!!
			{
				item = FindItem("Tommygun");
				client->pers.selected_item = ITEM_INDEX(item);
				client->pers.inventory[client->pers.selected_item] = 1;
				client->ammo_index = ITEM_INDEX(FindItem(item->ammo));
				client->pers.inventory[client->ammo_index] = 50;
				client->pers.weapon = item;
				ammo = FindItem(item->ammo);
			}


			//give weapon back to client. minimal ammo and no mods etc..
			//should this be the first selected wep?
			if (client->pers.weaponStore)
			{
				item = client->pers.weaponStore;
				client->pers.selected_item = ITEM_INDEX(item);
				client->pers.inventory[client->pers.selected_item] = 1;

                if (item->ammo)
				{
					client->ammo_index = ITEM_INDEX(FindItem(item->ammo));
					client->pers.inventory[client->ammo_index] = ammo->quantity;
					ammo = FindItem (item->ammo);
				}
				client->pers.weapon = item;

			}
			AutoLoadWeapon( client, item, ammo );
	}
	else	// start holstered in single player
	{
		client->pers.holsteredweapon = item;
		client->pers.weapon = NULL;
	}


	client->pers.health			= 100;
	client->pers.max_health		= 100;

	client->pers.max_bullets	= 200;
	client->pers.max_shells		= 100;
	client->pers.max_rockets	= 25;
	client->pers.max_grenades	= 12;
	client->pers.max_gas		= 200;
	client->pers.max_308cal		= 90;

	// RAFAEL
//	client->pers.max_magslug	= 50;//Q2 Xatrix MOD
//	client->pers.max_trap		= 5;

	client->pers.connected = true;
}
/*//FREDZ not using
void InitClientResp (gclient_t *client)
{
	// MH: this seems unnecessary
	int cash;
	int hits;
	int fav[8];
	int acch;
	int accs;
	int time;
	int i;

	hits = client->resp.score;
	cash = client->resp.deposited;
	time = client->resp.enterframe;
	for(i=0;i<8;i++)
	{
		fav[i] = client->resp.fav[i];
	}
	accs = client->resp.accshot;
	acch = client->resp.acchit;

	memset (&client->resp, 0, sizeof(client->resp));

	client->resp.score = hits;
	client->resp.deposited = cash;
	for(i=0;i<8;i++)
	{
		client->resp.fav[i] = fav[i];
	}
	client->resp.accshot = accs;
	client->resp.acchit = acch;
	client->resp.enterframe = time;

	client->resp.coop_respawn = client->pers;

    client->resp.checkdelta=level.framenum+17;
	client->resp.checkpvs=level.framenum+23;
	client->resp.checktime=level.framenum+11;
	client->resp.checktex=level.framenum+30;
    client->resp.checkfoot=level.framenum+25;
    client->resp.checkmouse=level.framenum+35;
}*/
void InitClientRespClear (gclient_t *client)
{
	memset (&client->resp, 0, sizeof(client->resp));

	client->resp.enterframe = level.framenum;
	client->resp.coop_respawn = client->pers;

 	client->resp.checkdelta=level.framenum+17;
	client->resp.checkpvs=level.framenum+23;
    client->resp.checkfoot=level.framenum+25;
	client->resp.checktime=level.framenum+11;
	client->resp.checktex=level.framenum+30;
    client->resp.checkmouse=level.framenum+35;
}


/*
==================
SaveClientData

Some information that should be persistant, like health,
is still stored in the edict structure, so it needs to
be mirrored out to the client structure before all the
edicts are wiped.
==================
*/
void SaveClientData (void)
{
	int		i;
	edict_t	*ent;

	for (i=0 ; i<game.maxclients ; i++)
	{
		ent = &g_edicts[1+i];
		if (!ent->inuse)
			continue;
		game.clients[i].pers.health = ent->health;
		game.clients[i].pers.max_health = ent->max_health;
		game.clients[i].pers.savedFlags = (ent->flags & (FL_GODMODE|FL_NOTARGET|FL_POWER_ARMOR));
		if (coop->value)
			game.clients[i].pers.score = ent->client->resp.score;
	}
}

void FetchClientEntData (edict_t *ent)
{
	ent->health = ent->client->pers.health;
	ent->max_health = ent->client->pers.max_health;
	ent->flags |= ent->client->pers.savedFlags;
	if (coop->value)
		ent->client->resp.score = ent->client->pers.score;
}



/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
PlayersRangeFromSpot

Returns the distance to the nearest player from the given spot
================
*/
float	PlayersRangeFromSpot (edict_t *spot)
{
	edict_t	*player;
	float	bestplayerdistance;
	vec3_t	v;
	int		n;
	float	playerdistance;


	bestplayerdistance = 9999;

	for (n = 1; n <= maxclients->value; n++)
	{
		player = &g_edicts[n];

		if (!player->inuse)
			continue;

		if (player->health <= 0 || player->solid == SOLID_NOT) // MH: exclude spectators
			continue;

		VectorSubtract (spot->s.origin, player->s.origin, v);
		playerdistance = VectorLength (v);

		if (playerdistance <= 32) // MH: at least partially occupied
			return 0;

		if (playerdistance < bestplayerdistance)
			bestplayerdistance = playerdistance;
	}

	return bestplayerdistance;
}

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point, but NOT the two points closest
to other players
================
*/
// MH: copied from MM2
edict_t *SelectRandomDeathmatchSpawnPoint (edict_t *ent)
{
	edict_t	*spot, *spot1, *spot2;
	int		count = 0, count0 = 0;
	int		selection;
	float	range, range1, range2;

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		count++;
		if (ent->client->pers.spectator != SPECTATING)
		{
			range = PlayersRangeFromSpot(spot);
			if (range < range1)
			{
				range2 = range1;
				spot2 = spot1;
				range1 = range;
				spot1 = spot;
			}
			else if (range < range2)
			{
				range2 = range;
				spot2 = spot;
			}
			// count occupied spots
			if (!range)
				count0++;
		}
	}

	if (!count)
		return NULL;

	if (count <= 2)
		spot1 = spot2 = NULL;
	if (count0 == count) // all spots occupied
		count0 = 0;
	else if (count0 > 2)
		count -= count0 - 2;
	count -= (spot1 != NULL) + (spot2 != NULL);

	selection = rand() % count;

	spot = NULL;
	do
	{
		spot = G_Find (spot, FOFS(classname), "info_player_deathmatch");
		if (spot == spot1 || spot == spot2 || (count0 > 2 && !PlayersRangeFromSpot(spot)))
			selection++;
	}
	while (selection--);

	return spot;
}

/*
================
SelectFarthestDeathmatchSpawnPoint

================
*/
// MH: copied from MM2
edict_t *SelectFarthestDeathmatchSpawnPoint (edict_t *ent, qboolean team_spawnbase)
{
	edict_t	*bestspot;
	float	bestdistance, bestplayerdistance;
	edict_t	*spot;

	// prevent respawning where they died
	if (ent->health <= 0 && ent->solid)
		ent->health = 1;

spotagain:

	spot = NULL;
	bestspot = NULL;
	bestdistance = -1;
	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		// Teamplay, don't go here if it's not in our base
/*		if (teamplay->value && ent->client->pers.team
			&&	spot->style && spot->style != ent->client->pers.team)	// Never spawn in the enemy base
		{
			continue;
		}*/

		if (team_spawnbase && spot->style != ent->client->pers.team)
		{
			continue;
		}
		// teamplay, done.

		bestplayerdistance = PlayersRangeFromSpot (spot);

		if ((0.8 * bestplayerdistance) > bestdistance
			|| (bestplayerdistance >= (0.8 * bestdistance) && !(rand() & 3)))
		{
			bestspot = spot;
			bestdistance = bestplayerdistance;
		}
	}

	if (!bestspot && team_spawnbase)
	{
		team_spawnbase = false;
		goto spotagain;
	}

	return bestspot;
}

// TiCaL - for bagman
edict_t *SelectBagmanSpawnPoint (edict_t *ent)
{
    edict_t	*bestspot;
    float	bestdistance=0.0;
    float	nearestplayerdistance;
    edict_t	*spot;

    spot = NULL;
    bestspot = NULL;
    bestdistance = 0.0;

    if(ent->client->pers.team == 1)
    {
        while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
        {
            if(spot->style != 1)
            {
             //   spot = NULL;
                continue;
            }
            nearestplayerdistance = PlayersRangeFromSpot (spot);
            if (nearestplayerdistance > bestdistance)
            {
                bestspot = spot;
                bestdistance = nearestplayerdistance;
            }
        }
    }
    else if(ent->client->pers.team == 2)
    {
        while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
        {
            if(spot->style != 2)
            {
              //  spot = NULL;
                continue;
            }
            nearestplayerdistance = PlayersRangeFromSpot (spot);
            if (nearestplayerdistance > bestdistance)
            {
                bestspot = spot;
                bestdistance = nearestplayerdistance;
            }
        }
    }

    return bestspot;
}

edict_t *SelectDeathmatchSpawnPoint (edict_t *ent)
{
	// Ridah, in teamplay, spawn at base
//	if (teamplay->value == 1 && ent->client->pers.team!=0)
//		return SelectFarthestDeathmatchSpawnPoint (ent, true); // MH: re-enabled but always spawn at own base if possible
//	else
    if ( (int)(dmflags->value) & DF_SPAWN_FARTHEST)
		return SelectFarthestDeathmatchSpawnPoint (ent, false);
	else
		return SelectRandomDeathmatchSpawnPoint(ent);
}


edict_t *SelectPlayerSpawnPoint (edict_t *ent)
{
	int		index;
	edict_t	*spot = NULL;
	char	*target;

	index = ent->client - game.clients;

	// player 0 starts in normal player spawn point
	if (!index)
		return NULL;

	spot = NULL;

	// assume there are four coop spots at each spawnpoint
	while (1)
	{
		spot = G_Find (spot, FOFS(classname), "info_player_coop");
		if (!spot)
			return NULL;	// we didn't have enough...

		target = spot->targetname;
		if (!target)
			target = "";
		if ( Q_stricmp(game.spawnpoint, target) == 0 )
		{	// this is a coop spawn point for one of the clients here
			index--;
			if (!index)
				return spot;		// this is it
		}
	}


	return spot;
}


/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, coop start, etc
============
*/
void SelectSpawnPoint (edict_t *ent, vec3_t origin, vec3_t angles)
{
	edict_t	*spot = NULL;

//	if (deathmatch->value)
//		spot = SelectDeathmatchSpawnPoint (ent);
//	else if (coop->value)
		spot = SelectPlayerSpawnPoint (ent);//FREDZ thugfloor only info_player_start

	// find a single player start spot
	if (!spot)
	{
		while ((spot = G_Find (spot, FOFS(classname), "info_player_start")) != NULL)
		{
			if (!game.spawnpoint[0] && !spot->targetname)
				break;

			if (!game.spawnpoint[0] || !spot->targetname)
				continue;

			if (Q_stricmp(game.spawnpoint, spot->targetname) == 0)
				break;
		}

		if (!spot)
		{
			if (!game.spawnpoint[0])
			{	// there wasn't a spawnpoint without a target, so use any
				spot = G_Find (spot, FOFS(classname), "info_player_start");
			}
			if (!spot)
            {
				gi.error ("Couldn't find spawn point %s\n", game.spawnpoint);
            }
		}
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);
}

//======================================================================


//thugfloor
float cast_PlayersRangeFromSpot(edict_t *spot)
{
	edict_t	*player;
	float	bestplayerdistance;
	vec3_t	v;
	int		n;
	float	playerdistance;


	bestplayerdistance = 9999;

	//check all ents
	for (n = 1; n <= globals.num_edicts; n++)
	{
		player = &g_edicts[n];

		if (!player->inuse)
			continue;

		if (player->health <= 0 || player->solid == SOLID_NOT) // MH: exclude spectators
			continue;

		VectorSubtract(spot->s.origin, player->s.origin, v);
		playerdistance = VectorLength(v);

		if (playerdistance <= 32) // MH: at least partially occupied
			return 0;

		if (playerdistance < bestplayerdistance)
			bestplayerdistance = playerdistance;
	}

	return bestplayerdistance;
}

//duplicate of dm spawn. but also tests for ai presence
edict_t *cast_SelectRandomDeathmatchSpawnPoint(edict_t *ent)
{
	edict_t	*spot, *spot1, *spot2;
	int		count = 0, count0 = 0;
	int		selection;
	float	range, range1, range2;

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find(spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		count++;
		//if (ent->client->pers.spectator != SPECTATING)
		{
			range = cast_PlayersRangeFromSpot(spot);
			if (range < range1)
			{
				range2 = range1;
				spot2 = spot1;
				range1 = range;
				spot1 = spot;
			}
			else if (range < range2)
			{
				range2 = range;
				spot2 = spot;
			}
			// count occupied spots
			if (!range)
				count0++;
		}
	}
	//hypov8 todo: should we spawn around the boss?
	if (!count)
		return NULL;

	if (count <= 2)
		spot1 = spot2 = NULL;
	if (count0 == count) // all spots occupied
		count0 = 0;
	else if (count0 > 2)
		count -= count0 - 2;
	count -= (spot1 != NULL) + (spot2 != NULL);

	selection = rand() % count;

	spot = NULL;
	do
	{
		spot = G_Find(spot, FOFS(classname), "info_player_deathmatch");
		if (spot == spot1 || spot == spot2 || (count0 > 2 && !cast_PlayersRangeFromSpot(spot)))
			selection++;
	} while (selection--);

	return spot;

}
void InitBodyQue (void)
{
	int		i;
	edict_t	*ent;

	level.body_que = 0;
	for (i=0; i<BODY_QUEUE_SIZE ; i++)
	{
		ent = G_Spawn();
		ent->classname = "bodyque";
        ent->svflags = SVF_NOCLIENT; // MH: added
	}
}

void body_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int mdx_part, int mdx_subobject)
{
//	int	n;

	if (damage > 50)
	{
		// send the client-side gib message
		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_GIBS);
		gi.WritePosition (self->s.origin);
		gi.WriteDir (vec3_origin);
		gi.WriteByte ( 20 );	// number of gibs
		gi.WriteByte ( 0 );	// scale of direction to add to velocity
		gi.WriteByte ( 16 );	// random offset scale
		gi.WriteByte ( 200 );	// random velocity scale
		gi.multicast (self->s.origin, MULTICAST_PVS);

		// MH: done with this body
		self->solid = SOLID_NOT;
		self->svflags |= SVF_NOCLIENT;
		self->nextthink = 0;
	}
}

void HideBody( edict_t *ent )
{
//	ent->svflags |= SVF_NOCLIENT;
	ent->s.effects &= ~EF_FLAMETHROWER;
}

void Body_Animate( edict_t *ent )
{
	ent->s.frame++;

	if (ent->s.frame >= ent->cal)
	{
		ent->s.frame = ent->cal;
	}

	// sink into ground
	if ((ent->timestamp < (level.time - 5)) && (level.framenum & 1)) // MH: use framenum
	{
		ent->s.origin[2] -= 0.5;
		ent->s.renderfx2 |= RF2_NOSHADOW; // MH: remove shadow when sinking

		if (ent->s.origin[2] + 24 < ent->take_cover_time)
		{
			// done with this body
			ent->solid = SOLID_NOT; // MH: added
			ent->svflags |= SVF_NOCLIENT;
			return;
		}
	}

	ent->nextthink = level.time + 0.1;
}

void CopyToBodyQue (edict_t *ent)
{
	edict_t		*body;

	// grab a body que and cycle to the next one
	body = &g_edicts[(int)maxclients->value + level.body_que + 1];
	level.body_que = (level.body_que + 1) % BODY_QUEUE_SIZE;

	// FIXME: send an effect on the removed body

	gi.unlinkentity (ent);

	gi.unlinkentity (body);
	body->s = ent->s;
	body->s.number = body - g_edicts;

    // MH: prevent lerping from old body position
	if (!(body->svflags & SVF_NOCLIENT))
		body->s.event = EV_OTHER_TELEPORT;

	body->cal = ent->client->anim_end;

	body->svflags = ent->svflags;
//	VectorCopy (ent->mins, body->mins);
//	VectorCopy (ent->maxs, body->maxs);

	VectorSet (body->mins, -64, -64, -24);
	VectorSet (body->maxs,  64,  64, -4);

	VectorCopy (ent->absmin, body->absmin);
	VectorCopy (ent->absmax, body->absmax);
	VectorCopy (ent->size, body->size);
	body->solid = ent->solid;
	body->clipmask = ent->clipmask;
	body->owner = ent->owner;
	body->movetype = ent->movetype;

	body->svflags &= ~SVF_NOCLIENT;

	// Ridah so we can shoot the body
	body->svflags |= (SVF_MONSTER | SVF_DEADMONSTER);

	body->cast_info.scale = 1.0;

	body->s.renderfx = 0;
	body->s.renderfx2 = (ent->s.renderfx2 & RF2_ONLY_PARENTAL_LOCKED);
	body->s.renderfx2 |= (ent->s.renderfx2 & (RF2_NOSHADOW | RF2_DIR_LIGHTS)); // MH: keep shadow and directional lighting state
	body->s.effects = 0;
	body->s.angles[PITCH] = 0;

	body->gender = ent->gender;
	body->deadflag = ent->deadflag;

	body->die = body_die;
	body->takedamage = DAMAGE_YES;

	body->take_cover_time = body->s.origin[2];
	body->timestamp = level.time;

//	body->think = HideBody;
//	body->nextthink = level.time + 30;
	body->think = Body_Animate;
	body->nextthink = level.time + 0.1;

	gi.linkentity (body);

	// if the body que is full, make sure the oldest is sinking
	body = &g_edicts[(int)maxclients->value + level.body_que + 1];
	if (body->timestamp > (level.time - 5))
		body->timestamp = (level.time - 5);
}


void respawn (edict_t *self)
{
	if (deathmatch->value || coop->value)
	{
		// make sure on the last death frame
//		self->s.frame = self->client->anim_end;

		if (!(self->svflags & SVF_NOCLIENT)) // MH: only if visible
			CopyToBodyQue (self);
		PutClientInServer (self);

		// add a teleportation effect
		self->s.event = EV_PLAYER_TELEPORT;

		// hold in place briefly
		self->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
		self->client->ps.pmove.pm_time = 14;

		self->client->respawn_time = level.time;

		return;
	}

	// restart the entire server
	gi.AddCommandString ("menu_loadgame\n");
}

//==============================================================


/*
===========
PutClientInServer

Called when a player connects to a server or respawns in
a deathmatch.
============
*/
void PutClientInServer (edict_t *ent)
{
	vec3_t	mins = {-16, -16, -24};
	vec3_t	maxs = {16, 16, 48};
	int		index;
	vec3_t	spawn_origin, spawn_angles;
	gclient_t	*client;
	int		i;
	client_persistant_t	saved;
	client_respawn_t	resp;
	edict_t *self;
	int      count_players = 0;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	SelectSpawnPoint (ent, spawn_origin, spawn_angles);

	index = ent-g_edicts-1;
	client = ent->client;

	// deathmatch wipes most client data every spawn
	if (deathmatch->value)
	{
		char		userinfo[MAX_INFO_STRING];
		resp = client->resp;
		memcpy (userinfo, client->pers.userinfo, sizeof(userinfo));
		InitClientPersistant (client);
		ent->move_frame=0;
        ent->name_change_frame = -80;  //just to be sure
		ClientUserinfoChanged (ent, userinfo);
	}
/*
	else if (coop->value)
	{
//		int			n;
		char		userinfo[MAX_INFO_STRING];

		resp = client->resp;
		memcpy (userinfo, client->pers.userinfo, sizeof(userinfo));
		// this is kind of ugly, but it's how we want to handle keys in coop
//		for (n = 0; n < game.num_items; n++)
//		{
//			if (itemlist[n].flags & IT_KEY)
//				resp.coop_respawn.inventory[n] = client->pers.inventory[n];
//		}
		resp.coop_respawn.game_helpchanged = client->pers.game_helpchanged;
		resp.coop_respawn.helpchanged = client->pers.helpchanged;
		client->pers = resp.coop_respawn;
		ClientUserinfoChanged (ent, userinfo);
		if (resp.score > client->pers.score)
			client->pers.score = resp.score;
	}
*/
	else
	{
//		for (i=0; i<level.num_characters; i++)
//			if (level.characters[i] == ent)
//				break;

		if (ent != level.characters[0])
		{
			AddCharacterToGame(ent);
		}

		memset (&resp, 0, sizeof(resp));
	}

	ent->name_index = -1;

	// clear everything but the persistant data
	saved = client->pers;
	memset (client, 0, sizeof(*client));
	client->pers = saved;
	if (client->pers.health <= 0)
		InitClientPersistant(client);
	client->resp = resp;

	// copy some data from the client to the entity
	FetchClientEntData (ent);

	//count current active players. limit to 8
	for_each_player(self, i)
	{
		if (self != ent && self->client->pers.spectator != SPECTATING)
			count_players++; //todo: test
	}

	// clear entity values
	ent->groundentity = NULL;
	ent->client = &game.clients[index];
	ent->takedamage = DAMAGE_AIM;
//	if (((deathmatch->value) && (level.modeset == MATCHSETUP) || (level.modeset == FINALCOUNT)))
//		|| (level.modeset == FREEFORALL) || (ent->client->pers.spectator == SPECTATING))
	if ((level.modeset == WAVE_START) ||
		(level.modeset == PREGAME) || (ent->client->pers.spectator == SPECTATING) || level.intermissiontime
		|| (level.modeset == WAVE_ACTIVE && ent->client->pers.spectator == PLAYING) //(ent->movetype == MOVETYPE_TOSS) //was dead
		|| (level.modeset == WAVE_ACTIVE && (!timelimit->value || ent->client->resp.enterframe == level.framenum))
		|| (count_players >= 8)
        )
	{
		if (ent->client->pers.spectator == PLAYING) //player died mid wave
		{
			ent->client->pers.player_dead = TRUE;//FREDZ
			ent->client->pers.spectator = PLAYER_READY;
			ent->client->pers.player_died = true;//reset player died mid wave. deny cash
		}
//		ent->movetype = MOVETYPE_NOCLIP;
        ent->movetype = MOVETYPE_SPECTATOR;//FREDZ example
		ent->solid = SOLID_NOT;
		ent->svflags |= SVF_NOCLIENT;
		ent->client->pers.weapon = NULL;
		if (count_players >= 8) //message
			ent->client->pers.spectator = SPECTATING;
	}
	else
	{
		ent->client->pers.spectator = PLAYING;
		ent->movetype = MOVETYPE_WALK;
		ent->solid = SOLID_BBOX;
		ent->client->pers.player_dead = FALSE;//FREDZ
		//ent->client->pers.weaponStore = NULL;
		AddCharacterToGame(ent); //hypov8 add player to level.characters

//		ent->svflags = SVF_DEADMONSTER;//FREDZ thugfloor should make transparant player from kitmod
//        ent->svflags |= SVF_DEADMONSTER;
		ent->svflags &= ~(SVF_DEADMONSTER|SVF_NOCLIENT);

		//give 3 seconds of imortality on each spawn (anti-camp)
	    if(anti_spawncamp->value)
		        client->invincible_framenum = level.framenum + 30;  //3 seconds
	}
	// RAFAEL
	ent->viewheight = 40;

    ent->inuse = true;
	ent->classname = "player";
	ent->mass = 200;
	ent->deadflag = DEAD_NO;
	ent->air_finished = level.time + 12;
	ent->clipmask = MASK_PLAYERSOLID;
//	ent->model = "players/male/tris.md2";
	ent->pain = player_pain;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags &= ~FL_NO_KNOCKBACK;

	ent->s.renderfx2 = 0;
	ent->onfiretime = 0;

	ent->cast_info.aiflags |= AI_GOAL_RUN;	// make AI run towards us if in pursuit

	VectorCopy (mins, ent->mins);
	VectorCopy (maxs, ent->maxs);
	VectorClear (ent->velocity);

	ent->cast_info.standing_max_z = ent->maxs[2];

	ent->cast_info.scale = MODEL_SCALE;
	ent->s.scale = ent->cast_info.scale - 1.0;

	// clear playerstate values
	memset (&ent->client->ps, 0, sizeof(client->ps));

	// MH: move slightly if spawn point is occupied
	if (ent->solid)
	{
		trace_t tr;
		tr = gi.trace(spawn_origin, ent->mins, ent->maxs, spawn_origin, NULL, CONTENTS_MONSTER);
		if (tr.startsolid)
		{
			// spawn point is occupied, try next to it
			vec3_t origin1;
			int c;
			VectorCopy(spawn_origin, origin1);
			for (c=0;;)
			{
				for (i=0; i<4; i++)
				{
					vec3_t start, end;
					float angle = (spawn_angles[YAW] + i * 90 - 45) / 360 * M_PI * 2;
					start[0] = spawn_origin[0] + cos(angle) * 50;
					start[1] = spawn_origin[1] + sin(angle) * 50;
					start[2] = spawn_origin[2];
					VectorCopy(start, end);
					end[2] -= 25;
					tr = gi.trace(start, ent->mins, ent->maxs, end, NULL, MASK_PLAYERSOLID);
					if (!tr.startsolid && tr.fraction < 1)
					{
						VectorCopy(start, spawn_origin);
						break;
					}
				}
				if (i < 4) break;
				if (++c == 2) break;
				// try another spawn point
				for (i=0; i<3; i++)
				{
					SelectSpawnPoint(ent, spawn_origin, spawn_angles);
					if (!VectorCompare(spawn_origin, origin1))
						break;
				}
				if (i == 3) break;
			}
		}
	}

	client->ps.pmove.origin[0] = spawn_origin[0]*8;
	client->ps.pmove.origin[1] = spawn_origin[1]*8;
	client->ps.pmove.origin[2] = spawn_origin[2]*8;

	ent->client->update_cam = 0;

	if (deathmatch->value && ((int)dmflags->value & DF_FIXED_FOV))
	{
		client->ps.fov = 90;
	}
	else
	{
		client->ps.fov = atoi(Info_ValueForKey(client->pers.userinfo, "fov"));
		if (client->ps.fov < 1)
			client->ps.fov = 90;
		else if (client->ps.fov > 160)
			client->ps.fov = 160;
	}

	// RAFAEL
	// weapon mdx
	{
		int i;

		memset(&(client->ps.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);

		client->ps.num_parts++;
	// JOSEPH 22-JAN-99
		if (client->pers.weapon)
			client->ps.model_parts[PART_HEAD].modelindex = gi.modelindex(client->pers.weapon->view_model);

		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			client->ps.model_parts[PART_HEAD].skinnum[i] = 0; // will we have more than one skin???
	}


	if (client->pers.weapon)
		client->ps.gunindex = gi.modelindex(client->pers.weapon->view_model);
	// END JOSEPH

	// clear entity state values
	ent->s.effects = 0;
	ent->s.skinnum = ent - g_edicts - 1;
	ent->s.modelindex = MAX_MODELS-1;		// will use the skin specified model
//	ent->s.modelindex2 = MAX_MODELS-1;		// custom gun model
	ent->s.frame = 0;
	VectorCopy (spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;	// make sure off ground
	VectorCopy (ent->s.origin, ent->s.old_origin);

// bikestuff
ent->biketime = 0;
ent->bikestate = 0;

// JOSEPH 29-MAR-99
//gi.soundindex ("vehicles/motorcycle/idle.wav");
// gi.soundindex ("motorcycle/running.wav");
//gi.soundindex ("vehicles/motorcycle/decel.wav");
//gi.soundindex ("vehicles/motorcycle/accel1.wav");
//gi.soundindex ("vehicles/motorcycle/accel2.wav");
//gi.soundindex ("vehicles/motorcycle/accel3.wav");
//gi.soundindex ("vehicles/motorcycle/accel4.wav");
// END JOSEPH


// Ridah, Hovercars
	if (g_vehicle_test->value)
	{
		if (g_vehicle_test->value == 3)
			ent->s.modelindex = gi.modelindex ("models/props/moto/moto.mdx");
		else
			ent->s.modelindex = gi.modelindex ("models/vehicles/cars/viper/tris_test.md2");

//		ent->s.modelindex2 = 0;
		ent->s.skinnum = 0;
		ent->s.frame = 0;

		if ((int)g_vehicle_test->value == 1)
			ent->flags |= FL_HOVERCAR_GROUND;
		else if ((int)g_vehicle_test->value == 2)
			ent->flags |= FL_HOVERCAR;
		else if ((int)g_vehicle_test->value == 3)
			ent->flags |= FL_BIKE;
		else if ((int)g_vehicle_test->value == 4)
			ent->flags |= FL_CAR;
	}
// done.


// Ridah, not used anymore, since the frames don't match. In single player, we just enforce the thug with correct skins in UserinfoChanged()
/*
	else if (!deathmatch->value)	// normal fighting
	{
		char	skinstr[16];
		int		skin;

		strcpy( skinstr, "001" );
		// skinstr[2] += (char) (rand() % 6);

		// ------------------------------------------------------------------------
		// initialize all model_part data

		// ent->s.skinnum = 12;
		ent->s.skinnum = 0;

		memset(&(ent->s.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);

		ent->s.num_parts++;
		ent->s.model_parts[PART_HEAD].modelindex = gi.modelindex("models/actors/punk/head.mdx");
		skin = gi.skinindex( ent->s.model_parts[PART_HEAD].modelindex, "018" );
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			ent->s.model_parts[PART_HEAD].baseskin = ent->s.model_parts[PART_HEAD].skinnum[i] = skin;

		ent->s.num_parts++;
		ent->s.model_parts[PART_LEGS].modelindex = gi.modelindex("models/actors/punk/legs.mdx");
		skin = gi.skinindex( ent->s.model_parts[PART_LEGS].modelindex, "010" );
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			ent->s.model_parts[PART_LEGS].baseskin = ent->s.model_parts[PART_LEGS].skinnum[i] = skin;

		ent->s.num_parts++;
		ent->s.model_parts[PART_BODY].modelindex = gi.modelindex("models/actors/punk/body.mdx");
		skin = gi.skinindex( ent->s.model_parts[PART_BODY].modelindex, "016" );
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			ent->s.model_parts[PART_BODY].baseskin = ent->s.model_parts[PART_BODY].skinnum[i] = skin;


		// ------------------------------------------------------------------------
	}
*/
	else if (dm_locational_damage->value)	// deathmatch, note models must exist on server for client's to use them, but if the server has a model a client doesn't that client will see the default male model
	{
		char	*s;
		char	modeldir[MAX_QPATH];//, *skins;
		int		len;
		int		did_slash;
		char	modelname[MAX_QPATH];
//		int		skin;

		// NOTE: this is just here for collision detection, modelindex's aren't actually set

		ent->s.num_parts = 0;		// so the client's setup the model for viewing

		s =Info_ValueForKey (client->pers.userinfo, "skin");

        //gi.bprintf(PRINT_HIGH, "Working ... 1\n");

//		skins = strstr( s, "/" ) + 1;

		// converts some characters to NULL's
		len = strlen( s );
		did_slash = 0;
		for (i=0; i<len; i++)
		{
			if (s[i] == '/')
			{
				s[i] = '\0';
				did_slash = true;
			}
			else if (s[i] == ' ' && did_slash)
			{
				s[i] = '\0';
			}
		}

		if (strlen(s) > MAX_QPATH-1)
			s[MAX_QPATH-1] = '\0';

		strcpy(modeldir, s);

		if (strlen(modeldir) < 1)
			strcpy( modeldir, "male_thug" );

		memset(&(ent->s.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);

		ent->s.num_parts++;
		strcpy( modelname, "players/" );
		strcat( modelname, modeldir );
		strcat( modelname, "/head.mdx" );
		ent->s.model_parts[ent->s.num_parts-1].modelindex = MAX_MODELS-1;
		gi.GetObjectBounds( modelname, &ent->s.model_parts[ent->s.num_parts-1] );
		if (!ent->s.model_parts[ent->s.num_parts-1].object_bounds[0])
			gi.GetObjectBounds( "players/male_thug/head.mdx", &ent->s.model_parts[ent->s.num_parts-1] );

		ent->s.num_parts++;
		strcpy( modelname, "players/" );
		strcat( modelname, modeldir );
		strcat( modelname, "/legs.mdx" );
		ent->s.model_parts[ent->s.num_parts-1].modelindex = MAX_MODELS-1;
		gi.GetObjectBounds( modelname, &ent->s.model_parts[ent->s.num_parts-1] );
		if (!ent->s.model_parts[ent->s.num_parts-1].object_bounds[0])
			gi.GetObjectBounds( "players/male_thug/legs.mdx", &ent->s.model_parts[ent->s.num_parts-1] );

		ent->s.num_parts++;
		strcpy( modelname, "players/" );
		strcat( modelname, modeldir );
		strcat( modelname, "/body.mdx" );
		ent->s.model_parts[ent->s.num_parts-1].modelindex = MAX_MODELS-1;
		gi.GetObjectBounds( modelname, &ent->s.model_parts[ent->s.num_parts-1] );
		if (!ent->s.model_parts[ent->s.num_parts-1].object_bounds[0])
			gi.GetObjectBounds( "players/male_thug/body.mdx", &ent->s.model_parts[ent->s.num_parts-1] );

        ent->s.num_parts++;
        ent->s.model_parts[PART_GUN].modelindex = MAX_MODELS-1;

    }
	else	// make sure we can see their weapon
	{
		//gi.bprintf(PRINT_HIGH, "Working ... 2\n");
        memset(&(ent->s.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);
		ent->s.model_parts[PART_GUN].modelindex = MAX_MODELS-1;
		ent->s.num_parts = PART_GUN+1;	// make sure old clients recieve the view weapon index

        //tical
        //ent->s.num_parts++;
        //ent->s.model_parts[PART_GUN2].modelindex = MAX_MODELS-1;
        //gi.GetObjectBounds("models/props/flag/flag1.mdx", &ent->s.model_parts[ent->s.num_parts-1] );
        //gi.modelindex("models/props/flag/flag1.mdx");
    }


	// set the delta angle
	for (i=0 ; i<3 ; i++)
		client->ps.pmove.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy (ent->s.angles, client->ps.viewangles);
	VectorCopy (ent->s.angles, client->v_angle);

	// MH: don't telefrag for a spectator
	if (ent->solid)
		KillBox (ent);

	gi.linkentity (ent);

	// MH: we don't want players being backward-reconciled to the place they died
	if (antilag->value && ent->solid != SOLID_NOT)
		G_ResetHistory(ent);

	// force the current weapon up
	client->newweapon = client->pers.weapon;
	ChangeWeapon (ent);

	// MH: send effect here instead of in ClientBeginDeathmatch
	if (ent->solid != SOLID_NOT || client->resp.enterframe == level.framenum)
	{
		// send effect
		gi.WriteByte (svc_muzzleflash);
		gi.WriteShort (ent - g_edicts);
		gi.WriteByte (MZ_LOGIN);
		if (ent->solid != SOLID_NOT)
			gi.multicast (ent->s.origin, MULTICAST_PVS);
		else
			gi.unicast (ent, false);
	}

}

/*
=====================
ClientBeginDeathmatch

A client has just connected to the server in
deathmatch mode, so clear everything out before starting them.

  NOTE: called every level load/change in deathmatch
=====================
*/
//extern void Teamplay_AutoJoinTeam( edict_t *self );


// Papa - Here is where I control how a player enters the game

void ClientBeginDeathmatch (edict_t *ent)
{
	int		save;

	save = ent->client->showscores;
	G_InitEdict (ent);

#if 1 //mm 2.0
	if ((ent->client->pers.spectator != SPECTATING && (level.modeset == WAVE_BUYZONE || level.modeset == WAVE_SPAWN_PLYR)) //|| level.modeset == WAVE_START
		|| (ent->client->ps.pmove.pm_type >= PM_DEAD)
		|| (level.modeset == WAVE_ACTIVE && timelimit->value)
		|| (ent->client->resp.enterframe == level.framenum)
		)
	{
		if (ent->client->resp.enterframe == level.framenum)
		{
			if (save == SCORE_REJOIN)
			{
				if (level.intermissiontime)
				{
					ClientRejoin(ent, true); // auto-rejoin
					save = 0;
				}
			}
			//else if (teamplay->value && !ent->client->pers.team && ((int)dmflags->value & DF_AUTO_JOIN_TEAM) && !level.intermissiontime && level.modeset != MATCH)
			//	Teamplay_AutoJoinTeam(ent);
		}
		// locate ent at a spawn point
		PutClientInServer(ent);
	}
	else
	{
		ent->movetype = MOVETYPE_SPECTATOR; //MOVETYPE_NOCLIP; //
		ent->solid = SOLID_NOT;
		ent->svflags |= SVF_NOCLIENT;
		gi.linkentity(ent);
		ent->client->newweapon = ent->client->pers.weapon = NULL;
		ChangeWeapon(ent);
	}

	if (/*!teamplay->value &&*/ ent->client->pers.spectator != SPECTATING && level.modeset != WAVE_ACTIVE)
		gi.bprintf(PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname);

	if (save && ent->solid == SOLID_NOT)
		ent->client->showscores = save;
	//end mm2.0

#else //mm old
	//FREDZ remove
	// locate ent at a spawn point (MH: stay at same place if spectating and not dead)
	if (level.framenum == ent->client->resp.enterframe || ent->client->pers.spectator == PLAYING || ent->client->ps.pmove.pm_type >= PM_DEAD
		|| ent->client->pers.spectator == PLAYER_READY)//hypov8
	{
		PutClientInServer(ent);

		// MH: go to intermission spot during intermission
		if (level.intermissiontime)
		{
			MoveClientToIntermission(ent);
			ClientEndServerFrame(ent);
			return;
		}
	}


	else
	{
		if ((ent->client->pers.spectator == SPECTATING) || (ent->client->showscores == SCORE_REJOIN) || (level.modeset == ENDGAMEVOTE)
         /*|| (level.modeset != WAVE_ACTIVE && level.modeset != WAVE_START && level.modeset != WAVE_BUYZONE)*/)
		{
//			ent->movetype = MOVETYPE_NOCLIP;
            ent->movetype = MOVETYPE_SPECTATOR;//FREDZ example
			ent->solid = SOLID_NOT;
			ent->svflags |= SVF_NOCLIENT;
			ent->client->pers.weapon = NULL;
		}
		else
		{
			gi.bprintf (PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname);
//			sl_WriteStdLogPlayerEntered( &gi, level, ent );	// Standard Logging
		}
	}
	ent->client->showscores = save;



	if ((level.modeset == PREGAME) && (ent->client->showscores == NO_SCOREBOARD))
		ent->client->showscores = SCOREBOARD;
	else if (ent->solid != SOLID_NOT)
		ent->client->showscores = NO_SCOREBOARD;

	// make sure all view stuff is valid
	ClientEndServerFrame (ent);


	// If they're using an old version, make sure they're aware of it
	if (ent->client->pers.version < 120)//FREDZ should be 1.20 linux is only 1.20
	{
		gi.centerprintf( ent, "You are using an old version\nof Kingpin.\n\nGet the upgrade at:\n\nwww.kingpin.info" ); // MH: updated
     /*   ErrorMSGBox(ent, "\"You are using an old version of Kingpin. Get the 1.21 upgrade at http://www.monkeymod.com\"");
        KICKENT(ent,"%s is being kicked for old version of kingpin.exe\n");*/
	}

//gi.dprintf("OUT: ClientBeginDeathmatch(%d)\n",((int)ent-(int)g_edicts)/sizeof(edict_t));
#endif
}


/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
extern void MoveClientToPawnoMatic (edict_t *ent);	// note to Rafael, was causing an undefined warning
extern void ED_CallSpawn (edict_t *ent);

int	num_followers = 0;
follower_t	followers[MAX_FOLLOWERS];

extern int client_connected;

extern qboolean	changing_levels;
extern void Cmd_HolsterBar_f (edict_t *ent);

void ClientBegin (edict_t *ent)
{
	int		i;
	char *a;

	client_connected = 1;

	// MH: if just connected, get the client version
	if ((!ent->client->resp.enterframe) && (CHECKFORCLIENT))
		set_version(ent);

	ent->client = game.clients + (ent - g_edicts - 1);


// Papa - either show rejoin or MOTF scoreboards
	if (ent->client->showscores != SCORE_REJOIN || !level.player_num) // MH: check player_num in case map changed
		ent->client->showscores = SCORE_MOTD;

	ent->client->pers.fakeThief = 0;

	if (keep_admin_status) {
		a=gi.cvar("modadmin","",0)->string;
		if (a[0]) {
			edict_t *player;
			for (i=0 ; i<maxclients->value ; i++) {
				player = g_edicts + 1 + i;
				if (!player->inuse) continue;
				if (player->client->pers.admin>NOT_ADMIN) {
					gi.cvar_set("modadmin","");
                    gi.cvar_set("admintype","");
					break;
				}
			}
			if (i==maxclients->value) {
				if (!strcmp(a,ent->client->pers.ip)) {
					ent->client->pers.admin = atoi(gi.cvar("admintype", "", 0)->string);
                    gi.cvar_set("modadmin","");
                    gi.cvar_set("admintype","");
                    gi.bprintf(PRINT_HIGH,"%s is now admin.\n",ent->client->pers.netname);
				}
			}
		}
	}
	else
		ent->client->pers.admin=NOT_ADMIN; // MH: moved here from above (fix)

	a=gi.cvar("rconx","",0)->string;
	if (a[0]) {
		char *s,buf[256];
		strcpy(buf,a);
		s=Info_ValueForKey(buf,ent->client->pers.ip);
		if (s[0]) {
			strcpy(ent->client->pers.rconx,s);
			Info_RemoveKey(buf,ent->client->pers.ip);
			gi.cvar_set("rconx",buf);
		}
	}

	ent->client->pers.lastpacket = curtime; // MH: update last packet time
	ent->check_idle = level.framenum; // MH: init idle timer

	//TF: reset cash on level begin
	ent->client->pers.currentcash = 0; //waveGiveCash(1);

	if (deathmatch->value)
	{
		{ // MH: send weather effects
			edict_t *cloud = NULL;
			while (cloud = G_Find(cloud, FOFS(classname), "elements_raincloud"))
				think_new_first_raincloud_client(cloud, ent);
			while (cloud = G_Find(cloud, FOFS(classname), "elements_snowcloud"))
				think_new_first_snowcloud_client(cloud, ent);
		}

		InitClientRespClear(ent->client);
		ClientBeginDeathmatch (ent);
		// make sure all view stuff is valid
		ClientEndServerFrame(ent); //mm 2.0
		return;
	}


#if 1 //hypov8 note: nothing below this will get used in DM. unreachable code
	InitClientRespClear(ent->client); //hypov8 add


	ent->cast_group = 1;

	// Ridah, copy the episode_flags over
	ent->episode_flags = ent->client->pers.episode_flags;

	level.speaktime = 0;

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (ent->inuse == true)
	{
		// the client has cleared the client side viewangles upon
		// connecting to the server, which is different than the
		// state when the game is saved, so we need to compensate
		// with deltaangles
		for (i=0 ; i<3 ; i++)
			ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->ps.viewangles[i]);
	}
	else
	{
		// a spawn point will completely reinitialize the entity
		// except for the persistant data that was initialized at
		// ClientConnect() time
		G_InitEdict (ent);
		ent->classname = "player";
//		InitClientResp (ent->client);
		PutClientInServer (ent);
	}

	if (level.intermissiontime)
	{
		MoveClientToIntermission (ent);
	}
	else if (level.cut_scene_time)
	{
		MoveClientToCutScene (ent);
	}
	else if (level.pawn_time || strstr (level.mapname, "pawn_"))
	{
		level.pawn_time = 1.0;
		MoveClientToPawnoMatic (ent);
	}
	else if (strstr (level.mapname, "bar_"))
	{
		level.bar_lvl = true;
		Cmd_HolsterBar_f (ent);

		if (level.episode == 1)
		{
		//ent->episode_flags |= EP_BAR_FIRST_TIME;
		//ent->client->pers.episode_flags |= EP_BAR_FIRST_TIME;
		EP_Skidrow_Register_EPFLAG (ent, EP_BAR_FIRST_TIME);
		}
		else if (level.episode == 2)
		{
			EP_Skidrow_Register_EPFLAG (ent, EP_PV_BAR_FIRST_TIME);
		}

	}
	else if (strstr(level.mapname, "office_"))//FREDZ Office used in rc5 localteam
	{
		level.bar_lvl = true;
		Cmd_HolsterBar_f (ent);

		if (level.episode == 2)
		{
			EP_Skidrow_Register_EPFLAG (ent, EP_PV_OFFICE_FIRST_TIME);
		}
	}
	else
	{
		// send effect if in a multiplayer game
		if (game.maxclients > 1)
		{
			gi.WriteByte (svc_muzzleflash);
			gi.WriteShort (ent-g_edicts);
			gi.WriteByte (MZ_LOGIN);
			gi.multicast (ent->s.origin, MULTICAST_PVS);

			gi.bprintf (PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname);
		}
	}

	// Ridah, restore any following characters
	if (num_followers > 0)
	{
		int j, k;
		follower_t	*fol;
		edict_t		*newent, *spawnspot;
		qboolean	killed, fakespawn;

		if (num_followers > MAX_FOLLOWERS)
			num_followers = MAX_FOLLOWERS;

		for (i=0; i<num_followers; i++)
		{
			fol = &followers[i];
			killed = false;

			// spawn a similar entity

			newent = G_Spawn();

			// copy the necessary data for spawning
			newent->classname = gi.TagMalloc( strlen( fol->classname ) + 1, TAG_LEVEL );
			strcpy( newent->classname, fol->classname );

			if (fol->name)
			{
				newent->name = gi.TagMalloc( strlen( fol->name ) + 1, TAG_LEVEL );
				strcpy( newent->name, fol->name );
			}

			if (fol->art_skins)
			{
				newent->art_skins = gi.TagMalloc( strlen( fol->art_skins ) + 1, TAG_LEVEL );
				strcpy( newent->art_skins, fol->art_skins );
			}

			newent->cast_info.scale = fol->scale;
			newent->head = fol->head;
			newent->cast_group = 1;
			newent->spawnflags = fol->spawnflags;
			newent->count = fol->count;

			// find a spawn spot
			spawnspot = NULL;
			while (1)
			{
				if (!(spawnspot = G_Find( spawnspot, FOFS(classname), "info_player_coop" )))
					break;

				if (VectorDistance( spawnspot->s.origin, ent->s.origin ) > 384)
					continue;

				if (!ValidBoxAtLoc( spawnspot->s.origin, tv(-16, -16, -24), tv(16, 16, 48), NULL, MASK_PLAYERSOLID|CONTENTS_MONSTERCLIP ))
				{
					if(developer->value)
						gi.dprintf( "WARNING: coop spawn in solid at: %i, %i, %i\n", (int)spawnspot->s.origin[0], (int)spawnspot->s.origin[1], (int)spawnspot->s.origin[2] );
					continue;
				}

				if (!(spawnspot->spawnflags & 0x10000000))
				{
					spawnspot->spawnflags |= 0x10000000;
					break;
				}
			}

			fakespawn = false;

			if (!spawnspot)
			{
				vec3_t	 vec;

				gi.dprintf( "** WARNING: Unable to find a coop spawn for %s. Hacking a spawn spot.\n", fol->classname );

				spawnspot = G_Spawn();
				AngleVectors( ent->s.angles, vec, NULL, NULL );

				VectorMA( ent->s.origin, -48*(i+1), vec, spawnspot->s.origin );
				VectorCopy( ent->s.angles, spawnspot->s.angles );

				fakespawn = true;
			}

			VectorCopy( spawnspot->s.origin, newent->s.origin );
			newent->s.origin[2] += 1;

			VectorCopy( spawnspot->s.angles, newent->s.angles );

			if (fakespawn)
				G_FreeEdict( spawnspot );

			// add it to the characters listing
			if (killed)
			{
				level.characters[j] = newent;
			}
			else
			{
				AddCharacterToGame( newent );
			}


			// spawn it!
			ED_CallSpawn( newent );		// will get added to the game in here


			// make them aware of and hired by us
			AI_RecordSighting( newent, ent, 64 );		// dist = 64 will do, will get updated next sight anyway
			level.global_cast_memory[newent->character_index][ent->character_index]->flags |= (MEMORY_HIRED | MEMORY_HIRE_FIRST_TIME | MEMORY_HIRE_ASK);

			// make them follow us
			newent->leader = ent;

			// restore pain skins
			for (j=0; j<newent->s.num_parts; j++)
			{
				for (k=0; k<MAX_MODELPART_OBJECTS; k++)
				{
					newent->s.model_parts[j].skinnum[k] += (byte)fol->skinofs[j][k];
				}
			}

			// restore health
			newent->health = fol->health;
			newent->max_health = fol->max_health;

		}

		num_followers = 0;

		// clear coop spawnflags
		spawnspot = NULL;
		while (1)
		{
			if (!(spawnspot = G_Find( spawnspot, FOFS(classname), "info_player_coop" )))
				break;

			spawnspot->spawnflags &= ~0x10000000;
		}
	}

	// make sure all view stuff is valid
	ClientEndServerFrame (ent);

	// Ridah, if we've come from another level, save the current game (fixes hired guy's disappearing after restarting a level after dying)
	if (changing_levels)
	{
		gi.SaveCurrentGame();
		changing_levels = false;
	}
#endif
}

void maxrate_think(edict_t *self)
{
	gi.cprintf(self->owner, PRINT_HIGH, "Server restricting rate to %s\n",
		Info_ValueForKey(self->owner->client->pers.userinfo,"rate"));
	G_FreeEdict(self);
}

void nameclash_think(edict_t *self)
{
  //  if (!(Q_stricmp (self->owner->client->pers.netname, NAME_CLASH_STR) == 0))
	    gi.cprintf(self->owner, PRINT_HIGH, "Another player on the server is already using this name.\n");
	G_FreeEdict(self);
}
/*
===========
ClientUserInfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	char	*s, *s2;
//	char	*fog;
	//int		playernum;
	char	*extras;
    int     fIgnoreName = 0,a;

	// MH: version and IP retrieval moved to ClientConnect

	// check for malformed or illegal info strings
	if (!Info_Validate(userinfo))
	{
		// strcpy (userinfo, "\\name\\badinfo\\skin\\male_thug/018 016 010\\extras\\0");
		strcpy (userinfo, "\\name\\badinfo\\skin\\male_thug/009 019 017\\extras\\0");
	}

	if (!ent->client->resp.enterframe) {
		Info_SetValueForKey(userinfo,"msg","0");
	}

	// set name
    /* Standard Logging - don't change name */
/*    if( '\0' != ent->client->pers.netname[0] )
    {
        Info_SetValueForKey (userinfo, "name", ent->client->pers.netname);
        fIgnoreName = 1;
    }
*/
	s = Info_ValueForKey (userinfo, "name");
	s[13] = 0; // MH: only 13 chars are shown in scoreboard
    if (strchr(s,'%'))
    {
        s2 = s;
		while (s2=strchr(s,'%')) *s2=' ';
		Info_SetValueForKey (userinfo, "name", s);
	}

	for (a=strlen(s)-1;a>=0 && s[a]<33;a--) ;
	if (a==-1) {
		// MH: keep the existing name if they have one
		if (ent->client->pers.netname[0])
			s = ent->client->pers.netname; // keep the existing name
		else
			s=NAME_BLANK_STR;
		Info_SetValueForKey (userinfo, "name", s); // MH: let server know about change
	} else {
	s[a+1]=0;

    // Standard Logging - stop name clashes
    if( !fIgnoreName )
    {
        edict_t		*cl_ent;
        unsigned int i;

        for (i=0 ; i<game.maxclients ; i++)
        {
            cl_ent = g_edicts + 1 + i;
            if( cl_ent->inuse &&
                (cl_ent != ent) &&
                !strcmp(cl_ent->client->pers.netname, s) )
            {
				// MH: only delay clash notice and announce to others when connecting
				if (!ent->client->resp.enterframe)
				{
					edict_t *thinker;
					thinker = G_Spawn();
					thinker->think = nameclash_think;
					thinker->nextthink = level.time + 2 + random()*2;
					thinker->owner = ent;
					gi.bprintf(PRINT_HIGH, "Another player is trying to use %s's name\n", s);
				}
				else
					gi.cprintf(ent, PRINT_HIGH, "Another player on the server is already using this name\n"); // MH: show clash notice immediately

				// MH: keep the existing name if they have one
				if (ent->client->pers.netname[0])
					s = ent->client->pers.netname;
				else
	                s = NAME_CLASH_STR;

                Info_SetValueForKey (userinfo, "name", s);
            }
        }
    }
	}
    // Standard Logging

	// MH: let everyone know about name changes without flooding
	if (ent->client->pers.netname[0])
	{
		// has the name changed
		if (strcmp(ent->client->pers.netname, s))
		{
			// stop flooding
			if (level.framenum < (ent->name_change_frame + 20))
			{
				gi.cprintf(ent, PRINT_HIGH, "Overflow protection: Unable to change name yet\n");
				s = ent->client->pers.netname; // keep the existing name
				Info_SetValueForKey (userinfo, "name", s);
			}
			else
			{
				gi.bprintf(PRINT_HIGH, "%s changed name to %s\n", ent->client->pers.netname, s);
				ent->name_change_frame = level.framenum;
			}
		}
	}
	if (s != ent->client->pers.netname) //MH: don't bother if keeping old name
	{
	    strncpy (ent->client->pers.netname, s, sizeof(ent->client->pers.netname)-1);
		if (CheckNameBan(s))
			KICKENT(ent,"%s is being kicked because they're banned!\n");
	}

	// check maxrate
	s=Info_ValueForKey (userinfo, "rate");
	if (s[0] && (a=atoi(s))) {
		if ((int)maxrate->value && a>(int)maxrate->value && (int)maxrate->value<15000) { // MH: the server will limit to 15000 so don't bother with higher here
			if (!ent->client->resp.enterframe) {
				edict_t *thinker;
				thinker = G_Spawn();
				thinker->think = maxrate_think;
				thinker->nextthink = level.time + 2 + random()*2;
				thinker->owner = ent;
			} else {
				gi.cprintf(ent, PRINT_HIGH, "Server restricting rate to %d\n",(int)maxrate->value);
			}
			Info_SetValueForKey( userinfo, "rate", va("%i", (int)maxrate->value) );
		}
	}

	// set skin
	s=Info_ValueForKey (userinfo, "skin");
	if (strchr(ent->skins,'/') && level.framenum<(ent->move_frame+10)) {
		strcpy(s, ent->skins);
	}

	/*
	// Ridah, HACK for teamplay demo, set skins manually
	if (deathmatch->value && teamplay->value && ent->client->pers.team)
	{
		// NOTE: skin order is "HEAD BODY LEGS"
		char *skin, *body, *legs;
		char tempstr[MAX_QPATH];
		int i, valid, model_index;

		// Hard-coded skin sets for each model

		static char *valid_models[] = { "female_chick", "male_thug", "male_runt", NULL };
		static char *valid_skinsets[][2][2][2] =

			// ordering here is {"LEGS", "BODY"}
			{
				{	// Bitch
					{{"056","057"}, {"056","058"}},		// Team 1
					{{"033","032"}, {"031","031"}}		// Team 2
				},
				{	// Thug
					{{"057","056"}, {"058","091"}},
					{{"031","031"}, {"032","035"}}
				},
				{	// Runt
					{{"058","056"}, {"057","056"}},
					{{"031","030"}, {"032","031"}}
				}
			};

		// make sure they are using one of the standard models
		valid = false;
		i = 0;
		strcpy( tempstr, s );
		skin = strrchr( tempstr, '/' );

		if (!skin)
		{	// invalid model, so assign a default
			model_index = 2;
			strcpy( tempstr, valid_models[model_index] );

			// also recreate a new skin for "s"
			strcpy( s, tempstr );
			strcat( s, "/001 001 001" );

			valid = true;
		}
		else
		{
			skin[0] = '\0';

			while (valid_models[i])
			{
				if (!Q_stricmp( tempstr, valid_models[i] ))
				{
					valid = true;
					model_index = i;
					break;
				}

				i++;
			}
		}

		if (!valid)
		{	// assign a random model
			model_index = -1;

			// look for a gender match
			while (valid_models[i])
			{
				if (!strncmp( tempstr, valid_models[i], 4 ))
				{
					model_index = i;
					strcpy( tempstr, valid_models[model_index] );
					break;
				}

				i++;
			}

			if (model_index < 0)
			{
				model_index = 2;//rand()%i;
				strcpy( tempstr, valid_models[model_index] );
			}
		}

		// At this point, tempstr = model only (eg. "male_thug")

		// check that skin is valid
		skin = strrchr( s, '/' ) + 1;
		skin[3] = skin[7] = '\0';

		body = &skin[4];
		legs = &skin[8];

		valid = false;

		for (i=0; i<2; i++)
		{
			if (	!Q_stricmp( body, valid_skinsets[model_index][ent->client->pers.team-1][i][1] )
				&&	!Q_stricmp( legs, valid_skinsets[model_index][ent->client->pers.team-1][i][0] ))
			{
				valid = true;
				break;
			}
		}

		if (!valid)
		{	// Assign a random skin for this model
			i = rand()%2;

			strcpy( body, valid_skinsets[model_index][ent->client->pers.team-1][i][1] );
			strcpy( legs, valid_skinsets[model_index][ent->client->pers.team-1][i][0] );
		}

		skin[3] = skin[7] = ' ';

		// paste the skin into the tempstr
		strcat( tempstr, "/" );
		strcat( tempstr, skin );

		Info_SetValueForKey( userinfo, "skin", tempstr );
		strncpy(ent->skins,tempstr,31);
	}*/
//	else
    if (!deathmatch->value)	// enforce thug with single player skin set
	{
		static char *singleplayerskin = "male_thug/018 016 010";

		Info_SetValueForKey( userinfo, "skin", singleplayerskin );
	} else
		strncpy(ent->skins,s,31);

	// now check it again after the filtering, and set the Gender accordingly
	s=Info_ValueForKey (userinfo, "skin");
	if ((strstr(s, "female") == s))
		ent->gender = GENDER_FEMALE;
	else if ((strstr(s, "male") == s) || (strstr(s, "thug")))
		ent->gender = GENDER_MALE;
	else
		ent->gender = GENDER_NONE;

	extras=Info_ValueForKey (userinfo, "extras");

#if 1 //mm 2.0
	// combine name and skin into a configstring
	playerskin(ent - g_edicts - 1, va("%s\\%s %s", ent->client->pers.netname, s, extras)); //mm 2.0
#else
	playernum = ent-g_edicts-1;

	// combine name and skin into a configstring
	gi.configstring (CS_PLAYERSKINS+playernum, va("%s\\%s %s", ent->client->pers.netname, s, extras) );
#endif


	// fov
	if (deathmatch->value && ((int)dmflags->value & DF_FIXED_FOV))
	{
		ent->client->ps.fov = 90;
	}
	else
	{
		ent->client->ps.fov = atoi(Info_ValueForKey(userinfo, "fov"));
		// MH: check FOV zooming option
		if (ent->client->ps.fov < (no_zoom->value ? 90 : 1))
		{
			if (no_zoom->value && ent->client->resp.enterframe)
				gi.cprintf(ent, PRINT_HIGH, "FOV zooming is not allowed\n");
			Info_SetValueForKey(userinfo, "fov", "90"); // MH: update userinfo
			ent->client->ps.fov = 90;
		}
		else if (ent->client->ps.fov > 160)
			ent->client->ps.fov = 160;
	}


	/*
	{
		vec3_t vars1, vars2;

		fog = Info_ValueForKey (userinfo, "fogcolor");

		if (strlen (fog) == 17)
		{
			int i, cnt;
			char *varR, *varG, *varB;

			for (i=0; i<17; i++)
			{
				if (i < 5)
				{
					varR[i] = fog[i];
				}
				else if (i == 5)
					continue;
				else if (i < 11)
				{
					varG[i-5] = fog[i];
				}
				else if (i == 11)
					continue;
				else
				{
					varB[i-11] = fog[i];
				}
			}

			vars1[0] = atof (varR);
			vars1[1] = atof (varG);
			vars1[2] = atof (varB);

			gi.WriteByte (svc_temp_entity);
			gi.WriteByte (TE_FOG_BRUSH);

			gi.WritePosition (vars1);
			gi.WritePosition (vars2);

			gi.multicast (ent->s.origin, MULTICAST_PVS);

			gi.dprintf ("fog color <%s %s %s>\n", varR, varG, varB);
		}
		else
			gi.dprintf ("must be in <0.000 0.000 0.000> format\n");
	}
	*/

	// handedness
	s = Info_ValueForKey (userinfo, "hand");
	if (s[0])
        ent->client->pers.hand = atoi(s);

    // screen width
	s = Info_ValueForKey(userinfo, "gl_mode");
	s2 = strchr(s, '(');
	if (s2)
	{
		// resolution set by patch
		float scale;
		ent->client->pers.screenwidth = atoi(s2 + 1);
		scale = atof(Info_ValueForKey(userinfo, "scale"));
        if (scale > 0)
                ent->client->pers.screenwidth *= scale;
	}
	else
	{
		switch (atoi(s))
		{
			case 0:
				ent->client->pers.screenwidth = 640;
				break;
			case 1:
				ent->client->pers.screenwidth = 800;
				break;
			case 2:
				ent->client->pers.screenwidth = 960;
				break;
			case 3:
				ent->client->pers.screenwidth = 1024;
				break;
			case 4:
				ent->client->pers.screenwidth = 1152;
				break;
			default:
				// assuming anything else is at least 1280
				ent->client->pers.screenwidth = 1280;
				break;
		}
	}

	// save off the userinfo in case we want to check something later
	strncpy (ent->client->pers.userinfo, userinfo, MAX_INFO_STRING-1);

}



/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/
qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	char	*value, *skinvalue; //*command;
	int		i;
	edict_t	*doot; int j;

	ent->client = NULL;
	ent->inuse = false;
	ent->flags = 0; // MH: reset flags (togglecam)
	ent->skins[0]=0;
	ent->move_frame=0;

//	Snap, bunnyhop
	ent->strafejump_count = 0;
	ent->jump_framenum = 0;
	ent->land_framenum = 1;

	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");
	if (SV_FilterPacket(value))
		return false;

	// check for a password
	value = Info_ValueForKey (userinfo, "password");
	if (strcmp(password->string, value) != 0)
		return false;

	if (CheckPlayerBan (userinfo))
		return false;

// Ridah, if this isn't a loadgame, try to add them to the character list
	if (!deathmatch->value && (ent->inuse == false))
	{
		if (!AddCharacterToGame(ent))
		{
			return false;
		}
	}
// Ridah, done.

	// they can connect
	ent->client = game.clients + (ent - g_edicts - 1);

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (ent->inuse == false)
	{
		// clear the respawning variables
		//InitClientResp (ent->client);
		InitClientRespClear (ent->client);
		if (!game.autosaved || !ent->client->pers.weapon) {
			memset(&ent->client->pers, 0, sizeof(ent->client->pers)); // MH: just clear the whole thing
			InitClientPersistant (ent->client);
		}

		// JOSEPH 14-MAR-99
		if (!strcmp(level.mapname, "sr1") || !strcmp(level.mapname, "kpcut1"))
		{
			if (!(game.maxclients > 1))
			{
				ent->client->pers.health = 68;
				ent->health = 68;
			}
		}
		// END JOSEPH
	}

	// MH: get IP and version during connect only
	value = Info_ValueForKey (userinfo, "ip");
	strncpy(ent->client->pers.ip, value, sizeof(ent->client->pers.ip)-1);

	// MH: get player country
	value = Info_ValueForKey (userinfo, "country");
	strncpy(ent->client->pers.country, value, sizeof(ent->client->pers.country) - 1);

 	// client exe version
	value = Info_ValueForKey (userinfo, "ver");
	if (value[0])
		ent->client->pers.version = atoi(value);
	else	// assume client is old version
		ent->client->pers.version = 100;

	// MH: check for a patched version
	if (!strncmp(value, "121p", 4))
		ent->client->pers.patched = atoi(value + 4);
	else
		ent->client->pers.patched = 0;

	ent->client->resp.enterframe = 0;
    ent->name_change_frame = -80;  //just to be sure

	ClientUserinfoChanged (ent, userinfo);

	if (game.maxclients > 1)
	{
		// MH: log the client's country
		if (ent->client->pers.country[0])
			gi.dprintf ("%s (%s) connected from %s\n", ent->client->pers.netname, ent->client->pers.ip, ent->client->pers.country);
		else
			gi.dprintf ("%s (%s) connected\n", ent->client->pers.netname,ent->client->pers.ip);

		for_each_player (doot,j)
			if ((doot->client->pers.admin == ADMIN) || (doot->client->pers.rconx[0]))
			{
				// MH: display the client's country
				if (ent->client->pers.country[0])
					gi.cprintf(doot, PRINT_CHAT, "%s (%s) connected from %s\n", ent->client->pers.netname, ent->client->pers.ip, ent->client->pers.country);
				else
					gi.cprintf(doot, PRINT_CHAT, "%s (%s) connected\n", ent->client->pers.netname,ent->client->pers.ip);
			}
			else // MH: also tell non-admins about the connecting player
			{
				// MH: display the client's country
				if (ent->client->pers.country[0])
					gi.cprintf(doot, PRINT_CHAT, "%s connected from %s\n", ent->client->pers.netname, ent->client->pers.country);
				else
					gi.cprintf(doot, PRINT_CHAT, "%s connected\n", ent->client->pers.netname);
			}
	}

	// Ridah, make sure they have to join a team

//	if (teamplay->value)
//		ent->client->pers.team = 0;

// check to see if a player was disconnected
	i = 0;
	skinvalue = Info_ValueForKey (userinfo, "skin");
	while (i < level.player_num)
	{
		// MH: not checking skins because they are random
		if (Q_stricmp (ent->client->pers.netname,playerlist[i].player) == 0)
			ent->client->showscores = SCORE_REJOIN;

		i++;
	}

	// MH: fix to allow rejoin in DM
/*	if (teamplay->value || ent->client->showscores == SCORE_REJOIN)
		ent->client->pers.spectator = SPECTATING;
	else*/

    ent->client->pers.spectator = PLAYER_READY; //hypov8 dont enter a current wave
#if 1 //mm 2.0
	// check to see if a player was disconnected
	if (CheckClientRejoin(ent) >= 0)
	{
		ent->client->showscores = SCORE_REJOIN;
		ent->client->pers.spectator = SPECTATING;
	}
	//level.lastactive = level.framenum;
#endif

	ent->client->pers.lastpacket = curtime; // MH: set last packet time
	level.lastactive = level.framenum;

	return true;
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void ClientDisconnect (edict_t *ent)
{
	int		playernum;
	int		i, z;
//	char	*skinvalue;

	if (!ent->client || !ent->client->pers.connected) // MH: added connected check
		return;

	// MH: check they have fully connected (might be cancelled connection)
	if (ent->inuse)
	{
		// MH: drop cash before leaving
//		if ((ent->solid != SOLID_NOT) && (level.waveNum != 1))//FREDZ some people joint accidently first round.//Not using
//			DropCash(ent);

		// MH; cancel their vote
		if (ent->vote == CALLED_VOTE)
			level.voteset = NO_VOTES;

		playernum = ent - g_edicts - 1;

//	sl_LogPlayerDisconnect( &gi, level, ent );	// Standard Logging
	    ent->client->pers.fakeThief = 0;

// Papa - store player info after they disconnect
		if (ent->client->resp.time && ent->client->resp.score > 0) // MH: only store if they have played some time
		{
			if (level.player_num == 64)
			{
				memmove(playerlist, playerlist + 1, 63 * sizeof(playerlist[0]));
				level.player_num--;
			}
			playerlist[level.player_num].frags = ent->client->resp.score;
			playerlist[level.player_num].deposits = ent->client->resp.deposited;
			playerlist[level.player_num].acchit = ent->client->resp.acchit;
			playerlist[level.player_num].accshot = ent->client->resp.accshot;
			playerlist[level.player_num].stole = ent->client->resp.stole; // MH: added
			playerlist[level.player_num].mute = ent->client->pers.mute;
			//do the fav too
			for (z = 0;z < 8;z++)
			{
			   playerlist[level.player_num].fav[z] = ent->client->resp.fav[z];
			}
			playerlist[level.player_num].team = ent->client->pers.team;
			playerlist[level.player_num].time = ent->client->resp.time; // MH: changed
/* // MH: not used
			skinvalue = Info_ValueForKey (ent->client->pers.userinfo, "skin");
			strcpy (playerlist[level.player_num].skin, skinvalue);
*/
			strcpy (playerlist[level.player_num].player, level.playerskins[playernum]); //ent->client->pers.netname
			level.player_num++;
		}

		for (i=1; i<=maxclients->value; i++)
		{
			if (!g_edicts[i].inuse)
				continue;
			if (!g_edicts[i].client)
				continue;
			// MH: updated chasecam from MM2
			if (g_edicts[i].client->chase_target == ent)
				ChaseStop(&g_edicts[i]);
		}

		// send effect
		gi.WriteByte (svc_muzzleflash);
		gi.WriteShort (ent-g_edicts);
		gi.WriteByte (MZ_LOGOUT);
		gi.multicast (ent->s.origin, MULTICAST_PVS);

		gi.unlinkentity (ent);
		ent->s.modelindex = 0;
		ent->s.num_parts = 0;
		ent->solid = SOLID_NOT;
		ent->inuse = false;

		// MH: moved from below
		//playernum = ent-g_edicts-1;
		//gi.configstring (CS_PLAYERSKINS+playernum, "");
		playerskin(playernum, "");
	}

	// MH: kpded2 sets ping negative if they are reconnecting
	if (kpded2 && ent->client->ping < 0)
		gi.bprintf (PRINT_HIGH, "%s is reconnecting\n", ent->client->pers.netname);
	else
		gi.bprintf (PRINT_HIGH, "%s checked out\n", ent->client->pers.netname);

	// MH: moved from above
	ent->classname = "disconnected";
	ent->client->pers.connected = false;
}

//==============================================================


edict_t	*pm_passent;

// pmove doesn't need to know about passent and contentmask
#if __linux__
// MH: callee_pop_aggregate_return attribute allows modern GCC to be used
trace_t	__attribute__((callee_pop_aggregate_return(0))) PM_trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
#else
trace_t	PM_trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
#endif
{
	if (pm_passent->health > 0 ) //hypov8 nav. allow players to walk through each other and ai
	{
		if (nav_dynamic->value&& level.modeset == WAVE_ACTIVE)	// if dynamic on, get blocked by MONSTERCLIP brushes as the AI will be
			return gi.trace (start, mins, maxs, end, pm_passent, (CONTENTS_SOLID | CONTENTS_WINDOW| CONTENTS_MONSTERCLIP| CONTENTS_PLAYERCLIP) /*MASK_PLAYERSOLID | CONTENTS_MONSTER*/);
		else
			return gi.trace (start, mins, maxs, end, pm_passent, (CONTENTS_SOLID | CONTENTS_WINDOW| CONTENTS_PLAYERCLIP) ); //MASK_PLAYERSOLID
	}
	else
		return gi.trace (start, mins, maxs, end, pm_passent, MASK_DEADSOLID);
}

unsigned CheckBlock (void *b, int c)
{
	int	v,i;
	v = 0;
	for (i=0 ; i<c ; i++)
		v+= ((byte *)b)[i];
	return v;
}
void PrintPmove (pmove_t *pm)
{
	unsigned	c1, c2;

	c1 = CheckBlock (&pm->s, sizeof(pm->s));
	c2 = CheckBlock (&pm->cmd, sizeof(pm->cmd));
	Com_Printf ("sv %3i:%i %i\n", pm->cmd.impulse, c1, c2);
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	gclient_t	*client;
	edict_t	*other;
	int		i, j;
	pmove_t	pm;

	vec3_t	bike_premove_vel;

	level.lastactive = level.framenum;
	ent->client->pers.lastpacket = Sys_Milliseconds(); // MH: get packet's time

	// MH: skip this if they're gonna be kicked
	if (ent->kickdelay)
		return;

	if (ucmd->forwardmove|ucmd->sidemove|ucmd->upmove)
		ent->move_frame=level.framenum;//(ent->move_frame>>3)|ucmd->buttons|ucmd->forwardmove|ucmd->sidemove|ucmd->upmove;

	// MH: non-idle check
	if (ucmd->buttons|ucmd->forwardmove|ucmd->sidemove|ucmd->upmove)
		ent->check_idle = level.framenum;

	// BOT! (*nothing* uses impulse command so safe to assume)
	if (ucmd->impulse) {
		gi.dprintf("Received impulse: %s (%d)\n",ent->client->pers.netname,ucmd->impulse);
		KICKENT(ent,"%s is being kicked for using a bot!\n");
	}

	level.current_entity = ent;
	client = ent->client;

	// MH: textbuf stuff removed

    // JOSEPH 24-FEB-99
	if (level.cut_scene_end_count)
	{
		level.cut_scene_end_count--;

		if (!level.cut_scene_end_count)
			level.cut_scene_camera_switch = 0;
	}
	// END JOSEPH

	if (level.intermissiontime)
	{
		client->ps.pmove.pm_type = PM_FREEZE;
		// MH: no early exit
		return;
	}
	// RAFAEL
	else if (level.cut_scene_time)
	{
		client->ps.pmove.pm_type = PM_FREEZE;

		// note to self
		// need to do any precanned player move stuff

		if (level.time > level.cut_scene_time + 5.0
			&& (ucmd->buttons & BUTTON_ANY) )
			level.cut_scene_time = 0;

		return;
	}
	else if (level.pawn_time)
	{
		client->ps.pmove.pm_type = PM_FREEZE;
		return;
	}
    //FREDZ
	else if (ent->client->showscrollmenu)
	{
		//reset keys if up
		if (!ucmd->forwardmove && ent->client->scrollmenu_fwd_do == 2)
			ent->client->scrollmenu_fwd_do = 0;
		if (!ucmd->sidemove && ent->client->scrollmenu_side_do == 2)
			ent->client->scrollmenu_side_do = 0;

		//scroll menu update keys
		if (ucmd->forwardmove && ent->client->scrollmenu_fwd_do == 0)
		{
			ent->client->scrollmenu_fwd_do = 1;
			ent->client->scrollmenu_forward = ucmd->forwardmove;
		}
		if (ucmd->sidemove && ent->client->scrollmenu_side_do == 0)
		{
			ent->client->scrollmenu_side_do = 1;
			ent->client->scrollmenu_side = ucmd->sidemove;
		}

		//client->ps.pmove.pm_type = PM_FREEZE;
		client->ps.pmove.pm_type = PM_DEAD;
		return;
	}

	pm_passent = ent;

	// set up for pmove
	memset (&pm, 0, sizeof(pm));

	// MH: updated chasecam from MM2
	if (ent->client->chase_target)
	{
		// snap, for alternate chase modes...
		if (ucmd->upmove && 5 < (level.framenum - ent->client->chase_frame))
		{
			ent->client->chase_frame = level.framenum;
			if (ent->client->chasemode == LOCKED_CHASE)
				ent->client->chasemode = FREE_CHASE;
			else if (ent->client->chasemode == FREE_CHASE)
				ent->client->chasemode = EYECAM_CHASE;
			else
				ent->client->chasemode = LOCKED_CHASE;
			if (ent->client->prechase_ps.fov)
			{
				if (ent->client->chasemode == FREE_CHASE)
				{
					ent->client->prechase_ps.pmove.delta_angles[PITCH] = ANGLE2SHORT(ent->client->ps.viewangles[PITCH] - ent->client->resp.cmd_angles[PITCH] + 10);
					ent->client->prechase_ps.pmove.delta_angles[YAW] = ANGLE2SHORT(ent->client->ps.viewangles[YAW] - ent->client->resp.cmd_angles[YAW]);
				}

				ent->client->ps = ent->client->prechase_ps;
			}
			ent->client->resp.scoreboard_frame = 0;
		}//end snap
		if (ent->solid != SOLID_NOT || ent->client->chase_target->solid == SOLID_NOT)
		{	// stop chasing
			ChaseStop(ent);
		}
		else
		{
			if (ent->client->chasemode != FREE_CHASE)
				return;
			goto chasing;
		}
	}

	if (ent->flags & FL_CHASECAM)
	{
		client->ps.pmove.pm_flags |= PMF_CHASECAM;
	}
	else
	{
		client->ps.pmove.pm_flags &= ~PMF_CHASECAM;
	}

	if (ent->movetype == MOVETYPE_NOCLIP)
		client->ps.pmove.pm_type = PM_SPECTATOR;

    else if (ent->movetype == MOVETYPE_SPECTATOR)//FREDZ example some kind spec without noclip, just anoying sounds and movement abit weird.
    {
        client->ps.pmove.pm_type = PM_NORMAL_WITH_JETPACK;
    }


	// Ridah, Hovercars
	else if (ent->flags & FL_HOVERCAR)
	{
		ent->viewheight = 0;
		client->ps.pmove.pm_type = PM_HOVERCAR;

		ent->s.renderfx |= RF_REFL_MAP;		// FIXME: remove this once this flag is set in .mdx
	}
	else if (ent->flags & FL_HOVERCAR_GROUND)
	{
		ent->viewheight = 0;
		client->ps.pmove.pm_type = PM_HOVERCAR_GROUND;

		ent->s.renderfx |= RF_REFL_MAP;		// FIXME: remove this once this flag is set in .mdx
	}
	else if (ent->flags & FL_BIKE)
	{
		client->ps.pmove.pm_type = PM_BIKE;

		ent->s.renderfx |= RF_REFL_MAP;		// FIXME: remove this once this flag is set in .mdx

		if ((client->latched_buttons & BUTTON_ACTIVATE) && (ent->duration < level.time))
		{	// Thruster
			VectorScale( ent->velocity, 2, ent->velocity );
			ent->duration = level.time + 4;

			client->kick_angles[PITCH] = -20;

			gi.cprintf( ent, PRINT_HIGH, "Sound Todo: Thruster\n");
		}

		VectorCopy( ent->velocity, bike_premove_vel );
	}
	else if (ent->flags & FL_CAR)
	{
		// Cars don't use client-side prediction

		client->ps.pmove.pm_type = PM_CAR;
		client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;

		ent->s.renderfx |= RF_REFL_MAP;		// FIXME: remove this once this flag is set in .mdx

		// Set the pmove up as usual..

		client->ps.pmove.gravity = sv_gravity->value;
		pm.s = client->ps.pmove;

		if (memcmp(&client->old_pmove, &pm.s, sizeof(pm.s)))
		{
			pm.snapinitial = true;
		}

		pm.cmd = *ucmd;

		pm.trace = PM_trace;	// adds default parms

		pm.pointcontents = gi.pointcontents;

		// do controls, then get outta here

		Veh_ProcessFrame( ent, ucmd, &pm );

		goto car_resume;
	}
	// done.

	else if (ent->s.modelindex != MAX_MODELS-1)
		client->ps.pmove.pm_type = PM_GIB;
	else if (ent->deadflag)
		client->ps.pmove.pm_type = PM_DEAD;
	else
	{

		if (ent->flags & FL_JETPACK)
		{
			client->ps.pmove.pm_type = PM_NORMAL_WITH_JETPACK;	// Ridah, debugging
			gi.dprintf( "SOUND TODO: Jet Pack firing\n" );
			ent->s.sound = gi.soundindex("weapons/flame_thrower/flamepilot.wav");	// this should go into G_SetClientSound()
		}
		else
		{
			client->ps.pmove.pm_type = PM_NORMAL;
		}

	}

chasing:

	client->ps.pmove.gravity = sv_gravity->value;
	pm.s = client->ps.pmove;

	for (i=0 ; i<3 ; i++)
	{
		pm.s.origin[i] = ent->s.origin[i]*8;
		// MH: make sure the velocity can overflow ("trigger_push" may exceed the limit) for back-compatibility (newer GCC may otherwise give 0x8000)
		pm.s.velocity[i] = (int)(ent->velocity[i]*8);
	}

	if (memcmp(&client->old_pmove, &pm.s, sizeof(pm.s)))
	{
		pm.snapinitial = true;
//		gi.dprintf ("pmove changed!\n");
	}

#if 0
	// set run speed scale
	if (deathmatch->value)
	{
		if (sv_runscale->value > 2.0)
			gi.cvar_set ("sv_runscale", "2.0");
		else if (sv_runscale->value < 0.1)
			gi.cvar_set ("sv_runscale", "0.1");

		pm.s.runscale = 128 + (byte)(127.0 * (sv_runscale->value - 1.0));
	}
	else
	{
		pm.s.runscale = 128;
	}
#endif

	pm.cmd = *ucmd;

	// MH: player movement disabled by a trigger_push
	if (client->nomove && pm.s.pm_type < PM_SPECTATOR)
		pm.cmd.forwardmove = pm.cmd.sidemove = pm.cmd.upmove = 0;

	pm.trace = PM_trace;	// adds default parms

	pm.pointcontents = gi.pointcontents;

	// perform a pmove
	gi.Pmove (&pm);

	// save results of pmove
	client->ps.pmove = pm.s;
	client->old_pmove = pm.s;

	// JOSEPH 1-SEP-98
	ent->footsteptype = pm.footsteptype;

	for (i=0 ; i<3 ; i++)
	{
		ent->s.origin[i] = pm.s.origin[i]*0.125;
		ent->velocity[i] = pm.s.velocity[i]*0.125;
	}

	VectorCopy (pm.mins, ent->mins);
	VectorCopy (pm.maxs, ent->maxs);

	client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
	client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
	client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

	// Ridah, Hovercars
	if (!(ent->flags & (FL_HOVERCAR | FL_HOVERCAR_GROUND)))
	// done.
	if ((ent->groundentity && !pm.groundentity && (pm.cmd.upmove >= 10) && (pm.waterlevel == 0)) && !PM_NORMAL_WITH_JETPACK)
	{
		int rval;
		rval = rand()%100;
		if (rval > 66)
			gi.sound(ent, CHAN_VOICE, gi.soundindex("*jump1.wav"), 1, ATTN_NORM, 0);
		else if (rval > 33)
			gi.sound(ent, CHAN_VOICE, gi.soundindex("*jump2.wav"), 1, ATTN_NORM, 0);
		else
			gi.sound(ent, CHAN_VOICE, gi.soundindex("*jump3.wav"), 1, ATTN_NORM, 0);

		PlayerNoise(ent, ent->s.origin, PNOISE_SELF);

// BEGIN Snap, bunnyhop
		//store this jump frame
		ent->jump_framenum = level.framenum;
	}
	if (!ent->groundentity && pm.groundentity) // client landing
		ent->land_framenum = level.framenum;
	if (((int)dmflags->value&DF_NO_BUNNY) && ent->land_framenum==ent->jump_framenum) {  // they did a doublejump
		if (ent->strafejump_count == 0)
			ent->firstjump_frame = level.framenum;
		ent->strafejump_count++;
		ent->land_framenum--;  // so they wont be equal
		if (ent->strafejump_count == 2) {
			if (ent->firstjump_frame >= level.framenum - 50) {  // they are bunnyhopping
				float xyspeed=sqrt(ent->velocity[0]*ent->velocity[0] + ent->velocity[1]*ent->velocity[1]);
				if (xyspeed>300.0) {              // correct their speed back down to 'normal'
					ent->velocity[0]*=300.0/xyspeed;
					ent->velocity[1]*=300.0/xyspeed;
				}
			}
			ent->strafejump_count = 0;
		}
	}
// END Snap

#if !DEMO
	// bikestuff
	if (ent->flags & (FL_BIKE) || ent->flags & (FL_HOVERCAR | FL_HOVERCAR_GROUND) )
	{

		int		oldbikestate;
		qboolean accel = false;
		static  int bikegear = 0;
		float	xyspeed;
		static	float	old_xyspeed;
		vec3_t	xyvel;

		if (ent->flags & FL_BIKE)
		{
			vec3_t diffvec;
			float	difflength, prelength;

			VectorSubtract( bike_premove_vel, ent->velocity, diffvec );

			difflength = VectorLength( diffvec );
			prelength = VectorLength( bike_premove_vel );

			if (	((prelength > 300) && (difflength >= 300)))
//				||	((VectorLength( bike_premove_vel ) > 300) && (DotProduct(bike_premove_vel, ent->velocity) < 0)))
			{
				gi.dprintf( "SOUND TODO: CRASH!\n" );
			}
			else if (pm.wall_collision)
			{
				gi.dprintf( "SOUND TODO: Scraped wall\n");
			}
		}

		VectorCopy( ent->velocity, xyvel );
		xyvel[2] = 0;

		xyspeed = VectorLength( xyvel );

		oldbikestate = ent->bikestate;

		if (ucmd->forwardmove > 0 && ((old_xyspeed < xyspeed) || xyspeed>50))
		{
			//gi.dprintf ("ACCEL: %5.3f\n", xyspeed);
			accel = true;
			ent->bikestate = 2;
		}
		else
		{
			//gi.dprintf ("NO ACCEL: %5.3f\n", xyspeed);
			if (ent->bikestate == 2)
				ent->bikestate = 1;
			else if (ent->bikestate == 1)
			{
				if (xyspeed < 100)
					ent->bikestate = 0;
			}
		}

		// need a state change check

		if (ent->biketime < level.time || oldbikestate != ent->bikestate)
		{
			if (xyspeed < 400 && (accel == false))
			{
				if ((bikegear <= 1) || ent->biketime < level.time)
				{
					gi.sound ( ent, CHAN_VOICE, gi.soundindex ("motorcycle/idle.wav"), 0.5, ATTN_NORM, 0);
					ent->s.sound = 0;
					ent->biketime = level.time + 2.4;
				}

				bikegear = 0;
			}
			else
			{
				if (accel)
				{
					bikegear = (int)floor((xyspeed+100) / 280);

					if (oldbikestate == 0 || bikegear == 0)
					{
						gi.sound ( ent, CHAN_VOICE, gi.soundindex ("motorcycle/accel1.wav"), 1, ATTN_NORM, 0);
						ent->s.sound = 0;
						ent->biketime = level.time + 1.8;
						bikegear = 1;
					}
					else
					{
						if (bikegear == 1)
						{
							gi.sound ( ent, CHAN_VOICE, gi.soundindex ("motorcycle/accel2.wav"), 1, ATTN_NORM, 0);
							ent->s.sound = 0;
							ent->biketime = level.time + 2.4;
						}
						else if (bikegear == 2)
						{
							gi.sound ( ent, CHAN_VOICE, gi.soundindex ("motorcycle/accel3.wav"), 1, ATTN_NORM, 0);
							ent->s.sound = 0;
							ent->biketime = level.time + 2.4;
						}
/*
						else if (bikegear == 3)
						{
							gi.sound ( ent, CHAN_VOICE, gi.soundindex ("motorcycle/accel4.wav"), 1, ATTN_NORM, 0);
							ent->biketime = level.time + 2.1;
						}
*/
						else	// TODO: high speed rev (looped)
						{
//							gi.sound ( ent, CHAN_VOICE, gi.soundindex ("motorcycle/running.wav"), 1, ATTN_NORM, 0);
							ent->s.sound = gi.soundindex ("motorcycle/running.wav");
							ent->biketime = level.time + 9999;
							ent->volume = 1.0;
						}
/*
						bikegear++;
						if (bikegear >= 3)
							bikegear = 3;
*/
					}
				}
				else
				{
					ent->s.sound = 0;
					gi.sound ( ent, CHAN_VOICE, gi.soundindex ("motorcycle/decel.wav"), 1, ATTN_NORM, 0);

					bikegear--;
					if (bikegear > 0 && xyspeed > 100)
					{
						ent->biketime = level.time + 0.7 - (0.2 * bikegear);
						bikegear = 0;		// only do this short one once
					}
					else
					{
						bikegear = 0;
						ent->biketime = level.time + 2.4;
					}
				}
			}
		}

		old_xyspeed = xyspeed;
	}
#endif // DEMO

	ent->viewheight = pm.viewheight;
	ent->waterlevel = pm.waterlevel;
	ent->watertype = pm.watertype;
	ent->groundentity = pm.groundentity;
	if (pm.groundentity)
	{
		ent->groundentity_linkcount = pm.groundentity->linkcount;

		// if standing on an AI, get off
		if (pm.groundentity->svflags & SVF_MONSTER)
		{
			VectorSet( ent->velocity, rand()%400 - 200, rand()%400 - 200, 200 );

			if (pm.groundentity->maxs[2] == pm.groundentity->cast_info.standing_max_z)
			{	// duck
				if (pm.groundentity->cast_info.move_crouch_down)
					pm.groundentity->cast_info.currentmove = pm.groundentity->cast_info.move_crouch_down;
				pm.groundentity->maxs[2] = DUCKING_MAX_Z;
			}

			// avoid
			pm.groundentity->cast_info.avoid( pm.groundentity, ent, false );

		}

		// MH: re-enable player movement if it has been disabled by a trigger_push
		client->nomove = false;
	}

	if (ent->deadflag)
	{
		client->ps.viewangles[ROLL] = 40;
		client->ps.viewangles[PITCH] = -15;
		client->ps.viewangles[YAW] = client->killer_yaw;
	}
	else
	{
		VectorCopy (pm.viewangles, client->v_angle);
		VectorCopy (pm.viewangles, client->ps.viewangles);
	}

	gi.linkentity (ent);

	if (ent->movetype != MOVETYPE_NOCLIP && ent->movetype != MOVETYPE_SPECTATOR) //stop PLAYER_READY picking up items
		G_TouchTriggers (ent);

	// touch other objects
	for (i=0 ; i<pm.numtouch ; i++)
	{
		other = pm.touchents[i];
		for (j=0 ; j<i ; j++)
			if (pm.touchents[j] == other)
				break;
		if (j != i)
			continue;	// duplicated

		if (!other->touch)
			continue;
		other->touch (other, ent, NULL, NULL);
	}

    //they shoot...they are mortal
    if (((client->latched_buttons|client->buttons) & BUTTON_ATTACK)
		&& (client->invincible_framenum<level.framenum+29))
        client->invincible_framenum = 0;

    // JOSEPH 22-JAN-99
	// Activate button is pressed
	if (((client->latched_buttons|client->buttons) & BUTTON_ACTIVATE))
	{
		edict_t		*trav, *best;
		float		best_dist=9999, this_dist;

		// find the nearest pull-enabled object
		trav = best = NULL;
		while (trav = findradius( trav, ent->s.origin, 48 ))
		{
			if (!trav->pullable)
				continue;
			//if (!infront(ent, trav))
			//	continue;
			//if (!visible(ent, trav))
			//	continue;
			if (((this_dist = VectorDistance(ent->s.origin, trav->s.origin)) > best_dist) && (this_dist > 64))
				continue;

			best = trav;
			best_dist = this_dist;
		}

		// If we find something to drag
		if (best)
		{
			cplane_t plane;

			plane.type = 123;
			best->touch (best, ent, &plane, NULL);

			// Slow down the player
			// JOSEPH 24-MAY-99
			ent->velocity[0] /= 8;
			ent->velocity[1] /= 8;
			// END JOSEPH
		}
	}
	// END JOSEPH

#if !DEMO
car_resume:
#endif

	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons |= client->buttons & ~client->oldbuttons;

	// save light level the player is standing on for
	// monster sighting AI
	ent->light_level = ucmd->lightlevel;


	// fire weapon from final position if needed
	if (client->latched_buttons & BUTTON_ATTACK)
	{
		if (!client->weapon_thunk)
		{
			client->weapon_thunk = true;
			Think_Weapon(ent);
		}
	}

	Think_FlashLight (ent);

	// BEGIN:	Xatrix/Ridah/Navigator/18-mar-1998
	if (/*!deathmatch->value &&*/ nav_dynamic->value && !(ent->flags & (FL_HOVERCAR_GROUND | FL_HOVERCAR | FL_BIKE | FL_CAR | FL_JETPACK))
		&& level.modeset == WAVE_ACTIVE	&& ent->client && ent->client->pers.spectator == PLAYING && ent->solid == SOLID_BBOX &&!ent->deadflag) //hypov8 nav?
	{
		static float alpha;

		// check for nodes
		NAV_EvaluateMove( ent );

		// optimize routes (flash screen if lots of optimizations
		if (NAV_OptimizeRoutes( ent->active_node_data ) > 50)
		{
			alpha += 0.05;
			if (alpha > 1)
				alpha = 1;
		}
		else if (alpha > 0)
		{
			alpha -= 0.05;
		}

		if (nav_debug->value)
			ent->client->bonus_alpha = alpha;
	}
	// END:		Xatrix/Ridah/Navigator/18-mar-1998

	// Ridah, new AI
	if (maxclients->value == 1)
	{
		AI_UpdateCharacterMemories( 16 );
	}
	// done.

	// Ridah, special burn surface code for artists
	if ((maxclients->value == 1) && burn_enabled->value)
	{
		static vec3_t	last_endpos;

		if (!(client->buttons & BUTTON_ATTACK))
		{	// next press must draw, since they've just hit the attack button
			last_endpos[0] = -9999;
			last_endpos[1] = -9999;
			last_endpos[2] = -9999;
		}
		else if (num_lpbuf >= 0xFFFF)
		{
			gi.dprintf( "LightPaint buffers are full, you must save to continue painting.\n");
		}
		else
		{
			trace_t tr;
			vec3_t	start, end, fwd;

			VectorCopy( ent->s.origin, start );
			start[2] += ent->viewheight;

			AngleVectors( ent->client->v_angle, fwd, NULL, NULL );

			VectorMA( start, 4000, fwd, end );

			tr = gi.trace( start, NULL, NULL, end, ent, (MASK_OPAQUE & ~CONTENTS_MONSTER) );

			if (tr.fraction < 1 && (VectorDistance( last_endpos, tr.endpos ) > ((float)burn_size->value)*0.5))
			{
				VectorMA( tr.endpos, 1, tr.plane.normal, last_endpos );

				// spawn a burn ent at this location
				gi.WriteByte (svc_temp_entity);
				gi.WriteByte (TE_ART_BURN);
				gi.WritePosition (last_endpos);

				gi.WriteShort( (int)burn_size->value );

				gi.WriteByte ( (int) (255.0 * (float)burn_r->value) );
				gi.WriteByte ( (int) (255.0 * (float)burn_g->value) );
				gi.WriteByte ( (int) (255.0 * (float)burn_b->value) );

				gi.WriteByte ( (int) (127.0 * (float)burn_intensity->value) + 127 );

				gi.multicast (ent->s.origin, MULTICAST_ALL);

				// record this, so we can save them to a file
				lpbuf[num_lpbuf] = malloc( LP_SIZE );

				memcpy( lpbuf[num_lpbuf], last_endpos, 12 );

				*((short *) (lpbuf[num_lpbuf]+12)) = (short) burn_size->value;
				*(lpbuf[num_lpbuf]+14) = (unsigned char) (255.0 * (float)burn_r->value);
				*(lpbuf[num_lpbuf]+15) = (unsigned char) (255.0 * (float)burn_g->value);
				*(lpbuf[num_lpbuf]+16) = (unsigned char) (255.0 * (float)burn_b->value);
				*(lpbuf[num_lpbuf]+17) = (unsigned char) ((127.0 * (float)burn_intensity->value) + 127.0);

				num_lpbuf++;
			}
		}
	}
}


/*
==============
ClientBeginServerFrame

This will be called once for each server frame, before running
any other entities in the world.
==============
*/
void ClientBeginServerFrame (edict_t *ent)
{
	gclient_t	*client;
	int			buttonMask;

	if (ent->kickdelay)
	{
        if (!--ent->kickdelay)
		{
			gi.bprintf(PRINT_HIGH,ent->kickmess,ent->client->pers.netname);
			gi.AddCommandString(va("kick %i\n", (int)(ent - g_edicts - 1)));
		}
		return;
	}

	if (ent->moveout_ent && ent->moveout_ent->health <= 0)
	{
		ent->moveout_ent = NULL;
	}

	if (level.intermissiontime)
		return;

	// RAFAEL - tical commented out
	if (level.cut_scene_time)
    	return;

	if (level.pawn_time)
		return;

/*
	if (teamplay->value && !ent->client->pers.team && (ent->movetype == MOVETYPE_NOCLIP) && ((int)(level.time*10)%10 == 0))
	{
		gi.centerprintf( ent, "--------------------------------------------------------\n\nYou are a spectator!\n\nPress the corresponding number\nto join a team.\n\nValid teams are:\n\n%12s - 1\n%12s - 2\n\n--------------------------------------------------------\n", team_names[1], team_names[2] );
	}
*/

	client = ent->client;
#ifndef HYPODEBUG //allow debug builds to not kick you

	// MH: check if they're lagged-out
	if (client->pers.spectator != SPECTATING && curtime-client->pers.lastpacket >= 5000)
	{
		// 5 seconds since last contact from the client
		gi.bprintf(PRINT_HIGH, "%s has lost contact with the server\n", client->pers.netname);
		// make them a spectator
		Cmd_Spec_f(ent);
	}


	//hypov8 todo: this needs some thought...
	//
	// MH: moved idle checks here from ClientThink
     //check if idle
    if (ent->client->pers.spectator == PLAYING && ((level.modeset==WAVE_ACTIVE) || (level.modeset == WAVE_BUYZONE)))
    {
       /* if(((level.framenum - ent->check_idle)>(idle_client->value*10))) // MH: check_talk/shoot removed (included in check_idle)
        {
			gi.bprintf (PRINT_HIGH, "%s has been idle for over %d seconds\n", client->pers.netname, (int)idle_client->value); // MH: let everyone know they've idled-out
            //make them spectators
            Cmd_Spec_f(ent);
        }
        else*/ if (((level.framenum - ent->check_idle)>450) && (level.modeset == WAVE_BUYZONE))//45 secs in buyzone
        {
			gi.bprintf (PRINT_HIGH, "%s has been idle for over %d seconds\n", client->pers.netname, 45);
            //make them spectators
            Cmd_Spec_f(ent);
        }
    }

	// MH: count play time
	if (ent->client->pers.spectator == PLAYING && ((level.modeset==WAVE_ACTIVE) || (level.modeset == WAVE_BUYZONE)))
		ent->client->resp.time++;

#endif
	// Ridah, hack, make sure we duplicate the episode flags
	ent->episode_flags |= ent->client->pers.episode_flags;
	ent->client->pers.episode_flags |= ent->episode_flags;

	// run weapon animations if it hasn't been done by a ucmd_t
	if (!client->weapon_thunk)
		Think_Weapon (ent);
	else
		client->weapon_thunk = false;

	Think_FlashLight (ent);

	if (ent->deadflag)
	{
		// MH: don't respawn if lagging-out
		if (curtime - ent->client->pers.lastpacket > 100)
			return;

		// wait for any button just going down
		if ( level.time > client->respawn_time)
		{
		    //FREDZ make the person that dies a spectator, but still on the team
            if(level.modeset == WAVE_ACTIVE)
			{
				ent->flags &= ~FL_GODMODE;
				ent->health = 0;
				meansOfDeath = MOD_RESTART;
				//ent->client->pers.weaponStore = ent->client->pers.weapon;
				ClientBeginDeathmatch( ent );
				return;
			}

			// in deathmatch, only wait for attack button
			if (deathmatch->value)
				buttonMask = BUTTON_ATTACK;
			else
				buttonMask = -1;

			if ( ( client->latched_buttons & buttonMask ) ||
				(deathmatch->value && ((int)dmflags->value & DF_FORCE_RESPAWN) ) )
			{
				respawn(ent);
				client->latched_buttons = 0;
			}
		}
		return;
	}

// BEGIN: Xatrix/Ridah/Navigator/16-apr-1998
	if (/*!deathmatch->value &&*/ !ent->nav_build_data && nav_dynamic->value) //hypov8 nav init
	{
		// create the nav_build_data structure, so we can begin dropping nodes
		ent->nav_build_data = gi.TagMalloc(sizeof(nav_build_data_t), TAG_LEVEL);
		memset(ent->nav_build_data, 0, sizeof(ent->nav_build_data));

		ent->nav_build_data->jump_ent = G_Spawn();
		VectorCopy(ent->maxs, ent->nav_build_data->jump_ent->maxs );
		VectorCopy(ent->mins, ent->nav_build_data->jump_ent->mins );
	}
// END:		Xatrix/Ridah/Navigator/16-apr-1998

	// BEGIN:	Xatrix/Ridah/Navigator/23-mar-1998
	// show the debug path
	{
		extern int		showpath_on;
		extern edict_t	*showpath_ent;

		if (showpath_on)
		{
			NAV_Debug_DrawPath(ent, showpath_ent);
		}
	}
	// END:		Xatrix/Ridah/Navigator/23-mar-1998

	client->latched_buttons = 0;

	if (!(ent->flags & FL_JETPACK))
	{
		ent->client->jetpack_warned = false;

		if (ent->client->jetpack_power < 15.0)
			ent->client->jetpack_power += 0.05;
	}
	else
	{
		ent->client->jetpack_power -= 0.1;

		if (ent->client->jetpack_power <= 0.0)
		{	// disable the jetpack
			gitem_t	*jetpack;

			jetpack = FindItem("Jet Pack");
			jetpack->use( ent, jetpack );
		}
		else if (!ent->client->jetpack_warned && ent->client->jetpack_power < 5.0)
		{
			ent->client->jetpack_warned = true;
			gi.cprintf( ent, PRINT_HIGH, "SOUND TODO: WARNING: Jet Pack power is LOW\n");
		}
	}

	//scroll menu update keys
	if (ent->client->showscrollmenu)
		ScrollMenuKeyLogger(ent);

}

#if 1 //mm2.0
static int CheckClientRejoin(edict_t *ent)
{
	int i, playernum = ent - g_edicts - 1;
	char s[MAX_QPATH];

	strcpy(s, level.playerskins[playernum]);
	/*if (teamplay->value)
	{
		char *p = strrchr(s, '/' );
		memset(p+5, ' ', 7); // ignore body+legs
	}*/
	for (i=level.player_num-1; i>=0; i--)
	{
		if (!strcmp(s, playerlist[i].player)) //player
			break;
	}
	return i;
}
void ClientRejoin(edict_t *ent, qboolean rejoin)
{
	int	i, index;

	index = CheckClientRejoin(ent);
	if (rejoin && index >= 0)
	{
		ent->client->resp.score = playerlist[index].frags;
		ent->client->resp.deposited = playerlist[index].deposits;
		ent->client->resp.stole = playerlist[index].stole;
		ent->client->resp.acchit = playerlist[index].acchit;
		ent->client->resp.accshot = playerlist[index].accshot;
		for (i = 0; i<8; i++)
			ent->client->resp.fav[i] = playerlist[index].fav[i];
		ent->client->pers.team = playerlist[index].team;
		ent->client->resp.time = playerlist[index].time;
		/*if (ent->client->pers.team || !teamplay->value)
		{
			ent->client->pers.spectator = PLAYING;
			if (ent->client->resp.enterframe != level.framenum)
			{
				if (teamplay->value)
					Teamplay_ValidateJoinTeam(ent, ent->client->pers.team);
				else
					ClientBeginDeathmatch(ent);
			}
		}
		else*/
		{
			ent->client->showscores = SCOREBOARD;
			ent->client->resp.scoreboard_frame = 0;
		}
	}
	else
	{
		/*if (teamplay->value)
		{
			ent->client->showscores = SCOREBOARD;
			if (((int)dmflags->value & DF_AUTO_JOIN_TEAM) && !level.intermissiontime && level.modeset != MATCH)
				Teamplay_AutoJoinTeam(ent);
		}
		else*/
		{
			ent->client->showscores = NO_SCOREBOARD;
			ent->client->pers.spectator = PLAYER_READY; //was PLAYING
			if (ent->client->resp.enterframe != level.framenum)
				ClientBeginDeathmatch(ent);
		}
		ent->client->resp.scoreboard_frame = 0;
	}
	if (index >= 0)
	{
		level.player_num--;
		memmove(playerlist + index, playerlist + index + 1, (level.player_num - index) * sizeof(playerlist[0]));
	}
}
#endif

// MH: cprintf removed (switched back to using gi.cprintf)

// MH: drop cash
/*
void DropCash(edict_t *self)
{
	edict_t *cash;

	//hypov8 do we want to drop cash of dead players?

	if (self->client->pers.currentcash)
	{
		cash = SpawnTheWeapon( self, "item_cashroll" );
		cash->currentcash = self->client->pers.currentcash;
		self->client->pers.currentcash = 0;

		cash->velocity[0] = crandom() * 100;
		cash->velocity[1] = crandom() * 100;
		cash->velocity[2] = 0;

		VectorNormalize( cash->velocity );
		VectorScale( cash->velocity, 100, cash->velocity );
		cash->velocity[2] = 300;
	}

	if (self->client->pers.bagcash)
	{
		if (self->client->pers.bagcash > 100)
			cash = SpawnTheWeapon( self, "item_cashbaglarge" );
		else
		{
			cash = SpawnTheWeapon( self, "item_cashbagsmall" );
		}

//		cash->nextthink = level.time + 120;

		cash->currentcash = -self->client->pers.bagcash;
		self->client->pers.bagcash = 0;

		cash->velocity[0] = crandom() * 100;
		cash->velocity[1] = crandom() * 100;
		cash->velocity[2] = 0;

		VectorNormalize( cash->velocity );
		VectorScale( cash->velocity, 100, cash->velocity );
		cash->velocity[2] = 300;
	}
}
*/
