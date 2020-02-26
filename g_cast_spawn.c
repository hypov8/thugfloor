#include "g_local.h"
#include "g_cast_spawn.h"

#define HYPODEBUG 1

#define ARYSIZE(x) (sizeof(x) / sizeof(x[0])) //get dynamic size of array


#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT];// = {NULL, NULL, NULL}; //hypov8

void cast_TF_spawn();
//int currWave_castCount = 0; //enemy currently on map
static int currWave_plysCount = 0; //players
static int currWave_length = 0; //long, med, short
static int currWave_castMax = 0; //max enemy allowed on map

//boss info
edict_t *boss_entityID = NULL;
int boss_maxHP;
static int boss_called_help = 0; //1 = melee, 2= pistol, 3= shotty

#if 0
void cast_pawn_o_matic_free()
{
	int i;
	for (i = 0; i < 1; i++)
	{
		if (pawnGuy[i] && pawnGuy[i]->inuse && pawnGuy[i]->svflags & SVF_MONSTER)
		{
			AI_UnloadCastMemory(pawnGuy[i]);
			G_FreeEdict(pawnGuy[i]);
		}
	}
	level.currWave_castCount = 0;
	level.waveEnemyCount = 0;
}
void cast_pawn_o_matic_spawn ()//Randoms spawn testing
{
	edict_t *spawn,*spawnspot, *player;
	int count = 0, i;


	for_each_player(player, i)
	{
		if (player->client->pers.spectator != SPECTATING)
		{
			AddCharacterToGame(player); //add player to bot target list
		}
	}

//	for (i=0 ; i<globals.num_edicts ; i++)
	{
/*		spawnspot = g_edicts + i;
		if (!spawnspot->inuse)
			continue;
		if (!spawnspot->classname)
			continue;
		if (Q_stricmp(spawnspot->classname, "info_player_deathmatch"))//todo Should add special one for new maps or spawn on cast_pawn_o_matic spot
			continue;*/

		spawn = G_Spawn();
		spawnspot = cast_SelectRandomDeathmatchSpawnPoint(spawn);
		VectorCopy(spawnspot->s.angles, spawn->s.angles);
		VectorCopy(spawnspot->s.origin, spawn->s.origin);
		spawn->s.origin[2] += 1;

		spawn->classname = "cast_pawn_o_matic";
		spawn->cast_group = 1;
		spawn->moral = 1;
		gi.linkentity (spawn);

		if (level.num_characters == MAX_CHARACTERS)
		{
			gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
			G_FreeEdict(spawn);
			return ;
		}

/*		{//Todo
		    spawn->s.modelindex = 255;
		    VectorCopy(player->s.origin, spawn->s.origin );
		    gi.linkentity(spawn);
		}*/

		ED_CallSpawn(spawn);


		//copy entity our list so we can free later
		pawnGuy[count] = spawn;

		count++;
		//add enemy to counter
		level.currWave_castCount++;

		//no more required
		if (count == 1)
			return;
	}
}
#else
void cast_pawn_o_matic_free()
{
	int i;
	for (i = 0; i < BUYGUY_COUNT; i++)
	{
		if (pawnGuy[i] && pawnGuy[i]->inuse && pawnGuy[i]->svflags & SVF_MONSTER)
		{
			AI_UnloadCastMemory(pawnGuy[i]);
			G_FreeEdict(pawnGuy[i]);
		}
	}
	level.currWave_castCount = 0;
	level.waveEnemyCount = 0;
}
void cast_pawn_o_matic_spawn ()
{
	edict_t *spawn,*spawnspot, *player;
	int count = 0, i;


	for_each_player(player, i)
	{
		if (player->client->pers.spectator != SPECTATING)
		{
			AddCharacterToGame(player); //add player to bot target list
		}
	}

	//find first 3 dm spawns in entity list. pawnGuy will be at same 3 location every time, unless custom kpded2 itemlist
	for (i=0 ; i<globals.num_edicts ; i++)
	{
		spawnspot = g_edicts + i;
		if (!spawnspot->inuse)
			continue;
		if (!spawnspot->classname)
			continue;
		if (Q_stricmp(spawnspot->classname, "info_player_deathmatch"))
			continue;

		spawn = G_Spawn();
		VectorCopy (spawnspot->s.angles, spawn->s.angles);
		VectorCopy( spawnspot->s.origin, spawn->s.origin );
		spawn->s.origin[2] += 1;
		spawn->classname = "cast_pawn_o_matic";
		spawn->cast_group = 1;
		spawn->moral = 1;
		gi.linkentity (spawn);

		if (level.num_characters == MAX_CHARACTERS)
		{
			gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
			G_FreeEdict(spawn);
			return ;
		}

		ED_CallSpawn(spawn);

		//copy entity our list so we can free later
		pawnGuy[count] = spawn;

		count++;
		//add enemy to counter
		level.currWave_castCount++;

		//no more required
		if (count == BUYGUY_COUNT)
			return;
	}
}
#endif // 1

//apply skins.. consider skill
void cast_TF_applyRandSkin(edict_t *self, localteam_skins_s *skins, int skinCount)
{
#if TEST_NEWSKIN //only 2 waves can be tested. crashes on 3rd

	//this split skins into 5 groups. low skill needs to be at top and high skill at bottom of list
	//some overlap of skill settings will be generated. about 2 above/below actual skill.
	int range = ceil(((float)skinCount / 3)); //3 instead of 5. adds some varance
	int idxStart = range * ((int)skill->value *.6); //0-4
	int ran = rand() % (range);
	int idx = idxStart + ran;
	if (idx >= skinCount)
		idx = skinCount - 1; //just incase. ceil rounds up
#else
	int idx = skinCount;
#endif
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[idx].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[idx].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[idx].classname);
	self->spawnflags = skins[idx].spawnflags;
	self->health = skins[idx].health;
	self->count = skins[idx].count;
	self->head = skins[idx].head;
}

#if TEST_NEWSKIN //example skin method
/*
======================
cast_TF_Wave1_Skidrow
wave 1 skidrow

combine every skin that should be in skidrow
sort them from weakest to best
todo: this is example. skins only sorted by weapon, not skill
======================
*/
void cast_TF_Wave1_Skidrow(edict_t *self)
{
	static localteam_skins_s skins[] = {
		//name,		//skin,			classname		flags	HP		count	head
		"johnny",	"011 007 004",	"cast_thug",	64, 	80,	    0,      0,	//melee			//Skidrow_courtyard
		"Leroy",	"010 010 003",	"cast_thug",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names
		"Brewster", "002 001 001",	"cast_thug",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names
		"Bubba",	"017 016 008",	"cast_runt",	64,	    100,	9,  	0,	//sr1 melee		//Skidrow_names
		"Magicj",	"020 011 005",	"cast_runt",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names
		"buttmunch", "001 001 001",	"cast_runt",	64,	    100,	0,	    0,	//melee	//sr1 Sewer Rats
		"",	        "010 004 002",  "cast_runt",	64,	    100,	0,	    0,  //melee		//Skidrow_street //sr3 street1
		"Betty",	"008 006 003",	"cast_bitch",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names
		"Beth",		"009 007 004",	"cast_bitch",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names

		"punky",     "005 003 001",	"cast_runt",	0,	    50,	    0,	    0,	//pistol		//sr1 Sewer Rats
		"Momo",		"020 011 003",	"cast_runt",	0,	    100,	0,  	0,	//sr2 //bouncer pistol //Skidrow_names
		"Mona",		"014 012 003",	"cast_bitch",	0,	    100,	0,  	0,	//sr2 pistol			//Skidrow_names
		"Sluggo",	"019 010 011",	"cast_runt",	0,	    100,	0,  	0,   //bar_sr pistol		//Skidrow_names
		"Lenny",	"018 011 007",	"cast_runt",	0,	    100,	0,  	0,	//bar_sr pistol		//Skidrow_names
		"Rocko",	"016 009 006",	"cast_thug",	0,	    100,	0,  	0,	//bar_sr pistol		//Skidrow_names
		"",	        "006 006 002",	"cast_runt",	0,	    100,	0,	    0,  //pistol	//Skidrow_street //sr3 street1
		"",	        "008 008 002",	"cast_runt",	0,	    100,	0,	    0,  //pistol	//Skidrow_street //sr3 street1
		"bernie",   "011 012 004",  "cast_runt",	0,	    100,	1,	    0,	//pistol		//Skidrow_courtyard
		"louie",    "011 011 005",	"cast_runt",	0,	    100,	3,  	0,	//pistol //Not really police, more person they try to secure
	};

	cast_TF_applyRandSkin(self, skins, ARYSIZE(skins));
}
void cast_TF_Wave2_Skidrow(edict_t *self)
{
	static localteam_skins_s skins[] = {
		//name,		//skin,			classname		flags	HP		count	head

		"Leroy",	"010 010 003",	"cast_thug",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names
		"Brewster", "002 001 001",	"cast_thug",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names
		"Bubba",	"017 016 008",	"cast_runt",	64,	    100,	9,  	0,	//sr1 melee		//Skidrow_names
		"Beth",		"009 007 004",	"cast_bitch",	64,	    100,	0,  	0,	//sr1 melee		//Skidrow_names
		"Lisa",		"012 015 012",	"cast_bitch",	64,	    120,	0,  	0,	//sr1 melee		//Skidrow_names

		"",         "004 003 001",	"cast_runt",	0,	    100,	0,	    0,	//pistol		//sewer treehouse, ugly skins
		"",         "003 003 001",	"cast_runt",	0,	    100,	0,	    0,	//pistol		//sr2 motards, ugly skins
		"",         "002 001 001",	"cast_bitch",	0,	    100,	0,	    0,	//pistol		//sr2 motards, ugly skins
		"",	        "005 001 001",  "cast_thug",	0,	    100,	0,	    0,	//pistol		//sr2 motards, ugly skins
		"",	        "010 006 002",	"cast_runt",	0,	    100,	0,	    0,	//courtyard1 pistol
		"",	        "006 005 002",	"cast_runt",	0,	    100,	0,	    0,	//courtyard2 pistol
		"",     	"006 006 002",	"cast_runt",	0,	    100,	0,	    0,  //pistol		//sr3 hallway1
		"",	        "009 007 002",	"cast_runt",    0,      100,	0,		0,    //postbattery2  pistol
		"",	        "008 008 002",	"cast_runt",    0,      120,	0,		0,    //postbattery2  pistol
		"igmo",      "003 002 001", "cast_thug",	0,	    200,	0,	    0,	//pistol		//sr1 Sewer Rats

		"",	        "004 001 001",	"cast_punk",    0,      80,		0,		0,    //postbattery1  shotgun
		"",         "004 001 007",	"cast_punk",	0,	    100,	0,	    0,	//shotgun	//sewer treehouse, ugly skins
		"",	        "514 004 002",  "cast_punk",	0,	    100,	0,	    1,	//courtyard1 shotgun
		"",	        "513 004 002",  "cast_punk",	0,	    100,	0,	    1,	//courtyard1 shotgun
		"",	        "514 003 002",	"cast_punk",	0,	    100,	0,	    1,	//courtyard2 shotgun
		"",	        "514 003 002",	"cast_punk",	0,	    100,	0,	    0,  //shotgun		//sr3 hallway1
		"",	        "004 001 001",	"cast_punk",    0,      120,	0,		0,    //postbattery1  shotgun
		"",     	"514 003 002",	"cast_punk",    0,      120,	0,		1,    //postbattery2  shotgun
		"Momo",		"072 014 009",	"cast_shorty",	8192,   150,	0,  	0,	//sr1 shotgun		//Skidrow_names
		"arnold",	"012 007 004",	"cast_punk",	0,	    200,	0,	    0,	//Shotgun		//Skidrow_courtyard
	};

	cast_TF_applyRandSkin(self, skins, ARYSIZE(skins));
}

