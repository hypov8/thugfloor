#include "g_local.h"
#include "g_cast_spawn.h"

//#ifdef BETADEBUG
//hypov8 this should be "project" build dependent release/dev
#define DIRECTSTART 1//FREDZ put on 0 for normal game
//#endif

int	 vote_set[9];        // stores votes for next map

char admincode[16];		 // the admincode
char default_map[32];    // default settings
//char default_teamplay[16];
char default_dmflags[16];
char default_password[16];
/*
char default_timelimit[16];
char default_cashlimit[16];
char default_fraglimit[16];*/
char default_dm_realmode[16];
char default_wavetype[16];

char custom_map_filename[32];  // stores where various files can be found
char ban_name_filename[32];
char ban_ip_filename[32];
int allow_map_voting;
int wait_for_players; // MH: wait for players
int disable_admin_voting;
int scoreboard_first;
//int fph_scoreboard;//Never used
int num_custom_maps;
int num_netnames;
int num_ips;

int fixed_gametype;
int fixed_skilltype;//FREDZ
int enable_password;
char rconx_file[32];
//char server_url[64];
int num_rconx_pass;
int keep_admin_status;
int default_random_map;
int disable_anon_text;
//int disable_curse;
//int enable_asc;
//int unlimited_curse;
int enable_killerhealth;


MOTD_t	MOTD[20];

player_t playerlist[64];

ban_t	netname[100];
ban_t	ip[100];

ban_t	rconx_pass[100];

// MH: get the current admin
edict_t *GetAdmin()
{
	int		i;
	edict_t	*doot;

	for_each_player(doot, i)
	{
		if (doot->client->pers.admin > NOT_ADMIN)
			return doot;
	}
	return NULL;
}

//==============================================================
//
// Papa - This file contains all the functions that control the
//        modes a server may be in.
//
//===============================================================
/*
void MatchSetup () // Places the server in prematch mode
{
	edict_t		*self;
	int			i;
	int         count = 0;

    level.modeset = WAVE_SPAWN_PLYR;
	level.startframe = level.framenum;


	for_each_player (self,i)//FREDZ put everybody in spec
	{
		if (self->client->pers.spectator == SPECTATING)
			continue;

		meansOfDeath = MOD_RESTART;
		self->client->pers.spectator = PLAYER_READY; //hypov8 dont enter a current wave
		self->flags &= ~FL_GODMODE;
		self->health = 0;

		ClientBeginDeathmatch( self );
		count++;
	}

	if (count)//hypov8
	{
		gi.bprintf(PRINT_HIGH,"The server is now ready to setup a wave.\n");
		WaveStart();
	}
	else
	{
		gi.bprintf(PRINT_HIGH,"Players need to join to start a wave.\n");

		//put server into idle.
		level.modeset = WAVE_IDLE;
	}
}
*/
qboolean ResetServer(qboolean ifneeded) // completely resets the server including map
{
	char command[64];

    gi.bprintf (PRINT_HIGH, "Resetting server.\n\n");
	gi.cvar_set("dmflags",default_dmflags);
//	gi.cvar_set("teamplay",default_teamplay);
	gi.cvar_set("password",default_password);
/*	gi.cvar_set("timelimit",default_timelimit);
	gi.cvar_set("fraglimit",default_fraglimit);
	gi.cvar_set("cashlimit",default_cashlimit);*/
	gi.cvar_set("dm_realmode",default_dm_realmode);
	if (default_wavetype[0])
	    gi.cvar_set("wavetype",default_wavetype);	//only apply if keyword exists
	gi.cvar_set("anti_spawncamp", "1");//FREDZ

	// these do
	if (ifneeded
		//&& !(default_teamplay[0] && strcmp(teamplay->string, default_teamplay))
		&& !(default_dm_realmode[0] && strcmp(dm_realmode->string, default_dm_realmode)))
		return false;

	/*if (default_teamplay[0])
		gi.cvar_set("teamplay", default_teamplay);*/
	if (default_dm_realmode[0])
		gi.cvar_set("dm_realmode", default_dm_realmode);
#ifndef HYPODEBUG
	gi.cvar_set("cheats","0");
#endif
	if (default_random_map && num_custom_maps)
		Com_sprintf (command, sizeof(command), "map \"%s\"\n", custom_list[rand()%num_custom_maps].custom_map);
	else
		Com_sprintf (command, sizeof(command), "map \"%s\"\n", default_map);
	gi.AddCommandString (command);
	return true;
}

