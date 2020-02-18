#include "g_local.h"
#include "file.h"
#include <stdio.h>


void fgetline (FILE* infile, char* buffer)
{
	int		i=0;
	char	ch;

	ch = fgetc(infile);

	while ( (ch != EOF) && (ch != '\n') && (i < (MAX_STRING_LENGTH-1)) )
	{
		buffer[i++] = ch;
		ch = fgetc(infile);
	}

	if ( (ch != '\n') && (ch != EOF) )	// Keep going to the eof
		ch = fgetc(infile);

	buffer[i] = '\0';

	return;

}


int proccess_line(char*	buffer)
{
	// Make sure the line's not empty
	if (strlen(buffer) == 0 || buffer[0] == '\n') return COMMENT_LINE;

	// Check to see if this is a comment line
	if (buffer[0] == '/' && buffer[1] == '/')	return COMMENT_LINE;

	// Check if this is the custom map keyword
	if (strstr(buffer, ADMIN_CODE_STRING)) return ADMIN_CODE_KEYWORD;

	// Check if this is the custom map keyword
	if (strstr(buffer, CUSTOM_MAP_STRING))
		return CUSTOM_MAP_KEYWORD;

	// Check if this is the map rotation keyword
	if (strstr(buffer, MAP_ROTATION_STRING))
		return MAP_ROTATION_KEYWORD;

	// Check if this is the default rotation keyword
	if (strstr(buffer, DEFAULT_MAP_STRING))
		return DEFAULT_MAP_KEYWORD;

//	if (strstr(buffer, DEFAULT_TEAMPLAY_STRING))
//		return DEFAULT_TEAMPLAY_KEYWORD;

	if (strstr(buffer, DEFAULT_DMFLAGS_STRING))
		return DEFAULT_DMFLAGS_KEYWORD;

	if (strstr(buffer, DEFAULT_PASSWORD_STRING))
		return DEFAULT_PASSWORD_KEYWORD;

	if (strstr(buffer, DEFAULT_MOTD_STRING))
		return DEFAULT_MOTD_KEYWORD;

/*	if (strstr(buffer, DEFAULT_TIME_STRING))
		return DEFAULT_TIME_KEYWORD;

	if (strstr(buffer, DEFAULT_FRAG_STRING))
		return DEFAULT_FRAG_KEYWORD;

	if (strstr(buffer, DEFAULT_CASH_STRING))
		return DEFAULT_CASH_KEYWORD;*/

	if (strstr(buffer, ALLOW_MAP_VOTING_STRING))
		return MAP_VOTING_KEYWORD;

	if (strstr(buffer, BAN_NAME_STRING))
		return BAN_NAME_KEYWORD;

	if (strstr(buffer, BAN_IP_STRING))
		return BAN_IP_KEYWORD;

	if (strstr(buffer, SCOREBOARD_FIRST_STRING))
		return SCOREBOARD_FIRST_KEYWORD;

	if (strstr(buffer, FPH_SCOREBOARD_STRING))
		return FPH_SCOREBOARD_KEYWORD;

	if (strstr(buffer, DISABLE_ADMIN_STRING))
		return DISABLE_ADMIN_KEYWORD;

	if (strstr(buffer, DEFAULT_REAL_STRING))
		return DEFAULT_REAL_KEYWORD;

	if (strstr(buffer, FIXED_GAMETYPE_STRING))
		return FIXED_GAMETYPE_KEYWORD;

    if (strstr(buffer, FIXED_SKILLTYPE_STRING))
		return FIXED_SKILLTYPE_KEYWORD;

	if (strstr(buffer, ENABLE_PASSWORD_STRING))
		return ENABLE_PASSWORD_KEYWORD;

	if (strstr(buffer, RCONX_FILE_STRING))
		return RCONX_FILE_KEYWORD;

/*    if (strstr(buffer, URL_STRING))
		return URL_KEYWORD;*/

	if (strstr(buffer, KEEP_ADMIN_STRING))
		return KEEP_ADMIN_KEYWORD;

	if (strstr(buffer, DEFAULT_RANDOM_MAP_STRING))
		return DEFAULT_RANDOM_MAP_KEYWORD;

	if (strstr(buffer, DISABLE_ANON_TEXT_STRING))
		return DISABLE_ANON_TEXT_KEYWORD;

//	if (strstr(buffer, DISABLE_CURSE_STRING))
//		return DISABLE_CURSE_KEYWORD;

//   if (strstr(buffer, UNLIMITED_CURSE_STRING))
//		return UNLIMITED_CURSE_KEYWORD;

    if (strstr(buffer, DISABLE_ASC_STRING))
		return DISABLE_ASC_KEYWORD;

/*    if (strstr(buffer, ENABLE_NOFLAMEHACK_CHECK_STRING))
		return ENABLE_NOFLAMEHACK_CHECK_KEYWORD;*/

	if (strstr(buffer, ENABLE_SEE_KILLER_HEALTH_STRING))
		return ENABLE_SEE_KILLER_HEALTH_KEYWORD;




	// MH: wait for players
	if (strstr(buffer, "wait_for_players"))
		return WAIT_FOR_PLAYERS_KEYWORD;



	return FOUND_STRING;

}