#endif


///////////////////////
// setup cast skins

void cast_TF_dog(edict_t *self)
{
	self->classname = "cast_dog";
	self->health = 100;//Default otherwise 200
}
void cast_TF_rat(edict_t *self)
{
	self->classname = "cast_rat";
//    self->cast_info.scale = 2.00;//FREDZ bugged
//    self->s.origin[2] -= 16;
    self->cast_info.scale = 1.05;//Limited
}
void cast_TF_cast_melee(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	    head
	"Betty",	    "008 006 003",  "cast_bitch",	64,	    100,		0,		0,	//sr1
	"Beth",		    "009 007 004",  "cast_bitch",	64,	    100,		0,		0,	//sr1
	"Lisa",		    "012 015 012",  "cast_bitch",	64,	    100,		0,		0,	//sr1 Got normally 120 health
    "bubba",	    "017 016 008",  "cast_runt",	64,	    100,	    9,		0,	//sr1
	"Louie",	    "011 011 005",  "cast_runt",	64,	    100,	    3,		0,	//sr1
//	"Buttmunch",    "001 001 001",  "cast_runt",	64,	    100,	    0,		0,	//sr1 in sewer rats group
	"Magicj",		"020 011 005",  "cast_runt",	64,	    100,	    0,		0,  //sr1
	"Kroker",		"023 020 020",  "cast_runt",	64,	    100,	    1,		0,	//steel1 Got normally 150 health
	"kid_1",		"134 132 132",  "cast_runt",	64,	    100,	    0,		0,	//steel2
	"kid_2",		"132 132 132",  "cast_runt",	64,	    100,	    0,		0,	//steel2
	"kid_3",		"133 132 132",  "cast_runt",	64,	    100,	    0,		0,	//steel2
	"popeye",		"040 019 048",  "cast_runt",	64,	    100,	    0,		0,	//sy_h Got normaly health 300 and diffrent head?
	"harpo",		"142 140 140",  "cast_runt",	64,	    100,	    0,		0,	//rc1 Got normally 200 health
	"bubba",		"042 042 010",  "cast_runt",	64,	    100,	    9,		0,	//rc1
	"groucho",		"140 141 046",  "cast_runt",	64,	    100,	    0,		0,	//rc1 Got normally 150 health
	"chico",		"141 141 046",  "cast_runt",	64,	    100,	    0,		0,	//rc1 Got normally 150 health //same skin as grouncho
    "leroy",	    "010 010 003",  "cast_thug",	64,		100,		0,		0,  //sr1
//	"johnny",	    "011 007 004",  "cast_thug",	64,		100,		0,		0,	//sr1 Got normally 80 health in courtyard group
	"brewster",     "002 001 001",  "cast_thug",	64,		100,		0,		0,	//sr1
//	"IntroGuy",     "041 026 010",  "cast_thug",	64,		100,		0,		0,	//sr1 player
    "kid_4",	    "133 132 132",  "cast_thug",	64,		100,		0,		0,	//steel3
    "kid_5",	    "134 132 132",  "cast_thug",	64,		100,		0,		0,	//steel3
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_cast_pistol(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	    head
	"Mona",		    "014 012 003",  "cast_bitch",	0,	    100,		0,		0,	//sr2
	"yolanda",	    "041 050 003",  "cast_bitch",	0,	    100,		0,		0,	//pv_h got normally 200 health
	"candy",	    "015 017 013",  "cast_bitch",	0,	    100,		0,		0,	//bar_pv same name as in bar_rc
	"brittany",	    "046 040 010",  "cast_bitch",	0,	    100,		0,		0,	//steel1
	"bambi",	    "044 042 003",  "cast_bitch",	0,	    100,		0,		0,	//bar_st
	"lola",		    "045 049 015",  "cast_bitch",	0,	    100,		0,		0, 	//rc3
	"candy",	    "019 019 015",  "cast_bitch",	0,	    100,		0,		0,	//bar_rc same name as in bar_pv
	"selma",	    "600 043 003",  "cast_bitch",	0,	    100,		0,		0, 	//bar_sy
//	"bernie",	    "011 012 004",  "cast_runt",	0,	    100,		1,		0,	//sr1 Got normally 150 health in courtyard group
//	"punky",	    "005 003 001",  "cast_runt",	0,	    100,		0,		0,	//sr1 Got normally 50 health// in sewer rats group
	"momo",		    "020 011 003",  "cast_runt",	0,	    100,		0,		0,	//sr2 //bouncer
	"momo",		    "013 045 006",  "cast_runt",	0,	    100,		0,		0,	//steel1 //bouncer
	"momo",		    "049 041 009",  "cast_runt",	0,	    100,		0,		0,	//rc4 //bouncer
	"moe",		    "047 065 041",  "cast_runt",	0,	    100,		0,		0,	//sy_h
//	"jesus",	    "021 017 010",  "cast_runt",	0,	    100,		0,		0,	//sr4 Got normally 400 health some kind of boss
	"mathew",	    "048 042 015",  "cast_runt",	0,	    100,		0,		0,	//steel1
	"luke",		    "049 046 046",  "cast_runt",	0,	    100,		0,		0,	//ty1
	"clarence",     "047 013 003",  "cast_runt",	0,	    100,		9,		0,	//bar_pv
	"buster",	    "046 010 009",  "cast_runt",	0,	    100,		9,		0,	//bar_pv Got normally 120 health
	"louie",	    "043 041 009",  "cast_runt",	0,	    100,		0,		0,	//bar_pv
	"clarence",     "045 024 005",  "cast_runt",	0,	    100,		1,		0,	//bar_rc
	"sluggo",	    "019 010 011",  "cast_runt",	0,	    100,		0,		0,	//bar_sr
	"lenny",	    "018 011 007",  "cast_runt",	0,	    100,		0,		0,	//bar_sr
	"dogg",		    "016 014 003",  "cast_runt",	0,	    100,		0,		0, 	//bar_sy
    "fingers",	    "025 009 009",  "cast_thug",	0,		100,		0,		0,	//bar_pv Got normally 150 health
	"burt",	        "063 041 010",  "cast_thug",	0,		100,		0,		0,	//bar_pv
	"adolf",	    "047 042 009",  "cast_thug",	0,		100,		0,		0,	//bar_rc
    "burt",	        "023 017 005",  "cast_thug",	0,		100,		0,		0,	//bar_rc
    "scalper",	    "103 044 009",  "cast_thug",	0,		100,		0,		0,	//bar_rc
    "rocko",	    "016 009 006",  "cast_thug",	0,		100,		0,		0,	//bar_sr
//	"igmo",	        "003 002 001",  "cast_punk",	0,		100,		0,		0,	//sr1 Got normally 200 health in sewer rats group
    "lamont",	    "024 017 010",  "cast_thug",	0,		100,		0,		0,	//sr2 Got normally 300 health
    "hann",	        "029 006 010",  "cast_thug",	0,		100,		0,		0,	//ty1
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_cast_shotgun(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		  count	    head
    "Rochelle",	    "601 009 012",	"cast_whore",	0, 	    100,		0,		0,	//rc1
	"momo",		    "072 014 009",	"cast_shorty",	0, 	    100,		0,		0, 	//sr1 Got normally 150 health
	"butch",	    "131 130 010",	"cast_shorty",	0, 	    100,		0,		0,	//rc1
	"dude",		    "130 131 010",	"cast_shorty",	0, 	    100,		9,		0,  //rc3 Got normally 200 health
	"momo",		    "072 014 009",	"cast_shorty",	0, 	    100,		0,		0,  //pv_h
    "bigwillie",	"109 046 047",  "cast_punk",	0,	    100,		0,		0,	//bar_sy Got normally 350 health
	"Laurel",		"504 032 031",  "cast_punk",	0,	    100,		0,		1,	//pv_h Got normally 200 health
	"Hardey",		"114 032 031",  "cast_punk",	0,	    100,		0,		0,	//pv_h Got normally 200 health
//	"ToughGuy1",	"019 028 010",  "cast_punk",	0,	    100,		0,		0,	//sr1 intro guy
//	"ToughGuy2",	"020 029 010",  "cast_punk",	0,	    100,		0,		0,	//sr1 intro guy
//	"arnold",		"012 007 004",  "cast_punk",	0,	    100,		0,		0,	//sr1 Got normally 200 health in courtyard group
	"momo",			"107 044 010",  "cast_punk",	0,	    100,		0,		0,	//sy_h
	"larry",		"048 047 041",  "cast_punk",	0,	    100,		0,		0,	//sy_h Got normally 200 health
	"curly",		"100 047 041",  "cast_punk",	0,	    100,		0,		0,	//sy_h Got normally 200 health
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_cast_tommygun(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		    count	head
    "sal",		    "064 047 047",	"cast_shorty",	64,		100,	     0,		0,	//bar_sy Got normally 250 health
    "mung",		    "013 010 006",	"cast_shorty",	64,		100,		 9,		0,	//pv_h
    "bubba",	    "130 131 010",	"cast_shorty",	64,		100,		 9,		0,	//rc2
    "heilman",	    "121 121 121",	"cast_shorty",	64,		100,		 0,		0,	//sy2 Got normally 250 health
    "lefty",	    "064 046 047",  "cast_punk",	64,		100,		 0,		0,	//bar_sy Got normally 250 health
    "dubs",		    "108 009 006",  "cast_punk",	64,		100,		 0,		0,	//pv_h  Got normally 220 health
    "patrick",	    "511 130 010",  "cast_punk",	64,		100,		 0,		1,  //rc1
    "donny",	    "512 131 010",  "cast_punk",	64,		100,		 0,		1,  //rc4 Got normally 200 health
    "oscar",	    "044 012 006",  "cast_punk",	64,		100,		 0,		0,  //steel2 Got normally 250 health
    "david",	    "132 132 132",  "cast_punk",	64,		100,    	 0,		0,  //steel3 Got normally 300 health
 //   "tyrone",	    "507 082 054",  "cast_punk",	64,		100,	     0,		1,  //steel3 Got normally 500 health
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_cast_hmg(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL),  "Walter");//bar_rc Got normally 350 health
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "130 130 010");
	self->spawnflags = 16;
	self->classname = "cast_shorty";
}

//localteam
void cast_TF_Skidrow_courtyard(edict_t *self)//sr1 Police/Security
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Johnny",	"011 007 004",	"cast_thug",	64, 	80,	    0,       0, //melee
    "Bernie",   "011 012 004",  "cast_runt",	0,	    100,	1,	     0, //pistol
    "Arnold",	"012 007 004",	"cast_punk",	0,	    200,	0,	     0, //Shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}

void cast_TF_Skidrow_sewer_rats(edict_t *self)//sr1 Sewer Rats ugly skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Buttmunch", "001 001 001",	"cast_runt",	64,	    100,	0,	    0, //melee
    "Punky",     "005 003 001",	"cast_runt",	0,	    50,	    0,	    0, //pistol
    "Igmo",      "003 002 001", "cast_thug",	0,	    200,	0,	    0, //pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
//To Ugly skins
/*
void cast_TF_Skidrow_treehouse(edict_t *self)//sewer treehouse, ugly skins, no names
{
    //
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",         "004 003 001",	"cast_runt",	0,	    100,	0,	    0, //pistol
    "",         "004 001 007",	"cast_punk",	0,	    100,	0,	    0, //shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_motards(edict_t *self)//sr2 motards, ugly skins all pistol, no names
{	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",         "003 003 001",	"cast_runt",	0,	    0,		0,	    0, //pistol
    "",         "002 001 001",	"cast_bitch",	0,	    0,		0,	    0, //pistol
    "",	        "005 001 001",  "cast_thug",	0,	    0,		0,	    0, //pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}*/
//SR3 pack1 missing duo just 3 dogs
void cast_TF_Skidrow_courtyard2(edict_t *self)//sr3 courtyard2 and 1, no names
{	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "010 006 002",	"cast_runt",	0,	    0,		0,	    0,//courtyard1 pistol
    "",	        "514 004 002",  "cast_punk",	0,	    0,		0,	    1,//courtyard1 shotgun
    "",	        "513 004 002",  "cast_punk",	0,	    0,		0,	    1,//courtyard1 shotgun
    "",	        "514 003 002",	"cast_punk",	0,	    0,		0,	    1,//courtyard2 shotgun
    "",	        "006 005 002",	"cast_runt",	0,	    0,		0,	    0,//courtyard2 pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_street(edict_t *self)//sr3 street1, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "006 006 002",	"cast_runt",	0,	    0,		0,	    0,  //pistol
    "",	        "008 008 002",	"cast_runt",	0,	    0,		0,	    0,  //pistol
    "",	        "010 004 002",  "cast_runt",	64,	    0,		0,	    0,  //melee
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_hallway(edict_t *self)//sr3 hallway1,  no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",     	"006 006 002",	"cast_runt",	0,	    0,		0,	    0,  //pistol
	"",	        "514 003 002",	"cast_punk",	0,	    0,		0,	    0,  //shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
/*
//To Ugly skins
void cast_TF_Skidrow_postbattery(edict_t *self)//sr3 postbattery1 and 2,mostly shotty,  no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "004 001 001",	"cast_punk",    0,      120,	0,    0,    //postbattery1  shotgun
    "",	        "004 001 001",	"cast_punk",    0,      80,		0,    0,    //postbattery1  shotgun
	"",	        "008 008 002",	"cast_runt",    0,      120,	0,    0,    //postbattery2  pistol
	"",     	"514 003 002",	"cast_punk",    0,      120,	0,    1,    //postbattery2  shotgun
	"",	        "009 007 002",	"cast_runt",    0,      100,	0,    0,    //postbattery2  pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}*/
void cast_TF_Skidrow_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Betty",	"008 006 003",	"cast_bitch",	64,	    100,	0,  	0,    //sr1 melee
	"Beth",		"009 007 004",	"cast_bitch",	64,	    100,	0,  	0,    //sr1 melee
	"Lisa",		"012 015 012",	"cast_bitch",	64,	    120,	0,  	0,    //sr1 melee
	"Momo",		"072 014 009",	"cast_shorty",	8192,   150,	0,  	0, 	  //sr1 shogun
	"Leroy",	"010 010 003",	"cast_thug",	64,	    100,	0,  	0,    //sr1 melee
//	"Brewster", "002 001 001",	"cast_thug",	64,	    100,	0,  	0,	  //sr1 melee ugly skin
	"Bubba",	"017 016 008",	"cast_runt",	64,	    100,	9,  	0,    //sr1 melee
	"Magicj",	"020 011 005",	"cast_runt",	64,	    100,	0,  	0,    //sr1 melee
	"Momo",		"020 011 003",	"cast_runt",	0,	    100,	0,  	0,	  //sr2 //bouncer pistol
    "Mona",		"014 012 003",	"cast_bitch",	0,	    100,	0,  	0,    //sr2 pistol
	"Sluggo",	"019 010 011",	"cast_runt",	0,	    100,	0,  	0,    //bar_sr pistol
	"Lenny",	"018 011 007",	"cast_runt",	0,	    100,	0,  	0,	  //bar_sr pistol
	"Rocko",	"016 009 006",	"cast_thug",	0,	    100,	0,  	0,	  //bar_sr pistol
	"Louie",    "011 011 005",	"cast_runt",	0,	    100,	3,  	0,    //sr1 pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_names_melee(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Betty",	"008 006 003",	"cast_bitch",	64,	    100,	0,  	0,    //sr1 melee
	"Beth",		"009 007 004",	"cast_bitch",	64,	    100,	0,  	0,    //sr1 melee
	"Lisa",		"012 015 012",	"cast_bitch",	64,	    120,	0,  	0,    //sr1 melee
	"Leroy",	"010 010 003",	"cast_thug",	64,	    100,	0,  	0,    //sr1 melee
//	"Brewster", "002 001 001",	"cast_thug",	64,	    100,	0,  	0,	  //sr1 melee ugly skin
	"Bubba",	"017 016 008",	"cast_runt",	64,	    100,	9,  	0,    //sr1 melee
	"Magicj",	"020 011 005",	"cast_runt",	64,	    100,	0,  	0,    //sr1 melee
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_names_pistol(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Momo",		"020 011 003",	"cast_runt",	0,	    100,	0,  	0,	  //sr2 //bouncer pistol
    "Mona",		"014 012 003",	"cast_bitch",	0,	    100,	0,  	0,    //sr2 pistol
	"Sluggo",	"019 010 011",	"cast_runt",	0,	    100,	0,  	0,    //bar_sr pistol rented guy
    "Rocko",	"016 009 006",	"cast_thug",	0,	    100,	0,  	0,	  //bar_sr pistol rented guy
	"Lenny",	"018 011 007",	"cast_runt",	0,	    100,	0,  	0,	  //bar_sr pistol
	"Louie",    "011 011 005",	"cast_runt",	0,	    100,	3,  	0,    //sr1 pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_boss_lamont(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Lamont");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "024 017 010");
	self->spawnflags = 0;//pistol
	self->classname = "cast_thug";
	self->moral = 6;
//	self->scale = 1.05;
	self->health = 300 * currWave_plysCount;
}
void cast_TF_Skidrow_boss_jesus(edict_t *self)//sr4
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Jesus");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "21 017 010");
	self->spawnflags = 0;//pistol
	self->classname = "cast_runt";
	self->moral = 5;
	self->acc = 3;
	self->cal = 20;
	self->health = 400 * currWave_plysCount;
}
void cast_TF_Skidrow_boss(edict_t *spawn)
{
	switch (rand() % 2)
	{
	case 0:		cast_TF_Skidrow_boss_lamont(spawn);break;
	case 1:		cast_TF_Skidrow_boss_jesus(spawn);break;
	}
}
void cast_TF_Poisonville_boob(edict_t *self)//pv_1 boob, all pistol, no names, Nikki skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "030 031 032",	"cast_shorty",	0,	    100,	0,	    0,
	"",	        "503 033 032",	"cast_punk",	0,	    100,	0,	    0,//Cyclops
	"",	        "033 032 032",	"cast_shorty",	0,	    100,	0,	    0,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Poisonville_tanks(edict_t *self)//pv_b tanks, all tommygun, no names, Nikki skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "506 036 032",	"cast_punk",    64,     160,    0,      1,//Cyclops black nikki
	"",     	"504 031 031",	"cast_punk",    64,     140,    0,      1,//Cyclops yellow nikki
	"",	        "030 032 033",	"cast_whore",   64,     120,    0,      0,
	"",	        "035 030 031",	"cast_shorty",  64,     140,    0,      0,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Poisonville_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Mung",     "013 010 006",	"cast_runt",    64,     100,    9,      0,	//pv_h melee
    "Yolanda",	"041 050 003",	"cast_bitch",   0,      200,    0,      0, 	//pv_h pistol
    "Laurel",	"504 032 031",	"cast_punk",    0,      200,    0,      1,	//pv_h shotgun nikki cyclops
	"Hardey",	"114 032 031",	"cast_punk",    0,      200,    0,      0,	//pv_h shotgun nikki
    "Momo",     "072 014 009",  "cast_shorty",  0,      100,    0,      0,	//pv_h shotgun
    "Dubs",     "108 009 006",  "cast_punk",    64,     220,    0,      0,	//pv_h tommygun rented guy
    "Fingers",	"025 009 009",	"cast_thug",    0,      150,    0,      0,	//bar_pv pistol rented guy
	"Burt",	    "063 041 010",	"cast_thug",    0,      100,    0,      0,	//bar_pv pistol
    "Clarence", "047 013 003",  "cast_runt",    0,      100,    9,      0,	//bar_pv pistol
    "Candy",	"015 017 013",  "cast_bitch",   0,      100,    0,      0,	//bar_pv pistol
    "Louie",    "043 041 009",  "cast_runt",    0,      100,    0,      0,	//bar_pv pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Poisonville_boss(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Nikkiblanco");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "122 122 122");
	self->spawnflags = 4;//Flamethrower
	self->classname = "cast_punk";
	self->moral = 5;
//	self->scale = 1.06;
	self->health = 450 * currWave_plysCount;
}
void cast_TF_Shipyard_deckmonkeys(edict_t *self)//sy1 deck_monkeys and 2, shotty and tommyguns, no names, heilman skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",     	"060 060 060",	"cast_shorty",  0,      250,    0,      0,    //deck_monkeys shotgun
    "",	        "515 062 060",	"cast_punk",    64,     250,    0,      1,    //deck_monkeys tommygun
	"",	        "515 060 060",	"cast_punk",    64,     250,    0,      1,    //deck_monkeys tommygun
    "",	        "061 062 060",	"cast_shorty",  0,      300,    0,      0,    //deck_monkeys2 shotgun
    "",	        "060 060 060",	"cast_whore",   64,     100,    0,      0,    //deck_monkeys2 tommygun
	"",	        "508 060 060",	"cast_punk",    0,      300,    0,      1,    //deck_monkeys2 shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_goons(edict_t *self)//sy2 goons, shotty and tommyguns, no names, heilman skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "515 063 060",	"cast_punk",    0,      250,    0,      1, //shotgun
	"",     	"060 060 060",	"cast_whore",   0,      250,    0,      0, //shotgun
	"",	        "515 063 060",	"cast_punk",    0,       20,    0,      1,//Health not a typer error or Xatrix did that :p  //shotgun
	"",	        "515 063 060",	"cast_punk",    0,      250,    0,      1, //shotgun
    "",	        "063 064 060",	"cast_shorty",  64,     225,    0,      0, //tommygun
	"",	        "101 065 060",	"cast_punk",    64,     225,    0,      0, //tommygun
    "",	        "113 065 060",	"cast_punk",    64,     300,    0,      0, //tommygun
    "",	        "064 066 060",	"cast_shorty",  64,     300,    0,      0, //tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_swabs(edict_t *self)//sy2 swabs, mostly shotty and low health, no names, heilman skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "045 064 060",	"cast_punk",    64,      75,    0,      0, //tommygun
	"",     	"060 060 060",	"cast_shorty",   0,      75,    0,      0, //shotgun
	"",	        "060 060 060",	"cast_whore",    0,      75,    0,      0, //shotgun
	"",	        "066 064 060",	"cast_punk",     0,      75,    0,      0, //shotgun
    "",	        "515 063 060",	"cast_punk",     0,      75,    0,      1, //shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Dogg",     "016 014 003",  "cast_runt",    0,      100,    0,      0,    //bar_sy pistol
    "Selma",	"600 043 003",  "cast_bitch",   0,      100,    0,      2, 	  //bar_sy pistol
    "BigWillie","109 046 047",  "cast_punk",    0,      350,    0,      0,    //bar_sy shotgun
    "Sal",      "064 047 047",  "cast_shorty",  64,     250,    0,      0,    //bar_sy tommygun
    "Lefty",    "064 046 047",  "cast_punk",    64,     250,    0,      0,    //bar_sy tommygun
    "Moe",      "047 065 041",  "cast_runt",    0,       50,    0,      0,    //sy_h pistol
    "Popeye",   "040 019 048",  "cast_runt",    64,     300,    0,      1,    //sy_h melee
    "Momo",		"107 044 010",  "cast_punk",    0,      100,    0,      0,    //sy_h shotgun
	"Larry",	"048 047 041",  "cast_punk",    0,      200,    0,      0,    //sy_h shotgun
	"Curly",	"100 047 041",  "cast_punk",    0,      200,    0,      0,    //sy_h shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_boss(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Heilman");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "121 121 121");
	self->spawnflags = 64;
	self->classname = "cast_shorty";
	self->moral = 5;
    self->acc = 4;
	self->health = 650 * currWave_plysCount;
}