//int team_startcash[2]={0,0};
/*
void MatchStart()  // start the match
{
	int			i;
	edict_t		*self;

	level.player_num = 0;
	level.modeset = WAVE_START;
	level.startframe = level.framenum;
	gi.bprintf (PRINT_HIGH,"Game is starting in 15 seconds.\n");

//	team_cash[1]=team_startcash[0]; team_startcash[0]=0;
//	team_cash[2]=team_startcash[1]; team_startcash[1]=0;
//	UPDATESCORE

    G_ClearUp (NULL, FOFS(classname));

	for_each_player (self,i)
	{
		self->client->pers.bagcash = 0;
		self->client->resp.deposited = 0;
		self->client->resp.score = 0;
		self->client->pers.currentcash = 0;
		self->client->resp.acchit = self->client->resp.accshot = 0;
		self->client->resp.stole = 0; // MH: added
		self->client->resp.time = 0; // MH: added
		memset(self->client->resp.fav,0,8*sizeof(int));

        if (self->client->pers.spectator == SPECTATING)
            continue;

        meansOfDeath = MOD_RESTART;
        self->client->pers.spectator = SPECTATING;

        self->client->pers.spectator = PLAYER_READY; //hypov8 dont enter a current wave

        self->flags &= ~FL_GODMODE;
        self->health = 0;
        ClientBeginDeathmatch( self );
    }

	gi.WriteByte( svc_stufftext );
	gi.WriteString( va("play world/cypress%i.wav", 2+(rand()%4)) );
	gi.multicast (vec3_origin, MULTICAST_ALL);
}

*/
void SpawnPlayer () // Here I spawn players - 1 per server frame in hopes of reducing overflows
{
	edict_t		*self;
	int			i;
	int			team1=false;

//gi.dprintf("IN: SpawnPlayer\n");

	for (i=0 ; i<maxclients->value; i++)
	{
		self = g_edicts + 1 + i;
		if (!self->inuse)
			continue;
		if (!self->client->resp.is_spawn && self->client->pers.spectator != SPECTATING) // MH: don't spawn spectators
            {
                self->flags &= ~FL_GODMODE;
                self->health = 0;
                meansOfDeath = MOD_RESTART;
				self->client->pers.weaponStore = NULL;
                team1 = true;
                ClientBeginDeathmatch( self );
                self->client->resp.is_spawn = true;
                self->client->pers.player_dead = FALSE;//FREDZ
				self->client->pers.player_died = 0;//reset player died mid wave. deny cash
                break;
            }
		}
	if (!team1)
    {
        level.is_spawn = true;
    }


//gi.dprintf("OUT: SpawnPlayer\n");
}


/*
void SpawnPlayers ()  // Same idea but 1 player per team
{
	edict_t		*self;
	int			i;
	int			team1,team2;

	team1 = false;
	team2 = false;

	for (i=0 ; i<maxclients->value && (!team1 || !team2); i++)
	{
		self = g_edicts + 1 + i;
		if (!self->inuse)
			continue;
		if ((self->client->pers.team == 1) && (!team1) && (!self->client->resp.is_spawn))
		{
			self->flags &= ~FL_GODMODE;
			self->health = 0;
			meansOfDeath = MOD_RESTART;
			team1 = true;
			self->client->pers.spectator = PLAYING;
//			player_die (self, self, self, 1, vec3_origin, 0, 0);
			ClientBeginDeathmatch( self );
			self->client->resp.is_spawn = true;
		}
		if ((self->client->pers.team == 2) && (!team2) && (!self->client->resp.is_spawn))
		{
			self->flags &= ~FL_GODMODE;
			self->health = 0;
			meansOfDeath = MOD_RESTART;
			team2 = true;
			self->client->pers.spectator = PLAYING;
//			player_die (self, self, self, 1, vec3_origin, 0, 0);
			ClientBeginDeathmatch( self );
			self->client->resp.is_spawn = true;
		}
	}
	if ((!team1) && (!team2))
		level.is_spawn = true;
}
*/

