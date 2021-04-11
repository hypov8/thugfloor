#include "g_local.h"
#include "g_cast_spawn.h"

#ifdef BETADEBUG
//hypov8 this should be "project" build dependent release/dev
#define DIRECTSTART 1//FREDZ put on 0 for normal game
#endif

int	 vote_set[9];        // stores votes for next map

char admincode[16];		 // the admincode
char default_map[32];    // default settings
//char default_teamplay[16];
char default_dmflags[16];
char default_password[16];
char default_timelimit[16];
/*
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
	gi.cvar_set("timelimit",default_timelimit);
/*	gi.cvar_set("fraglimit",default_fraglimit);
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
#ifndef BETADEBUG
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

	//finish node dropping
	if (level.nav_TF_autoRoute == 2){
		level.nav_TF_autoRoute = 0; 
		NAV_WriteActiveNodes(level.node_data, level.mapname); //hypov8 note. this normaly writen with nav_dynamic ON
	}

	for_each_player(self,i)
	{
	    if ((int)wavetype->value == WAVETYPE_SHORT)//Short
        {
            switch (level.waveNum)
            {
            case 0:  gi.centerprintf(self,"The wave %i from Skidrow has begun.", level.waveNum + 1);
                break;
            case 1:  gi.centerprintf(self, "The wave %i from Poisonville has begun.", level.waveNum + 1);
                break;
            case 2:  gi.centerprintf(self,"The wave %i from Steeltown has begun.", level.waveNum + 1);
                break;
            case 3:  gi.centerprintf(self,"The wave %i from Radio City has begun.", level.waveNum + 1);
                break;
            case 4: gi.centerprintf(self,"The wave %i from Boss has begun.", level.waveNum + 1);//Crystal Palace?
                break;
            default: //should never happen
                    gi.centerprintf(self,"The wave %i has begun.", level.waveNum + 1);
            }

        }
        else if ((int)wavetype->value == WAVETYPE_MED)//Medium
        {
            switch (level.waveNum)
            {
            case 0:
            case 1:  gi.centerprintf(self,"The wave %i from Skidrow has begun.", level.waveNum + 1);
                break;
            case 2:  gi.centerprintf(self, "The wave %i from Poisonville has begun.", level.waveNum + 1);
                break;
            case 3:
            case 4:  gi.centerprintf(self,"The wave %i from Shipyard has begun.", level.waveNum + 1);
                break;
            case 5:  gi.centerprintf(self,"The wave %i from Trainyard has begun.", level.waveNum + 1);
                break;
            case 6:  gi.centerprintf(self,"The wave %i from Radio City has begun.", level.waveNum + 1);
                break;
            case 7: gi.centerprintf(self,"The wave %i from Boss has begun.", level.waveNum + 1);//Crystal Palace?
                break;
            default: //should never happen
                    gi.centerprintf(self,"The wave %i has begun.", level.waveNum + 1);
            }
        }
        else //WAVETYPE_LONG	//Long
        {
            switch (level.waveNum)
            {
            case 0:
            case 1:  gi.centerprintf(self,"The wave %i from Skidrow has begun.", level.waveNum + 1);
                break;
            case 2:
            case 3:  gi.centerprintf(self, "The wave %i from Poisonville has begun.", level.waveNum + 1);
                break;
            case 4:  gi.centerprintf(self,"The wave %i from Shipyard has begun.", level.waveNum + 1);
                break;
            case 5:
            case 6:  gi.centerprintf(self,"The wave %i from Steeltown has begun.", level.waveNum + 1);
                break;
            case 7:  gi.centerprintf(self,"The wave %i from Trainyard has begun.", level.waveNum + 1);
                break;
            case 8:
            case 9:  gi.centerprintf(self,"The wave %i from Radio City has begun.", level.waveNum + 1);
                break;
            case 10: gi.centerprintf(self,"The wave %i from Boss has begun.", level.waveNum + 1);//Crystal Palace?
                break;
            default: //should never happen
                    gi.centerprintf(self,"The wave %i has begun.", level.waveNum + 1);
            }
        }

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
		}
		//hypov8 start everyone off with 150 cash?
		if (level.waveNum == 0)
			self->client->pers.currentcash = waveGiveCash(0);
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

//tweek cast aim. they are deadly if u stand still, even at skil=0
void TF_cast_AttackSkill(edict_t *cast, vec3_t startPos, vec3_t aimDir, int wepType)
{
	//weptype 0=bullet 1=rocket 2=grenade, 3=flamegun
	int i;
	float skil = ( 1.0f - ( skill->value / 4.0f ) ); //1.0 to 0.0 min
	float rnd = random();
	float delay = ( -0.05f - ( 0.04f * ( rnd * skil ) ) ); //0.050 100% hit 0.075 100% miss
	float result2 = crandom()* 0.1f *( 0.2f* ( rnd * skil ) );
	if (result2 < 0.0f)
		result2 -= ( 0.05f * ( rnd * skil ) );
	else
		result2 += ( 0.05f * ( rnd * skil ) );

	float distMin; //maintain min velocity
	float distMax; //max rand velocity to add
	float rocketDist;
	vec3_t target, shiftVel, dist; //add velocity so vectorMA can do its thing

	if (!cast->enemy)
		return;

	//if (VectorCompare(enemyPos, vec3_origin))
	VectorCopy(cast->enemy->s.origin, target); //move

	
	if (wepType == 0){ //bullets
		distMin = 200;
		distMax = 120;
		target[2] += cast->enemy->viewheight;
	}
	else if (wepType == 1){ //rocket
		distMin = 200;
		distMax = 200;
		target[2] -= 24;
		//result *= 2; //move aim back more..
		delay = ( -0.1f - ( 0.3f * ( rnd * skil ) ) );
	}
	else if (wepType == 2){ //grenade
		distMin = 200;
		distMax = 200;
		target[2] += cast->enemy->viewheight;
	}
	else	//flamegun
	{
		distMin = 200;
		distMax = 400;
		target[2] += cast->enemy->viewheight;
	}

	//copy orig velocity
	VectorCopy(cast->enemy->velocity, shiftVel);

	//dont predict small jump/falls
	if (shiftVel[2] > -200.0f && shiftVel[2] < 350.0f)
		shiftVel[2] = 0.0f;

	//prediction for rockets. use rocket speed/distance
	if (wepType == 1){ 
		VectorSubtract(cast->enemy->s.origin, cast->s.origin, dist);
		rocketDist = VectorDistance(cast->enemy->s.origin, startPos);
		if (rocketDist >300)
			VectorMA(target, rocketDist/900, shiftVel, target);
	}

	for (i = 0; i < 2; i++)
	{
		//move player to fake bad aim..
		if (shiftVel[i] < 300 && shiftVel[i] > -300)
		{
			float xy = ( 320 - ( fabs(shiftVel[i]) ) ) / 320; //speed multiplyer. based on existing speed
			//float rand1 = distMin + ( distMax * skil ) * random(); //add randomness +min value
			float dist1 = distMin + ( distMax*random() ); //add randomness +min value

			if (shiftVel[i] < 0)
				shiftVel[i] -= dist1 * xy * skil;
			else if (shiftVel[i] > 0)
				shiftVel[i] += dist1 * xy * skil;
			else
				shiftVel[i] = crandom() * ( ( distMin*1.5f ) + ( distMax*1.5f* skil) ) ;
		}
		else
			gi.dprintf("to fast %s..\n", (i==0)? "x": "y");	
	}
#if 0
	//move player to fake bad aim..
	if (shiftVel[0] < 300 && shiftVel[0] > -300)
	{
		float x = ( 320 - ( fabs(shiftVel[0]) ) ) / 320; //speed multiplyer. based on existing speed
		//float rand1 = distMin + ( distMax * skil ) * random(); //add randomness +min value
		float dist1 = distMin + ( distMax*random() ); //add randomness +min value
		if (shiftVel[0] < 0)
			shiftVel[0] -= dist1 * x * skil;
		else if (shiftVel[0] > 0)
			shiftVel[0] += dist1 * x * skil;
		else
			shiftVel[0] = crandom() * ( ( distMin*1.5f ) + ( distMax*1.5f* skil) ) ;
	}
	else
		gi.dprintf("to fast x..\n");

	if( shiftVel[1] < 300 && shiftVel[1] > -300)
	{
		float y = (320-(fabs(shiftVel[1])))/320; //speed multiplyer. based on existing speed
		float rand2 = distMin + ( distMax * random() ); //add randomness +min value

		if (shiftVel[1] < 0)
			shiftVel[1] -= rand2 * y * skil;
		else if (shiftVel[1] > 0)
			shiftVel[1] += rand2 * y * skil;
		else
			shiftVel[1] = crandom() * ((distMin*1.5f)+(distMax*1.5f * skil));
	}
	else
	{
		gi.dprintf("to fast y..\n");
	}
#endif

	//shift aim point backwards
	VectorMA(target, delay, shiftVel, target);
	VectorSubtract (target, startPos, aimDir);
	VectorNormalize (aimDir);


	if (wepType == 2){ // lob the grenade
		if (VectorDistance(cast->s.origin, cast->enemy->s.origin) > 512)
			aimDir[2] += 0.4;
	}
}

extern edict_t *boss_entityID;
//give cash to killer. moved here for easy update
void TF_giveCashOnKill(int type , edict_t *cast)
{
    //kf:
    //cyst 7$
    //clot 7$
    //rioter 7$
    //slasher 7$
    //crawler 7 or  10$ same as rat
    //bloat 17$
    //gorefast 12$
    //hust 17$
    //stalker 15$
    //siren 25$
    //scrake 75$
    //fleshpound 200$ //mini boss
    //edar 17$

	int cashOut = 0;

	static const int cashmelee=9;//was 6
	static const int cashpistol=12;//was 8
    static const int cashshotgun=15;//was 10
    static const int cashtommygun=17;//was 10
    static const int cashhmg=30;//was 20
    static const int cashgrenade=21;//was 14
    static const int cashbazooka=24;//was 16
    static const int cashflamethrower=18;//was 12
    static const int cashrat=15;//was 10
    static const int cashdog=10;//was 6
    static const int cashthug_sit=4;//Never used but use default pistol
    static const int cashbum_sit=1;//Never used does not attack
	//FREDZ i think spawn_cash need to be abit higher, but not sure. first wave they can shotgun, tommygun
	//Problem is new players that join in wave cash is abit low maybe?
	//But yeah need maybe more tweaking. added 50% more cash
	//Tested abit looks better now for 1 player
    //Wave 4 already hmg after shotgun bought in wave 2 with new cash
    //Wave 8 endless money
    //Wave 10 full cash 1000 and all weapons`153 kills
    //wave 11 cash 1814 kills 175
    //End 179 kills 1859 cash.


	switch (type)
	{
    case BOT_RAT:			cashOut = cashrat; break;			/*rat*/
    case BOT_DOG:			cashOut = cashdog; break;			/*dog*/
    case BOT_BUM:			cashOut = cashbum_sit; break;		/*bum sit*/
    case BOT_THUG_SIT:		cashOut = cashthug_sit; break;		/*thug sit*/
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
	//return cashOut;

	if (cast == boss_entityID)
	{
		int numWaves = WAVELEN_LONG-1;	//long
			//get wave count
		if ((int)wavetype->value == WAVETYPE_SHORT)		//short
			numWaves =WAVELEN_SHORT-1;
		else if ((int)wavetype->value == WAVETYPE_MED)	//med
			numWaves = WAVELEN_MED-1;
		
		cashOut *= ceil(2.0f + (2.0f * ((float)(level.waveNum) / numWaves))); //2x to 4x
	}

	if (cashOut && cast->max_health > 0)
	{
		int i;
		edict_t *dood;

		for_each_player(dood, i)
		{	//add percent of hits to calculate cash 
			if (cast->damageTF_Counter[i])
			{
				float val = ( (float) cast->damageTF_Counter[i] / cast->max_health ) * cashOut;
				dood->client->pers.currentcash += ceil(val);

				//add score.
				dood->client->resp.score++; //count as 1 hit at any damage	
				if (cast->damageTF_Counter[i] >= (int)floor( cast->max_health / 2 ))
					dood->client->resp.score ++; //count as 2 hits if they took most of the health
			}
		}
	}
	else
	{
		gi.dprintf("WARNING: invalid cashOut(%i) or max_health(%i)\n", cashOut, cast->max_health);
	}
}