int proccess_ini_file()
{
	FILE*	infile;		// Config file to be opened
	int	status = OK;
	int	mode = -1;
	char	buffer[MAX_STRING_LENGTH];
	char	map[32], dummy[32];
	char	filename[32],dir[32];
	cvar_t	*game_dir;


	default_map[0]=0;
//	default_teamplay[0]=0;
	default_dmflags[0]=0;
	default_password[0]=0;
	default_dm_realmode[0]=0;
	default_wavetype[0]= WAVE_DEFAULT; //long

	admincode[0]=0;

	custom_map_filename[0]=0;
	ban_name_filename[0]=0;
	ban_ip_filename[0]=0;
	rconx_file[0]=0;
//    server_url[0]=0;

	// Set number of custom/rotation maps to 0
//	game.num_rmaps = 0;
	game.num_cmaps = 0;
	allow_map_voting = false;
	wait_for_players = false; // MH: wait for players
	disable_admin_voting = false;
	scoreboard_first = false;
	num_custom_maps = 0;
	num_netnames = 0;
	num_ips = 0;
	fixed_gametype = false;
	fixed_skilltype = false;
	enable_password = false;
	keep_admin_status = false;
	default_random_map = false;
	disable_anon_text = false;
//	disable_curse = false;
//   unlimited_curse = false;
//    enable_asc = false;
 	enable_killerhealth = false;




	// Open config file
	game_dir = gi.cvar("game", "", 0);
	if (game_dir->string[0]==0)
		strcpy(dir, "main");
	else
		strcpy(dir, game_dir->string);

	Com_sprintf (filename, sizeof(filename), "%s"DIR_SLASH"thugfloor.ini",dir);
	infile = fopen(filename, "r");
	if (infile == NULL)	return FILE_OPEN_ERROR;

	// Read first line of the file
	fgetline(infile, buffer);

	while (!feof(infile))	// while there's still stuff
	{
		switch (proccess_line(buffer))	// Determine what to do with the line, based offa what's in it
		{
		case ADMIN_CODE_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(admincode, map, 16);
			break;
		case COMMENT_LINE:	// do nothing
			break;
		case MAP_ROTATION_KEYWORD:	// add maps to map rotation
			mode = ADD_ROTATION;
			break;
		case DEFAULT_MOTD_KEYWORD:	// add MOTD line
			mode = ADD_MOTD_LINE;
			break;
		case FOUND_STRING:	// Found a string
			if (mode == ADD_CUSTOM)
				add_to_custom_maps(buffer);
			if (mode == ADD_MOTD_LINE)
				add_to_MOTD(buffer);
			break;
		case DEFAULT_MAP_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_map, map, 32);
			break;
/*		case DEFAULT_TEAMPLAY_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_teamplay, map, 16);
			break;*/
		case DEFAULT_DMFLAGS_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_dmflags, map, 16);
			break;
		case DEFAULT_PASSWORD_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_password, map, 16);
			break;
/*		case DEFAULT_TIME_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_timelimit, map, 16);
			break;
		case DEFAULT_CASH_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_cashlimit, map, 16);
			break;
		case DEFAULT_FRAG_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_fraglimit, map, 16);
			break;*/
		case CUSTOM_MAP_KEYWORD: // add maps to custom map list