void cast_TF_Steeltown_teamno(edict_t *self)//steel1,steel2,steel3,steel4 teamno1, 2, 3, 7 no names, Moker team skin
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
//	"",	        "017 044 010",	"cast_bitch",   0,      100,    0,      0,    //steel1 teamno1  melee //Blue normal skin? doesn't fit
    "",	        "030 024 022",	"cast_punk",    0,      100,    0,      0,    //steel1 teamno1 shotgun
    "",	        "022 020 022",	"cast_shorty",  64,     150,    0,      0,    //steel3 teamno1 tommygun
    "",	        "500 021 022",	"cast_punk",    0,      100,    0,      1,    //steel3 teamno1 shotgun
    "",	        "023 021 022",	"cast_shorty",  0,      120,    0,      0,    //steel3 teamno1 shotgun
    "",	        "500 021 022",	"cast_punk",    64,     200,    0,      1,    //steel4 teamno1 tommygun
    "",	        "501 024 022",	"cast_punk",    64,     100,    0,      1,    //steel1 teamno2 tommygun
	"",	        "135 025 022",	"cast_punk",    64,     100,    0,      0,    //steel1 teamno2 tommygun
    "",	        "501 021 022",	"cast_punk",    0,      120,    0,      1,    //steel3 teamno2 shotgun
    "",	        "025 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno2 shotgun
    "",	        "501 024 022",	"cast_punk",    0,      120,    0,      0,    //steel3 teamno2 shotgun
    "",	        "023 021 021",	"cast_shorty",  0,      200,    0,      0,    //steel4 teamno2 shotgun
    "",	        "022 020 020",	"cast_shorty",  64,     200,    0,      0,    //steel4 teamno2 tommygun
	"",	        "500 021 022",	"cast_punk",    0,      100,    0,      1,    //steel1 teamno3 shotgun
    "",     	"022 020 022",	"cast_runt",    0,      100,    0,      0,    //steel1 teamno3 pistol
    "",	        "700 025 022",	"cast_punk",    4,      120,    0,      0,    //steel1 teamno3 flamethrower
    "",	        "022 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno3 shotgun
    "",	        "024 020 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno3 shotgun
    "",	        "700 025 022",	"cast_punk",    4,      100,    0,      0,    //steel3 teamno3 flamethrower
    "",	        "500 021 022",	"cast_punk",    64,     120,    0,      1,    //steel2 teamno7 tommygun
    "",	        "500 022 020",	"cast_punk",    0,      120,    0,      1,    //steel2 teamno7 shotgun
    "",	        "024 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno7 shotgun
    "",	        "022 020 022",	"cast_shorty",  16,     100,    0,      0,    //steel3 teamno7 hmg
    "",	        "136 024 022",	"cast_punk",    16,     100,    0,      0,    //steel3 teamno7 hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Steeltown_grp(edict_t *self)//steel2 grp1, 2 and 3, no names, Moker team skin
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "024 021 020",	"cast_runt",    0,      150,    0,      0,    //grp1 shotgun
	"",	        "020 021 021",	"cast_whore",   0,      100,    0,      0,    //grp1 shotgun
	"",	        "500 021 020",	"cast_punk",    0,      150,    0,      1,    //grp2 shotgun
    "",	        "023 021 020",	"cast_runt",    0,      100,    0,      0,    //grp2 shotgun
	"",	        "020 020 021",	"cast_whore",   0,      100,    0,      0,    //grp3 shotgun
    "",     	"020 020 021",	"cast_whore",   64,     100,    0,      0,    //grp3 tommygun
    "",	        "700 024 020",	"cast_punk",    4,      150,    0,      0,    //grp3 flamethrower
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Steeltown_Kid_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Kid_1",	"134 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2 melee
	"Kid_2",	"132 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2 melee
	"Kid_3",	"133 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2 melee
    "Kid_4",	"133 132 132",  "cast_thug",	64,		100,	0,		0,	//steel3 melee
    "Kid_5",	"134 132 132",  "cast_thug",	64,		100,	0,		0,	//steel3 melee
    "David",	"132 132 132",  "cast_punk",	64,		300,	0,		0,  //steel3 tommygun //Basicly a Kid_ just much health
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Steeltown_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Bambi",	"044 042 003",  "cast_bitch",	0,	    100,	0,		0,	//bar_st pistol
    "Rochelle",	"601 009 012",  "cast_whore",	0,	    300,	0,		2,	//bar_st shotgun rented girl
    "Brittany",	"046 040 010",  "cast_bitch",	0,	    100,	0,		0,	//steel1 pistol
    "Kroker",	"023 020 020",  "cast_runt",	64,	    150,	1,		0,	//steel1 tommygun
    "Momo",		"013 045 006",  "cast_runt",	0,	    100,	0,		0,	//steel1 //bouncer pistol
    "Mathew",	"048 042 015",  "cast_runt",	0,	    100,	0,		0,	//steel1 pistol
    "Oscar",	"044 012 006",  "cast_punk",	64,		250,	0,		0,  //steel2 tommygun rented guy
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Steeltown_boss(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Moker");//steel4
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "111 027 023");
	self->spawnflags = 8;//bazooka
    self->moral = 5;
    self->acc = 5;
