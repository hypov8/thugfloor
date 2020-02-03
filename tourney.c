#include "g_local.h"

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

char custom_map_filename[32];  // stores where various files can be found
char ban_name_filename[32];
char ban_ip_filename[32];
int allow_map_voting;
int wait_for_players; // MH: wait for players
int disable_admin_voting;
int scoreboard_first;
int fph_scoreboard;
int num_custom_maps;
int num_netnames;
int num_ips;

int fixed_gametype;
int enable_password;
char rconx_file[32];
//char server_url[64];
int num_rconx_pass;
int keep_admin_status;
int default_random_map;
int disable_anon_text;
int disable_curse;
//int enable_asc;
int unlimited_curse;
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
/*//FREDZ team modus only
void PublicSetup ()  // returns the server into ffa mode and resets all the cvars (settings)
{
	edict_t		*self;
	int			i;

	level.modeset = FREEFORALL;
	gi.cvar_set("dmflags",default_dmflags);
//	gi.cvar_set("teamplay",default_teamplay);
	gi.cvar_set("password",default_password);
/*	gi.cvar_set("timelimit",default_timelimit);
	gi.cvar_set("fraglimit",default_fraglimit);
	gi.cvar_set("cashlimit",default_cashlimit);*/
/*	gi.cvar_set("dm_realmode",default_dm_realmode);
	gi.cvar_set("anti_spawncamp", "1");//FREDZ
	level.startframe = level.framenum;
	for_each_player (self,i)
	{
		self->flags &= ~FL_GODMODE;
		self->health = 0;
		meansOfDeath = MOD_RESTART;
//		player_die (self, self, self, 1, vec3_origin, 0, 0);

		ClientBeginDeathmatch( self );
	}

	gi.bprintf(PRINT_HIGH,"The server is once again public.\n");
}
*/
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
void ResetServer () // completely resets the server including map
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
	gi.cvar_set("anti_spawncamp", "1");//FREDZ
	gi.cvar_set("cheats","0");

	if (default_random_map && num_custom_maps)
		Com_sprintf (command, sizeof(command), "map \"%s\"\n", custom_list[rand()%num_custom_maps].custom_map);
	else
		Com_sprintf (command, sizeof(command), "map \"%s\"\n", default_map);
	gi.AddCommandString (command);
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
//        if (!self->client->resp.is_spawn)//FREDZ give problems to join
		{
            if (self->client->pers.spectator != PLAYING) //hypov8
            {
                self->flags &= ~FL_GODMODE;
                self->health = 0;
                meansOfDeath = MOD_RESTART;
                team1 = true;
                self->client->pers.spectator = PLAYING;
    //			player_die (self, self, self, 1, vec3_origin, 0, 0);
                ClientBeginDeathmatch( self );
                self->client->resp.is_spawn = true;
                self->client->pers.player_dead = FALSE;//FREDZ
                break;
            }
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

void Start_Match () // Starts the match
{
	edict_t		*self;
	int			i;

	level.startframe = level.framenum;
    level.modeset = WAVE_SPAWN_PLYR;
	level.is_spawn = false;
	for_each_player(self,i)
	{
        if (level.waveNum == 1)//FREDZ not working?
			self->client->pers.currentcash += 300;//Give some cash

		gi.centerprintf(self,"The wave has begun.");
		self->client->resp.is_spawn = false;
	}

	gi.WriteByte( svc_stufftext );
	gi.WriteString("play world/pawnbuzz_out.wav");
	gi.multicast (vec3_origin, MULTICAST_ALL);

}

/*
void Start_Pub () // Starts a pub
{
	edict_t		*self;
	int			i;

	level.startframe = level.framenum;
	level.modeset = WAVE_SPAWN_PLYR;
	level.is_spawn = false;
	for_each_player(self,i)
	{
		gi.centerprintf(self,"Let the killing Begin!");
		self->client->resp.is_spawn = false;
	}

	gi.WriteByte( svc_stufftext );
	gi.WriteString("play world/pawnbuzz_out.wav");
	gi.multicast (vec3_origin, MULTICAST_ALL);
}*/
void SetupMapVote () // at the end of a level - starts the vote for the next map
{
	edict_t *self;
	int i;
/*	int		found;
	int		i,j,k;
	int		unique;
	int		selection;
*/

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
void GameEND ()//FREDZ
{
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

	level.modeset = WAVE_END;

	for_each_player(self,i)
	{
		if (self->client->pers.spectator != SPECTATING)
			count_players++;
	}

	if (!count_players)
	{
		gi.bprintf(PRINT_HIGH, "Players need to join to start a wave.\n");
		//put server in idle.
		WaveIdle();
	}
	else
		WaveStart();
}


void WaveStart()  // start the match
{
	level.player_num = 0;
	level.modeset = WAVE_START;
	level.startframe = level.framenum;
	gi.bprintf (PRINT_HIGH,"Wave %i will start in 15 seconds.\n", level.waveNum + 1);
    G_ClearUp (NULL, FOFS(classname));

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
		ResetServer ();

	if (count_players)
	{
		gi.bprintf(PRINT_HIGH,"The server is now ready to start a wave.\n");
		WaveStart();
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
		level.modeset = WAVE_ACTIVE;

}
/*
void CheckIdleMatchSetup () // restart the server if its empty in matchsetup mode
{
	int		count=0;
	int		i;
	edict_t	*doot;

	for_each_player (doot,i)
		count++;
	if (count == 0)
		ResetServer ();
}
*/

void CheckStartWave ()  // 15 countdown before matches
{
    edict_t *self;
	int      i;
	int      count_players = 0;

	if (level.framenum >= level.startframe + 145)
	{
		Start_Match ();
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
//		gi.bprintf(PRINT_HIGH,"Players need to join to start a wave.\n");
//		WaveIdle();
        gi.bprintf(PRINT_HIGH,"Players not playing, other map.\n");
        GameEND ();
		return;
	}

	if ((level.framenum % 10 == 0 ) && (level.framenum > level.startframe + 49))
		gi.bprintf(PRINT_HIGH,"Wave %i will start in %d seconds!\n", level.waveNum + 1, (140 - (level.framenum - level.startframe)) / 10);
}


void CheckStartPub () // 30 second countdown before server starts (MH: reduced from 35s)
{

	if (level.framenum >= 300)
	{
		edict_t		*self;
        int         i;
		int         count = 0;

		for_each_player(self,i)
			count++;

		if (!count)
			gi.bprintf(PRINT_HIGH, "Players need to join to start a wave.\n");

		WaveIdle();

		return;
	}
}
void CheckEndGame ()//FREDZ only starts if people have joined
{
    edict_t *self;
    int		i;
    int     count_players = 0;
    int     count_players_inserver = 0;
    int     count_players_dead = 0;

    for_each_player (self,i)
    {
        if (self->client->pers.player_dead = FALSE)
            count_players_dead++;

        if (self->client->pers.spectator != SPECTATING)
			count_players++;

        count_players_inserver++;
    }

	if (count_players_inserver == 0)
		ResetServer ();

	if (!count_players)
	{
		gi.bprintf(PRINT_HIGH, "No players anymore playing.\n");
		GameEND ();
		return;
	}

    if (count_players_dead == count_players)
    {
        gi.bprintf(PRINT_HIGH, "All players dead, new map.\n");
        GameEND ();
    }

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
	int		i,count[9];
	edict_t *player;
	char	command[64];
	int		wining_map;

	if (level.framenum > (level.startframe + 270)) // MH: changed voting time to 27s (music duration)
	{
		memset (&count, 0, sizeof(count));
		for_each_player(player,i)
		{
			count[player->vote]++;
		}
		wining_map = 1;
		for (i = 2;i < 9 ; i++)
		{
			// MH: removed rank counting
			if (count[i] > count[wining_map])
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