void WaveStart () // Starts the match
{
	edict_t		*self;
	int			i;

	cast_TF_setupEnemyCounters();
	level.startframe = level.framenum;
	level.modeset = WAVE_ACTIVE;
	cast_TF_checkEnemyState(); //will spawn 1 enemy

	for_each_player(self,i)
	{
	    if (level.waveNum <2)
            gi.centerprintf(self,"The wave %i from Skidrow has begun.", level.waveNum + 1);
        else if (level.waveNum == 3 || 4)
            gi.centerprintf(self,"The wave %i from Poisonville has begun.", level.waveNum + 1);
        else if (level.waveNum == 5 || 6)
            gi.centerprintf(self,"The wave %i from Shipyard has begun.", level.waveNum + 1);
        else if (level.waveNum == 7)
            gi.centerprintf(self,"The wave %i from Steeltown has begun.", level.waveNum + 1);
        else if (level.waveNum == 8)
            gi.centerprintf(self,"The wave %i from Trainyard has begun.", level.waveNum + 1);
        else if (level.waveNum == 9 || 10)
            gi.centerprintf(self,"The wave %i from Radiocity has begun.", level.waveNum + 1);
        else if (level.waveNum == 11)
            gi.centerprintf(self,"The wave %i from Boss has begun.", level.waveNum + 1);
        else
            gi.centerprintf(self,"The wave %i has begun.", level.waveNum + 1);

		//hypov8 end buy menu.
		if (self->client->pers.spectator == PLAYING)
		{
			self->client->showscores = NO_SCOREBOARD;
			self->client->showhelp = false;
			self->client->showinventory = false;
			self->client->showscrollmenu = false;
			//reset buy menu. did you want to let ppl buy after the 60 secs?
			self->current_menu_side = 0;
			self->current_menu_left = 0;
			self->current_menu_right = 0;

			//give cash to ppl entering a new game
			if (level.waveNum == 0)
				self->client->pers.currentcash = 150; //waveGiveCash(1);
		}
	}

	gi.WriteByte( svc_stufftext );
	gi.WriteString("play world/pawnbuzz_out.wav");
	gi.multicast (vec3_origin, MULTICAST_ALL);

}

void SetupMapVote () // at the end of a level - starts the vote for the next map
{
	edict_t *self;
	int i;

	level.modeset = ENDGAMEVOTE;
	level.startframe = level.framenum;

	for_each_player (self,i)
	{
		HideWeapon(self);
		if (self->client->flashlight)
            self->client->flashlight = false;
		// MH: stuff not needed for intermission removed from here
	}

	// MH: music clip played in BeginIntermission
	BeginIntermission(NULL, ""); // MH: move everyone to intermission
}
/*
void MatchEnd () // end of the match
{
	edict_t *self;
	int i;

	level.modeset = MATCHSETUP;
	level.startframe = level.framenum;
	for_each_player(self,i)
	{
		HideWeapon(self);
		if (self->client->flashlight) self->client->flashlight = false;
		gi.centerprintf(self,"The Match has ended!");
		self->flags &= ~FL_GODMODE;
		self->health = 0;
		meansOfDeath = MOD_RESTART;
//		player_die (self, self, self, 1, vec3_origin, 0, 0);

		ClientBeginDeathmatch( self );
	}
	gi.WriteByte( svc_stufftext );
	gi.WriteString( va("play world/cypress%i.wav", 2+(rand()%4)) );
	gi.multicast (vec3_origin, MULTICAST_ALL);

}
*/