//    self->localteam = teamno2;
	self->classname = "cast_punk";
    self->health = 800 * currWave_plysCount;
}

void cast_TF_Trainyard_trainwreck(edict_t *self)//ty1 trainwreck, no names, Tyrone bee skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "507 081 054",	"cast_punk",    64,     100,    0,      1,  //tommygun
	"",	        "051 051 052",	"cast_punk",    64,     150,    0,      0,  //tommygun
	"",	        "050 053 054",	"cast_shorty",  16,     100,    0,      0,  //hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_frontline(edict_t *self)//ty2 frontline, all tommygun no names, Tyrone bee skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "054 051 054",	"cast_shorty",  64,     100,    0,      0, //tommygun
	"",	        "507 051 052",	"cast_punk",    64,     100,    0,      1, //tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_trainteam(edict_t *self)//ty3 trainteam, no names, Tyrone bee skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "071 070 021",	"cast_whore",   64,     100,    0,      0, //tommygun
    "",	        "050 051 054",	"cast_shorty",  16,     100,    0,      0,  //hmg
	"",	        "054 052 052",	"cast_punk",    128,    100,    0,      0,  //grenade
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_poop(edict_t *self)//ty4 poop, no names, Tyrone bee skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "507 052 052",	"cast_punk",    0,      100,    0,      1, //shotgun
    "",	        "507 081 054",	"cast_punk",    16,     100,    0,      1,  //hmg
	"",	        "072 073 021",	"cast_whore",   0,      100,    0,      0, //shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_names(edict_t *self)//Only 2 names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Luke",	    "049 046 046",  "cast_runt",	0,	    100,	0,		0,	//ty1 pistol
    "Hann",	    "029 006 010",  "cast_thug",	0,	    100,	0,		0,	//ty1 pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_boss(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Tyrone");//ty4
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "507 082 054");
	self->spawnflags = 64;//Tommygun
    self->moral = 5;
    self->acc = 4;