//			if ((int)teamplay->value != 1 || !custom_map_filename[0]) // MH: don't override bagman map list
            if (!custom_map_filename[0])
			{
				sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
				strncpy(custom_map_filename, map, 32);
			}
			break;
		case MAP_VOTING_KEYWORD:
			allow_map_voting = true;
			break;
		case BAN_NAME_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(ban_name_filename, map, 32);
			break;
		case BAN_IP_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(ban_ip_filename, map, 32);
			break;
		case SCOREBOARD_FIRST_KEYWORD:
			scoreboard_first = true;
			break;
		case FPH_SCOREBOARD_KEYWORD:
			fph_scoreboard = true;
			break;
		case DISABLE_ADMIN_KEYWORD:
			disable_admin_voting = true;
			break;
		case DEFAULT_REAL_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(default_dm_realmode, map, 16);
			break;
        case DEFAULT_MAXWAVES_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);
			strncpy(default_wavetype, map, 16);
			break;
		case FIXED_GAMETYPE_KEYWORD:
			fixed_gametype = true;
			break;
        case FIXED_SKILLTYPE_KEYWORD:
			fixed_skilltype = true;
			break;
		case ENABLE_PASSWORD_KEYWORD:
			enable_password = true;
			break;
		case RCONX_FILE_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(rconx_file, map, 32);
			break;
 /*       case URL_KEYWORD:
			sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
			strncpy(server_url, map, 64);
			break;*/
		case KEEP_ADMIN_KEYWORD:
			keep_admin_status = true;
			break;
		case DEFAULT_RANDOM_MAP_KEYWORD:
			default_random_map = true;
			break;
		case DISABLE_ANON_TEXT_KEYWORD:
			disable_anon_text = true;
			break;
/*		case DISABLE_CURSE_KEYWORD:
			disable_curse = true;
			break;
        case UNLIMITED_CURSE_KEYWORD:
			unlimited_curse = true;
			break;*/
			// MH: removed (disble_asc = enable anti spawncamp???)
/*        case DISABLE_ASC_KEYWORD:
		//	enable_asc = true;
            gi.cvar_set("anti_spawncamp", "1");
			break;*/
      /*  case ENABLE_NOFLAMEHACK_CHECK_KEYWORD:
			//noflamehackcheck = true;
            gi.cvar_set("kick_flamehack", "1");
			break;*/
		case ENABLE_SEE_KILLER_HEALTH_KEYWORD:
			enable_killerhealth = true;
			break;


		// MH: wait for players
		case WAIT_FOR_PLAYERS_KEYWORD:
			wait_for_players = true;
			break;

		// MH: bagman map list
/*		case BM_CUSTOM_MAP_KEYWORD:
			if ((int)teamplay->value == 1)
			{
				sscanf(buffer, "%s %s", dummy, map);	// Quick ugly hack :)
				strncpy(custom_map_filename, map, 32);
			}
			break;*/

		default:	// wtf is this?
			gi.dprintf("Unhandled line!\n");
		}
		fgetline(infile, buffer);		// Retrieve next line from the input file
	}

	// close the ini file
	fclose(infile);

	if (ban_name_filename[0]) {
	Com_sprintf (filename, sizeof(filename), "%s"DIR_SLASH"%s",dir, ban_name_filename);
	infile = fopen(filename, "r");
	if (infile != NULL)
	{
	// Read first line of the file
		fgetline(infile, buffer);
		num_netnames = 0;
		while (!feof(infile))	// while there's still stuff
		{
			if (strlen(buffer) == 0 || buffer[0] == '\n')
			{
				fgetline(infile, buffer);
				continue;
			}
		// Check to see if this is a comment line
			if (buffer[0] == '/' && buffer[1] == '/')
			{
				fgetline(infile, buffer);
				continue;
			}
			kp_strlwr(buffer); // MH: replaced tolower loop
			strncpy(netname[num_netnames].value,buffer,16);
			fgetline(infile, buffer);
			num_netnames++;
			if (num_netnames==100) break;
		}
		fclose(infile);
	}
	}

	if (ban_ip_filename[0]) {
	Com_sprintf (filename, sizeof(filename), "%s"DIR_SLASH"%s",dir, ban_ip_filename);
	infile = fopen(filename, "r");
	if (infile != NULL)
	{
	// Read first line of the file
		fgetline(infile, buffer);
		num_ips = 0;
		while (!feof(infile))	// while there's still stuff
		{
			if (strlen(buffer) == 0 || buffer[0] == '\n')
			{
				fgetline(infile, buffer);
				continue;
			}
		// Check to see if this is a comment line
			if (buffer[0] == '/' && buffer[1] == '/')
			{
				fgetline(infile, buffer);
				continue;
			}
			strncpy(ip[num_ips].value,buffer,16);
			fgetline(infile, buffer);
			num_ips++;
			if (num_ips==100) break;
		}
		fclose(infile);
	}
	}

	if (rconx_file[0]) {
	Com_sprintf (filename, sizeof(filename), "%s"DIR_SLASH"%s",dir, rconx_file);
	infile = fopen(filename, "r");
	if (infile != NULL)
	{
	// Read first line of the file
		fgetline(infile, buffer);
		num_rconx_pass = 0;
		while (!feof(infile))	// while there's still stuff
		{
			if (strlen(buffer) == 0 || buffer[0] == '\n')
			{
				fgetline(infile, buffer);
				continue;
			}
		// Check to see if this is a comment line
			if (buffer[0] == '/' && buffer[1] == '/')
			{
				fgetline(infile, buffer);
				continue;
			}
			strncpy(rconx_pass[num_rconx_pass].value,buffer,32);
			rconx_pass[num_rconx_pass].value[31]=0;
			fgetline(infile, buffer);
			num_rconx_pass++;
			if (num_rconx_pass==100) break;
		}
		fclose(infile);
	}
	}

	return OK;
}