//give cash to killer. moved here for easy update
int giveCashOnKill(int type)
{
	int cashOut = 0;

	int cashmelee=6;
	int cashpistol=8;
    int cashshotgun=10;
    int cashtommygun=10;
    int cashhmg=20;
    int cashgrenade=14;
    int cashbazooka=16;
    int cashflamethrower=12;

	switch (type)
	{
    case BOT_RAT:			cashOut = 10; break;		/*rat*/
    case BOT_DOG:			cashOut = 6; break;		/*dog*/
    case BOT_BUM:			cashOut = 1; break;		/*bum sit*/
    case BOT_THUG_SIT:		cashOut = 4; break;		/*thug sit*/
	case BOT_BITCH_ME:		cashOut = cashmelee; break;		    /*bitch melee*/
	case BOT_BITCH:			cashOut = cashpistol; break;	    /*bitch*/
	case BOT_PUNK_SG:		cashOut = cashshotgun; break;		/*punk SG*/
	case BOT_PUNK_TG:		cashOut = cashtommygun; break;		/*punk TG*/
	case BOT_PUNK_HMG:		cashOut = cashhmg; break;	        /*punk HMG*/
	case BOT_PUNK_RL:		cashOut = cashbazooka; break;		/*punk RL*/
	case BOT_PUNK_FL:		cashOut = cashflamethrower; break;	/*punk FL*/
	case BOT_PUNK_GL:		cashOut = cashgrenade; break;		/*punk GL*/
	case BOT_PUNK_ME:		cashOut = cashmelee; break;		    /*punk melee*/
	case BOT_RUNT_ME:		cashOut = cashmelee; break;		    /*runt melee*/
	case BOT_RUNT:			cashOut = cashpistol; break;		/*runt*/
	case BOT_SHORTY_SG:		cashOut = cashshotgun; break;		/*shorty SG*/
	case BOT_SHORTY_TG:		cashOut = cashtommygun; break;		/*shorty TG*/
	case BOT_SHORTY_HMG:	cashOut = cashhmg; break;	        /*shorty HMG*/
	case BOT_SHORTY_RL:		cashOut = cashbazooka; break;		/*shorty RL*/
	case BOT_SHORTY_FL:		cashOut = cashflamethrower; break;	/*shorty FL*/
	case BOT_SHORTY_GL:		cashOut = cashgrenade; break;		/*shorty GL*/
	case BOT_SHORTY_ME:		cashOut = cashmelee; break;		    /*shorty melee*/
	case BOT_THUG_ME:		cashOut = cashmelee; break;		    /*thug melee*/
	case BOT_THUG:			cashOut = cashpistol; break;		/*thug*/
	case BOT_WHORE_SG:		cashOut = cashshotgun; break;		/*whore SG*/
	case BOT_WHORE_TG:		cashOut = cashtommygun; break;		/*whore TG*/
	case BOT_WHORE_HMG:		cashOut = cashhmg; break;	        /*whore HMG*/
	case BOT_WHORE_RL:		cashOut = cashbazooka; break;		/*whore RL*/
	case BOT_WHORE_FL:		cashOut = cashflamethrower; break;	/*whore FL*/
	case BOT_WHORE_GL:		cashOut = cashgrenade; break;		/*whore GL*/
	case BOT_WHORE_ME:		cashOut = cashmelee; break;		    /*whore melee*/
	}
	return cashOut;
}

/*
================
waveGiveCash

setup giving cash to player
dependent on the type of respawn
0= inital game spawn
1= survived round
2= died or just joined game
See https://wiki.killingfloor2.com/index.php?title=Mechanics_(Killing_Floor_2)#Gameplay_-_Dosh
================
*/
int waveGiveCash(int type)
{
	int numWaves; //hypov8 note: more cash on shorter waves. make this = 11 if not desired
	float spawn_cash;
	float novice	= 1900.0f / 2.5f;//Not in KF example
	float easy		= 1750.0f / 2.5f;//2.5 is to comprise difference
	float medium	= 1550.0f / 2.5f;
	float hard		= 1700.0f / 2.5f;
	float real		= 1550.0f / 2.5f;

	//get wave count
	if ((int)wavetype->value == 0)		//short
		numWaves =WAVELEN_SHORT;
	else if ((int)wavetype->value == 1)	//med
		numWaves = WAVELEN_MED;
	else 								//long
		numWaves = WAVELEN_LONG;

	//use skill value
	if (skill->value == 0)			spawn_cash = novice;
	else if (skill->value == 1)		spawn_cash = easy;
	else if (skill->value == 2)		spawn_cash = medium;
	else if (skill->value == 3)		spawn_cash = hard;
	else							spawn_cash = real;


	switch (type)
	{
	case 1:		//survived the round
		return (int)(spawn_cash * ((float)(level.waveNum + 1) / numWaves));
	case 2:		//dead, respawn with no existing cash.
		return (int)(spawn_cash * ((float)(level.waveNum + 1) / numWaves));
	}

	//first round. give standard cash
	return 50;
}

