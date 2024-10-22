
#include "g_local.h"
//#include "stdlog.h"	//	Standard Logging
//#include "gslog.h"	//	Standard Logging
#include "voice_bitch.h"
#include "voice_punk.h"


game_locals_t	game;
level_locals_t	level;
game_import_t	gi;
game_export_t	globals;
spawn_temp_t	st;

int	sm_meat_index;
int	snd_fry;


int meansOfDeath;

edict_t		*g_edicts;

cast_memory_t	*g_cast_memory;
cast_group_t	*g_cast_groups;

int		num_object_bounds=0;
object_bounds_t	*g_objbnds[MAX_OBJECT_BOUNDS];

cvar_t	*deathmatch;

// RAFAEL
// cvar_t	*marines;

cvar_t	*coop;
cvar_t	*dmflags;
cvar_t	*skill; //skill is 0-4 int. values are rounded down if decimal
cvar_t	*fraglimit;
cvar_t	*timelimit;
cvar_t	*cashlimit;
cvar_t	*password;
cvar_t	*maxclients;
cvar_t	*maxentities;
cvar_t	*g_select_empty;
cvar_t	*dedicated;

cvar_t	*maxrate;

// BEGIN HOOK //FREDZ
cvar_t  *hook_is_homing;
cvar_t  *hook_homing_radius;
cvar_t  *hook_homing_factor;
cvar_t  *hook_players;
cvar_t  *hook_sky;
cvar_t  *hook_min_length;
cvar_t  *hook_max_length;
cvar_t  *hook_pull_speed;
cvar_t  *hook_fire_speed;
cvar_t  *hook_messages;
cvar_t  *hook_hold_time;
// END

cvar_t	*filterban;

cvar_t	*sv_maxvelocity;
cvar_t	*sv_gravity;

cvar_t	*sv_rollspeed;
cvar_t	*sv_rollangle;
cvar_t	*gun_x;
cvar_t	*gun_y;
cvar_t	*gun_z;

cvar_t	*run_pitch;
cvar_t	*run_roll;
cvar_t	*bob_up;
cvar_t	*bob_pitch;
cvar_t	*bob_roll;

cvar_t	*sv_cheats;
cvar_t	*no_spec;
cvar_t	*no_shadows;
cvar_t	*no_zoom; // MH: FOV zooming

cvar_t	*flood_msgs;
cvar_t	*flood_persecond;
cvar_t	*flood_waitdelay;

cvar_t  *kick_flamehack;
cvar_t  *anti_spawncamp;
cvar_t  *idle_client;

// Ridah, new cvar's
cvar_t	*developer;
cvar_t	*ai_debug_memory;

cvar_t	*g_vehicle_test;

cvar_t	*dm_locational_damage;

cvar_t	*showlights;

cvar_t	*r_directional_lighting;

cvar_t	*cl_captions;

cvar_t	*sv_runscale;	// 2.0 = double speed, 0.0 = zero movement

cvar_t	*burn_enabled;
cvar_t	*burn_size;
cvar_t	*burn_intensity;
cvar_t	*burn_r;
cvar_t	*burn_g;
cvar_t	*burn_b;

cvar_t	*timescale;

cvar_t	*wavetype;//hypov8
cvar_t	*nav_dynamic;//hypov8
//cvar_t	*teamplay;
//cvar_t	*g_cashspawndelay;

cvar_t	*cl_parental_lock;
cvar_t	*cl_parental_override;

cvar_t	*dm_realmode;

cvar_t	*g_mapcycle_file;
// Ridah, done.

cvar_t	*antilag; // MH: antilag option

int		starttime; // MH: server start time (replaces old uptime counters)

qboolean	kpded2; // MH: the server is kpded2

// MH: GeoIP stuff
void (*_GeoIP_delete)(void* gi);
const char *(*_GeoIP_country_name_by_addr)(void* gi, const char *addr);
void *geoip=NULL;

void SpawnEntities (char *mapname, char *entities, char *spawnpoint);
void ClientThink (edict_t *ent, usercmd_t *cmd);
qboolean ClientConnect (edict_t *ent, char *userinfo);
void ClientUserinfoChanged (edict_t *ent, char *userinfo);
void ClientDisconnect (edict_t *ent);
void ClientBegin (edict_t *ent);
void ClientCommand (edict_t *ent);
void RunEntity (edict_t *ent);
void WriteGame (char *filename, qboolean autosave);
void ReadGame (char *filename);
void WriteLevel (char *filename);
void ReadLevel (char *filename);
void InitGame (void);
void G_RunFrame (void);
void HideWeapon (edict_t *ent);

unsigned char		*lpbuf[0xFFFF];
int					num_lpbuf;

// JOSEPH 23-OCT-98
void Killed (edict_t *targ, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int mdx_part, int mdx_subobject);

//===================================================================

int gameinc=0;