/*
================
waveGiveCash

setup giving cash to player
dependent on the type of respawn

type
0= inital game spawn
1= survived round
2= died or just joined game
See https://wiki.killingfloor2.com/index.php?title=Mechanics_(Killing_Floor_2)#Gameplay_-_Dosh
================
*/
int waveGiveCash(int type)
{
	int numWaves;
	int outCash;
	float spawn_cash;
	static const float novice	= 1900.0f / 2.5f;//Not in KF example
	static const float easy		= 1750.0f / 2.5f;//2.5 is to comprise difference
	static const float medium	= 1550.0f / 2.5f;
	static const float hard		= 1700.0f / 2.5f;
	static const float real		= 1550.0f / 2.5f;

	//get wave count
	if ((int)wavetype->value == WAVETYPE_SHORT)		//short
		numWaves = WAVELEN_SHORT;
	else if ((int)wavetype->value == WAVETYPE_MED)	//med
		numWaves = WAVELEN_MED;
	else	//WAVETYPE_LONG							//long
		numWaves = WAVELEN_LONG;

	//use skill value
	if ((int)skill->value == 0)			spawn_cash = novice;
	else if ((int)skill->value == 1)		spawn_cash = easy;
	else if ((int)skill->value == 2)		spawn_cash = medium;
	else if ((int)skill->value == 3)		spawn_cash = hard;
	else								spawn_cash = real;

	if (type == 1) //survived the round
		outCash = (int) ( spawn_cash * ( (float) ( level.waveNum + 1 ) / numWaves ) );
	else if (type == 2) //died
		outCash = (int) (( spawn_cash * ( (float) ( level.waveNum + 1 ) / numWaves ) ) * 0.5f); //halve cash
	else //first round. give standard cash
		outCash = 150;

	return outCash;
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

void TF_WaveEnd_GiveWeapons(edict_t *ent)
{
#if 1 //give back previous wep they died with 
	int i;
	for (i = 0; i < game.num_items; i++)
	{
		if (( itemlist[i].flags & IT_WEAPON ) && ent->client->resp.inventory_TF[i] && ! ent->client->pers.inventory[i] )
		{
			edict_t *it_ent;
			//drop item on client
			it_ent = G_Spawn();			
			//it_ent->classname = strcpy(gi.TagMalloc(strlen(itemlist[i].classname)+1, TAG_LEVEL), itemlist[i].classname);
			it_ent->classname = itemlist[i].classname;
			SpawnItem (it_ent, &itemlist[i]);
			Touch_Item (it_ent, ent, NULL, NULL);
			if (it_ent->inuse)
				G_FreeEdict(it_ent);

			//ent->client->newweapon = &itemlist[i];
			//ChangeWeapon(ent);
			break;
		}
			//self->client->resp.coop_respawn.inventory[n] = self->client->pers.inventory[n];

		//else if (itemlist[n].flags & (IT_WEAPON| IT_AMMO/*|IT_SILENCER|IT_POWERUP*/)) //TF keep weps?
		//	self->client->resp.inventory_TF[n] = self->client->pers.inventory[n];

		//self->client->pers.inventory[n] = 0;
		//	int itmIdx = ITEM_INDEX(self->client->pers.weapon);
		//self->client->resp.inventory_TF[itmIdx] = self->client->pers.inventory[itmIdx];
	}




#else//give free weps
	gitem_t		*item, *ammo;
	int i, id_item, id_ammo, gaveItem = 0;
	char *weps[3] = {"Pistol Magnum Mod", "Shotgun", "Tommygun"};
	int givWave[3] = { 2, 3, 4}; //end of wave # to give the weapon
	char msg[256];
	msg[0] = 0;
	strcpy(msg, "You recieved a free");

	for (i = 0; i < 3; i++)
	{
		if (level.waveNum < givWave[i])
			continue;

		item = FindItem(weps[i]);
		id_item = ITEM_INDEX(item);
		if (id_item && !ent->client->pers.inventory[id_item])
		{
			gaveItem = (1 << i);
			strcat(msg, "\n");
			strcat(msg, weps[i]);

			ent->client->pers.inventory[id_item] = 1;
			if (item->ammo)
			{
				ammo = FindItem(item->ammo);
				id_ammo = ITEM_INDEX(ammo);
				ent->client->pers.inventory[id_ammo] = ammo->quantity;
				if (ent->client->pers.inventory[id_ammo] > ent->client->pers.max_bullets)
					ent->client->pers.inventory[id_ammo] = ent->client->pers.max_bullets;
			}

			if ( i==0 && ent->client->pers.spectator == PLAYING &&
				(ent->client->pers.weapon == FindItem ("Pistol Silencer") 
				|| ent->client->pers.weapon == FindItem("Pistol")))
			{
				ent->client->newweapon = ent->client->pers.weapon;
				ChangeWeapon (ent);
			}
		}
	}

	if (gaveItem)
	{
		strcat(msg, "\n");
		gi.cprintf(ent, PRINT_CHAT, msg);
	}
#endif
}

void WaveEnd () //hypov8 end of the match
{
	edict_t *self;
	int     i;
	int     count_players = 0;

	if (level.nav_TF_autoRoute)
	{
		level.nav_shownode = 0;
		gi.cvar_set("nav_dynamic", "0");
		level.waveNum = 0;
		level.nav_TF_autoRoute = 2; //reset at WaveStart(). let nav_dynamic run for a bit to clean node table
	}

    level.waveNum++;
	level.modeset = WAVE_BUYZONE;
	cast_TF_free(); //set me free!!

	for_each_player(self,i)
	{
		if (self->client->pers.spectator != SPECTATING)
		{
			count_players++;
			self->check_idle = level.framenum; //reset idle timer. this has changed...
		}

		//give cash to ppl that survived the wave
		if (self->client->pers.spectator == PLAYING)
		{
			self->client->pers.currentcash += waveGiveCash(1);
			//TF_WaveEnd_GiveWeapons(self);
		}
		//spawn players into buying time
		else if (self->client->pers.spectator == PLAYER_READY)
		{
            self->flags &= ~FL_GODMODE;
            self->health = 0;
            meansOfDeath = MOD_RESTART;
            ClientBeginDeathmatch( self );
			if (self->client->pers.player_died)
				self->client->pers.currentcash += waveGiveCash(2); //died
			else
				self->client->pers.currentcash += waveGiveCash(1); //spec
		}

		if (level.nav_TF_autoRoute)//add money to all players
			self->client->pers.currentcash = 200 + waveGiveCash(1); //bonus!!

		//reset player died mid wave. deny cash
		self->client->pers.player_died = 0;

		if (self->client->showscores == SCORE_TF_HUD) //todo show players?
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
	edict_t *self;
	int     i;

	level.modeset = WAVE_BUYZONE;
	level.startframe = level.framenum;
	gi.bprintf (PRINT_HIGH,"Buy zone for wave %i will end in 60 seconds.\n", level.waveNum+1);
    G_ClearUp (NULL, FOFS(classname));

	//ding ding ding?
	gi.WriteByte(svc_stufftext);
	gi.WriteString("play world/ding.wav\n");
	gi.multicast(vec3_origin, MULTICAST_ALL);

    level.buyzone = true;//FREDZ stop shooting

    //Show msgs
	if (level.waveNum < 5) //only show in early rounds
	{	for_each_player(self, i)
		{
			if (self->client->pers.spectator == PLAYING && self->client->showscores != INFO_BUYZONE) 
			{
				self->client->showscores = INFO_BUYZONE;
				self->client->resp.scoreboard_frame = 0;
			}
		}
	}

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
		if (level.nav_TF_autoRoute)
		{
			cast_TF_free();
			WaveStart ();
		}
		else
		{
			gi.bprintf(PRINT_HIGH, "The server is now ready to start a wave.\n");
			WaveStart_Countdown();
		}
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

    int framenum = level.framenum - level.startframe;

	if (framenum >= 150)
//	if (level.framenum >= level.startframe + 145)
	{
		WaveStart ();
		return;
	}

	//stop counter if nobody wants to play
	for_each_player(self,i)
	{
		if (self->client->pers.spectator != SPECTATING)
			count_players++;

		TF_ScoreBoard(self); //TF: update mini hud
	}

	if (!count_players)
	{
		if (level.waveNum == 0)
		{
			WaveIdle();
		}
		else
		{
			gi.bprintf(PRINT_HIGH, "Players not playing, other map.\n");
			GameEND();
		}
		return;
	}

//	if ((level.framenum % 10 == 0 ) && (level.framenum > level.startframe + 105)) //150-45
//		gi.bprintf(PRINT_HIGH,"Wave %i will start in %d seconds!\n", level.waveNum + 1, (150 - (level.framenum - level.startframe)) / 10);

    if ((framenum % 10 == 0 ) && (framenum > 99))
	{
        gi.bprintf(PRINT_HIGH,"Wave %i will start in %d seconds!\n", level.waveNum + 1, (150 - framenum) / 10);
		gi.WriteByte(svc_stufftext);
		gi.WriteString("play world/pawnomatic/menubuzz.wav\n");
		gi.multicast(vec3_origin, MULTICAST_ALL);
	}
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

		for_each_player(self, i)
		{
			count++;
		}

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
		else if (level.dmSpawnPointCount < 4)
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

#if 0 // BETADEBUG
	if (level.framenum >= level.startframe + 30)//3 seconds
#else
	if (level.framenum >= level.startframe + 595)//60 seconds
#endif
	{	//remove scoreboard
		for_each_player(self, i)
		{
			if (self->client->showscores == INFO_BUYZONE)
				self->client->showscores = NO_SCOREBOARD;
		}

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
//				gi.WriteByte(TE_BUBBLETRAIL);//Testing but to confusing
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
	if ((int)wavetype->value == WAVETYPE_SHORT) {		//short
		if (level.waveNum == WAVELEN_SHORT -1)
			return  1;
	}
	else if ((int)wavetype->value == WAVETYPE_MED) {	//med
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
                if (self->client->showscores == INFO_WIN_GAME)
                    continue;
                self->client->showscores = INFO_WIN_GAME;
                self->client->resp.scoreboard_frame = 0;
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
		if (skill->latched_string) //hypov8 force skill changes
			Com_sprintf (command, sizeof(command), "map \"%s\"\n", custom_list[vote_set[wining_map]].custom_map);
		else
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
			case VOTE_ON_MAP:
				gi.bprintf(PRINT_HIGH, "The request for a map change has failed\n");
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