void GameEND ()//FREDZ
{
	level.modeset = WAVE_IDLE; //dose not matter what it is!!
	cast_TF_free(); //set me free!!

    if (!allow_map_voting)
        EndDMLevel ();
    else
        SetupMapVote();
}
void WaveEnd () //hypov8 end of the match
{
	edict_t *self;
	int     i;
	int     count_players = 0;

    level.waveNum++;
	level.modeset = WAVE_BUYZONE;
	cast_TF_free(); //set me free!!

	for_each_player(self,i)
	{
		if (self->client->pers.spectator != SPECTATING)
		{
			count_players++;
			self->check_idle = level.framenum; //reset idle timer. boot idle players after 55 secs in buy time
		}

		//give cash to ppl that survived the wave
		if (self->client->pers.spectator == PLAYING && !self->client->pers.player_died)
			self->client->pers.currentcash += waveGiveCash(1);



		//spawn players into buying time
		if (self->client->pers.spectator == PLAYER_READY)
		{
            self->flags &= ~FL_GODMODE;
            self->health = 0;
            meansOfDeath = MOD_RESTART;
            ClientBeginDeathmatch( self );
			self->client->pers.currentcash += self->client->pers.player_died? 0 : waveGiveCash(2); //dead or spec
		}

		//reset player died mid wave. deny cash
		self->client->pers.player_died = 0;

		if (self->client->showscores == SCORE_TF_HUD)
			self->client->showscores = NO_SCOREBOARD;
	}

	if (!count_players)
	{
		gi.bprintf(PRINT_HIGH, "Players need to join to start a wave.\n");
		//put server in idle.
		WaveIdle();
	}
	else
    {
        WaveBuy();
    }
}

void WaveBuy()  // start buy zone
{
//	edict_t *self;
//	int     i;

	level.modeset = WAVE_BUYZONE;
	level.startframe = level.framenum;
	gi.bprintf (PRINT_HIGH,"Buy zone for wave %i will end in 60 seconds.\n", level.waveNum+1);
    G_ClearUp (NULL, FOFS(classname));

    level.buyzone = true;//FREDZ stop shooting

	gi.WriteByte( svc_stufftext );
	gi.WriteString( va("play world/cypress%i.wav", 2+(rand()%4)) );
	gi.multicast (vec3_origin, MULTICAST_ALL);

	//call our pawnOmatic
	cast_pawn_o_matic_spawn();

}

void WaveStart_Countdown()  // start the match
{
//	edict_t *player;
//	int     i;

	//free 3 pawnOmatic guys
	cast_pawn_o_matic_free();
	level.modeset = WAVE_START;
	level.startframe = level.framenum;
	gi.bprintf (PRINT_HIGH,"Wave %i will start in 15 seconds.\n", level.waveNum + 1);
    G_ClearUp (NULL, FOFS(classname));

    level.buyzone = false;//FREDZ allow shooting

	gi.WriteByte( svc_stufftext );
	gi.WriteString( va("play world/cypress%i.wav", 2+(rand()%4)) );
	gi.multicast (vec3_origin, MULTICAST_ALL);
}


//hypov8 inital game begin or end of the match with no players
void WaveIdle()
{
	edict_t *self;
	int     i;
	int     count_players = 0;
    int     count_players_inserver=0;

	level.modeset = WAVE_IDLE;

	for_each_player(self,i)
	{
		if (self->client->pers.spectator != SPECTATING)
			count_players++;

        count_players_inserver++;
	}

	if ((count_players_inserver == 0) && (level.framenum > 12000))
		ResetServer (false);

	if (count_players)
	{
		gi.bprintf(PRINT_HIGH,"The server is now ready to start a wave.\n");
		WaveStart_Countdown();
	}
}

void CheckAllPlayersSpawned () // when starting a match this function is called until all the players are in the game
{
	level.startframe = level.framenum; // MH: delay clock until all players have spawned
/*	if (teamplay->value)
		SpawnPlayers ();
	else*/
		SpawnPlayer ();

	if ((level.is_spawn) && (level.modeset == WAVE_SPAWN_PLYR))
		level.modeset = WAVE_IDLE;

}

void CheckStartWave ()  // 15 countdown before matches
{
    edict_t *self;
	int      i;
	int      count_players = 0;

	if (level.framenum >= level.startframe + 145)
	{
		WaveStart ();
		return;
	}
	//stop counter if nobody wants to play
	for_each_player(self,i)
	{
		if (self->client->pers.spectator != SPECTATING)
			count_players++;
	}
	if (!count_players)
	{
	    #if DIRECTSTART
		gi.bprintf(PRINT_HIGH,"Players need to join to start a wave.\n");
		WaveIdle();
		return;
        #else
        gi.bprintf(PRINT_HIGH,"Players not playing, other map.\n");
        GameEND ();
		return;
        #endif
	}

	if ((level.framenum % 10 == 0 ) && (level.framenum > level.startframe + 105)) //150-45
		gi.bprintf(PRINT_HIGH,"Wave %i will start in %d seconds!\n", level.waveNum + 1, (150 - (level.framenum - level.startframe)) / 10);
}