void ShutdownGame (void)
{
	int			i;
	edict_t		*ent;
	char buf[1024];

	buf[0]=0;
	for (i=0 ; i< (int)maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (ent->inuse && ent->client->pers.rconx[0])
			Info_SetValueForKey(buf, ent->client->pers.ip, ent->client->pers.rconx);
	}
	gi.cvar_set("rconx",buf);

	if (keep_admin_status) //mm2.0
	{
		buf[0] = 0;
		ent = GetAdmin();
		if (ent)
		{
			Info_SetValueForKey(buf, "ip", ent->client->pers.ip);
			Info_SetValueForKey(buf, "type", ent->client->pers.admin == ADMIN ? "2" : "1");
		}
		gi.cvar_set("modadmin", buf);
	}

	// MH: old uptime cvars removed

	gi.dprintf ("==== ShutdownGame ====\n");

//	sl_GameEnd( &gi, level );	// Standard Logging

//TF catch any abnormal events. like map vote in middle of node editing.
	//hypov8 note: this stops nods being saved
	if (nav_dynamic->value)
		gi.cvar_set("nav_dynamic","0");

// BEGIN:	Xatrix/Ridah/Navigator/21-mar-1998
	NAV_PurgeActiveNodes (level.node_data);
// END:		Xatrix/Ridah/Navigator/21-mar-1998

	// Ridah, clear the lightpaint buffers
	if (num_lpbuf > 0)
	{
		int i;

		for (i=0; i<num_lpbuf; i++)
			free( lpbuf[i] );
	}

	gi.FreeTags (TAG_LEVEL);
	gi.FreeTags (TAG_GAME);

	gi.ClearObjectBoundsCached();	// make sure we wipe the cached list

	// MH: release GeoIP stuff
	if (geoip)
	{
		_GeoIP_delete(geoip);
		geoip = NULL;
	}
}

int *GetNumObjectBounds (void)
{
	return &num_object_bounds;
}

void *GetObjectBoundsPointer (void)
{
	return (void *)(&g_objbnds);
}

int GetNumJuniors (void)
{
	return level.num_light_sources;
}

/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/
game_export_t *GetGameAPI (game_import_t *import)
{
	gi = *import;

	globals.apiversion = GAME_API_VERSION;
	globals.Init = InitGame;
	globals.Shutdown = ShutdownGame;
	globals.SpawnEntities = SpawnEntities;

	globals.WriteGame = WriteGame;
	globals.ReadGame = ReadGame;
	globals.WriteLevel = WriteLevel;
	globals.ReadLevel = ReadLevel;

	globals.ClientThink = ClientThink;
	globals.ClientConnect = ClientConnect;
	globals.ClientUserinfoChanged = ClientUserinfoChanged;
	globals.ClientDisconnect = ClientDisconnect;
	globals.ClientBegin = ClientBegin;
	globals.ClientCommand = ClientCommand;

	globals.RunFrame = G_RunFrame;

	globals.ServerCommand = ServerCommand;

	globals.edict_size = sizeof(edict_t);

	globals.GetNumObjectBounds = GetNumObjectBounds;
	globals.GetObjectBoundsPointer = GetObjectBoundsPointer;

	globals.GetNumJuniors = GetNumJuniors;

	// MH: check if the server is kpded2
	kpded2 = !strncmp(gi.cvar("version", "", 0)->string, "kpded", 5);

	return &globals;
}

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link
void Sys_Error (char *error, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	gi.error (ERR_FATAL, "%s", text);
}

void Com_Printf (char *msg, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	gi.dprintf ("%s", text);
}

#endif

//======================================================================


/*
=================
ClientEndServerFrames
=================
*/
void ClientEndServerFrames (void)
{
	int		i;
	edict_t	*ent;

	// calc the player views now that all pushing
	// and damage has been added
	for (i=0 ; i<maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inuse || !ent->client)
			continue;
		ClientEndServerFrame (ent);
	}

}

/*
=================
MapCycleNext

  Uses maps.lst and teammaps.lst to cycle the maps during deathmatch
=================
*/
char *MapCycleNext( char *map )
{
	char	*basevars[] = {"basedir", "cddir", NULL};	// consol variables that point to possible file locations
	cvar_t	*game_dir, *base_dir;
	char	filename[MAX_QPATH], dir[MAX_QPATH];
	FILE	*f;
	static char	nextmap[MAX_QPATH];
	char	firstmap[MAX_QPATH];
	char	travmap[MAX_QPATH];
	qboolean matched = false;
	int		i;
	char	ch;
	qboolean eof = false;

	// MH: use map list for rotation
	if (num_custom_maps)
	{
		for (i=0; i<num_custom_maps; i++)
		{
			if (!Q_stricmp(custom_list[i].custom_map, level.mapname))
			{
				if (++i == num_custom_maps)
					i = 0;
				strcpy(nextmap, custom_list[i].custom_map);
				return nextmap;
			}
		}
		strcpy(nextmap, custom_list[default_random_map ? rand() % num_custom_maps : 0].custom_map);
		return nextmap;
	}

	game_dir = gi.cvar("game", "", 0);

	// dir, eg: .\gamedir\routes
	for (i=0; basevars[i]; i++)
	{
		base_dir = gi.cvar(basevars[i], ".", 0);

		strcpy( dir, base_dir->string);

		if (dir[strlen(dir)-1] != DIR_SLASH[0])
			strcat( dir, DIR_SLASH);

		if (strlen(game_dir->string) == 0)
			strcat( dir, "main");
		else
			strcat( dir, game_dir->string);

		// filename, eg: .\gamedir\maps.lst
		strcpy( filename, dir);
		strcat( filename, DIR_SLASH);
		if (g_mapcycle_file->string && strlen(g_mapcycle_file->string) > 0)
			strcat( filename, g_mapcycle_file->string);
//		else if (!teamplay->value)
			strcat( filename, "maps.lst");
//		else
//			strcat( filename, "teammaps.lst");

		// try and open the file for reading
		f = fopen ( filename, "rb");
		if (f)
			break;	// we have a valid file
	}

	if (!f)	// no valid file found
		return NULL;

	// read in the first map
	fscanf( f, "%s", firstmap );
	strcpy( travmap, firstmap );
	ch = 0;
	while (ch!='\n' && !feof(f))
		fscanf(f, "%c", &ch);

	do
	{
		eof = feof(f);

		if (!Q_stricmp( travmap, level.mapname ))
		{
			matched = true;
		}

		if (!eof)
		{
			fscanf( f, "%s", travmap );
			ch = 0;
			while (ch!='\n' && !feof(f))
				fscanf(f, "%c", &ch);
		}

		if (matched)
		{
			if (strcmp(travmap, level.mapname) != 0 && strlen(travmap) > 1)
			{	// take this map
				strcpy( nextmap, travmap );
			}
			else
			{	// use the firstmap
				strcpy( nextmap, firstmap );
			}

			fclose(f);
			return nextmap;
		}
	}
	while (!eof);

	fclose(f);

	// no match, so return nothing
	return NULL;
}