int read_map_file()
{

	FILE*	infile;		// Config file to be opened
	char	buffer[MAX_STRING_LENGTH];
	char	map[32], rank[32];
	char	filename[32], dir[32];
	cvar_t	*game_dir;
	int		c;

	game_dir = gi.cvar("game", "", 0);
	if (game_dir->string[0]==0)
		strcpy(dir, "main");
	else
		strcpy(dir, game_dir->string);

	if (custom_map_filename[0]) {
	Com_sprintf (filename, sizeof(filename), "%s"DIR_SLASH"%s",dir, custom_map_filename);
	infile = fopen(filename, "r");
	if (infile == NULL)	return FILE_OPEN_ERROR;

	num_custom_maps = 0;

	// Read first line of the file
	fgetline(infile, buffer);
	while (!feof(infile))	// while there's still stuff
	{
		if (strlen(buffer) == 0 || buffer[0] == '\n')
		{
			fgetline(infile, buffer);
			continue;
		}
	// Check to see if this is a comment line
		if (buffer[0] == '/' && buffer[1] == '/')
		{
			fgetline(infile, buffer);
			continue;
		}

		// MH: rank not used now but still support old lists that include it
		c = sscanf(buffer, "%s %s", rank, map);
		if (c == 1)
			strncpy(custom_list[num_custom_maps].custom_map, rank, 31);
		else if (c == 2 && rank[0]>='1' && rank[0]<='9') // old map list with rank included?
			strncpy(custom_list[num_custom_maps].custom_map, map, 31);
		else
		{
			fgetline(infile, buffer);
			continue;
		}
		kp_strlwr(custom_list[num_custom_maps].custom_map); // MH: prevent MAX_GLTEXTURES errors caused by uppercase letters in the map vote pic names
		num_custom_maps++;
		fgetline(infile, buffer);
	}

	// MH: fix for leak
	fclose(infile);
	}

	return OK;
}

void add_to_custom_maps(char*	buffer)
{
	strncpy(custom_list[game.num_cmaps].custom_map, buffer, 16);
	game.num_cmaps++;
}

void add_to_MOTD(char*	buffer)
{
	strncpy(MOTD[game.num_MOTD_lines].textline, buffer, 99);
	game.num_MOTD_lines++;
}


// MH: write_map_file removed (no map rank to update)


// MH: check if file exists
#ifdef _WIN32
#include <io.h>
#define access(a,b) _access(a,b)
#else
#include <unistd.h>
#endif

int file_exist(char *file)
{
	char	buf[MAX_QPATH];
	cvar_t	*game_dir = gi.cvar("game", "", 0);
	if (game_dir->string[0])
	{
		Com_sprintf(buf, sizeof(buf), "%s/%s", game_dir->string, file);
		if (!access(buf, 4)) return true;
	}
	Com_sprintf(buf, sizeof(buf), "main/%s", file);
	if (!access(buf, 4)) return true;
	// assume kpdm1-5 and team_pv/rc/sr are available if pak0.pak exists
	if (!Q_stricmp(file,"maps/kpdm1.bsp") || !Q_stricmp(file,"maps/kpdm2.bsp") || !Q_stricmp(file,"maps/kpdm3.bsp") || !Q_stricmp(file,"maps/kpdm4.bsp") || !Q_stricmp(file,"maps/kpdm5.bsp")
			|| !Q_stricmp(file,"maps/team_pv.bsp") || !Q_stricmp(file,"maps/teamp_rc.bsp") || !Q_stricmp(file,"maps/team_sr.bsp"))
		return !access("main/pak0.pak", 4);
	return false;
}