void CheckStartPub () // 30 second countdown before server starts (MH: reduced from 35s)
{

    #if !DIRECTSTART
	if (level.framenum >= 300)
#else
	if (level.framenum >= 60) //hypov8 allow 6 secs to let other players join
    #endif
	{
		edict_t		*self;
        int         i;
		int         count = 0;

		for_each_player(self,i)
			count++;

		if (!count)
			gi.bprintf(PRINT_HIGH, "Players need to join to start a wave.\n");

		level.modeset = WAVE_SPAWN_PLYR; //spawn players straight away

		//make sure we have enough spawns
        if (!level.spSpawnPointCount)
		{
			gi.bprintf(PRINT_HIGH, "ERROR: No single player spawn points in map.\n");
			GameEND();
		}
		if (!level.dmSpawnPointCount)
		{
			gi.bprintf(PRINT_HIGH, "ERROR: No deathmatch spawn points in map.\n");
			GameEND();
		}
		if (level.dmSpawnPointCount < 4)
		{
			gi.bprintf(PRINT_HIGH, "WARNING: Not enough deathmatch spawn points in map.\n");
			//GameEND(); //hypov8 this is not 'required'. bots can spawn ontop of each other
		}

		return;
	}
}
void CheckBuyWave ()
{
    edict_t *self;
    edict_t	*ent = NULL;
	int      i, j;
	int      count_players = 0;
	vec3_t spot1, spot2;

#if 0 //HYPODEBUG
	if (level.framenum >= level.startframe + 100)//10 seconds
#else
	if (level.framenum >= level.startframe + 595)//60 seconds
#endif
	{
		WaveStart_Countdown();
		return;
	}

	//stop counter if nobody wants to play
	for_each_player(self,i)
	{
		float  best = 9999;
		int index = 0;

		if (self->client->pers.spectator != SPECTATING)
			count_players++;

		//display line to pawn guy
		if (self->client->pers.spectator == PLAYING)
		{
			for (j = 0; j < MAX_CHARACTERS; j++)
			{
				if (level.characters[j] && level.characters[j]->svflags & SVF_MONSTER) //cast
				{
					float dist = VectorDistance(self->s.origin, level.characters[j]->s.origin);
					if (dist < best)
					{
						best = dist;
						index = j;
					}
				}
			}

			if (index > 0)
			{
				VectorCopy(self->s.origin, spot1);
				VectorCopy(level.characters[index]->s.origin, spot2);
				spot1[2] += 24;
				spot2[2] += 24;
				//NAV_DrawLine(spot1, spot2); //sends to all players
				gi.WriteByte(svc_temp_entity);
				gi.WriteByte(TE_BFG_LASER);
				gi.WritePosition(spot1);
				gi.WritePosition(spot2);
				gi.unicast(self, false);
			}
		}
	}

	if (!count_players)
	{
        gi.bprintf(PRINT_HIGH,"Players not playing, other map.\n");
        GameEND ();
		return;
	}

}

int CheckEndWave_GameType()
{
	if ((int)wavetype->value == 0) {		//short
		if (level.waveNum == WAVELEN_SHORT -1)
			return  1;
	}
	else if ((int)wavetype->value == 1) {	//med
		if (level.waveNum == WAVELEN_MED -1)
			return 1;
	}
	else {									//long
		if (level.waveNum == WAVELEN_LONG -1)
			return 1;
	}

	return 0;
}

qboolean CheckEndWave() //add timelimit
{
	//check for level cleared
	cast_TF_checkEnemyState();

	if ( level.waveEnemyCount_cur <= 0)
	{
		if (!CheckEndWave_GameType())
		{
			gi.bprintf(PRINT_HIGH, "Wave %i ended.\n", level.waveNum + 1);
			WaveEnd();
			return true;
		}
		else
		{
			edict_t *self;
			int		i;

			for_each_player(self, i)
			{
				gi.cprintf(self, PRINT_HIGH,
					"===========================================================\n"
					"The Boss is now dead.. It's time for a new Kingpin..\n"
					"                       YOU!!!!\n"
					"===========================================================\n"); //todo: better name then players? cutsceen..
			}

			//print only this to ded console
			gi.dprintf("Wave Limit hit\n");

			GameEND();
			return true;
		}
	}
	return false;
}