/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================
*/
void EndDMLevel (void)
{
	edict_t		*ent;
	char		*nextmap, changenext[MAX_QPATH];
	int i;

//	gi.cvar_set("password",default_password);

	// stay on same level flag
	if ((int)dmflags->value & DF_SAME_LEVEL)
	{
		ent = G_Spawn ();
		ent->classname = "target_changelevel";
		ent->map = level.mapname;

		goto done;
	}

	if (nextmap = MapCycleNext( level.mapname ))
	{
		ent = G_Spawn ();
		ent->classname = "target_changelevel";
		ent->map = nextmap;
//		gi.bprintf (PRINT_HIGH, "Next map will be: %s.\n", ent->map);

		goto done;
	}

	if (level.nextmap[0])
	{	// go to a specific map
		ent = G_Spawn ();
		ent->classname = "target_changelevel";
		ent->map = level.nextmap;
	}
	else
	{	// search for a changeleve
		ent = G_Find (NULL, FOFS(classname), "target_changelevel");
		if (!ent)
		{	// the map designer didn't include a changelevel,
			// so create a fake ent that goes back to the same level
			ent = G_Spawn ();
			ent->classname = "target_changelevel";
			ent->map = level.mapname;
		}
	}

done:

	if (ent && ent->map)
		gi.dprintf("DM changelevel: %s (time: %i secs)\n", ent->map, (int)level.time );

	//hack to fix bug
	strcpy(changenext, ent->map);

	for_each_player(ent,i)
	{
		HideWeapon(ent);
		if (ent->client->flashlight) ent->client->flashlight = false;
	}

	// MH: signal end of game
	level.modeset = ENDGAME;
	level.startframe = level.framenum;

	// MH: music clip played in BeginIntermission
	BeginIntermission (ent, changenext);
}

//force mini hud
void TF_ScoreBoard(edict_t *ent)
{
	//hypo dont include spectators
	if (ent->client->pers.spectator == SPECTATING)
		return;

	if (ent->client->showscores != NO_SCOREBOARD)
		return;

	if (ent->client->resp.scoreboard_frame > level.framenum) //esc used
		return;

	//set scoreboard
	ent->client->showscores = SCORE_TF_HUD;
}

