#include "g_local.h"

#define ENABLE_INDEX_NAMES		1

extern edict_t *boss_entityID;
extern int boss_maxHP;

static const char *gameheader[] =
{
	GAMEVERSION " By FREDZ and Hypov8",
	"with parts from Monkey Mod v2.0",
	"www.kingpin.info",
	NULL
};

// MH: copied from MM2 with hitmen support added
int GetGameModeMessage(char *entry, int yofs)
{
	const char *p;

/*	if ((int)teamplay->value == 1)
	{
		if (hitmen->value)
			p = "This server is running Hitmen Bagman";
		else if (dm_realmode->value)
			p = "This server is running Realmode Bagman";
		else
			p = "This server is running Bagman";
	}
	else if ((int)teamplay->value == 4)
	{
		if (hitmen->value)
			p = "This server is running Hitmen Team Deathmatch";
		else if (dm_realmode->value)
			p = "This server is running Realmode Team Deathmatch";
		else
			p = "This server is running Team Deathmatch";
	}
	else*/
	{

		if (dm_realmode->value)
			p = "This server is running Realmode Deathmatch";
		else
			p = "This server is running Standard Deathmatch";
	}
	sprintf (entry, "xm %i yv %i dmstr 874 \"%s\" ",
		-5*strlen(p), yofs + -60-49, p);
	yofs += 20;

	if (level.modeset == PREGAME)
		p = "Wait until server is ready.";
	else if (level.modeset == WAVE_IDLE)
		p = "in setup mode. please join.";
    else if (level.modeset == WAVE_BUYZONE)
		p = "in buy mode. please join.";
	else if (level.modeset == WAVE_START)
		p = "and a game is about to start.";
	else
		p = "in game mode.";
	sprintf (entry+strlen(entry), "xm %i yv %i dmstr 874 \"%s\" ",
		-5*strlen(p), yofs + -60-49, p);
	yofs += 20;

	if (level.modeset == PREGAME)
	{
		sprintf (entry+strlen(entry), "xm %i yv %i dmstr 874 \"The game will start shortly.\" ",
			-5*28, yofs + -60-49);
		yofs += 20;
	}

	{
		edict_t	*admin = GetAdmin();
		if (admin || level.modeset == WAVE_IDLE)
		{
			char temp[128];
			if (admin)
				sprintf (temp, "Your admin is %s", admin->client->pers.netname);
			else
				strcpy (temp, "No one currently has admin");
			sprintf (entry+strlen(entry), "xm %i yv %i dmstr 874 \"%s\" ",
				-5*strlen(temp), yofs + -60-49, temp );
			yofs += 20;
		}
	}

	yofs += 10;
	return yofs;
}

// MH: copied from MM2
void GetChaseMessage(edict_t *ent, char *entry)
{
	entry[0]=0;
/*	if (level.modeset==MATCH && no_spec->value && !ent->client->pers.admin && !ent->client->pers.rconx[0])
	{
		strcpy(entry, "xm -110 yb -50 dmstr 773 \"spectating is disabled\" ");
	}*/
	if (!ent->client->chase_target)
//	else if (!ent->client->chase_target)
	{
		strcpy(entry, "xm -105 yb -50 dmstr 773 \"hit ACTIVATE to chase\" ");
	}
	else if (ent->client->chase_target->client)
	{
		if(ent->client->chasemode == FREE_CHASE)
			sprintf(entry, "xm -160 yb -64 dmstr 773 \"chasing %s in freelook mode\" xm -225 yb -40 dmstr 552 \"[ and ] cycles, JUMP changes mode, ACTIVATE quits\" ",
				ent->client->chase_target->client->pers.netname);
		else if(ent->client->chasemode == EYECAM_CHASE)
			sprintf(entry, "xm -150 yb -64 dmstr 773 \"chasing %s in eyecam mode\" xm -225 yb -40 dmstr 552 \"[ and ] cycles, JUMP changes mode, ACTIVATE quits\" ",
				ent->client->chase_target->client->pers.netname);
		else //if(ent->client->chasemode == LOCKED_CHASE)
			sprintf(entry, "xm -150 yb -64 dmstr 773 \"chasing %s in locked mode\" xm -225 yb -40 dmstr 552 \"[ and ] cycles, JUMP changes mode, ACTIVATE quits\" ",
				ent->client->chase_target->client->pers.netname);
	}
}

static const char *weapnames[8] = { "Pipe", "Pist", "Shot", "Tomm", "HMG", "GL", "RL", "Flam" }; // MH: modified to fit 4 chars


/*
======================================================================

INTERMISSION

======================================================================
*/

void MoveClientToIntermission (edict_t *ent)
{
#if 1 //mm 2.0
	if (ent->client->showscores == SCORE_REJOIN)
		ClientRejoin(ent, true); // auto-rejoin
#endif

	if (deathmatch->value || coop->value)
		ent->client->showscores = SCOREBOARD;
	VectorCopy (level.intermission_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.intermission_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.intermission_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.intermission_origin[2]*8;
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pmove.pm_type = PM_FREEZE;
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	// RAFAEL
	ent->client->quadfire_framenum = 0;

	// RAFAEL
	ent->client->trap_blew_up = false;
	ent->client->trap_time = 0;

	ent->viewheight = 0;
	ent->s.modelindex = 0;
//	ent->s.modelindex2 = 0;
//	ent->s.modelindex3 = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;

	ent->client->chase_target = 0; // MH: stop chasing
	ent->vote = 0; // MH: reset vote

	// add the layout

	if (deathmatch->value || coop->value)
	{
		ent->client->resp.scoreboard_frame = 0; // MH: trigger scoreboard update instead of calling DeathmatchScoreboard
	}

}


void BeginIntermission (edict_t *targ, char *changenext)
{
	int		i, n;
	edict_t	*ent, *client;

	if (level.intermissiontime)
		return;		// already activated

	game.autosaved = false;

	// respawn any dead clients
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		if (client->health <= 0)
			respawn(client);
	}

	level.intermissiontime = level.time;
	level.changemap = (char *)gi.TagMalloc(MAX_QPATH, TAG_LEVEL); // MH: using gi.TagMalloc instead of malloc to fix small leak

	//level.changemap = targ->map;
	strcpy (level.changemap, changenext);
	//gi.bprintf (PRINT_HIGH, "2 %s\n", level.changemap);

	//gi.bprintf (PRINT_HIGH, "Next map will be: %s,%s.\n", level.changemap, targ->map);

	if (strstr(level.changemap, "*"))
	{
		//if (coop->value)
		{
			for (i=0 ; i<maxclients->value ; i++)
			{
				client = g_edicts + 1 + i;
				if (!client->inuse)
					continue;
				// strip players of all keys between units
				for (n = 0; n < MAX_ITEMS; n++)
				{
					if (itemlist[n].flags & IT_FLASHLIGHT)
						continue;

					if (itemlist[n].flags & IT_KEY)
						client->client->pers.inventory[n] = 0;
				}

				client->episode_flags = client->client->pers.episode_flags = 0;
				client->client->pers.friends = 0;
				level.helpchange = 0;
			}
		}

		// Dan: this should eliminate the need to press a key to exit the end of episode
		if (!deathmatch->value)
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}
	else
	{
		if (!deathmatch->value)
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}

	level.exitintermission = 0;

	// find an intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = G_Find (NULL, FOFS(classname), "info_player_start");
		if (!ent)
			ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		i = rand() & 3;
		while (i--)
		{
			ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = G_Find (ent, FOFS(classname), "info_player_intermission");
		}
	}

	VectorCopy (ent->s.origin, level.intermission_origin);
	VectorCopy (ent->s.angles, level.intermission_angle);

	// move all clients to the intermission point
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		MoveClientToIntermission (client);
	}

	// MH: stop any looping sounds (for intermission music)
	for (i=0 ; i<globals.num_edicts ; i++)
	{
		ent = g_edicts + i;
		ent->s.sound = 0;
	}


	// play a music clip
	gi.WriteByte(svc_stufftext);
//	gi.WriteString(va("play world/cypress%i.wav\n", 2+(rand()%4)));
	gi.WriteString("play world/cypress2.wav\n");
	gi.multicast(vec3_origin, MULTICAST_ALL);
}
//===================================================================
//
// Papa - The following are the various scoreboards that I use
//
//		phear id's confusing format - check qdevels on planetquake if
//		you don't understand this crap
//
//		one hint - dmstr xxx is the rgb color of the text :)
//
//===================================================================
//===================================================================