//	self->scale = 1.05;
    self->head = 1;
	self->classname = "cast_punk";
    self->health = 500 * currWave_plysCount;
}
void cast_TF_Radio_City_dragon(edict_t *self)//rc2 dragon1 and 2, no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "057 055 058",	"cast_shorty",  0,      150,    0,      0,  //dragon1 shotgun
    "",	        "056 091 059",	"cast_punk",    64,     125,    0,      0,  //dragon2 tommygun
	"",	        "057 058 057",	"cast_punk",     8,     100,    0,      0,  //dragon2 bazooka
    "",	        "055 056 056",	"cast_whore",   64,     100,    0,      0,  //dragon1 tommygun
    "",	        "055 056 056",	"cast_whore",    0,     175,    0,      0,  //dragon2 shotgun
    "",	        "057 055 058",	"cast_shorty",  16,     100,    0,      0,  //dragon1 hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragonalley(edict_t *self)//rc2 dragonalley, no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "057 058 056",	"cast_bitch",    0,      150,    0,      0,  //pistol
    "",	        "058 057 056",	"cast_whore",    64,     125,    0,      0,  //tommygun
	"",	        "056 091 059",	"cast_punk",     16,     100,    0,      0,  //hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_streets(edict_t *self)//rc3 dragon_streets, no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "057 058 057",	"cast_punk",     64,     125,    0,      0,  //tommygun
    "",	        "057 058 056",	"cast_whore",    64,     150,    0,      0,  //tommygun
	"",	        "058 059 057",	"cast_punk",     16,      80,    0,      0,  //hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_hoppers(edict_t *self)//rc3 dragon_hoppers, no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "057 058 057",	"cast_punk",      0,     125,    0,      0,  //shotgun
    "",	        "058 057 056",	"cast_whore",    64,     125,    0,      0,  //tommygun
	"",	        "056 056 057",	"cast_shorty",    8,     125,    0,      0,  //bazooka
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_rooftop(edict_t *self)//rc3 dragon_rooftop, all bazooka, no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "057 058 057",	"cast_punk",      8,     125,    0,      0,  //bazooka
    "",	        "058 057 056",	"cast_whore",     8,     150,    0,      0,  //bazooka
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_building(edict_t *self)//rc3 dragon_building, no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "058 059 057",	"cast_punk",     16,     150,    0,      0,  //hmg
    "",	        "057 058 056",	"cast_whore",     8,     250,    0,      0,  //bazooka
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_street(edict_t *self)//rc4 and rc5 street1, no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "058 055 058",	"cast_runt",     0,     100,    0,      0,   //rc4 shotgun
    "",	        "057 058 056",	"cast_whore",    64,    100,    0,      0,   //rc4 tommygun
    "",	        "056 059 057",	"cast_punk",     0,     125,    0,      0,   //rc4 shotgun
    "",	        "058 090 059",	"cast_punk",     8,     100,    0,      0,   //rc5 bazooka