/*
=================
CheckDMRules
=================
*/
void CheckDMRules (void)
{
	int			i;
	int		count=0;
	int     count_players = 0; //ready to play
	edict_t	*doot;
	int respawntime = (int)timelimit->value * 600; //minutes in frametime. use timelimit as respawn counter
	qboolean respawn = false;


	if (level.intermissiontime)
		return;

	if (!deathmatch->value)
		return;
#if 1 //ndef HYPODEBUG
	if (level.framenum - level.lastactive == 600)
	{
		// the server has been idle for a minute, reset to default settings if needed
		if (ResetServer(true))
			return;
	}
#endif
	if (CheckEndWave())
		return;

	if (respawntime)
	{	//if changed mid wave, this will be incorrect
		if (level.startframe - level.framenum + respawntime < 0)
			level.startframe = level.framenum;
	}

	//check state of all players
	for_each_player(doot, i)
	{
		count++;

		if (respawntime && !((level.framenum + 1 - level.startframe) % respawntime))
		{
			//spawn players into active wave
			if (doot->client->pers.spectator == PLAYER_READY)
			{
				doot->flags &= ~FL_GODMODE;
				doot->health = 0;
				meansOfDeath = MOD_RESTART;
				ClientBeginDeathmatch(doot);
				doot->client->pers.weaponStore = NULL;
			}
			respawn = true;
		}

		//this will end wave if everyone died
		if (doot->client->pers.spectator == PLAYING)
			count_players++;

		TF_ScoreBoard(doot); //TF: update mini hud
	}

	if (respawn) //clients entered wave
		level.startframe = level.framenum;
#if 1 //ndef HYPODEBUG
	if (!count_players)
	{
		if (count > 0 && level.nav_TF_autoRoute && level.waveNum == 0)
			WaveIdle(); //routing player went to spec
		else
		{
			gi.bprintf(PRINT_HIGH, "No players currently playing.\n");
			GameEND();
			return;
		}
	}

	if ((count == 0) && (level.framenum > 12000))
	{
		if (ResetServer(true))
			return;
	}
#endif
    #if 0 //FREDZ for testing rounds
	if (timelimit->value)
	{
		if (level.framenum > (level.startframe + ((int)timelimit->value) * 600 - 1))
		{
			gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
			if (count == 0)
				ResetServer();
			else
				if (!CheckEndWave_GameType())
				{
					WaveEnd(); //hypov8 note: timelimit is time to finish a wave.
				}
				else
				{
				    gi.bprintf (PRINT_HIGH, "Player won.\n");
					GameEND ();//FREDZ
				}
			return;
		}
	}
 #endif

/*
	if (fraglimit->value && (int)teamplay->value!=1) // MH: ignore fraglimit in bagman
	{
		if ((int)teamplay->value==4){
			if (team_cash[1]>=(int)fraglimit->value || team_cash[2]>=(int)fraglimit->value) {
				gi.bprintf (PRINT_HIGH, "Fraglimit hit.\n");
				if (!allow_map_voting)
					EndDMLevel ();
				else
					SetupMapVote();
				return;
			}
		} else {
			for (i=0 ; i<maxclients->value ; i++) {
				cl = game.clients + i;
				if (!g_edicts[i+1].inuse)
					continue;

				if (cl->resp.score >= fraglimit->value) {
					gi.bprintf (PRINT_HIGH, "Fraglimit hit.\n");
					if (!allow_map_voting)
						EndDMLevel ();
					else
						SetupMapVote();
					return;
				}
			}
		}
	}

	if (cashlimit->value)
	{
		if ((team_cash[1] >= cashlimit->value) || (team_cash[2] >= cashlimit->value))
		{
			gi.bprintf (PRINT_HIGH, "Cashlimit hit.\n");
				if (!allow_map_voting)
					EndDMLevel ();
				else
					SetupMapVote();
			return;
		}
	}*/
}

/*=====================
Hide Weapon
======================*/
void HideWeapon(edict_t *ent)
{

	gclient_t *cl;

	cl = ent->client;
	if (!cl->pers.holsteredweapon)
	{
		if (!cl->pers.weapon)
			return;

		cl->pers.holsteredweapon = cl->pers.weapon;
		cl->newweapon = NULL;
		cl->weaponstate = WEAPON_DROPPING;

		if (ent->s.renderfx2 & RF2_FLAMETHROWER)
			ent->s.renderfx2 &= ~RF2_FLAMETHROWER;
	}
}

/*
=============
ExitLevel
=============
*/

void ExitLevel (void)
{
	int		i;
	edict_t	*ent;
	char	command [256];
	int		count = 0;

	level.player_num=0;

	/*for_each_player(ent, i){ //MM2.0
		count++;
	}
	if (!count && ResetServer(true))
		return;*/ // server reset instead
	

	if (skill->latched_string) //hypov8 force skill changes
		Com_sprintf (command, sizeof(command), "map \"%s\"\n", level.changemap);
	else
		Com_sprintf (command, sizeof(command), "gamemap \"%s\"\n", level.changemap);
	gi.AddCommandString (command);
	level.changemap = NULL;
	level.exitintermission = 0;
	level.intermissiontime = 0;
	ClientEndServerFrames ();

// RAFAEL
	level.cut_scene_time = 0;
	level.speaktime = 0;
	// JOSEPH 25-FEB-99
	level.cut_scene_end_count = 0;
    level.fadeendtime = 0;
	level.totalfade = 0;
	// END JOSEPH

	// JOSEPH 13-JUN-99
    level.helpchange = 0;
	// END JOSEPH

	if (level.bar_lvl)
	{
		extern void Cmd_HolsterBar_f (edict_t *ent);
		edict_t *player;

		player = &g_edicts[1];
		level.bar_lvl = false;

		Cmd_HolsterBar_f (player);
	}

	// level.pawn_time = 0;
	// level.pawn_exit = false;

	// clear some things before going to next level
	for (i=0 ; i<maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inuse)
			continue;
		if (ent->health > ent->client->pers.max_health)
			ent->health = ent->client->pers.max_health;

	}

//TF catch any abnormal events. like map vote in middle of node editing.
	//hypov8 note: this stops nods being saved
	if (nav_dynamic->value)
		gi.cvar_set("nav_dynamic","0");

// BEGIN:	Xatrix/Ridah/19-apr-1998
	// make sure Nav data isn't carried over to next level
	NAV_PurgeActiveNodes(level.node_data);
	level.node_data = NULL;
// BEGIN:	Xatrix/Ridah/19-apr-1998

}