void SpectatorScoreboardMessage (edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j, k;
	edict_t	*player;
	char	*tag;
    char    status[10];

	string[0] = 0;
	stringlength = 0;

	Com_sprintf (entry, sizeof(entry),
		"xm %i yv %i dmstr 999 \"Spectators\" ",
		-5*10, -60-49);
	j = strlen(entry);
	strcpy (string + stringlength, entry);
	stringlength += j;

	Com_sprintf (entry, sizeof(entry),
		"xm %i yv %i dmstr 663 \"Status  NAME           ping  watching\" ",
		-5*28, -60-21);
	j = strlen(entry);
	strcpy (string + stringlength, entry);
	stringlength += j;

	for (k=i=0 ; i<maxclients->value ; i++)
	{
		player = g_edicts + 1 + i;
		if (!player->inuse || player->client->pers.spectator == PLAYING || player->client->pers.spectator == PLAYER_READY)
			continue;

		if (curtime - player->client->pers.lastpacket >= 5000)
        {
            tag = "666";
            strcpy(status,"connect");//CNCT shorter?
        }
		else if (player->client->pers.rconx[0])
        {
            tag = "096";
            strcpy(status,"rconx");
        }
		else if (player->client->pers.admin > NOT_ADMIN)
        {
            tag = "779";
            strcpy(status,"admin");
        }
		else if (player == ent)
        {
 			tag = "990";
 			strcpy(status,"");//you
        }
		else
        {
            tag = "999";	// fullbright
            strcpy(status,"");
        }

        /*
        		if (player->client->chase_target)
			Com_sprintf (entry, sizeof(entry), "yv %i dmstr %s \"%-13s %4i  %s\" ",
			-60+k*17, tag, player->client->pers.netname, player->client->ping, player->client->chase_target->client->pers.netname);
		else
			Com_sprintf (entry, sizeof(entry), "yv %i dmstr %s \"%-13s %4i\" ",
				-60+k*17, tag, player->client->pers.netname, player->client->ping);
            */

		if (player->client->chase_target)
			Com_sprintf (entry, sizeof(entry), "yv %i dmstr %s \"%-7s %-13s %4i %-13s\"",
			-60+k*17, tag, status, player->client->pers.netname, player->client->ping, player->client->chase_target->client->pers.netname);
		else
			Com_sprintf (entry, sizeof(entry), "yv %i dmstr %s \"%-7s %-13s %4i\"",
				-60+k*17, tag, status, player->client->pers.netname, player->client->ping);
		j = strlen(entry);
		if (stringlength + j >= 1024)
			break;
		strcpy (string + stringlength, entry);
		stringlength += j;
		k++;
	}

	if (k)
		k++;
/*
	for (i=0 ; i<maxclients->value ; i++)//Double now?
	{
		player = g_edicts + 1 + i;
		if (player->inuse || !player->client || !player->client->pers.connected || !(kpded2 || curtime - player->client->pers.lastpacket < 120000))
			continue;

		tag = "666";

		Com_sprintf (entry, sizeof(entry), "yv %i dmstr %s \"%-13s CNCT\" ",
			-60+k*17, tag, player->client->pers.netname);
		j = strlen(entry);
		if (stringlength + j >= 1024)
			break;
		strcpy (string + stringlength, entry);
		stringlength += j;
		k++;
	}*/

	if (level.modeset == ENDGAMEVOTE)
	{
		static const char *votenote = "xm -230 yb -40 dmstr 552 \"hit your scoreboard key (f1) for the vote menu\" ";
		j = strlen(votenote);
		if (stringlength + j < 1024)
		{
			strcpy (string + stringlength, votenote);
			stringlength += j;
		}
	}

	if (ent->client->pers.spectator == SPECTATING)// &&  ((level.modeset == WAVE_ACTIVE) || (level.modeset == WAVE_BUYZONE) || (level.modeset == PREGAME) || (level.modeset == WAVE_IDLE)))
	{
		GetChaseMessage(ent,entry);
		j = strlen(entry);
		if (stringlength + j < 1024)
		{
			strcpy (string + stringlength, entry);
			stringlength += j;
		}
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

//===================================================================
//===================================================================
//===================================================================

void VoteMapScoreboardMessage (edict_t *ent)
{
	char	entry[1024];
	char	temp[32];
	char	string[1400];
	int		stringlength;
	int		i, j;
	int		yofs;
	int		num_vote_set;
	int		count[9];
	edict_t *player;
	const char	*selectheader[] =
		{
			"Please vote your choice for the next map.",
			"Hit the corresponding number (weapon key)",
			"or use [ and ] to place your vote.",
			NULL
		};
	const char	*basechoice[] =
		{
			"1 (Pipe)     ",
			"2 (Pistol)   ",
			"3 (Shotgun)  ",
			"4 (Tommygun) ",
			"5 (HMG)      ",
			"6 (GL)       ",
			"7 (Bazooka)  ",
			"8 (FlameThr) ",
			NULL
		};

	if (num_custom_maps < 8 )
		num_vote_set = num_custom_maps;
	else
		num_vote_set = 8;

	string[0] = 0;
	stringlength = 0;
	yofs = 0;

	for (i=0; selectheader[i]; i++)
	{
		Com_sprintf (entry, sizeof(entry),"xm %i yv %i dmstr 863 \"%s\" ",
			-5*41, yofs + -60-49, selectheader[i] );
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
		yofs += 20;
	}
	yofs += 10;

	memset (&count, 0, sizeof(count));
	for_each_player(player,i)
	{
		count[player->vote]++;
	}
	if (ent->vote == 0)
		Com_sprintf (entry, sizeof(entry), "yv %i dmstr 999 \"-->      %d players have not voted\" ",
				yofs + -60-49, count[0]);
	else
		Com_sprintf (entry, sizeof(entry), "yv %i dmstr 777 \"         %d players have not voted\" ",
				yofs + -60-49, count[0]);
	j = strlen(entry);
	strcpy (string + stringlength, entry);
	stringlength += j;
	yofs += 30;

	for (i=1; i <= num_vote_set; i++)
	{
		if (count[i] == 1)
			strcpy (temp, "1 vote  -");
		else
			Com_sprintf (temp, sizeof(temp), "%d votes -",count[i]);
		if (ent->vote == i)
			Com_sprintf (entry, sizeof(entry), "yv %i dmstr 999 \"--> %s %s %s\" ",
					yofs + -60-49, basechoice[i-1],temp,custom_list[vote_set[i]].custom_map);
		else
			Com_sprintf (entry, sizeof(entry), "yv %i dmstr 777 \"    %s %s %s\" ",
					yofs + -60-49, basechoice[i-1],temp,custom_list[vote_set[i]].custom_map);
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
		yofs += 20;
	}

	//if client has voted for a map, print this maps levelshot
	if (ent->vote > 0)
	{
		yofs += 15;
		//print the pcx levelshot on screen
		//name of pcx must be same as bsp -- pcx files must be in pics dir
		Com_sprintf (entry, sizeof(entry),
			"xm %i yv %i picn %s ",
			-5*20, yofs + -60-49, custom_list[vote_set[ent->vote]].custom_map);
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
		//tical
	}

	{
		static const char *votenote = "xm -230 yb -40 dmstr 552 \"hit your scoreboard key (f1) for the scoreboard\" ";
		j = strlen(votenote);
		if (stringlength + j < 1024)
		{
			strcpy (string + stringlength, votenote);
			stringlength += j;
		}
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

//===================================================================
//===================================================================
//===================================================================

void MOTDScoreboardMessage (edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j;
	int		yofs;
	char		*seperator = "==================================";

	string[0] = 0;
	stringlength = 0;

	yofs = 110 - game.num_MOTD_lines * 10;
	if (yofs < 0 )
		yofs = 0;

	for (i=0; gameheader[i]; i++)
	{
		Com_sprintf (entry, sizeof(entry),
			"xm %i yv %i dmstr 863 \"%s\" ",
			-5*strlen(gameheader[i]), yofs + -60-49, gameheader[i] );
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
		yofs += 20;
	}
	yofs += 10;

	Com_sprintf (entry, sizeof(entry),
//		"xm %i yv %i picn /pics/thugfloor/separator2.tga ",
//		-98, yofs + -60-49);
        "xm %i yv %i dmstr 772 \"%s\" ",
		-5*strlen(seperator), yofs + (int)(-60.0+-3.5*14), seperator );

	j = strlen(entry);
	strcpy (string + stringlength, entry);
	stringlength += j;
	yofs += 30;

	if (game.num_MOTD_lines)
	{
		for (i=0; i<game.num_MOTD_lines; i++)
		{
			Com_sprintf (entry, sizeof(entry),
				"xm %i yv %i dmstr 953 \"%s\" ",
				-5*strlen(MOTD[i].textline), yofs + -60-49, MOTD[i].textline );
			j = strlen(entry);
			if (stringlength + j < 1024)
			{
				strcpy (string + stringlength, entry);
				stringlength += j;
			}
			yofs += 20;
		}
		yofs += 10;
		Com_sprintf (entry, sizeof(entry),
//			"xm %i yv %i picn /pics/thugfloor/separator.tga ",
//			-98, yofs + -60-49);
		"xm %i yv %i dmstr 772 \"%s\" ",
		-5*strlen(seperator), yofs + (int)(-60.0+-3.5*14), seperator );

		j = strlen(entry);
		if (stringlength + j < 1024)
		{
			strcpy (string + stringlength, entry);
			stringlength += j;
		}
		yofs += 30;
	}

	i=GetGameModeMessage(entry, yofs);
	j = strlen(entry);
	if (stringlength + j < 1024)
	{
		strcpy (string + stringlength, entry);
		stringlength += j;
		yofs = i;
	}

	if (ent->client->pers.version < 121)
	{
		yofs += 30;
		Com_sprintf (entry, sizeof(entry), "xm %i yv %i dmstr 990 \"Warning: You are using an old version of Kingpin.\" "
			"xm %i yv %i dmstr 990 \"You should download and install the v1.21 patch.\" ",
			-5*49, yofs + -60-49, -5*49, yofs + -60-49+20);
		j = strlen(entry);
		if (stringlength + j < 1024)
		{
			strcpy (string + stringlength, entry);
			stringlength += j;
		}
	}
	else
	{
		int rate = atoi(Info_ValueForKey(ent->client->pers.userinfo, "rate"));
		if (rate < 15000)
		{
			yofs += 30;
			Com_sprintf (entry, sizeof(entry), "xm %i yv %i dmstr 990 \"Warning: You currently have 'rate' set to only %d.\" "
				"xm %i yv %i dmstr 990 \"If you have broadband, at least 15000 is recommended.\" ",
				-5*52, yofs + -60-49, rate, -5*52, yofs + -60-49+20);
			j = strlen(entry);
			if (stringlength + j < 1024)
			{
				strcpy (string + stringlength, entry);
				stringlength += j;
			}
		}
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

//===================================================================
//===================================================================
//===================================================================

void NewPlayerMessageBoard (edict_t *ent)//Todo
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j;
	int		yofs=0;
	char	*selectheader[] =
		{
			"You wait until next round",
			NULL
		};

	string[0] = 0;
	stringlength = 0;

	for (i=0; selectheader[i]; i++)
	{
		Com_sprintf (entry, sizeof(entry),
			"xm %i yv %i dmstr 953 \"%s\" ",
			-5*strlen(selectheader[i]), yofs + (int)(-60.0+-3.5*14), selectheader[i] );

		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;

		yofs += 20;
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}
void PlayerWinMessageBoard (edict_t *ent)//Todo
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j;
	int		yofs=0;
	char	*selectheader[] =
		{
		    "===========================================================\n",
			"The Boss is now dead.. It's time for a new Kingpin..\n",
			"YOU!!!!\n",
			"===========================================================\n",
			NULL
		};

	string[0] = 0;
	stringlength = 0;

	for (i=0; selectheader[i]; i++)
	{
		Com_sprintf (entry, sizeof(entry),
			"xm %i yv %i dmstr 953 \"%s\" ",
			-5*strlen(selectheader[i]), yofs + (int)(-60.0+-3.5*14), selectheader[i] );

		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;

		yofs += 20;
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

//===================================================================
//===================================================================
//===================================================================

void RejoinScoreboardMessage (edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j;
	int		yofs;
    char	*seperator = "==================================";
	const char	*rejoinheader[] =
		{
			"You were just playing on this server.",
			"Would you like to continue where you left off?",
			NULL
		};
	const char	*choices[] =
		{
			"1 - Yes",
			"2 - No",
			NULL
		};

	string[0] = 0;
	stringlength = 0;
	yofs = 100;

	for (i=0; gameheader[i]; i++)
	{
		Com_sprintf (entry, sizeof(entry),
			"xm %i yv %i dmstr 863 \"%s\" ",
			-5*strlen(gameheader[i]), yofs + -60-49, gameheader[i] );
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
		yofs += 20;
	}
	yofs += 10;

	Com_sprintf (entry, sizeof(entry),
//		"xm %i yv %i picn /pics/thugfloor/separator.tga ",
//		-98, yofs + -60-49);
		"xm %i yv %i dmstr 772 \"%s\" ",
		-5*strlen(seperator), yofs + (int)(-60.0+-3.5*14), seperator );

	j = strlen(entry);
	strcpy (string + stringlength, entry);
	stringlength += j;
	yofs += 30;

	for (i=0; rejoinheader[i]; i++)
	{
		Com_sprintf (entry, sizeof(entry),
			"xm %i yv %i dmstr 999 \"%s\" ",
			-5*strlen(rejoinheader[i]), yofs + -60-49, rejoinheader[i] );
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
		yofs += 20;
	}
	yofs += 30;

	for (i=0; choices[i]; i++)
	{
		Com_sprintf (entry, sizeof(entry), "xm %i yv %i dmstr 999 \"%s\" ",
			-5*7, yofs + -60-49, choices[i]);
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
		yofs += 20;
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

//===================================================================
//===================================================================
//===================================================================


// MH: MatchSetupScoreboardMessage removed (merged with GrabDaLootScoreboardMessage)


/*
==================
TF_Build_HudMessage

constantly update players in match
==================
*/
void TF_Build_HudMessage(edict_t *ent, char *string)
{
	char	entry[1024];
	//char	string[1024];
	int		stringlength;
	int		i, j, count = 0, yoffs;
	edict_t *dood;


	string[0] = 0;
	yoffs = -200;
	strcpy(string, "xr -130 ");
	stringlength = strlen(string);
	entry[0] = 0;

	for_each_player(dood, i)
	{
		if (dood->client->pers.spectator == PLAYING)
		{
			count++;
			Com_sprintf(entry, sizeof(entry),
				"%s yb %i dmstr 990 \"%s\" "
				, entry, yoffs, dood->client->pers.netname
			);
			yoffs += 15;

			j = strlen(entry); //make sure size ok
			if (stringlength + j < 1024)
			{
				strcpy(string + stringlength, entry);
				stringlength += j;
			}
		}
	}

	if (!count)
		entry[0] = 0;
}


void TF_DeathmatchScoreboardMessage(edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength = 0, j;

	entry[0] = 0;
	strcpy(string, " ");

	if (level.modeset == WAVE_ACTIVE && ent->client->pers.spectator == PLAYING)// && ((level.modeset == WAVE_ACTIVE) || (level.modeset == WAVE_BUYZONE) || (level.modeset == PREGAME) || (level.modeset == WAVE_IDLE)))
	{
		TF_Build_HudMessage(ent, entry);
		j = strlen(entry);
		if (stringlength + j < 1024)
		{
			strcpy(string + stringlength, entry);
			stringlength += j;
		}
	}

	gi.WriteByte(svc_layout);
	gi.WriteString(string);
}


/*
==================
DeathmatchScoreboardMessage

==================
*/

void DeathmatchScoreboardMessage (edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j, k, l;
	int		sorted[MAX_CLIENTS];
	int		sortedscores[MAX_CLIENTS];
	int		score, total, realtotal;
	gclient_t	*cl;
	edict_t		*cl_ent;
	char	*tag;
	int		tmax;
    float	pldist;
    char	nfill[64];
    char    status[10];

	string[0] = 0;
	stringlength = 0;

	if (!ent->client->showscores)
		goto skipscores;

	// sort the clients by score
	total = 0;
	for (i=0 ; i<game.maxclients ; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse || (cl_ent->client->pers.spectator == SPECTATING))// && ((level.modeset == WAVE_ACTIVE) || (level.modeset == WAVE_BUYZONE) || (level.modeset == PREGAME) || (level.modeset == WAVE_IDLE)))
			continue;

		pldist = VectorDistance(ent->s.origin, cl_ent->s.origin);
		if (pldist > 9999.0f)
           pldist = 0;

/*		if (fph_scoreboard)
			score = game.clients[i].resp.time ? game.clients[i].resp.score * 36000 / game.clients[i].resp.time : 0;
		else*/
			score = (game.clients[i].resp.score<<8) - game.clients[i].resp.deposited;

		for (j=0 ; j<total ; j++)
		{
			if (score > sortedscores[j] || (score == sortedscores[j] && cl_ent == ent))
				break;
		}
		for (k=total ; k>j ; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	realtotal = total;

	Com_sprintf (entry, sizeof(entry), "xm %i yt 5 dmstr 752 \"%s\" ", -5*strlen(level.mapname), level.mapname);
	j = strlen(entry);
	strcpy (string + stringlength, entry);
	stringlength += j;

	// Send the current leader
	if (total)
	{
		Com_sprintf (entry, sizeof(entry),
			"leader %i ",
			sorted[0] );
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	// header
	if (ent->client->showscores == SCOREBOARD)
	{
/*		if (fph_scoreboard)
			Com_sprintf (entry, sizeof(entry),
				"xr %i yv %i dmstr 663 \"NAME         ping hits   fph\" ",
				-36*10 - 10, -60+-21 );
		else*/
			Com_sprintf (entry, sizeof(entry),
				"xr %i yv %i dmstr 663 \"Stat NAME           ping time  hits\" ",	//hypov8 note: "xr" is X aligned to Right.
				-56*10 - 10, -60+-21 );//56 was 36									//menu overlaps player display on low rez.
	}																				//either move menu to right or use "xm" (X aligned Middle)
    else if (ent->client->showscores==SCOREBOARD2) //FREDZ							//or you can test client rez if patched and move accorodingly
    {
        Com_sprintf (entry, sizeof(entry),
        "xr %i yv %i dmstr 663 \"NAME       health  cash  range\" ",
            -56*10 - 10, -60+-21 );//56 was 36
	}
	else
		Com_sprintf (entry, sizeof(entry),
			"xr %i yv %i dmstr 663 \"NAME        deaths  acc  fav\" ",
			-56*10 - 10, -60+-21 );//56 was 36
	j = strlen(entry);
	strcpy (string + stringlength, entry);
	stringlength += j;

	tmax = (1024 - stringlength) / (ent->client->pers.patched >= 3 ? 28 : 48);
	if (tmax > 21)
		tmax = 21;
	if (total > tmax)
		total = tmax - 1;

	if (ent->client->pers.patched >= 3)
	{
		// set Y start position and auto-increment for players list
		Com_sprintf (entry, sizeof(entry), "yv -60 yi 17 ");
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	for (i=0 ; i<total ; i++)
	{
		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

//		if (cl_ent == ent)//FREDZ not yellow
//			tag = "990";
        if (cl_ent->client->pers.player_dead == TRUE && level.modeset == WAVE_ACTIVE)
        {
            tag = "900";//FREDZ dead people are black //hypov8 test red
            strcpy(status,"dead");
        }
        else  if (cl_ent->client->pers.spectator == PLAYER_READY && (level.modeset == WAVE_ACTIVE||level.modeset == WAVE_START))
        {
            tag = "333";//FREDZ just joined, grey to black
            strcpy(status,"wait");
        }
		else
        {
            tag = "999";	// fullbright
            strcpy(status,"");
        }

        if (cl_ent->client->pers.rconx[0])
			tag = "096";
		else if (cl_ent->client->pers.admin > NOT_ADMIN)
			tag = "779";


/*
		if (ent->client->showscores == SCOREBOARD)//Old
		{
			if (ent->client->pers.patched >= 3)
			{
					Com_sprintf (entry, sizeof(entry),
						"ds %s %i %i %i %i %s",
						tag, sorted[i], cl->ping, cl->resp.time/600, cl->resp.score );
			}
			else
			{
					Com_sprintf (entry, sizeof(entry),
						"yv %i ds %s %i %i %i %i ",
						-60+i*17, tag, sorted[i], cl->ping, cl->resp.time/600, cl->resp.score );
			}
		}*/
        strcpy( nfill, cl->pers.netname );
		if (strlen(nfill) > 13)
			nfill[13] = '\0';

		if (strlen(cl->pers.netname) < 13)
		{
			for (l=0; l<13-strlen(cl->pers.netname); l++)
				strcat( nfill, " " );
		}

        if (ent->client->showscores == SCOREBOARD)
		{
			{
/*					Com_sprintf (entry, sizeof(entry),
						"yv %i ds %s %i %i %i %i ",
						-60+i*17, tag, sorted[i], cl->ping, cl->resp.time/600, cl->resp.score );*/
				Com_sprintf (entry, sizeof(entry),
					"yv %i dmstr %s \"%4s %s %4i %4i %5i\"",
					-60+i*17, tag, status, nfill, cl->ping, cl->resp.time/600, cl->resp.score);
			}
		}
        else if (ent->client->showscores==SCOREBOARD2) //FREDZ
		{
			if (ent->client->pers.patched >= 3)
			{
					Com_sprintf (entry, sizeof(entry),
						"ds %s %i %i %i %5.0f ",
						tag, sorted[i], cl_ent->health, cl->pers.currentcash, pldist );
			}
			else
			{
					Com_sprintf (entry, sizeof(entry),
						"yv %i ds %s %i %i %i %5.0f ",
						-60+i*17, tag, sorted[i], cl_ent->health, cl->pers.currentcash, pldist );
			}
		}
		else//SCOREBOARD3
		{
			int fc = 0;
			const char *fn = "-";
			for (j=0; j<8; j++)
			{
				if (cl->resp.fav[j] > fc)
				{
					fc = cl->resp.fav[j];
					fn = weapnames[j];
				}
			}
			if (ent->client->pers.patched >= 3)
				Com_sprintf (entry, sizeof(entry),
					"dt %s %i \"%5i %4i %4s\" ",
					tag, sorted[i], cl->resp.deposited, cl->resp.accshot?cl->resp.acchit*1000/cl->resp.accshot:0, fn);
			else
				Com_sprintf (entry, sizeof(entry),
					"yv %i dmstr %s \"%-13s%5i %4i %4s\" ",
					-60+i*17, tag, cl->pers.netname, cl->resp.deposited, cl->resp.accshot?cl->resp.acchit*1000/cl->resp.accshot:0, fn);
		}
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	if (realtotal > i)
	{
		Com_sprintf (entry, sizeof(entry),
			"yv %i dmstr 777 \"(%i players)\" ",
			-60+i*17+6, realtotal );
		j = strlen(entry);
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	if (level.modeset == ENDGAMEVOTE)
	{
		static const char *votenote = "xm -230 yb -40 dmstr 552 \"hit your scoreboard key (f1) for the vote menu\" ";
		j = strlen(votenote);
		if (stringlength + j < 1024)
		{
			strcpy (string + stringlength, votenote);
			stringlength += j;
		}
	}

skipscores:
	if (level.modeset == WAVE_ACTIVE && ent->client->pers.spectator == PLAYING)// && ((level.modeset == WAVE_ACTIVE) || (level.modeset == WAVE_BUYZONE) || (level.modeset == PREGAME) || (level.modeset == WAVE_IDLE)))
	{
		TF_Build_HudMessage(ent, entry);
		j = strlen(entry);
		if (stringlength + j < 1024)
		{
			strcpy(string + stringlength, entry);
			stringlength += j;
		}
	}

	if (ent->client->pers.spectator == SPECTATING)// && ((level.modeset == WAVE_ACTIVE) || (level.modeset == WAVE_BUYZONE) || (level.modeset == PREGAME) || (level.modeset == WAVE_IDLE)))
	{
		GetChaseMessage(ent,entry);
		j = strlen(entry);
		if (stringlength + j < 1024)
		{
			strcpy (string + stringlength, entry);
			stringlength += j;
		}
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}


void TF_setScoreboard(gclient_t *client)
{
	//ent->client->resp.scoreboard_hide = false;
	if (client->showscores == NO_SCOREBOARD
		&& client->resp.scoreboard_frame < level.framenum
		&& (level.modeset == WAVE_ACTIVE)
		&& (client->pers.spectator != SPECTATING)) //chase scorebord
	{
		client->showscores = SCORE_TF_HUD;
	}
}

/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
Papa - Here is where i determine what scoreboard to display
==================
*/
void DeathmatchScoreboard (edict_t *ent)
{
	if (ent->client->showscores == SCORE_MOTD)
		MOTDScoreboardMessage (ent);
	else if (ent->client->showscores == SCORE_REJOIN)
		RejoinScoreboardMessage (ent);
	else if (ent->client->showscores == SPECTATORS)
		SpectatorScoreboardMessage (ent);
	else if (ent->client->showscores == SCORE_MAP_VOTE)
		VoteMapScoreboardMessage(ent);
	else if (ent->client->showscores == SCORE_TF_HUD) //constant hud shows names
		TF_DeathmatchScoreboardMessage(ent);
	else
    {
        DeathmatchScoreboardMessage (ent);
	}


	gi.unicast (ent, !ent->client->resp.scoreboard_frame); // MH: make reliable if it's a requested (not periodic) update

	ent->client->resp.scoreboard_frame = level.framenum; // MH: update frame
	// MH: faster refreshing during map voting
	if (ent->client->showscores == SCORE_MAP_VOTE)
		ent->client->resp.scoreboard_frame = level.framenum - 20;
}
/*
void NoScoreboardMessage (edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		j;

	string[0] = 0;
	stringlength = 0;

	if ((level.modeset != MATCHSETUP) && (level.modeset != FINALCOUNT)//SNAP
		&& ent->client->pers.spectator == SPECTATING) {
		SHOWCHASENAME
		CHASEMESSAGE
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

void DeathmatchScoreboardNew (edict_t *ent)
{
	switch(ent->client->showscores)
    {
    case SCORE_MOTD:
		MOTDScoreboardMessage (ent);
        break;
    case SCORE_REJOIN:
		RejoinScoreboardMessage (ent);
        break;
    case SPECTATORS:
		SpectatorScoreboardMessage (ent);
        break;
    case SCORE_MAP_VOTE:
		VoteMapScoreboardMessage(ent);
        break;
    case NO_SCOREBOARD:
        NoScoreboardMessage(ent);
        break;
	default:
        {
            if (teamplay->value)
                if ((level.modeset == MATCHSETUP) || (level.modeset == FINALCOUNT) || (level.modeset == FREEFORALL))
                    MatchSetupScoreboardMessage (ent, ent->enemy);
                else
                    GrabDaLootScoreboardMessage (ent, ent->enemy);
                else
                    DeathmatchScoreboardMessage (ent, ent->enemy);
        }
        break;
    }


	if (level.intermissiontime)
		gi.unicast (ent, true);
	else
		gi.unicast (ent, false);//true);
}
*/


/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/

// Papa - This is the start of the scoreboard command, this sets the showscores value
void Cmd_Score_f (edict_t *ent)//FREDZ todo
{
	int		i,found;
	edict_t	*dood;

	ent->client->showinventory = false;
	ent->client->showhelp = false;

	//FREDZ For menu code
	ent->client->showscrollmenu = false;

#if 1 //mm 2.0
	if (ent->client->showscores == SCORE_REJOIN)
	{
		ClientRejoin(ent, false);
		return;
	}
#endif

	if (ent->client->showscores == SCOREBOARD)
		ent->client->showscores = SCOREBOARD2;
	else if (level.modeset == ENDGAMEVOTE)
	{
		if (ent->client->showscores == SCORE_MAP_VOTE)
//			ent->client->showscores = BESTSCORES;
//		else if (ent->client->showscores == BESTSCORES)
			ent->client->showscores = SCOREBOARD;
		else if (ent->client->showscores == SCOREBOARD2)
			ent->client->showscores = SCOREBOARD3;
		else if (ent->client->showscores == SCOREBOARD3)
		{
			found = false;
            // MH: also check for connecting players, which are included in spectators list
			for (i=0 ; i<maxclients->value ; i++)
			{
				dood = g_edicts + 1 + i;
				if (dood->client && ((dood->inuse && dood->client->pers.spectator == SPECTATING)
					|| (!dood->inuse && dood->client->pers.connected && (kpded2 || curtime - dood->client->pers.lastpacket < 120000))))
					found = true;
			}
			if (found)
				ent->client->showscores = SPECTATORS;
			else
				ent->client->showscores = SCORE_MAP_VOTE;
		}
		else
			ent->client->showscores = SCORE_MAP_VOTE;
	}
	else if (ent->client->showscores == SCOREBOARD2)
		ent->client->showscores = SCOREBOARD3;
	else if (ent->client->showscores == SCOREBOARD3)
	{
		found = false;
		for_each_player(dood,i)
		{
			if (dood->client->pers.spectator == SPECTATING)
				found = true;
		}
		if (found)
			ent->client->showscores = SPECTATORS;
		else
			ent->client->showscores = NO_SCOREBOARD;
	}
	else if (ent->client->showscores == SPECTATORS)
	{
		if (level.intermissiontime)
			ent->client->showscores = SCOREBOARD;
		else
			ent->client->showscores = NO_SCOREBOARD;
	}
	else
		ent->client->showscores = SCOREBOARD;

	TF_setScoreboard(ent->client); //TF: hud

    ent->client->resp.scoreboard_frame = 0; // MH: trigger scoreboard update instead of calling DeathmatchScoreboard
//	DeathmatchScoreboard (ent); //hypov8 do we need this enabled?
}
void Cmd_Motd_f (edict_t *ent)//FREDZ todo: still broken
{
	ent->client->showinventory = false;
	ent->client->showhelp = false;
	ent->client->showscrollmenu = false;//FREDZ For menu code

	MOTDScoreboardMessage (ent);
}

/*
==================
HelpComputer

Draw help computer.
==================
*/
void HelpComputer (edict_t *ent, int page)
{
	char	string[1024];
	/*char	*sk;

	if (skill->value == 0)
		sk = "easy";
	else if (skill->value == 1)
		sk = "medium";
	else if (skill->value == 2)
		sk = "hard";
	else
		sk = "hard+";

	// send the layout
	Com_sprintf (string, sizeof(string),
		"xv 33 yv 7 picnote " // background
		"xv 202 yv 12 string2 \"%s\" "		// skill
		"xv 0 yv 24 cstring2 \"%s\" "		// level name
		"xv 0 yv 54 cstring2 \"%s\" "		// help 1
		"xv 0 yv 110 cstring2 \"%s\" "		// help 2
		"xv 50 yv 164 string2 \" kills     goals    secrets\" "
		"xv 50 yv 172 string2 \"%3i/%3i     %i/%i       %i/%i\" ",
		sk,
		level.level_name,
		game.helpmessage1,
		game.helpmessage2,
		level.killed_monsters, level.total_monsters,
		level.found_goals, level.total_goals,
		level.found_secrets, level.total_secrets);*/

	EP_PlayerLog (ent, page);

	level.helpchange = 0;

	Com_sprintf (string, sizeof(string),
		"picnote \"%s\" "
		" \"%s\" ",
		game.helpmessage1,
		game.helpmessage2);

//	Com_sprintf (string, sizeof(string),
//		"xv 32 yv 8 string2 \"Help screen not yet implemented\" "	);

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
	gi.unicast (ent, true);
}

/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (edict_t *ent, int page)
{
	// this is for backwards compatability
	if (deathmatch->value)
	{
		Cmd_Score_f (ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = NO_SCOREBOARD;

	ent->client->showscrollmenu = false;//FREDZ For menu code

	if (ent->client->showhelp && (ent->client->pers.game_helpchanged == game.helpchanged) && !(page))
	{
		ent->client->showhelp = false;
		return;
	}

	ent->client->showhelp = true;
	ent->client->pers.helpchanged = 0;

	HelpComputer (ent, page);
}


//=======================================================================

/*
===============
G_SetStats
===============
*/
void G_SetStats (edict_t *ent)
{
	gitem_t		*item;
	int			index, cells;
	int			power_armor_type;

	// if chasecam, show stats of player we are following
	if (ent->client->chase_target && ent->client->chase_target->client)
	{
		memcpy( ent->client->ps.stats, ent->client->chase_target->client->ps.stats, sizeof( ent->client->ps.stats ) );
		ent->client->ps.stats[STAT_LAYOUTS] = true;
//	ent->client->ps.stats[STAT_LAYOUTS] = 0;

		if (deathmatch->value) {
			if (level.intermissiontime || ent->client->showscores)
				ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		} else {
			if (ent->client->showscores || ent->client->showhelp)
				ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		}

		// MH: show teammate cash only in eyecam mode
//		if (ent->client->chasemode != EYECAM_CHASE && teamplay->value == 1)
//			ent->client->ps.stats[23] = 0;

		// MH: keep own score
		ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;
		ent->client->ps.stats[STAT_ENEMYCOUNT] = level.waveEnemyCount_total; // ent->client->resp.deposited;

		return;
	}

	//
	// health
	//
	// JOSEPH 23-MAR-99
	//ent->client->ps.stats[STAT_HEALTH_ICON] = level.pic_health;
	{
		int     index1, index2, index3;

		item = FindItem ("Cash");
		index1 = ITEM_INDEX (item);
		item = FindItem ("Large Cash Bag");
		index2 = ITEM_INDEX (item);
		item = FindItem ("Small Cash Bag");
		index3 = ITEM_INDEX (item);

		if (!((ent->client->ps.stats[STAT_PICKUP_STRING] == CS_ITEMS+index1) ||
			  (ent->client->ps.stats[STAT_PICKUP_STRING] == CS_ITEMS+index2) ||
			  (ent->client->ps.stats[STAT_PICKUP_STRING] == CS_ITEMS+index3)))
			ent->client->ps.stats[STAT_CASH_PICKUP] = 0;

		if (ent->client->invincible_framenum>level.framenum &&
			((ent->client->invincible_framenum-level.framenum)&4) &&
            ent->client->pers.spectator!=SPECTATING) {
			item = FindItem ("Helmet Armor Heavy");
			ent->client->ps.stats[STAT_ARMOR1] = 2023;
			item = FindItem ("Jacket Armor heavy");
			ent->client->ps.stats[STAT_ARMOR2] = 2023;
			item = FindItem ("Legs Armor Heavy");
			ent->client->ps.stats[STAT_ARMOR3] = 2023;
		} else {
			// JOSEPH 1-APR-99-B
			item = FindItem ("Helmet Armor");
			ent->client->ps.stats[STAT_ARMOR1] = ent->client->pers.inventory[ITEM_INDEX(item)];
			item = FindItem ("Jacket Armor");
			ent->client->ps.stats[STAT_ARMOR2] = ent->client->pers.inventory[ITEM_INDEX(item)];
			item = FindItem ("Legs Armor");
			ent->client->ps.stats[STAT_ARMOR3] = ent->client->pers.inventory[ITEM_INDEX(item)];
			item = FindItem ("Helmet Armor Heavy");
			if (ent->client->pers.inventory[ITEM_INDEX(item)])
				ent->client->ps.stats[STAT_ARMOR1] = ent->client->pers.inventory[ITEM_INDEX(item)] + 1024;
			item = FindItem ("Jacket Armor heavy");
			if (ent->client->pers.inventory[ITEM_INDEX(item)])
				ent->client->ps.stats[STAT_ARMOR2] = ent->client->pers.inventory[ITEM_INDEX(item)] + 1024;
			item = FindItem ("Legs Armor Heavy");
			if (ent->client->pers.inventory[ITEM_INDEX(item)])
				ent->client->ps.stats[STAT_ARMOR3] = ent->client->pers.inventory[ITEM_INDEX(item)] + 1024;
			// END JOSEPH
		}
	}
	// END JOSEPH

	ent->client->ps.stats[STAT_HEALTH] = ent->health;

	//
	// ammo
	//

	// JOSEPH 28-APR-99
	if (!level.bar_lvl)
	{
        if (!ent->client->ammo_index /* || !ent->client->pers.inventory[ent->client->ammo_index] */)
        {
            ent->client->ps.stats[STAT_AMMO_ICON] = 0;
            ent->client->ps.stats[STAT_AMMO] = 0;
        }
        else
        {
            item = &itemlist[ent->client->ammo_index];
            ent->client->ps.stats[STAT_AMMO_ICON] = gi.imageindex (item->icon);
            ent->client->ps.stats[STAT_AMMO] = ent->client->pers.inventory[ent->client->ammo_index];
        }

        // RAFAEL 01-11-99
        // JOSEPH 9-MAR-99
        if (ent->client->ammo_index)
        {
            item = &itemlist[ent->client->ammo_index];
        }
        else
        {
            item = NULL;
        }

        if ((item) && (item->pickup_name) && (/*(!strcmp(item->pickup_name, "Rockets")) ||*/ ((!strcmp(item->pickup_name, "Gas")))))
        {
            ent->client->ps.stats[STAT_CLIP] = -1;
        }
        else
        {
            // ent->client->ps.stats[STAT_CLIP_ICON] = gi.imageindex (clipname);
            ent->client->ps.stats[STAT_CLIP] = ent->client->pers.weapon_clip[ent->client->clip_index];
        }
	}
	else
	{
		ent->client->ps.stats[STAT_AMMO_ICON] = 0;
		ent->client->ps.stats[STAT_AMMO] = 0;
		ent->client->ps.stats[STAT_CLIP] = -1;
	}
	// END JOSEPH

	//
	// money
	//

	// Ridah, 26-may-99, show frag count
//	if (deathmatch->value && teamplay->value!=1)
//		ent->client->ps.stats[STAT_CASH] = ent->client->resp.score;
//	else	// show cash
		ent->client->ps.stats[STAT_CASH] = ent->client->pers.currentcash;

	if (level.pawn_time)
		ent->client->ps.stats[STAT_FORCE_HUD] = 1;
	else
		ent->client->ps.stats[STAT_FORCE_HUD] = 0;
	// END JOSEPH

	// JOSEPH 4-MAR-99
	if ((level.cut_scene_time) || (level.fadeendtime > level.time))
		ent->client->ps.stats[STAT_HIDE_HUD] = 1;
	else
		ent->client->ps.stats[STAT_HIDE_HUD] = 0;
	// END JOSEPH

	if ((level.cut_scene_time || level.cut_scene_end_count) && level.cut_scene_camera_switch)
		ent->client->ps.stats[STAT_SWITCH_CAMERA] = 1;
	else
		ent->client->ps.stats[STAT_SWITCH_CAMERA] = 0;
	// END JOSEPH

	// JOSEPH 2-FEB-99
	if (level.time > ent->client->hud_enemy_talk_time)
	{
		ent->client->ps.stats[STAT_HUD_ENEMY_TALK] = 0;
	}
	else if ((ent->client->hud_enemy_talk_time - level.time) > 1.0)
	{
		ent->client->ps.stats[STAT_HUD_ENEMY_TALK_TIME] = 255;
	}
	else
	{
		ent->client->ps.stats[STAT_HUD_ENEMY_TALK_TIME] =
			(short)((255.0/1.0)*(ent->client->hud_enemy_talk_time - level.time));
		if (ent->client->ps.stats[STAT_HUD_ENEMY_TALK_TIME] < 0)
			ent->client->ps.stats[STAT_HUD_ENEMY_TALK_TIME] = 0;
	}

	if (level.time > ent->client->hud_self_talk_time)
	{
		ent->client->ps.stats[STAT_HUD_SELF_TALK] = 0;
	}
	else if ((ent->client->hud_self_talk_time - level.time) > 1.0)
	{
		ent->client->ps.stats[STAT_HUD_SELF_TALK_TIME] = 255;
	}
	else
	{
		ent->client->ps.stats[STAT_HUD_SELF_TALK_TIME] =
			(short)((255.0/1.0)*(ent->client->hud_self_talk_time - level.time));
		if (ent->client->ps.stats[STAT_HUD_SELF_TALK_TIME] < 0)
			ent->client->ps.stats[STAT_HUD_SELF_TALK_TIME] = 0;
	}
	// END JOSEPH

	// JOSEPH 16-FEB-99
	{
		cast_memory_t *mem;
		int firsthire = 0;
		int i;
		edict_t *theent;

		ent->client->ps.stats[STAT_HUD_HIRE1] = 0;
		ent->client->ps.stats[STAT_HUD_HIRE2] = 0;

		theent = &g_edicts[0];
		for (i=0 ; i<globals.num_edicts ; i++, theent++)
		{
			if (!theent->inuse)
				continue;

			if ((theent->leader) && (theent->leader == ent))
			{
				mem = level.global_cast_memory[theent->character_index][ent->character_index];

				if (!mem)
					continue;

				if (mem->flags & MEMORY_HIRED)
				{
					if (!firsthire)
					{
						if (theent->health > 0)
						{
							// JOSEPH 28-APR-99
							ent->client->ps.stats[STAT_HUD_HIRE1] = (int)((100.0/(float)theent->max_health)*((float)theent->health));
							// END JOSEPH

							if ((theent->enemy) && (theent->enemy->health > 0))
							{
								ent->client->ps.stats[STAT_HUD_HIRE1_CMD] = 3;
							}
							else if (theent->cast_info.aiflags & AI_MOVEOUT || theent->cast_info.aiflags & AI_DOKEY)
							{
								ent->client->ps.stats[STAT_HUD_HIRE1_CMD] = 1;
							}
							else if (theent->cast_info.aiflags & AI_HOLD_POSITION)
							{
								ent->client->ps.stats[STAT_HUD_HIRE1_CMD] = 2;
							}
							else
							{
								ent->client->ps.stats[STAT_HUD_HIRE1_CMD] = 0;
							}

							firsthire = 1;
						}
					}
					else
					{
						if (theent->health > 0)
						{
							// JOSEPH 28-APR-99
							ent->client->ps.stats[STAT_HUD_HIRE2] = (int)((100.0/(float)theent->max_health)*((float)theent->health));
							// END JOSEPH

							if ((theent->enemy) && (theent->enemy->health > 0))
							{
								ent->client->ps.stats[STAT_HUD_HIRE1_CMD] = 3;
							}
							else if (theent->cast_info.aiflags & AI_MOVEOUT || theent->cast_info.aiflags & AI_DOKEY)
							{
								ent->client->ps.stats[STAT_HUD_HIRE2_CMD] = 1;
							}
							else if (theent->cast_info.aiflags & AI_HOLD_POSITION)
							{
								ent->client->ps.stats[STAT_HUD_HIRE2_CMD] = 2;
							}
							else
							{
								ent->client->ps.stats[STAT_HUD_HIRE2_CMD] = 0;
							}
						}
						break;
					}
				}
			}
		}
	}
	// END JOSEPH

	// JOSEPH 4-FEB-99-C
	ent->client->ps.stats[STAT_HUD_INV] = 0;

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Battery"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 1;

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Coil"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 2;

	// JOSEPH 17-MAR-99
	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Watch"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 4;
	// END JOSEPH

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Safe docs"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 8;

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Fuse"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 16;

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Valve"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 32;
	// END JOSEPH

	// JOSEPH 10-JUN-99
	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Lizzy Head"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 64;

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Whiskey"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 128;

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Oil Can"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 256;

	if (ent->client->pers.inventory[ITEM_INDEX(FindItem ("Ticket"))])
		ent->client->ps.stats[STAT_HUD_INV] |= 512;
	// END JOSEPH

	//
	// armor
	//
	power_armor_type = PowerArmorType (ent);
	if (power_armor_type)//Q2
	{
		cells = ent->client->pers.inventory[ITEM_INDEX(FindItem ("Gas"))];
		if (cells == 0)
		{	// ran out of cells for power armor
			ent->flags &= ~FL_POWER_ARMOR;
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/power2.wav"), 1, ATTN_NORM, 0);
			power_armor_type = 0;;
		}
	}

	index = ArmorIndex (ent);

	// JOSEPH 1-APR-99
	/*if (power_armor_type && (!index || (level.framenum & 8) ) )
	{	// flash between power armor and other armor icon
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex ("i_powershield");
		ent->client->ps.stats[STAT_ARMOR] = cells;
	}
	else if (index)
	{
		item = GetItemByIndex (index);
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex (item->icon);
		ent->client->ps.stats[STAT_ARMOR] = ent->client->pers.inventory[index];
	}
	else
	{
		// ent->client->ps.stats[STAT_ARMOR_ICON] = 0;
		ent->client->ps.stats[STAT_ARMOR] = 0;
	}*/
	// END JOSEPH

	//
	// pickup message
	//
	// JOSEPH 25-JAN-99
	if (level.time > ent->client->pickup_msg_time)
	{
		ent->client->ps.stats[STAT_PICKUP_ICON] = 0;
		ent->client->ps.stats[STAT_PICKUP_STRING] = 0;
		ent->client->ps.stats[STAT_PICKUP_COUNT] = 0;
	}
	else if ((ent->client->pickup_msg_time - level.time) > 1.5)
	{
		ent->client->ps.stats[STAT_PICKUP_COUNT] = 255;
	}
	else
	{
		ent->client->ps.stats[STAT_PICKUP_COUNT] =
			(short)((255.0/1.5)*(ent->client->pickup_msg_time - level.time));
		if (ent->client->ps.stats[STAT_PICKUP_COUNT] < 0)
			ent->client->ps.stats[STAT_PICKUP_COUNT] = 0;
	}
	// END JOSEPH

	//
	// timers
	//
	// JOSEPH 12-MAY-99-B
	/*
	if (ent->client->quad_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_quad");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->quad_framenum - level.framenum)/10;
	}
	// RAFAEL
	else if (ent->client->quadfire_framenum > level.framenum)
	{
		// note to self
		// need to change imageindex
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_quadfire");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->quadfire_framenum - level.framenum)/10;
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_invulnerability");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->invincible_framenum - level.framenum)/10;
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_envirosuit");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->enviro_framenum - level.framenum)/10;
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_rebreather");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->breather_framenum - level.framenum)/10;
	}
	else
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = 0;
		ent->client->ps.stats[STAT_TIMER] = 0;
	}
	*/

	//show boss health
	if (level.modeset == WAVE_ACTIVE)
	{
		if (boss_entityID !=NULL &&(
			((int)wavetype->value == 0 && level.waveNum == WAVELEN_SHORT - 1) ||
			((int)wavetype->value == 1 && level.waveNum == WAVELEN_MED - 1) ||
			((int)wavetype->value >= 2 && level.waveNum == WAVELEN_LONG - 1)
			))
		{
			ent->client->ps.stats[STAT_BOSS] = (int)(100.0f * boss_entityID->health / (float)boss_maxHP);
			if (ent->client->ps.stats[STAT_BOSS] <= 0)
				ent->client->ps.stats[STAT_BOSS] = 1; //zero dont display
		}
		else
			ent->client->ps.stats[STAT_BOSS] = 0;
	}
	else
		ent->client->ps.stats[STAT_BOSS] = 0;


	//hypov8 this should go somewhere else!!!
	//show distance to pawnGuy an last 4 players
	if (level.modeset == WAVE_BUYZONE || (level.modeset == WAVE_ACTIVE && level.waveEnemyCount_cur <=4))
	{
		if (!(level.framenum % 5) || !ent->client->botRange) // is it updating fast enough?
		{
			int i, index = 0;
			float best = 99999.0f;
			for (i = 0; i < MAX_CHARACTERS; i++)
			{
				if (level.characters[i] && level.characters[i]->inuse && !level.characters[i]->client
					&&level.characters[i]->health > 0 && !level.characters[i]->deadflag)
				{
					float v = VectorDistance(ent->s.origin, level.characters[i]->s.origin);
					if (v < best)
					{
						best = v;
						index = i;
					}
				}
			}

			if (index > 0)
				ent->client->botRange = (int)best;
		}

		if (ent->client->botRange)
			ent->client->ps.stats[STAT_ENEMYRANGE] = ent->client->botRange;
		else
			ent->client->ps.stats[STAT_ENEMYRANGE] = 0;
	}
	else
	{
		ent->client->ps.stats[STAT_ENEMYRANGE] = 0;
	}


// Papa - Here is the Timer for the hud
	// MH: fixed to show starting timer when timelimit=0
	if (level.modeset == PREGAME)
		ent->client->ps.stats[STAT_TIMER] = ((300 + 9 -  level.framenum ) / 10); // MH: changed from 350, +9 to round up

//	else if ((level.modeset == FREEFORALL) && (timelimit->value))
//		if (level.framenum > ((timelimit->value * 600) - 605))
//			ent->client->ps.stats[STAT_TIMER] = (((timelimit->value * 600) - level.framenum ) / 10);
//		else
//			ent->client->ps.stats[STAT_TIMER] = (((timelimit->value * 600) - level.framenum ) / 600);
//	else if ((level.modeset == TEAMPLAY) && (timelimit->value))
//		if (level.framenum > ((timelimit->value * 600) - 605))
//			ent->client->ps.stats[STAT_TIMER] = (((timelimit->value * 600) - level.framenum ) / 10);
//		else
//			ent->client->ps.stats[STAT_TIMER] = (((timelimit->value * 600) - level.framenum ) / 600);

	else if (level.modeset == WAVE_START)//FREDZ
		ent->client->ps.stats[STAT_TIMER] =	((150 - (level.framenum - level.startframe)) / 10);

    else if (level.modeset == WAVE_BUYZONE)//FREDZ
		ent->client->ps.stats[STAT_TIMER] =	((600 - (level.framenum - level.startframe)) / 10);

	else if (level.intermissiontime) // MH: applies to non-voting intermission too
		ent->client->ps.stats[STAT_TIMER] =	((270 + 9 - (level.framenum - level.startframe)) / 10); // MH: changed voting time to 27s (music duration)

	else if ((int)timelimit->value && level.modeset == WAVE_ACTIVE) // MH: only when timelimit!=0
		if (level.framenum > (level.startframe + (((int)timelimit->value * 600) - 605)))
			ent->client->ps.stats[STAT_TIMER] = ((((int)timelimit->value * 600) + level.startframe - level.framenum) / 10);
		else
			ent->client->ps.stats[STAT_TIMER] = ((((int)timelimit->value * 600) + level.startframe - level.framenum) / 600);
	else
		ent->client->ps.stats[STAT_TIMER] = 0;

	if (ent->client->ps.stats[STAT_TIMER] < 0 )
		ent->client->ps.stats[STAT_TIMER] = 0;


	//display current/total wave numbers
	if (level.modeset != PREGAME && level.modeset != WAVE_IDLE) //show waves left at end of match
	{
		if ((int)wavetype->value == 0 )			//short
			ent->client->ps.stats[STAT_WAVEROUND] = WAVELEN_SHORT;
		else if  ((int)wavetype->value ==  1)	//med
			ent->client->ps.stats[STAT_WAVEROUND] = WAVELEN_MED;
		else									//long
			ent->client->ps.stats[STAT_WAVEROUND] = WAVELEN_LONG;

		ent->client->ps.stats[STAT_WAVENUM] = level.waveNum+1;
	}
	else
	{
		ent->client->ps.stats[STAT_WAVEROUND] = 0;
		ent->client->ps.stats[STAT_WAVENUM] = 0;
	}

	// END JOSEPH

	//
	// selected item
	//
	// JOSEPH 1-APR-99
	/*if (ent->client->pers.selected_item == -1)
		ent->client->ps.stats[STAT_SELECTED_ICON] = 0;
	else
		ent->client->ps.stats[STAT_SELECTED_ICON] = gi.imageindex (itemlist[ent->client->pers.selected_item].icon);
	*/
	// END JOSEPH
	ent->client->ps.stats[STAT_SELECTED_ITEM] = ent->client->pers.selected_item;

	//
	// layouts
	//
	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if (deathmatch->value)
	{
		if (ent->client->pers.health <= 0 || level.intermissiontime
			|| ent->client->showscores || ent->client->showscrollmenu)//FREDZ For menu code
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}
	else
	{
		if (ent->client->showscores || ent->client->showhelp || ent->client->showscrollmenu)//FREDZ For menu code
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}

	//
	// frags
	//
	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;
	ent->client->ps.stats[STAT_ENEMYCOUNT] = level.waveEnemyCount_total;// ent->client->resp.deposited;

	// END

	//
	// help icon / current weapon if not shown
	//
	if (ent->client->resp.helpchanged && (level.framenum&8) )
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex ("i_help");
	else if ( (ent->client->pers.hand == CENTER_HANDED || ent->client->ps.fov > 91)
		&& ent->client->pers.weapon)
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex (ent->client->pers.weapon->icon);
	else
		ent->client->ps.stats[STAT_HELPICON] = 0;

	// JOSEPH 14-JUN-99
	ent->client->ps.stats[STAT_HELPICON] = level.helpchange;
	// END JOSEPH
/*
	// Teamplay
	if (deathmatch->value && teamplay->value)
	{
		int i;

		// show team scores
		for (i=0; i<2; i++)
		{
			ent->client->ps.stats[STAT_TEAM1_SCORE + i] = team_cash[1+i];	// set score

			ent->client->ps.stats[STAT_TEAM1_FLASH + i] = 0;	// set normal

			if (last_safe_withdrawal[i+1] > last_safe_deposit[i+1])
			{
				if (last_safe_withdrawal[i+1] > (level.time - 3.0))
					ent->client->ps.stats[STAT_TEAM1_FLASH + i] = 2;	// flash red
			}
			else
			{
				if (last_safe_deposit[i+1] > (level.time - 3.0))
					ent->client->ps.stats[STAT_TEAM1_FLASH + i] = 1;	// flash green
			}
		}

		if (teamplay->value==1) {
			// show bagged cash
			ent->client->ps.stats[STAT_BAGCASH] = ent->client->pers.bagcash; //STAT_BAG_CASH
			ent->client->ps.stats[23]=0;
			if (ent->client->pers.team) {
				// MH: updated to do the same as the client does for crosshair player name display
				edict_t *best = NULL;
				float bestdist = 2048;
				edict_t	*dood;
				vec3_t forward;
				AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
				for_each_player(dood, i)
				{
					vec3_t v;
					float dist;
					if (dood == ent || dood->solid == SOLID_NOT)
						continue;
					VectorSubtract(dood->s.origin, ent->s.origin, v);
					dist = VectorNormalize(v);
					if (dist < bestdist)
					{
						float dist2 = dist > 512 ? 512 : dist;
						if (DotProduct(v, forward) > dist2 * 0.0001171875 + 0.9)
						{
							trace_t tr;
							VectorAdd(ent->s.origin, ent->client->ps.viewoffset, v);
							tr = gi.trace(v, NULL, NULL, dood->s.origin, ent, MASK_PLAYERSOLID);
							if (tr.fraction * dist > dist - 64)
							{
								best = dood;
								bestdist = dist;
							}
						}
					}
				}
				if (best && ent->client->pers.team == best->client->pers.team)
					ent->client->ps.stats[23] = best->client->pers.bagcash + best->client->pers.currentcash;
			}
		}
	}*/
}

// JOSEPH 16-DEC-98

// RAFAEL
/*void BeginCutScene (edict_t *ent)
{
	edict_t *player;

	player = g_edicts + 1;

	level.cut_scene_time = level.time + 0.1;

	VectorCopy (ent->s.angles, level.cut_scene_angle);
	VectorCopy (ent->s.origin, level.cut_scene_origin);

	VectorCopy (player->s.origin, level.player_oldpos);
	VectorCopy (player->s.angles, level.player_oldang);

	MoveClientToCutScene (player);

}*/

void FollowEnt (edict_t *ent)
{
    /*if (ent->target2_ent)
	{
    	vec3_t	forward, right, up, forward2, right2, up2, avect, avect2;

		AngleVectors (ent->s.origin, forward, right, up);
	    AngleVectors (ent->target2_ent->s.origin, forward2, right2, up2);
		avect[0] = ((forward[0] + right[0] + up[0]))/3;
		avect[1] = ((forward[1] + right[1] + up[1]))/3;
		avect[2] = ((forward[2] + right[2] + up[2]))/3;
		avect2[0] = ((forward2[0] + right2[0] + up2[0]))/3;
		avect2[1] = ((forward2[1] + right2[1] + up2[1]))/3;
		avect2[2] = ((forward2[2] + right2[2] + up2[2]))/3;
		VectorSubtract(avect, avect2, avect);
		VectorNormalize(avect);
		vectoangles(avect, ent->s.angles);
	}*/

    if (ent->target2_ent)
	{
//    	vec3_t	forward, forward2;
		vec3_t	avect;

// Ridah, not sure how this is supposed to work, doesn't make sense to do an AngleVectors() call on an origin
//		AngleVectors (ent->s.origin, forward, NULL, NULL);
//	    AngleVectors (ent->target2_ent->s.origin, forward2, NULL, NULL);
//		VectorSubtract(forward, forward2, avect);

		VectorSubtract(ent->target2_ent->s.origin, ent->s.origin, avect);

		VectorNormalize(avect);
		vectoangles(avect, ent->s.angles);
	}
}

// JOSEPH 23-FEB-99
void BeginCutScene (edict_t *ent)
{
	edict_t *player;

	player = g_edicts + 1;

    level.cut_scene_camera_switch = 1;

	level.cut_scene_time = level.time + 0.1;

	VectorCopy(ent->s.angles, ent->save_avel);
	VectorCopy(ent->s.origin, ent->savecameraorigin);

	if (ent->cameraangle[0])
	{
		ent->s.angles[0] += ent->cameraangle[0];
	}

	if (ent->cameraangle[1])
	{
		ent->s.angles[1] += ent->cameraangle[1];
	}

	if (ent->cameraangle[2])
	{
		ent->s.angles[2] += ent->cameraangle[2];
	}

    FollowEnt (ent);

	VectorCopy (ent->s.angles, level.cut_scene_angle);
	VectorCopy (ent->s.origin, level.cut_scene_origin);

	VectorCopy (player->s.origin, level.player_oldpos);
	VectorCopy (player->s.angles, level.player_oldang);

	MoveClientToCutSceneCamera (player, ent->deadticks);
}

void NewCutSceneCamera (edict_t *ent)
{
	edict_t *player;

	player = g_edicts + 1;

	level.cut_scene_camera_switch = 1;

	VectorCopy(ent->s.angles, ent->save_avel);
	VectorCopy(ent->s.origin, ent->savecameraorigin);

	level.cut_scene_time = level.time + 0.1;

	// JOSEPH 19-MAR-99-B
	ent->alphalevel = level.time + ent->reactdelay;
	// END JOSEPH

	if (ent->cameraangle[0])
	{
		ent->s.angles[0] += ent->cameraangle[0];
	}

	if (ent->cameraangle[1])
	{
		ent->s.angles[1] += ent->cameraangle[1];
	}

	if (ent->cameraangle[2])
	{
		ent->s.angles[2] += ent->cameraangle[2];
	}

    FollowEnt (ent);

	VectorCopy (ent->s.angles, level.cut_scene_angle);
	VectorCopy (ent->s.origin, level.cut_scene_origin);

	MoveClientToCutSceneCamera (player, ent->deadticks);
}

void AdjustCutSceneCamera(edict_t *ent)
{
	edict_t *player;

	player = g_edicts + 1;

	if (ent->accel)
	{
		if (ent->delay < (level.time - ent->timestamp))
		{
			if ((ent->delay + ent->reactdelay) < (level.time - ent->timestamp))
			{
				if (ent->speed > 0)
				{
					ent->speed -= ent->decel * FRAMETIME;

					if (ent->speed < 0)
						ent->speed = 0;
				}
			}
			else if (ent->speed < 1)
			{
				ent->speed += ent->accel * FRAMETIME;
				if (ent->speed > 1)
					ent->speed = 1;
			}
		}
	}

    level.cut_scene_camera_switch = 0;

	level.cut_scene_time = level.time + 0.1;

	if (ent->rotate[0])
	{
		ent->s.angles[0] += ent->rotate[0]*FRAMETIME*ent->speed;
	}

	if (ent->rotate[1])
	{
		ent->s.angles[1] += ent->rotate[1]*FRAMETIME*ent->speed;
	}

	if (ent->rotate[2])
	{
		ent->s.angles[2] += ent->rotate[2]*FRAMETIME*ent->speed;
	}

	// Ridah, not sure what ->save_avel is, but we need a way of setting an absolute velocity
	VectorMA(ent->s.origin, FRAMETIME*ent->speed, ent->cameravel, ent->s.origin);
/*
	if (ent->cameravel[0])
	{
    	vec3_t	forward;

	    AngleVectors (ent->save_avel, forward, NULL, NULL);
		VectorMA(ent->s.origin, ent->cameravel[0]*FRAMETIME, forward, ent->s.origin);
	}

	if (ent->cameravel[1])
	{
    	vec3_t	right;

	    AngleVectors (ent->save_avel, NULL, right, NULL);
		VectorMA(ent->s.origin, ent->cameravel[1]*FRAMETIME, right, ent->s.origin);
	}

	if (ent->cameravel[2])
	{
    	vec3_t	up;

	    AngleVectors (ent->save_avel, NULL, NULL, up);
		VectorMA(ent->s.origin, ent->cameravel[2]*FRAMETIME, up, ent->s.origin);
	}
*/
	if (ent->cameravelrel[0])
	{
    	vec3_t	forward;

	    AngleVectors (ent->s.angles, forward, NULL, NULL);
		VectorMA(ent->s.origin, ent->cameravelrel[0]*FRAMETIME*ent->speed, forward, ent->s.origin);
	}

	if (ent->cameravelrel[1])
	{
    	vec3_t	right;

	    AngleVectors (ent->s.angles, NULL, right, NULL);
		VectorMA(ent->s.origin, ent->cameravelrel[1]*FRAMETIME*ent->speed, right, ent->s.origin);
	}

	if (ent->cameravelrel[2])
	{
    	vec3_t	up;

	    AngleVectors (ent->s.angles, NULL, NULL, up);
		VectorMA(ent->s.origin, ent->cameravelrel[2]*FRAMETIME*ent->speed, up, ent->s.origin);
	}

    FollowEnt (ent);

	VectorCopy (ent->s.angles, level.cut_scene_angle);
	VectorCopy (ent->s.origin, level.cut_scene_origin);

    //VectorCopy (ent->s.origin, ent->origin);
    //VectorCopy (ent->s.origin, ent->oldorigin);
	//VectorCopy (player->s.origin, level.player_oldpos);
	//VectorCopy (player->s.angles, level.player_oldang);

	// JOSEPH 19-MAR-99-B
	// Fade out
	if (ent->duration)
	{
		if ((level.time + ent->duration) > ent->alphalevel)
		{
			level.totalfade = ent->duration;
			level.fadeendtime = level.time + level.totalfade;
			level.inversefade = 1;
			ent->duration = 0;
		}
	}
    // END JOSEPH

    MoveClientToCutSceneCamera (player, ent->deadticks);
}

void EndCutScene (edict_t *ent)
{
	edict_t *player;

	player = g_edicts + 1;

    level.cut_scene_camera_switch = 0;

    player->client->ps.fov = 90;

	VectorCopy (level.player_oldpos, player->s.origin);
	VectorCopy (level.player_oldang, player->s.angles);

	// JOSEPH 24-FEB-99
    level.cut_scene_camera_switch = 1;
	level.cut_scene_end_count = 5;
	// END JOSEPH

	level.cut_scene_time = 0;

	player->client->ps.pmove.origin[0] = player->s.origin[0]*8;
	player->client->ps.pmove.origin[1] = player->s.origin[1]*8;
	player->client->ps.pmove.origin[2] = player->s.origin[2]*8;

	VectorCopy (level.player_oldang, player->client->ps.viewangles);


	if (!player->client->pers.weapon)
		return;

	{
		int i;


		if (!strcmp (player->client->pers.weapon->pickup_name , "SPistol"))
		{
			if ((player->client->pers.pistol_mods) && (player->client->pers.inventory[ITEM_INDEX(FindItem("Pistol Magnum Mod"))]))
				player->client->ps.gunindex = gi.modelindex("models/weapons/silencer_mdx/magnum.mdx");
			else
				player->client->ps.gunindex = gi.modelindex(player->client->pers.weapon->view_model);
		}
		else if (!strcmp (player->client->pers.weapon->pickup_name , "Pistol"))
		{
			if ((player->client->pers.pistol_mods) && (player->client->pers.inventory[ITEM_INDEX(FindItem("Pistol Magnum Mod"))]))
				player->client->ps.gunindex = gi.modelindex("models/weapons/v_colt/magnum.mdx");
			else
				player->client->ps.gunindex = gi.modelindex(player->client->pers.weapon->view_model);
		}
		else if (!strcmp (player->client->pers.weapon->pickup_name, "Heavy machinegun"))
		{
			if (player->client->pers.inventory[ITEM_INDEX(FindItem("HMG Cooling Mod"))])
				player->client->ps.gunindex = gi.modelindex("models/weapons/v_hmgcool/hmgcool.mdx");
			else
				player->client->ps.gunindex = gi.modelindex(player->client->pers.weapon->view_model);
		}
		else
			player->client->ps.gunindex = gi.modelindex(player->client->pers.weapon->view_model);


		memset(&(player->client->ps.model_parts[0]), 0, sizeof(model_part_t) * MAX_MODEL_PARTS);

		player->client->ps.num_parts++;
		// player->client->ps.model_parts[PART_HEAD].modelindex = gi.modelindex(player->client->pers.weapon->view_model);
		player->client->ps.model_parts[PART_HEAD].modelindex = player->client->ps.gunindex;
		for (i=0; i<MAX_MODELPART_OBJECTS; i++)
			player->client->ps.model_parts[PART_HEAD].skinnum[i] = 0; // will we have more than one skin???

		// HACK, set arm model
		if (!strcmp(player->client->pers.weapon->pickup_name, "Shotgun"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/shotgun/hand.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_LEGS].modelindex = gi.modelindex("models/weapons/shotgun/shell.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_LEGS].skinnum[i] = 0; // will we have more than one skin???
		}
		else if (!strcmp(player->client->pers.weapon->pickup_name, "Pipe"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/blakjak/hand.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???
		}
		else if (!strcmp(player->client->pers.weapon->pickup_name, "Crowbar"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/crowbar/hand.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???
		}
		else if (!strcmp(player->client->pers.weapon->pickup_name, "FlameThrower"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/v_flamegun/hand.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???
		}
		// JOSEPH 4-MAR-99
		else if (!strcmp(player->client->pers.weapon->pickup_name, "Bazooka"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/v_rocket_launcher/hand.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_LEGS].modelindex = gi.modelindex("models/weapons/v_rocket_launcher/shell_a.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_LEGS].skinnum[i] = 0; // will we have more than one skin???


			/*
			// note to self: not using this anymore
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_GUN].modelindex = gi.modelindex("models/weapons/v_rocket_launcher/shell_b.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_GUN].skinnum[i] = 0; // will we have more than one skin???
			*/

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_GUN2].modelindex = gi.modelindex("models/weapons/v_rocket_launcher/clip.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_GUN2].skinnum[i] = 0; // will we have more than one skin???


		}
		// END JOSEPH
		else if (!strcmp (player->client->pers.weapon->pickup_name , "SPistol"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/silencer_mdx/handl.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_LEGS].modelindex = gi.modelindex("models/weapons/silencer_mdx/handr.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_LEGS].skinnum[i] = 0; // will we have more than one skin???

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_GUN].modelindex = gi.modelindex("models/weapons/silencer_mdx/clip.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_GUN].skinnum[i] = 0;

		}
		else if (!strcmp (player->client->pers.weapon->pickup_name , "Pistol"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/v_colt/handl.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_LEGS].modelindex = gi.modelindex("models/weapons/v_colt/handr.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_LEGS].skinnum[i] = 0; // will we have more than one skin???

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_GUN].modelindex = gi.modelindex("models/weapons/v_colt/clip.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_GUN].skinnum[i] = 0;
		}
		// JOSEPH 16-APR-99
		else if (!strcmp (player->client->pers.weapon->pickup_name , "Heavy machinegun"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_LEGS].modelindex = gi.modelindex("models/weapons/v_hmg/lhand.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_LEGS].skinnum[i] = 0; // will we have more than one skin???

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/v_hmg/clip.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???

	}
		// END JOSEPH
		else if (!strcmp (player->client->pers.weapon->pickup_name , "Grenade Launcher"))
		{
			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_LEGS].modelindex = gi.modelindex("models/weapons/v_grenade_launcher/lhand.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_LEGS].skinnum[i] = 0; // will we have more than one skin???

			player->client->ps.num_parts++;
			player->client->ps.model_parts[PART_BODY].modelindex = gi.modelindex("models/weapons/v_grenade_launcher/clip.mdx");
			for (i=0; i<MAX_MODELPART_OBJECTS; i++)
				player->client->ps.model_parts[PART_BODY].skinnum[i] = 0; // will we have more than one skin???

		}

	}

// 	player->client->ps.gunindex = gi.modelindex(player->client->pers.weapon->view_model);
}
// END JOSEPH

void MoveClientToCutScene (edict_t *ent)
{
	VectorCopy (level.cut_scene_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.cut_scene_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.cut_scene_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.cut_scene_origin[2]*8;
	VectorCopy (level.cut_scene_angle, ent->client->ps.viewangles);
	ent->client->ps.pmove.pm_type = PM_FREEZE;

	// note to self
	// this may cause a problem
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	// RAFAEL
	ent->client->quadfire_framenum = 0;

	// RAFAEL
	ent->client->trap_blew_up = false;
	ent->client->trap_time = 0;

	/*
	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.modelindex2 = 0;
	ent->s.modelindex3 = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;
	*/

}

void MoveClientToCutSceneCamera (edict_t *ent, int fov)
{
	ent->client->ps.fov = fov;

	VectorCopy (level.cut_scene_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.cut_scene_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.cut_scene_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.cut_scene_origin[2]*8;
	VectorCopy (level.cut_scene_angle, ent->client->ps.viewangles);
	ent->client->ps.pmove.pm_type = PM_FREEZE;

	// note to self
	// this may cause a problem
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	// RAFAEL
	ent->client->quadfire_framenum = 0;

	// RAFAEL
	ent->client->trap_blew_up = false;
	ent->client->trap_time = 0;

	/*
	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.modelindex2 = 0;
	ent->s.modelindex3 = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;
	*/

}
// END JOSEPH

void MoveClientToPawnoMatic (edict_t *ent)
{
// RAFAEL 03-14-99
// Drew will compensate by moveing the start pos
//	ent->client->ps.fov = 96;

// 	ent->s.origin[2] = level.pawn_origin[2];
	ent->client->ps.pmove.origin[0] = level.cut_scene_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.cut_scene_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.cut_scene_origin[2]*8;

	ent->client->ps.pmove.pm_type = PM_FREEZE;

	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	// RAFAEL
	ent->client->quadfire_framenum = 0;

	// RAFAEL
	ent->client->trap_blew_up = false;
	ent->client->trap_time = 0;


}