//    "",	        "058 090 059",	"cast_punk",     8,     100,    0,      0,   //rc5 bazooka double name
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
/*
//To much double
void cast_TF_Radio_City_dragon_office(edict_t *self)//rc5 office, allot of double skins :/ no names, dragon skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "058 090 059",	"cast_punk",     64,    100,    0,      0,   //rc5 tommygun
    "",	        "058 090 059",	"cast_punk",     64,    100,    0,      0,   //rc5 tommygun  double name
    "",	        "058 090 059",	"cast_punk",     16,    100,    0,      0,   //rc5 hmg
    "",	        "058 090 059",	"cast_punk",     64,    100,    0,      0,   //rc5 tommygun
    "",	        "058 090 059",	"cast_punk",     0,     100,    0,      0,   //rc5 shotgun
    "",	        "058 090 059",	"cast_punk",     64,    100,    0,      0,   //rc5 tommygun double name
    "",	        "058 090 059",	"cast_punk",      0,    100,    0,      0,   //rc5 shotgun double name
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}*/
void cast_TF_Radio_City_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Clarence", "045 024 005",  "cast_runt",	0,	    100,	1,		0,      //bar_rc pistol
	"Adolf",	"047 042 009",  "cast_thug",	0,		100,	0,		0,		//bar_rc pistol
    "Burt",	    "023 017 005",  "cast_thug",	0,		100,	0,		0,		//bar_rc pistol
    "Candy",	"019 019 015",  "cast_bitch",	0,	    100,	0,		0,	    //bar_rc pistol
    "Scalper",	"103 044 009",  "cast_thug",	0,		100,	0,		0,		//bar_rc pistol
    "Walter",	"130 130 010",  "cast_shorty",	16,		350,	0,		0,		//bar_rc hmg joker skin
    "Harpo",	"142 140 140",  "cast_runt",	64,	    200,	0,		0,	    //rc1 tommygun
    "Butch",	"131 130 010",	"cast_shorty",	0,	    100,	0,		0,	    //rc1 shotgun joker skin
    "Bubba",	"042 042 010",  "cast_runt",	64,	    100,	9,		0,	    //rc1 tommygun
    "Patrick",	"511 130 010",  "cast_punk",	64,		100,	0,		1,      //rc1 tommygun joker skin
  	"Groucho",	"140 141 046",  "cast_runt",	64,	    150,	0,		0,	    //rc1 tommygun
	"Chico",	"141 141 046",  "cast_runt",	64,	    150,	0,		0,	    //rc1 tommygun //same skin as grouncho
	"Bubba",	"130 131 010",	"cast_shorty",	64,		100,    9,		0,	    //rc2 tommygun joker skin
    "Lola",		"045 049 015",  "cast_bitch",	0,	    100,	0,		0, 	    //rc3 pistol
	"Dude",		"130 131 010",	"cast_shorty",	0,	    200,	9,		0,      //rc3 pistol joker skin
    "Momo",		"049 041 009",  "cast_runt",	0,	    100,	0,		0,	    //rc4 //bouncer pistol
    "Donny",	"512 131 010",  "cast_punk",	64,		200,	0,		1,      //rc4 tommygun joker skin
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_boss(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "nikkiblanco");//rcboss1
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "122 122 122");
	self->spawnflags = 64;//Tommygun
	self->classname = "cast_punk";
	self->health = 800 * currWave_plysCount;
}


void cast_TF_Crystal_Palace_boss_kingpin(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Kingpin");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "120 120 120");
	self->spawnflags = 0;//pistol
    self->acc = 5;
    self->cal = 5;
//	self->scale = 1.15;
	self->classname = "cast_runt";
	self->health = 1500 * currWave_plysCount;
}
void cast_TF_Crystal_Palace_boss_blunt(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Blunt");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "080 059 059");
	self->spawnflags = 16;//hmg //18 =triger spawned (>>2)
	self->classname = "cast_whore";
	self->health = 1500 * currWave_plysCount;//150000 normally
}
void cast_TF_Crystal_Palace_boss(edict_t *spawn)
{
	switch (rand() % 4) //a little more rand!!
	{
	case 0:		cast_TF_Crystal_Palace_boss_kingpin(spawn);break;
	case 1:		cast_TF_Crystal_Palace_boss_blunt(spawn);break;
	case 2:		cast_TF_Crystal_Palace_boss_kingpin(spawn);break;
	case 3:		cast_TF_Crystal_Palace_boss_blunt(spawn);break;
	}

	//store boss hp/ID.
	boss_maxHP = spawn->health;
	boss_entityID = spawn;
}
// end setup cast skins
///////////////////////

extern voice_table_t nickiblanco[];
extern voice_table_t ty_tyrone[];
extern voice_table_t steeltown_moker[];
extern voice_table_t sr_jesus[];
extern voice_table_t heilman[];
extern voice_table_t blunt[];
extern voice_table_t kingpin[];



void cast_boss_sounds()
{
	voice_table_t *tmpVoice = kingpin;
	edict_t *self;
	int i, index;


	//set bot specific sound table and index
	switch (boss_entityID->name_index)
	{
	case NAME_NICKIBLANCO: tmpVoice = nickiblanco;	break; //todo: more boss?
	case NAME_TYRONE: tmpVoice = ty_tyrone;	break;
	case NAME_MOKER: tmpVoice = steeltown_moker;	break;
	case NAME_JESUS: tmpVoice = sr_jesus;	break;
	case NAME_HEILMAN: tmpVoice = heilman;	break;
	case NAME_BLUNT: tmpVoice = blunt;
		index = 8; //I'm gonna unload in your fucking face!
		if (boss_called_help == 2)
			index = 13; // You're getting me all worked up!
		else if (boss_called_help == 3)
			index = 9; //You like to watch? Well, watch this!
		break;

	default:
	case NAME_KINGPIN: tmpVoice = kingpin;
		index = 8; //sick the dogs on your ass
		if (boss_called_help == 2)
			index = 7; // Your ass is goin' down
		else if (boss_called_help == 3)
			index = 9; //You come close, but you never made it

		break;
	}

	for_each_player(self, i)
	{
		if (self->client->pers.spectator == PLAYING)
			Voice_Specific(boss_entityID, self, tmpVoice, index);
	}

}

//endWave/endGame
void cast_TF_free(void)
{
	int i, count = 0;
	level.currWave_castCount;

	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (level.characters[i])
		{
			//free characters
			if (level.characters[i]->svflags & SVF_MONSTER) //cast
			{
				AI_UnloadCastMemory(level.characters[i]);
				G_FreeEdict(level.characters[i]);
			}
		}
	}

	//clear configstrings. overloaded with to many skins asigned to the same model index
	/*for (i = CS_MODELSKINS; i < CS_MODELSKINS + MAX_MODELS; i++)
	{
		//need a way to test if the exist
		gi.configstring(i, "");		//hypov8 sending more then 1 of these in a row crash kp.
									//its used in ClientDisconnect. posible bug if 2 clients leave at once

	}*/
	//not sure yet why server will get skin data incorrect
	//this causes svc_muzzleflash3 issues (3 is char "count" of skin "001")
	//some times its sent in packet. some times its not.


	if (level.currWave_castCount)
		level.currWave_castCount = 0; //reset spawned cast count
}