///////////////////////////////////////////////////////////////////////
// Show the node for debugging (utility function)
///////////////////////////////////////////////////////////////////////
void NAV_TF_ShowNode(int type , vec3_t origin, int index)
{
	edict_t *ent;
	int isCrouch = (type&NODE_DUCKING);
	static float ang = 0.0;

	//show crouch on normal nodes
	if (isCrouch&& type != NODE_DUCKING)
		type &= ~NODE_DUCKING;

	ent = G_Spawn();
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	//ent->s.effects = EF_ROTATE;
	ent->s.renderfx2 = RF2_NOSHADOW;
	ent->s.renderfx = RF_FULLBRIGHT;
	if (index == 0) 
		ang += 4.5; // 90/20
	ent->s.angles[YAW] = index*4 + ang;


	if (type== NODE_NORMAL)			// walk nodes
		ent->s.skinnum = 0; 
	else if (type == NODE_JUMP)		// jump from this node
		ent->s.skinnum = 1;
	else if (type == NODE_LANDING)	// landing from a jump node
		ent->s.skinnum = 2; 
	else if (type == NODE_PLAT)		// start riding a platform
		ent->s.skinnum = 3; 
	else if (type == NODE_TELEPORT)	// move into a teleporter
		ent->s.skinnum = 4; 
	else if (type == NODE_BUTTON)	// func_button
		ent->s.skinnum = 5; 
	else //if (type & NODE_DUCKING)
		ent->s.skinnum = 6;

	ent->model = "models/bot/tris.md2"; //hypov8 custom model
	ent->s.modelindex = gi.modelindex(ent->model);

	ent->owner = ent;
	ent->nextthink = level.time + 0.1;//localnode

	ent->think = G_FreeEdict;
	ent->dmg = 0;

	VectorCopy(origin, ent->s.origin);
	gi.linkentity(ent);

}

void NAV_TF_Check_NodeData(void)
{
	int j, k, l;

	return;
	//hypov8 only in nav dynamic. 
	if (nav_dynamic->value)
	{
		for (j = 0; j < CELL_AXIS_SUBDIVISION; j++)
		{
			for (k = 0; k < CELL_AXIS_SUBDIVISION; k++)
			{
				if (level.node_data->cells[j][k] != NULL)
				{
					cell_node_t *cell = level.node_data->cells[j][k]->next, **nCell;
					if (cell == NULL)
						break;

					for (l = 0; l < CELL_AXIS_SUBDIVISION; l++)
					{
						int lastNode = cell->node->index;
						nCell = &cell->next;
						cell = cell->next;
						if (cell == NULL) {
							break;
						}
						//hypov8 todo: checkl previous used nodes. stop loops
						if (l >= level.node_data->node_count
							|| cell->node->index >= level.node_data->node_count
							|| cell->node->index < 0
							|| lastNode == cell->node->index)
						{
							*nCell = NULL;
							break;
						}
					}
				}
			}
		}
	}
	//test for invalid nodes
	for (j = 0; j < level.node_data->node_count; j++)
	{
		for (k = 0; k < level.node_data->nodes[j]->num_visible; k++)
		{
			if (level.node_data->nodes[j]->visible_nodes[k] == j)
				return;
		}
	}

}

void NAV_TF_Debug_Nodes(void)
{
	float distToTarget, bestDist = 9999;
	edict_t *firstPlayer;
	int j, k, l, iPlyr;
	static int exceded = 0;


	int closestNode = -1;

	NAV_TF_Check_NodeData();

	if (exceded == 2)	
		exceded = 1;

	k = 0;
	if (level.nav_shownode) //cmd "nav_shownode"
	{
		//only used first player
		for_each_player(firstPlayer, iPlyr)
		{
			if (firstPlayer->client->pers.spectator != PLAYING)
				continue;

			//hypov8 show all close nodes
			for (j = 0; j < level.node_data->node_count ; j++)
			{
				distToTarget = VectorDistance(firstPlayer->s.origin, level.node_data->nodes[j]->origin);
				if (distToTarget < 160)
				{
					NAV_TF_ShowNode(level.node_data->nodes[j]->node_type, level.node_data->nodes[j]->origin, k); //hypov8 show closest node
					if (distToTarget < bestDist)
					{
						bestDist = distToTarget;
						closestNode = j;
					}


					k++;
					if (k > 20) //only do 20 nodes
					{
						if (!exceded)
							gi.cprintf(firstPlayer, PRINT_MEDIUM, "*ERROR* more than 20 nodes in your area\n");
						exceded = 2;
						break;
					}
				}
			}

			//show path
			if (closestNode >= 0)
			{
				for (l = 0; l < level.node_data->nodes[closestNode]->num_visible; l++)
				{
					short nodeID = level.node_data->nodes[closestNode]->visible_nodes[l];

					if (level.node_data->nodes[closestNode]->routes[nodeID].path >= 0)
					{
						gi.WriteByte(svc_temp_entity);
						gi.WriteByte(TE_BFG_LASER);
						gi.WritePosition(level.node_data->nodes[closestNode]->origin);
						gi.WritePosition(level.node_data->nodes[nodeID]->origin);
						gi.multicast(level.node_data->nodes[closestNode]->origin, MULTICAST_PVS);
					}
				}
			}

			break; //found first valid player
		}
	}

	if (exceded == 1) //reset print if we didnt excede it this frame
		exceded = 0;
}




/*
================
G_RunFrame

Advances the world by 0.1 seconds
================
*/
extern int bbox_cnt;
extern edict_t	*mdx_bbox[];

//void ED_CallSpawn (edict_t *ent);//FREDZ thugfloor

void AI_ProcessCombat (void);