//void getTeamTags();
/*
void CheckEndMatch () // check if time,frag,cash limits have been reached in a match
{
	int			i;
	int		count=0;
	edict_t	*doot;*/

    // snap - team tags
/*	if(level.framenum % 100 == 0 && !level.manual_tagset){
		getTeamTags();
	}*/
/*
	for_each_player (doot,i)
		count++;
	if (count == 0)
		ResetServer ();*/
/*
	if ((int)fraglimit->value && (int)teamplay->value==4){
		if (team_cash[1]>=(int)fraglimit->value || team_cash[2]>=(int)fraglimit->value) {
			gi.bprintf (PRINT_HIGH, "Fraglimit hit.\n");
			MatchEnd ();
			return;
		}
	}

	if ((int)cashlimit->value)
	{
		if ((team_cash[1] >= (int)cashlimit->value) || (team_cash[2] >= (int)cashlimit->value))
		{
			gi.bprintf (PRINT_HIGH, "Cashlimit hit.\n");
			MatchEnd ();
			return;
		}
	}
*/
/*
	if ((int)timelimit->value) {
		if (level.framenum > (level.startframe + ((int)timelimit->value) * 600 - 1))
		{
			gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
			MatchEnd();
			return;
		}
		if (((level.framenum - level.startframe ) % 10 == 0 ) && (level.framenum > (level.startframe + (((int)timelimit->value  * 600) - 155))))
		{
			gi.bprintf(PRINT_HIGH,"The Match will end in  %d seconds\n", (((int)timelimit->value * 600) + level.startframe - level.framenum ) / 10);
			return;
		}
		if (((level.framenum - level.startframe ) % 600 == 0 ) && (level.framenum > (level.startframe + (((int)timelimit->value * 600) - 3000))))
		{
			gi.bprintf(PRINT_HIGH,"The Match will end in  %d minutes\n", (((int)timelimit->value * 600) + level.startframe - level.framenum ) / 600);
			return;
		}
		if ((level.framenum - level.startframe ) % 3000 == 0 )
			gi.bprintf(PRINT_HIGH,"The Match will end in  %d minutes\n", (((int)timelimit->value * 600) + level.startframe - level.framenum ) / 600);
	}
}
*/
void CheckEndVoteTime () // check the timelimit for voting next level/start next map
{
	int		i,count=0, votes[9];
	edict_t *player;
	char	command[64];
	int		wining_map;

	memset (&votes, 0, sizeof(votes));
	for_each_player(player,i)
	{
		count++;
		votes[player->vote]++;
	}

#if 1 // mm 2.0
	if (!count && level.framenum > (level.lastactive + 30))
	{
		if (ResetServer(true))
			return;
		if (wait_for_players)
		{
			level.startframe = level.framenum;
			level.player_num = 0;
			/*if (team_cash[1] || team_cash[2])
			{
				team_cash[2] = team_cash[1] = 0;
				UpdateScore();
			}*/
			level.lastactive = -1;
			gi.dprintf("Waiting for players\n");
			UpdateTime();
			if (kpded2) // enable kpded2's idle mode for reduced CPU usage while waiting for players (automatically disabled when players join)
				gi.cvar_forceset("g_idle", "1");
		}
	}
#endif

	if (level.framenum > (level.startframe + 270)) // MH: changed voting time to 27s (music duration)
	{
		wining_map = 1;
		for (i = 2;i < 9 ; i++)
		{
			// MH: removed rank counting
			if (votes[i] > votes[wining_map])
				wining_map = i;
		}
		// MH: not writing maplist (no rank to update)

		Com_sprintf (command, sizeof(command), "gamemap \"%s\"\n", custom_list[vote_set[wining_map]].custom_map);
		gi.AddCommandString (command);
	}
}

void CheckVote() // check the timelimit for an admin vote
{

	if (level.framenum > (level.voteframe + 1200))
	{
		switch (level.voteset)
		{
			case VOTE_ON_ADMIN:
				gi.bprintf(PRINT_HIGH,"The request for admin has failed!\n");
				break;
		}
		level.voteset = NO_VOTES;
	}
}



int	CheckNameBan (char *name)
{
	char n[64];
	int i;

	strcpy(n,name);
	kp_strlwr(n); // MH: replaced tolower loop
	for (i=0;i<num_netnames;i++) {
		if (strstr(n,netname[i].value))
			return true;
	}
	return false;
}