//check if enemy has died. add if required
void cast_TF_checkEnemyState()
{
	int i, count = 0;

	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (level.characters[i])
		{
			if (level.characters[i]->svflags & SVF_MONSTER) //cast
			{
				if (level.characters[i]->inuse == 0 ||
					level.characters[i]->health <= 0 ||
					level.characters[i]->deadflag == DEAD_DEAD ||
					level.characters[i]->s.origin[2] < -4096 //fallen in void!!!
					)

				{
					//check if boss died
					if (level.waveNum == currWave_length && boss_entityID && level.characters[i] == boss_entityID)
					{
						level.currWave_castCount = 1;
						level.num_characters = 1;
						level.waveEnemyCount = 1;
					}

					//free any dead cast
					level.characters[i]->character_index = 0;
					level.characters[i] = NULL;
					level.num_characters--;
					level.currWave_castCount--;
					level.waveEnemyCount--;
				}
				else
					count++; //count current cast

			}
			else //human
			{
				if (level.characters[i]->inuse == 0)
				{
					//free clients
					level.characters[i]->character_index = 0;
					level.characters[i] = NULL;
					level.num_characters--;
				}
			}
		}
	}


	//final boss. spawn some help
	if (level.waveNum == currWave_length)
	{
		if (boss_called_help == 0 && boss_entityID && boss_entityID->health < (boss_maxHP *.75))
		{
			boss_called_help = 1;
			level.waveEnemyCount = level.currWave_castCount+ 5 + (int)skill->value; //how many boss helpers to spawn??
			cast_boss_sounds();
		}
		else if (boss_called_help == 1 && boss_entityID && boss_entityID->health < (boss_maxHP *.5))
		{
			boss_called_help = 2;
			level.waveEnemyCount = level.currWave_castCount+ 5 + (int)skill->value; //how many boss helpers to spawn??
			cast_boss_sounds();
		}
		else if (boss_called_help == 2 && boss_entityID && boss_entityID->health < (boss_maxHP *.25))
		{
			boss_called_help = 3;
			level.waveEnemyCount = level.currWave_castCount+ 5 + (int)skill->value; //how many boss helpers to spawn??
			cast_boss_sounds();
		}
	}

	//free slot. add enemy
	if (count < level.waveEnemyCount && count < currWave_castMax)
		cast_TF_spawn();


	//return level.currWave_castCount;
}

// find the closest player to pick on
void cast_TF_setEnemyPlayer(edict_t *spawn)
{
	int j, best = 1;
	float dist = 99999;
	edict_t *player;

	//attack closest player.
	for_each_player(player, j)
	{
		if (player->health <= 0 || player->solid == SOLID_NOT) // MH: exclude spectators
			continue;

		AI_MakeEnemy(spawn, player, 0);
	}
}

//////////////////
// rand spawn types
void cast_TF_spawnWave1(edict_t *spawn)//Skidrow no rats
{
    if (skill->value < 2)
    {
        switch (rand() % 6)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_Skidrow_names_melee(spawn);break;
        case 2:	cast_TF_Skidrow_names_melee(spawn);break;
        case 3:	cast_TF_Skidrow_names_melee(spawn);break;
        case 4:	cast_TF_Skidrow_names_melee(spawn);break;
        case 5:	cast_TF_Skidrow_names_melee(spawn);break;
        }
    }
    else
    {
        switch (rand() % 6)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_Skidrow_names_melee(spawn);break;
        case 2:	cast_TF_Skidrow_names_melee(spawn);break;
        case 3:	cast_TF_Skidrow_names_melee(spawn);break;
        case 4:	cast_TF_Skidrow_names_pistol(spawn);break;
        case 5:	cast_TF_Skidrow_courtyard(spawn);break;//Police
        }
    }
}
void cast_TF_spawnWave2(edict_t *spawn)//Skidrow
{
	if (level.waveEnemyCount == currWave_castMax)
		cast_TF_Skidrow_boss(spawn);//spawn boss when almost done
	else
	{
		switch (rand() % 12)
		{
		case 0:	cast_TF_dog(spawn);break;
		case 1:	cast_TF_dog(spawn);break;
		case 2:	cast_TF_rat(spawn);break;
		case 3:	cast_TF_rat(spawn);break;
		case 4:	cast_TF_Skidrow_courtyard(spawn);break;//Police
		case 5:	cast_TF_Skidrow_courtyard2(spawn);break;
		case 6:	cast_TF_Skidrow_street(spawn);break;
		case 7:	cast_TF_Skidrow_hallway(spawn);break;
		case 8: cast_TF_Skidrow_names(spawn);break;
		case 9: cast_TF_Skidrow_names(spawn);break;
		case 10: cast_TF_Skidrow_names(spawn);break;
		case 11: cast_TF_Skidrow_names(spawn);break;
		}
	}
}
void cast_TF_spawnWave3(edict_t *spawn)//Poisonville
{
	switch (rand() % 7)
	{
	case 0:	cast_TF_dog(spawn);break;
	case 1:	cast_TF_rat(spawn);break;
	case 2:	cast_TF_Poisonville_names(spawn);break;
	case 3:	cast_TF_Poisonville_names(spawn);break;
	case 4:	cast_TF_Poisonville_names(spawn);break;
    case 5:	cast_TF_Poisonville_names(spawn);break;
	case 6:	cast_TF_Poisonville_names(spawn);break;
	}
}
void cast_TF_spawnWave4(edict_t *spawn)//Poisonville
{
    if (level.waveEnemyCount == currWave_castMax)
		cast_TF_Poisonville_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 7)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Poisonville_tanks(spawn);break;//Nikki skins
        case 3:	cast_TF_Poisonville_tanks(spawn);break;//Nikki skins
        case 4:	cast_TF_Poisonville_boob(spawn);break;//Nikki skins
        case 5:	cast_TF_Poisonville_boob(spawn);break;//Nikki skins
//        case 6:	cast_TF_Poisonville_names(spawn);break;
        }
	}
}
void cast_TF_spawnWave5(edict_t *spawn)//Shipyard
{
	switch (rand() % 7)
	{
    case 0:	cast_TF_dog(spawn);break;
	case 1:	cast_TF_rat(spawn);break;
	case 2:	cast_TF_Shipyard_names(spawn);break;
	case 3:	cast_TF_Shipyard_names(spawn);break;
	case 4:	cast_TF_Shipyard_names(spawn);break;
    case 5:	cast_TF_Shipyard_names(spawn);break;
	case 6:	cast_TF_Shipyard_names(spawn);break;
	}
}
void cast_TF_spawnWave6(edict_t *spawn)//Shipyard
{
    if (level.waveEnemyCount == currWave_castMax)
		cast_TF_Shipyard_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 8)
        {
            case 0:	cast_TF_dog(spawn);break;
            case 1:	cast_TF_rat(spawn);break;
            case 2:	cast_TF_Shipyard_deckmonkeys(spawn);break;//heilman skins
            case 3:	cast_TF_Shipyard_deckmonkeys(spawn);break;//heilman skins
            case 4:	cast_TF_Shipyard_goons(spawn);break; //heilman skins
            case 5:	cast_TF_Shipyard_goons(spawn);break; //heilman skins
            case 6:	cast_TF_Shipyard_swabs(spawn);break; //heilman skins
            case 7:	cast_TF_Shipyard_swabs(spawn);break; //heilman skins
        }
	}
}
void cast_TF_spawnWave7(edict_t *spawn)//Steeltown
{
    if (level.waveEnemyCount == currWave_castMax)
		cast_TF_Steeltown_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 8)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Steeltown_teamno(spawn);break; //Moker team skin
        case 3:	cast_TF_Steeltown_teamno(spawn);break; //Moker team skin
        case 4:	cast_TF_Steeltown_grp(spawn);break; //Moker team skin
        case 5:	cast_TF_Steeltown_grp(spawn);break;//Moker team skin
        case 6:	cast_TF_Steeltown_Kid_names(spawn);break;
        case 7:	cast_TF_Steeltown_names(spawn);break;
        }
	}
}
void cast_TF_spawnWave8(edict_t *spawn)//Trainyard
{
    if (level.waveEnemyCount == currWave_castMax)
		cast_TF_Trainyard_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 7)
        { //hypov8 note: posibly a bad actor here or above.. any sfx not used on other models?
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Trainyard_trainwreck(spawn);break;
        case 3:	cast_TF_Trainyard_frontline(spawn);break;
        case 4:	cast_TF_Trainyard_trainteam(spawn);break;//Only team with gl
        case 5:	cast_TF_Trainyard_trainteam(spawn);break;//Only team with gl
        case 6:	cast_TF_Trainyard_poop(spawn);break;
//        case 7:	cast_TF_Trainyard_names(spawn);break;//Only 2 names and not really fitting skins
        }
	}
}
void cast_TF_spawnWave9(edict_t *spawn)//Radiocity
{
	switch (rand() % 7)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_rat(spawn);break;
	case 2:	cast_TF_Radio_City_dragon(spawn);break;
    case 3:	cast_TF_Radio_City_dragonalley(spawn);break;
	case 4:	cast_TF_Radio_City_names(spawn);break;
	case 5:	cast_TF_Radio_City_names(spawn);break;
    case 6:	cast_TF_Radio_City_names(spawn);break;
	}
}
void cast_TF_spawnWave10(edict_t *spawn)//Radiocity
{
    if (level.waveEnemyCount == currWave_castMax)
		cast_TF_Radio_City_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 7)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Radio_City_dragon_street(spawn);break;
        case 3:	cast_TF_Radio_City_dragon_hoppers(spawn);break;
        case 4:	cast_TF_Radio_City_dragon_rooftop(spawn);break;
        case 5:	cast_TF_Radio_City_dragon_building(spawn);break;
        case 6:	cast_TF_Radio_City_dragon_street(spawn);break;
    //    case 7:	cast_TF_Radio_City_dragon_office(spawn);break;
        }
	}
}
void cast_TF_spawnWaveBoss(edict_t *spawn)
{
	//boss.. or his helpers?
	if (boss_called_help == 0)
		cast_TF_Crystal_Palace_boss(spawn);
	else if (boss_called_help == 1)
		cast_TF_cast_melee(spawn);
	else if (boss_called_help == 2)
		cast_TF_cast_pistol(spawn);
	else if (boss_called_help == 3)
		cast_TF_cast_shotgun(spawn);
}
//end rand spawn types
//////////////////////