void G_RunFrame (void)
{
	int		i;
	edict_t	*ent;

	//mm 2.0
	if (level.framenum == 50 && wait_for_players && !allow_map_voting && !level.lastactive)
	{
		level.lastactive = -1;
		gi.dprintf("Waiting for players\n");
		UpdateTime();
		if (kpded2) // enable kpded2's idle mode for reduced CPU usage while waiting for players (automatically disabled when players join)
			gi.cvar_forceset("g_idle", "1");
	}

	// skip frame processing if the server is waiting for players
	if (level.lastactive < 0)
		goto uptime;

	level.framenum++;
	level.time = level.framenum*FRAMETIME;

	level.frameStartTime = Sys_Milliseconds(); // MH: get frame's start time

	// exit intermissions

	if (level.exitintermission)
	{
		ExitLevel ();
		return;
	}


	// do character sighting/memory stuff
	if (1 /*(maxclients->value == 1 && !deathmatch->value)*/)//FREDZ any
	{	// coop server, do more checking here
		//FREDZ solve allot bugs :)
		AI_UpdateCharacterMemories( 256 );
	}

	// Process Generic Combat AI layer
	AI_ProcessCombat ();

	//debug navigation nodes
	NAV_TF_Debug_Nodes();


	//
	// treat each object in turn
	// even the world gets a chance to think
	//
	ent = &g_edicts[0];
	for (i=0 ; i<globals.num_edicts ; i++, ent++)
	{
		if (!ent->inuse)
			continue;

		level.current_entity = ent;

		VectorCopy (ent->s.origin, ent->s.old_origin);


		if (ent->svflags & SVF_MONSTER || ent->client)
		{
			if (ent->waterlevel > 1)
			{
				ent->onfiretime = 0;
			}

			// On fire
			if (ent->onfiretime < 0)
			{
				ent->onfiretime++;
			}
			else if (ent->onfiretime > 0)
			{
#if 1 //TF new flame sfx on AI.. less network intensive then below
				if (!ent->client)
				{
					vec3_t	point;

					ent->s.effects |= EF_FLAMETHROWER;

					if (ent->health <= 0)
					{
						ent->onfiretime -= 1; //burn out flame faster
						// do one smoke cloud
						if (ent->onfiretime == 1)
						{
							int fCount;
							for (fCount = 0; fCount < 3; fCount++)
							{
								int h[3] = {-16, -8, -12};
								point[2] = ( ent->s.origin[2] + h[fCount]);
								point[1] = ( ent->s.origin[1] + ((rand()%14) - 7));
								point[0] = ( ent->s.origin[0] + ((rand()%14) - 7));

								gi.WriteByte(svc_temp_entity);
								gi.WriteByte(TE_SFXSMOKE);
								gi.WritePosition(point);
								gi.WriteByte(16 + ( rand() % 24 ));
								gi.WriteByte(0);
								gi.multicast(point, MULTICAST_PVS);
							}
						}
					}
				}
#else //hypov8 note: disabled. use above 4 AI.
				vec3_t	point, org, dir;
				int		i,j;
				float	dist;

				// Deathmatch flames done on client-side
				if ((!deathmatch->value /*|| ent->onfiretime == 1*/) && (deathmatch->value || !ent->client))
				{
					VectorSubtract( g_edicts[1].s.origin, ent->s.origin, dir );
					dist = VectorNormalize( dir );

					// Ridah, spawn flames at each body part
					MDX_HitCheck( ent, world, world, vec3_origin, vec3_origin, vec3_origin, 0, 0, 0, 0, vec3_origin );

					for (i = 0; i < bbox_cnt; i++)
					{

						// don't draw so many if the client is up close
						if (dist < 256)
						{
							if (random() > dist/256)
								continue;
						}

						VectorAdd( mdx_bbox[i]->s.origin, dir, org );

						if (!deathmatch->value)
						{
							for (j=0; j<2; j++)
							{
								point[2] = (org[2] + ((rand()%18) - 6) + 6);
								point[1] = (org[1] + ((rand()%10) - 5));
								point[0] = (org[0] + ((rand()%10) - 5));

								gi.WriteByte (svc_temp_entity);
								gi.WriteByte (TE_SFXFIREGO);
								gi.WritePosition (point);

								if (ent->onfiretime == 1)
									gi.WriteByte (1.2 * 10.0);
								else
									gi.WriteByte (0.6 * 10.0);

								gi.multicast (point, MULTICAST_PVS);
							}
						}

						// just do one smoke cloud
						if ((ent->onfiretime == 1) && (rand()%2))
						{
							point[2] = (org[2] + 20);// + ((rand()&31) - 16) + 20);
							point[1] = (org[1]);// + ((rand()%14) - 7));
							point[0] = (org[0]);// + ((rand()%14) - 7));

							gi.WriteByte (svc_temp_entity);
							gi.WriteByte (TE_SFXSMOKE);
							gi.WritePosition (point);
							// gi.WriteDir (ent->s.angles);
							gi.WriteByte (16 + (rand()%24));
							gi.WriteByte (0);
							gi.multicast (point, MULTICAST_PVS);
						}
					}
				}
#endif
			    if (!ent->deadflag && ent->onfireent && ent->onfireent->inuse )//hypov8 add: bugfix for free'd entity)
				{
					edict_t *trav=NULL;
					float	damage = 1;

					if (/*ent->onfiretime == 25 ||*/ random() < 0.3f) //hypov8 add. to much damage inflicted while "just" burning
						T_Damage( ent, ent->onfireent, ent->onfireent, vec3_origin, ent->s.origin, vec3_origin, damage, 0, DAMAGE_NO_KNOCKBACK, MOD_FLAMETHROWER );
#if 1 //TF disable non attack event?
					// make sure they are in the "catch_fire" motion
					if (/*deathmatch->value && */(ent->health > 0) && ent->cast_info.catch_fire)
					{
						ent->cast_info.catch_fire( ent, ent->onfireent );
					}
#endif
				}

				ent->onfiretime--;

				if (ent->onfiretime <= 0 || !ent->onfireent || !ent->onfireent->inuse )//hypov8 add: bugfix for free's entity
				{
					ent->onfireent = NULL;
					ent->onfiretime = 0;
					ent->s.effects &= ~EF_FLAMETHROWER; //hypov8 add: remove effect for AI
				}

				// JOSEPH 3-JUN-99
				if (ent->health > 0 && ent->onfiretime == 0)
				{
					ent->s.model_parts[PART_GUN].invisible_objects = 0;
					ent->s.model_parts[PART_GUN2].invisible_objects = 0;
				}
				else
				{
					ent->s.model_parts[PART_GUN].invisible_objects = (1<<0 | 1<<1);
					ent->s.model_parts[PART_GUN2].invisible_objects = (1<<0 | 1<<1);
				}
				// END JOSEPH

				if (ent->health > 0)
				{//FREDZ moved to g_combar or i need to make here new one?
					// use voice tables for this?

					// gi.dprintf( "SOUND TODO: ARRRGGGHH!!! (on fire)\n" );
				}
				else	// dead
				{
					if (ent->onfiretime > 20)
						ent->onfiretime = 20;

					if (ent->onfiretime == 1)
					{
						gi.WriteByte (svc_temp_entity);
						gi.WriteByte (TE_BURN_TO_A_CRISP);
						gi.WritePosition (ent->s.origin);
						gi.multicast (ent->s.origin, MULTICAST_PVS);
					}

				}
			}
		}

		if (ent->svflags & SVF_MONSTER)
		{
			// Blood trail
			// JOSEPH 24-JAN-99
			if ((!ent->deadflag) && (ent->health < (int)(ent->max_health*.25)) && (!(ent->svflags & SVF_PROP))/*&& ((ent->healtimer&3) == 2)*/)
			// END JOSEPH
			{
				vec3_t	stop;
				vec3_t	start;
				trace_t	trace;

				VectorCopy(ent->s.origin, start);
				start[0] += ((rand()&15)-8);
				start[1] += ((rand()&15)-8);
				VectorCopy(start, stop);
				stop[2] -= 16*16;

				trace = gi.trace (start, NULL, NULL, stop, ent, MASK_DEADSOLID);
				if (trace.fraction < 1)
				{
					float	rnd;

					rnd = (0.2 + 1.5*random());
					SurfaceSpriteEffect(SFX_SPRITE_SURF_BLOOD1, (byte)(rnd * SFX_BLOOD_WIDTH), (byte)(rnd * SFX_BLOOD_HEIGHT),
										trace.ent, trace.endpos, trace.plane.normal);
				}
			}
#if 0
//FREDZ better to disable this probably?
			// Heal thy self (healspeed -1 = no heal, healspeed 0 = 1, healspeed >1 = value)
			if ((!ent->deadflag) && (ent->health < ent->max_health) && (ent->healspeed >= 0)
					&&	(!ent->leader || ent->cast_group != 1 || (((int)timescale->value) == 1))	// Ridah, added this or they can set timescale = 100 and followers will restore full health in second
					&&	(ent->healtimer++ > 30))
			// END JOSEPH
			{
				int i, i2, baseskin;

				ent->healtimer = 0;

				if (!ent->healspeed)
				  	ent->health += 1; //hypov8 note: this is increasing cast health!!!! add -1 to cast spawns
				else
					ent->health += ent->healspeed;

				if (ent->health > ent->max_health)
					ent->health = ent->max_health;

				for (i = 0; i < MAX_MODEL_PARTS; i++)
				{
					for (i2 = 0; i2 < MAX_MODELPART_OBJECTS; i2++)
					{
						baseskin = ent->s.model_parts[i].baseskin;

						if (ent->s.model_parts[i].skinnum[i2] > baseskin)
						{
							if (ent->health > (ent->max_health * 0.75))
							{
								ent->s.model_parts[i].skinnum[i2] = baseskin;
							}
							else if (ent->health > (ent->max_health * 0.5))
							{
								if (cl_parental_lock->value && !cl_parental_override->value)
									ent->s.model_parts[i].skinnum[i2] = baseskin;
								else
									ent->s.model_parts[i].skinnum[i2] = baseskin + 1;
							}
						}
					}
				}
			}
#endif // 0
		}
		// END JOSEPH

		// if the ground entity moved, make sure we are still on it
		if ((ent->groundentity) && (ent->groundentity->linkcount != ent->groundentity_linkcount))
		{
			ent->groundentity = NULL;
			if ( !(ent->flags & (FL_SWIM|FL_FLY)) && (ent->svflags & SVF_MONSTER) )
			{
				M_CheckGround (ent);
			}
		}

		if (i > 0 && i <= maxclients->value)
		{
			ClientBeginServerFrame (ent);
		}
		else
		{
			G_RunEntity (ent);

			// Ridah, fast walking speed
			if (	(ent->cast_info.aiflags & AI_FASTWALK)
				&&	(ent->svflags & SVF_MONSTER)
				&&	(ent->cast_info.currentmove)
//				&&	(ent->cast_info.currentmove->frame->aifunc == ai_run)
				&&	(ent->cast_info.currentmove->frame->dist < 20)
				&&	(!ent->enemy))
			{
				G_RunEntity (ent);
				// JOSEPH 12-MAR-99
				if (ent->think) ent->think(ent);
				// END JOSEPH
			}
		}

		// Ridah, update lights if using directional lighting
		if (!(r_directional_lighting->value) && !deathmatch->value)
		{
			if (ent->s.renderfx2 & RF2_DIR_LIGHTS)
			{
				VectorSet( ent->s.last_lighting_update_pos, -9999, -9999, -9999 );
			}
		}
		else if (((ent->s.renderfx2 & RF2_DIR_LIGHTS) || (ent->client) || deathmatch->value))
		{
			if (!level.num_light_sources)	// no lights to source from, so default back to no dir lighting
			{
				ent->s.renderfx2 &= ~RF2_DIR_LIGHTS;
			}
			else
			{

				if (ent->client)
					ent->s.renderfx2 |= RF2_DIR_LIGHTS;

				// if single player, only calculate if it's visible to our player
				if (	(!VectorCompare(ent->s.last_lighting_update_pos, ent->s.origin))
					 &&	(	(ent->client && !deathmatch->value)
						||	(	(VectorDistance( ent->s.origin, ent->s.last_lighting_update_pos ) > (deathmatch->value ? 128 : 64))
							 &&	(	(deathmatch->value)
								 ||	(level.cut_scene_time)
								 ||	(	(gi.inPVS( g_edicts[1].s.origin, ent->s.origin))
									 &&	(infront( &g_edicts[1], ent ) ))))))
				{
					UpdateDirLights( ent );

					VectorCopy( ent->s.origin, ent->s.last_lighting_update_pos );
				}
				else if (showlights->value && gi.inPVS( g_edicts[1].s.origin, ent->s.origin))
				{
					UpdateDirLights( ent );
				}

			}
		}

	}

// Papa 10.6.99
// these is where the server checks the state of the current mode
// all the called functions are found in tourney.c

	if (level.modeset == PREGAME)
		CheckStartPub ();

    if (level.modeset == WAVE_SPAWN_PLYR)
		CheckAllPlayersSpawned ();

	if (level.modeset == WAVE_IDLE)
		WaveIdle();

	if (level.modeset == WAVE_START)
		CheckStartWave ();

    if (level.modeset == WAVE_BUYZONE)
		CheckBuyWave ();

	if (level.modeset == WAVE_ACTIVE)
        CheckDMRules ();

	if (level.modeset == ENDGAMEVOTE)
		CheckEndVoteTime ();

	// MH: exit non-voting intermission after 27s
	if (level.modeset == ENDGAME && level.framenum > (level.startframe + 270))
		level.exitintermission = true;

	if (level.voteset != NO_VOTES)
		CheckVote();

    /*
	if (!(level.framenum%10)) { // MH: fix to allow "start in" when timelimit=0
		char buf[16];
		int t;
		// MH: indicate intermission in serverinfo
		if (level.intermissiontime)
			gi.cvar_set(TIMENAME, "intermission");
		else if (level.modeset == FINALCOUNT) {
			t =	((150 - (level.framenum - level.startframe)) / 10);
			sprintf(buf,"start in %d",t);
			gi.cvar_set(TIMENAME,buf);
		} else if (level.modeset == FREEFORALL) {
			t = ((350 -  level.framenum ) / 10);
			sprintf(buf,"start in %d",t);
			gi.cvar_set(TIMENAME,buf);
		} else if ((int)timelimit->value && ((level.modeset == MATCH))) { // MH:
			t = ((((int)timelimit->value * 600) + level.startframe - level.framenum ) / 10);
			if (t>0) {
				sprintf(buf,"%d:%02d",t/60,t%60);
				gi.cvar_set(TIMENAME,buf);
			} else
				gi.cvar_set(TIMENAME,"");
		} else
			gi.cvar_set(TIMENAME,"");
	}*/

	// build the playerstate_t structures for all players
	ClientEndServerFrames ();

	if (idle_client->value < 60)
		gi.cvar_set("idle_client", "60");

	// MH: new uptime code
	if (!(level.framenum % 10))
	{
		UpdateTime();
uptime:
		if (starttime)
		{
			char buf[32];
			int secs = (int)time(NULL) - starttime;
			int mins = secs / 60;
			int hours = mins / 60;
			int days = hours / 24;
			if (days)
				sprintf(buf, "%dd %dh %dm", days, hours % 24, mins % 60);
			else if (hours)
				sprintf(buf, "%dh %dm", hours, mins % 60);
			else if (mins)
				sprintf(buf, "%dm", mins);
			else
				sprintf(buf, "%ds", secs);
			gi.cvar_set("uptime", buf);

			// snap - team tags
	//		if(teamplay->value)
	//			UPDATETEAM
		}

	}
}