// MH: fixed banned IP checking
int	CheckPlayerBan (char *userinfo)
{
	char	*value;
	int		i;

	if (num_netnames)
	{
		value = Info_ValueForKey (userinfo, "name");
		if (CheckNameBan(value))
			return true;
	}

	if (num_ips)
	{
		value = Info_ValueForKey (userinfo, "ip");
		for (i=0; i<num_ips; i++)
			if (!strncmp(value, ip[i].value, strlen(ip[i].value)))
				return true;
	}

	return false;
}

//mm 2.0
void UpdateTime()
{
	char buf[32] = " ";
	if (level.lastactive < 0)
		strcpy(buf, "waiting");
	else if (level.modeset == WAVE_START)
	{
		int t = ((150 - (level.framenum - level.startframe)) / 10);
		sprintf(buf, "start in %d", t);
	}
	else if (level.modeset == PREGAME)
	{
		int t = ((300 - level.framenum) / 10);
		sprintf(buf, "start in %d", t);
	}
	else if ( level.modeset == WAVE_ACTIVE)
	{
		sprintf(buf, "%i enemy left", level.waveEnemyCount_total);
	}
	else if (level.intermissiontime)
		strcpy(buf, "intermission");
	gi.cvar_set(TIMENAME, buf);
}



/*
/////////////////////////////////////////////////////
// snap - team tags
void setTeamName (int team, char *name) // tical's original code :D
{
	// MH: using gi.TagMalloc/TagFree instead of malloc/free
	static int memalloced[3] = {0,0,0};

	if (!name || !*name) return;

	if (strlen(name)<16 && name[0]!=' ')
	{
		if(memalloced[team])
			gi.TagFree(team_names[team]);

		team_names[team] = (char *)gi.TagMalloc(strlen(name) + 1, TAG_GAME);
		if(team_names[team]!=NULL){
			memalloced[team] = 1;
			sprintf(team_names[team], "%s", name);
		}
	}
}
// snap - new function.
void getTeamTags(){

	int			i;
	edict_t		*doot;
	char		names[2][64][16];
	int			namesLen[2] = { 0, 0 };
	char		teamTag[2][12];
	int			teamTagsFound[2]= { FALSE, FALSE };

	for_each_player (doot,i){
		int team = doot->client->pers.team;
		if(team && namesLen[team-1] < 64){
			strcpy(names[team-1][namesLen[team-1]++], doot->client->pers.netname);
		}
	}


	for(i=0;i<2;i++){
		int	j;
		for(j=0;j<namesLen[i] && teamTagsFound[i] == FALSE;j++){
			int	k;
			for(k=0;k<namesLen[i] && j != k && teamTagsFound[i] == FALSE;k++){
				char theTag[12];
				int	theTagNum = 0;
				int	y = 0;
				char s = names[i][j][y];

				while(s != '\0' && theTagNum == 0){
					int	z = 0;
					char t = names[i][k][z];
					while(t != '\0'){
						if(s == t && s != ' '){ // we have a matched char
							int	posY = y+1;
							int	posZ = z+1;
							char ss = names[i][j][posY];
							char tt = names[i][k][posZ];

							while(ss != '\0' && tt != '\0' && ss == tt && theTagNum < 11){
								if(theTagNum == 0){ // we have two consecutive matches, this is a tag
									theTag[theTagNum++] = s;
									theTag[theTagNum++] = ss;
								}
								else{
									theTag[theTagNum++] = ss;
								}
								ss = names[i][j][++posY];
								tt = names[i][k][++posZ];
							}
						}
						t = names[i][k][++z];
					}
					s = names[i][j][++y];
				}
				if(theTagNum > 0){
					int	e;
					float howmany = 0.0;
					float percentage;
					theTag[theTagNum] = '\0';

					for(e=0;e<namesLen[i];e++){
						if(strstr(names[i][e],theTag) != NULL){
							howmany += 1.0;
						}
					}
					percentage = howmany/(float)namesLen[i]*100.0;
					if(percentage > 75.0){
						strcpy(teamTag[i],theTag);
						teamTagsFound[i] = TRUE;
					}
				}
			}
		}
	}


	for(i=0;i<2;i++){
		if(teamTagsFound[i] == TRUE){
			setTeamName(i+1,teamTag[i]);
		}
		else if(i==0){
			setTeamName(i+1, "Dragons");
		}
		else if(i==1){
			setTeamName(i+1, "Nikki's Boyz");
		}
	}
}*/