//generate types based on game length
void cast_TF_spawnTypes(edict_t *spawn)
{
	if (currWave_length == WAVELEN_LONG -1)	// long wave
	{
		switch (level.waveNum)
		{
		case 0: cast_TF_spawnWave1(spawn);	break; //wave 1
		case 1:	cast_TF_spawnWave2(spawn);	break; //wave 2
		case 2: cast_TF_spawnWave3(spawn);	break; //wave 3
		case 3: cast_TF_spawnWave4(spawn);	break; //wave 4
		case 4: cast_TF_spawnWave5(spawn);	break; //wave 5
		case 5: cast_TF_spawnWave6(spawn);	break; //wave 6
		case 6: cast_TF_spawnWave7(spawn);	break; //wave 7
		case 7: cast_TF_spawnWave8(spawn);	break; //wave 8
		case 8: cast_TF_spawnWave9(spawn);	break; //wave 9
		case 9: cast_TF_spawnWave10(spawn);	break; //wave 10
		case 10: cast_TF_spawnWaveBoss(spawn); break; //wave 11
		}
	}
	else if (currWave_length == WAVELEN_MED - 1)//med wave
	{
		switch (level.waveNum)
		{
		case 0: cast_TF_spawnWave1(spawn);	break; //wave 1
		case 1:	cast_TF_spawnWave2(spawn);	break; //wave 2
		case 2: cast_TF_spawnWave4(spawn);	break; //wave 3
		case 3: cast_TF_spawnWave5(spawn);	break; //wave 4
		case 4: cast_TF_spawnWave6(spawn);	break; //wave 5
		case 5: cast_TF_spawnWave8(spawn);	break; //wave 6
		case 6: cast_TF_spawnWave9(spawn);	break; //wave 7
		case 7: cast_TF_spawnWaveBoss(spawn); break; //wave 8
		}
	}
	else 	//WAVELEN_SHORT //short wave
	{
		switch (level.waveNum)
		{
		case 0: cast_TF_spawnWave2(spawn);	break; //wave 1
		case 1: cast_TF_spawnWave4(spawn);	break; //wave 2
		case 2: cast_TF_spawnWave6(spawn);	break; //wave 3
		case 3: cast_TF_spawnWave8(spawn);	break; //wave 4
		case 4: cast_TF_spawnWaveBoss(spawn); break; //wave 5
		}
	}
}

//spawn in a cast model
void cast_TF_spawn(void)
{
	edict_t *spawn, *spawnspot;

	{
		if (level.currWave_castCount >= MAX_CHARACTERS)
			return;

		spawn = G_Spawn();
		spawnspot = cast_SelectRandomDeathmatchSpawnPoint(spawn);
		VectorCopy(spawnspot->s.angles, spawn->s.angles);
		VectorCopy(spawnspot->s.origin, spawn->s.origin);
		spawn->s.origin[2] += 1;

		spawn->classname = "cast_dog"; //temp untill all waves are done

		//spawn rand type depands on wave num
		cast_TF_spawnTypes(spawn);

		gi.linkentity(spawn);

		if (level.num_characters == MAX_CHARACTERS)
		{
			gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
			G_FreeEdict(spawn);
			return;
		}

		ED_CallSpawn(spawn);

		spawn->start_ent = spawnspot; // go back to spawnpoint

		if(!Q_strcasecmp(spawn->classname, "cast_dog"))
			spawn->cast_group = 23; //hypov8 does this work?
		if(!Q_strcasecmp(spawn->classname, "cast_rat"))
			spawn->cast_group = 22;
		else
			spawn->cast_group = 21;

		if (!spawn->health)
            spawn->health = 100;

		//make enemy health varable

		if ((int)wavetype->value == 0)		//5 waves
			spawn->health =  (int)((float)spawn->health * (0.5f + (float)(level.waveNum) / ((WAVELEN_SHORT-1)*2))); //50% to 100%
		else if ((int)wavetype->value == 1)	//8 waves
			spawn->health =  (int)((float)spawn->health * (0.5f + (float)(level.waveNum) / ((WAVELEN_MED-1)*2))); //50% to 100%
		else								//11 waves
			spawn->health =  (int)((float)spawn->health * (0.5f + (float)(level.waveNum) / ((WAVELEN_LONG-1)*2))); //50% to 100%

		if (spawn->health < 10)
            spawn->health = 10;

		//set what player to attack
		cast_TF_setEnemyPlayer(spawn);

		//add enemy to counter
		level.currWave_castCount++;
	}
}

//total enemy counts per wave.
//this will be multiplied by player counts later
#if HYPODEBUG
static int wave_shortGame[5] = { 2, 2, 2, 2, 1 };
static int wave_medGame[8] = { 2, 2, 2, 2, 2, 2, 2, 1 };
static int wave_longGame[11] = { 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1 };
#else
static int wave_shortGame[5] = { 13, 16, 17, 21, 1 };
static int wave_medGame[8] = { 13, 14, 16, 17, 17, 20, 21, 1 };
static int wave_longGame[11] = { 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 1 };
#endif

//max number of active enemy's spawned into a level.
//the more players the more active enemy
static int wave_skill[5][9] = {
	//num players
	//0, 1, 2, 3, 4, 5, 6, 7,  8		    //max 8 players
	{ 0, 4, 5, 6, 7, 8, 9, 10, 11},         //novice
	{ 0, 5, 6, 7, 8, 9, 10, 11, 12},        //easy
	{ 0, 6, 8, 10, 12, 14, 16, 16, 16 },    //medium
	{ 0, 7, 9, 11, 13, 15, 16, 16, 16 },    //hard
	{ 0, 8, 11, 14, 16, 16, 16, 16, 16 }     //real
};

/*
==========
cast_TF_setupEnemyCounters

this is where we generate the eneny spawn counts
==========
*/
void cast_TF_setupEnemyCounters(void)
{
	edict_t *self;
	int playerCount = 0;//Maxplayers
	int i, sk;
	int firstPlayer = 0;


	if (skill->value == 0)			sk = 0;//novice
	else if (skill->value == 1)		sk = 1;//easy
	else if (skill->value == 2)		sk = 2;//medium
	else if (skill->value == 3)		sk = 3;//hard
	else							sk = 4;//real

	//count players that will enter the wave
	for_each_player(self, i)
	{
		if (self->client->pers.spectator == PLAYING)
		{
			playerCount++;
			AddCharacterToGame(self); //add player to level.characters
			if (!firstPlayer)
			{
				firstPlayer = 1;
				self->nav_TF_isFirstPayer = 1; //hypov8 nav. todo: if player dies
			}
			self->cast_group = 1;
		}
	}
	currWave_plysCount = playerCount;
	currWave_castMax = wave_skill[sk][(playerCount<8)? playerCount : 8 ]; //skill based enemy limits. force array limit to 8
	boss_called_help = 0;
	boss_entityID = NULL;

	//make sure we have enough spawns(level size)
	if (level.dmSpawnPointCount < currWave_castMax)
		currWave_castMax = (int)( (float)level.dmSpawnPointCount * .75);
	if (currWave_castMax < 4)
		currWave_castMax = 4; //force 4 as min


	//get wave count
	if ((int)wavetype->value == 0)
	{
		currWave_length = WAVELEN_SHORT - 1;//short wave
		level.waveEnemyCount =(level.waveNum == currWave_length)? 1 :  wave_shortGame[level.waveNum] * playerCount; //force 1 boss
	}
	else if ((int)wavetype->value == 1)
	{
		currWave_length = WAVELEN_MED - 1;//med wave
		level.waveEnemyCount =(level.waveNum == currWave_length)? 1 :  wave_medGame[level.waveNum] * playerCount; //force 1 boss
	}
	else //wavetype >= 2
	{
		currWave_length = WAVELEN_LONG - 1;//long wave
		level.waveEnemyCount = (level.waveNum == currWave_length)? 1 : wave_longGame[level.waveNum] * playerCount; //force 1 boss
	}

}

