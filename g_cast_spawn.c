#include "g_local.h"
#include "g_cast_spawn.h"

#define ARYSIZE(x) (sizeof(x) / sizeof(x[0])) //get dynamic size of array



#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT];// = {NULL, NULL, NULL}; //hypov8

void cast_TF_spawn();
static int currWave_plysCount = 0; //players
static int currWave_length = 0; //long, med, short
static int currWave_castMax = 0; //max enemy allowed on map

//boss info
edict_t *boss_entityID = NULL;
int boss_maxHP;
static int boss_called_help = 0; //1 = melee, 2= pistol, 3= shotty

#if 1
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
	level.waveEnemyCount_cur = 0;
	level.waveEnemyCount_total = 0;
}
void cast_pawn_o_matic_spawn ()//Randoms spawn testing
{
	edict_t *spawn,*spawnspot;
	int count = 0;

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

		ED_CallSpawn(spawn);


		//copy entity our list so we can free later
		pawnGuy[count] = spawn;

		count++;
		//add enemy to counter
		level.waveEnemyCount_cur++;

		//no more required
		if (count == 1)
			return;

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
	level.waveEnemyCount_cur = 0;
	level.waveEnemyCount_total = 0;
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
		level.waveEnemyCount_cur++;

		//no more required
		if (count == BUYGUY_COUNT)
			return;
	}
}
#endif // 1

//apply skins.. consider skill
void cast_TF_applyRandSkin(edict_t *self, localteam_skins_s *skins, int idx)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[idx].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[idx].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[idx].classname);
	self->spawnflags = skins[idx].spawnflags;
	self->health = skins[idx].health;
	self->count = skins[idx].count;
	self->head = skins[idx].head;
}

///////////////////////
// setup cast skins
//any matching cast model part can only have 21 differnt skins
//21 heads
//21 legs
//21 body
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
/*
//Not used anymore
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
//	"Kroker",		"023 020 020",  "cast_runt",	64,	    100,	    1,		0,	//steel1 Got normally 150 health
//	"kid_1",		"134 132 132",  "cast_runt",	64,	    100,	    0,		0,	//steel2
//	"kid_2",		"132 132 132",  "cast_runt",	64,	    100,	    0,		0,	//steel2
//	"kid_3",		"133 132 132",  "cast_runt",	64,	    100,	    0,		0,	//steel2
	"popeye",		"040 019 048",  "cast_runt",	64,	    100,	    0,		0,	//sy_h Got normaly health 300 and diffrent head?
	"harpo",		"142 140 140",  "cast_runt",	64,	    100,	    0,		0,	//rc1 Got normally 200 health
	"bubba",		"042 042 010",  "cast_runt",	64,	    100,	    9,		0,	//rc1
	"groucho",		"140 141 046",  "cast_runt",	64,	    100,	    0,		0,	//rc1 Got normally 150 health
	"chico",		"141 141 046",  "cast_runt",	64,	    100,	    0,		0,	//rc1 Got normally 150 health //same skin as grouncho
    "leroy",	    "010 010 003",  "cast_thug",	64,		100,		0,		0,  //sr1
//	"johnny",	    "011 007 004",  "cast_thug",	64,		100,		0,		0,	//sr1 Got normally 80 health in courtyard group
//	"Brewster", "002 001 001",	"cast_thug",	64,	    100,	0,  	0,	  //sr1 melee ugly skin
//	"IntroGuy",     "041 026 010",  "cast_thug",	64,		100,		0,		0,	//sr1 player
//    "kid_4",	    "133 132 132",  "cast_thug",	64,		100,		0,		0,	//steel3
//    "kid_5",	    "134 132 132",  "cast_thug",	64,		100,		0,		0,	//steel3
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_cast_pistol(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	    head
	"Mona",		    "014 012 003",  "cast_bitch",	0,	    100,		0,		0,	//sr2
	"Yolanda",	    "041 050 003",  "cast_bitch",	0,	    100,		0,		0,	//pv_h got normally 200 health
	"Candy",	    "015 017 013",  "cast_bitch",	0,	    100,		0,		0,	//bar_pv same name as in bar_rc
//	"Brittany",	    "046 040 010",  "cast_bitch",	0,	    100,		0,		0,	//steel1
//	"Bambi",	    "044 042 003",  "cast_bitch",	0,	    100,		0,		0,	//bar_st
	"Lola",		    "045 049 015",  "cast_bitch",	0,	    100,		0,		0, 	//rc3
	"Candy",	    "019 019 015",  "cast_bitch",	0,	    100,		0,		0,	//bar_rc same name as in bar_pv
	"Selma",	    "600 043 003",  "cast_bitch",	0,	    100,		0,		0, 	//bar_sy
//	"bernie",	    "011 012 004",  "cast_runt",	0,	    100,		1,		0,	//sr1 Got normally 150 health in courtyard group
//	"punky",	    "005 003 001",  "cast_runt",	0,	    100,		0,		0,	//sr1 Got normally 50 health// in sewer rats group
	"Momo",		    "020 011 003",  "cast_runt",	0,	    100,		0,		0,	//sr2 //bouncer
//	"Momo",		    "013 045 006",  "cast_runt",	0,	    100,		0,		0,	//steel1 //bouncer
	"Momo",		    "049 041 009",  "cast_runt",	0,	    100,		0,		0,	//rc4 //bouncer
	"Moe",		    "047 065 041",  "cast_runt",	0,	    100,		0,		0,	//sy_h
//	"Mathew",	    "048 042 015",  "cast_runt",	0,	    100,		0,		0,	//steel1
	"Luke",		    "049 046 046",  "cast_runt",	0,	    100,		0,		0,	//ty1
	"Clarence",     "047 013 003",  "cast_runt",	0,	    100,		9,		0,	//bar_pv
//	"Buster",	    "046 010 009",  "cast_runt",	0,	    100,		9,		0,	//bar_pv Got normally 120 health
	"Louie",	    "043 041 009",  "cast_runt",	0,	    100,		0,		0,	//bar_pv
	"Clarence",     "045 024 005",  "cast_runt",	0,	    100,		1,		0,	//bar_rc
//	"Sluggo",	    "019 010 011",	"cast_runt",	0,	    100,	    0,  	0,  //bar_sr pistol rented guy
	"Lenny",	    "018 011 007",  "cast_runt",	0,	    100,		0,		0,	//bar_sr
	"Dogg",		    "016 014 003",  "cast_runt",	0,	    100,		0,		0, 	//bar_sy
 //   "Fingers",	    "025 009 009",  "cast_thug",	0,		100,		0,		0,	//bar_pv Got normally 150 health
	"Burt",	        "063 041 010",  "cast_thug",	0,		100,		0,		0,	//bar_pv
	"Adolf",	    "047 042 009",  "cast_thug",	0,		100,		0,		0,	//bar_rc
    "Burt",	        "023 017 005",  "cast_thug",	0,		100,		0,		0,	//bar_rc
    "Scalper",	    "103 044 009",  "cast_thug",	0,		100,		0,		0,	//bar_rc
//	"Rocko",	    "016 009 006",	"cast_thug",	0,	    100,	    0,  	0,	//bar_sr pistol rented guy
//	"igmo",	        "003 002 001",  "cast_punk",	0,		100,		0,		0,	//sr1 Got normally 200 health in sewer rats group
    "Lamont",	    "024 017 010",  "cast_thug",	0,		100,		0,		0,	//sr2 Got normally 300 health
    "Hann",	        "029 006 010",  "cast_thug",	0,		100,		0,		0,	//ty1
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_cast_shotgun(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		  count	    head
 //   "Rochelle",	    "601 009 012",	"cast_whore",	0, 	    100,		0,		0,	//rc1
	"Momo",		    "072 014 009",	"cast_shorty",	0, 	    100,		0,		0, 	//sr1 Got normally 150 health
	"Butch",	    "131 130 010",	"cast_shorty",	0, 	    100,		0,		0,	//rc1
	"Dude",		    "130 131 010",	"cast_shorty",	0, 	    100,		9,		0,  //rc3 Got normally 200 health
	"Momo",		    "072 014 009",	"cast_shorty",	0, 	    100,		0,		0,  //pv_h
    "Bigwillie",	"109 046 047",  "cast_punk",	0,	    100,		0,		0,	//bar_sy Got normally 350 health
	"Laurel",		"504 032 031",  "cast_punk",	0,	    100,		0,		1,	//pv_h Got normally 200 health
	"Hardey",		"114 032 031",  "cast_punk",	0,	    100,		0,		0,	//pv_h Got normally 200 health
//	"ToughGuy1",	"019 028 010",  "cast_punk",	0,	    100,		0,		0,	//sr1 intro guy
//	"ToughGuy2",	"020 029 010",  "cast_punk",	0,	    100,		0,		0,	//sr1 intro guy
//	"arnold",		"012 007 004",  "cast_punk",	0,	    100,		0,		0,	//sr1 Got normally 200 health in courtyard group
	"Momo",			"107 044 010",  "cast_punk",	0,	    100,		0,		0,	//sy_h
	"Larry",		"048 047 041",  "cast_punk",	0,	    100,		0,		0,	//sy_h Got normally 200 health
	"Curly",		"100 047 041",  "cast_punk",	0,	    100,		0,		0,	//sy_h Got normally 200 health
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_cast_tommygun(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		    count	head
    "Sal",		    "064 047 047",	"cast_shorty",	64,		100,	     0,		0,	//bar_sy Got normally 250 health
    "Mung",		    "013 010 006",	"cast_shorty",	64,		100,		 9,		0,	//pv_h
    "Bubba",	    "130 131 010",	"cast_shorty",	64,		100,		 9,		0,	//rc2
    "Heilman",	    "121 121 121",	"cast_shorty",	64,		100,		 0,		0,	//sy2 Got normally 250 health
    "Lefty",	    "064 046 047",  "cast_punk",	64,		100,		 0,		0,	//bar_sy Got normally 250 health
//    "Dubs",          "108 009 006",  "cast_punk",    64,     100,    0,      0,	//pv_h tommygun rented guy
    "Patrick",	    "511 130 010",  "cast_punk",	64,		100,		 0,		1,  //rc1
    "Donny",	    "512 131 010",  "cast_punk",	64,		100,		 0,		1,  //rc4 Got normally 200 health
  //  "Oscar",	    "044 012 006",  "cast_punk",	64,		100,		 0,		0,  //steel2 Got normally 250 health
 //   "David",	    "132 132 132",  "cast_punk",	64,		100,    	 0,		0,  //steel3 Got normally 300 health
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
}*/

//localteam
void cast_TF_Skidrow_melee(edict_t *self)//New skin todo testing
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Johnny",	"011 007 004",	"cast_thug",	64, 	80,	    0,       0, //melee
    "Bernie",   "011 012 004",  "cast_runt",	64,	    100,	1,	     0, //melee
    "Arnold",	"012 007 004",	"cast_punk",	64,	    200,	0,	     0, //melee
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}

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
//	"Sluggo",	"019 010 011",	"cast_runt",	0,	    100,	0,  	0,    //bar_sr pistol rented guy
	"Lenny",	"018 011 007",	"cast_runt",	0,	    100,	0,  	0,	  //bar_sr pistol
//	"Rocko",	"016 009 006",	"cast_thug",	0,	    100,	0,  	0,	  //bar_sr pistol rented guy
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
//	"Sluggo",	"019 010 011",	"cast_runt",	0,	    100,	0,  	0,    //bar_sr pistol rented guy
//    "Rocko",	"016 009 006",	"cast_thug",	0,	    100,	0,  	0,	  //bar_sr pistol rented guy
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
    self->cast_info.scale = 1.25;//Or will give problems? hypov8 note: this fails to increase hitbox
	self->health = 300 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Skidrow_boss_jesus(edict_t *self)//sr4
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Jesus");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "021 017 010");
	self->spawnflags = 0;//pistol
	self->classname = "cast_runt";
	self->moral = 5;
	self->acc = 3;
	self->cal = 20;
	self->cast_info.scale = 1.25;//Or will give problems?
	self->health = 400 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Skidrow_boss(edict_t *spawn)
{
	switch (rand() % 4)
	{
	case 0:		cast_TF_Skidrow_boss_lamont(spawn);break;
	case 1:		cast_TF_Skidrow_boss_jesus(spawn);break;
    case 2:		cast_TF_Skidrow_boss_lamont(spawn);break;
	case 3:		cast_TF_Skidrow_boss_jesus(spawn);break;
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
void cast_TF_Poisonville_skins(edict_t *self)//New skins of tank boob to fix skin
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Runt",	        "030 031 031",	"cast_shorty",	0,	    100,	0,	    0,
    "Runt",	        "030 031 031",	"cast_shorty",  64,     140,    0,      0,
    "Thug",	        "504 031 031",	"cast_punk",	0,	    100,	0,	    1,
	"Thug",	        "504 031 031",	"cast_punk",    64,     160,    0,      1,
	"Thug",     	"504 031 031",	"cast_punk",    64,     140,    0,      1,
	"Bitch",	    "030 032 033",	"cast_whore",   64,     120,    0,      0,
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
//    "Dubs",     "108 009 006",  "cast_punk",    64,     220,    0,      0,	//pv_h tommygun rented guy
//    "Fingers",	"025 009 009",	"cast_thug",    0,      150,    0,      0,	//bar_pv pistol rented guy
	"Burt",	    "063 041 010",	"cast_thug",    0,      100,    0,      0,	//bar_pv pistol
    "Clarence", "047 013 003",  "cast_runt",    0,      100,    9,      0,	//bar_pv pistol
    "Candy",	"015 017 013",  "cast_bitch",   0,      100,    0,      0,	//bar_pv pistol
    "Louie",    "043 041 009",  "cast_runt",    0,      100,    0,      0,	//bar_pv pistol
 //   "Buster",	"046 010 009",  "cast_runt",	0,	    120,	9,		0,	//bar_pv rented guy
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
    self->cast_info.scale = 1.25;//Or will give problems?
	self->health = 450 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Shipyard_skins(edict_t *self)//New skins of deckmonkeys, goons and swabs to fix skin
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	head
	"Runt",     	"063 060 060",	"cast_shorty",   0,     250,    0,      0,  //shotgun
    "Runt",	        "063 060 060",	"cast_shorty",   0,     300,    0,      0, //shotgun
    "Runt",     	"063 060 060",	"cast_shorty",   0,      75,    0,      0, //shotgun
    "Runt",	        "063 064 060",	"cast_shorty",  64,     225,    0,      0, //tommygun
    "Runt",	        "063 064 060",	"cast_shorty",  64,     300,    0,      0, //tommygun
    "Thug",	        "515 060 060",	"cast_punk",     0,     300,    0,      1, //shotgun
    "Thug",	        "515 060 060",	"cast_punk",     0,     250,    0,      1, //shotgun
	"Thug",	        "515 060 060",	"cast_punk",     0,     250,    0,      1, //shotgun
	"Thug",	        "515 060 060",	"cast_punk",     0,      75,    0,      1, //shotgun
    "Thug",	        "045 060 060",	"cast_punk",     0,      75,    0,      1, //shotgun
    "Thug",	        "045 064 060",	"cast_punk",    64,     250,    0,      0, //tommygun
	"Thug",	        "045 064 060",	"cast_punk",    64,     250,    0,      0, //tommygun
    "Thug",	        "045 064 060",	"cast_punk",    64,     225,    0,      0, //tommygun
    "Thug",	        "045 064 060",	"cast_punk",    64,     300,    0,      0, //tommygun
    "Thug",	        "045 064 060",	"cast_punk",    64,      75,    0,      0, //tommygun
	"Bitch",     	"060 060 060",	"cast_whore",    0,     250,    0,      0, //shotgun
    "Bitch",	    "060 060 060",	"cast_whore",    0,      75,    0,      0, //shotgun
    "Bitch",	    "060 060 060",	"cast_whore",   64,     100,    0,      0, //tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
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
    "Sal",      "064 047 047",  "cast_shorty",  64,     250,    0,      0,    //bar_sy tommygun
    "Moe",      "047 065 041",  "cast_runt",    0,       50,    0,      0,    //sy_h pistol
    "Popeye",   "040 019 048",  "cast_runt",    64,     300,    0,      1,    //sy_h melee
    "Momo",		"107 044 010",  "cast_punk",    0,      100,    0,      0,    //sy_h shotgun
    "BigWillie","109 046 047",  "cast_punk",    0,      350,    0,      0,    //bar_sy shotgun same skin as lefty almost
    "Lefty",    "064 046 047",  "cast_punk",    64,     250,    0,      0,    //bar_sy tommygun
	"Larry",	"048 047 041",  "cast_punk",    0,      200,    0,      0,    //sy_h shotgun same skin as curly almost
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
    self->cast_info.scale = 1.25;//Or will give problems?
	self->health = 650 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Steeltown_skins(edict_t *self)//New skins to mix of teamno and grp fix skin
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Thug",	        "500 021 020",	"cast_punk",    0,      150,    0,      1,    //grp2 shotgun
    "Thug",	        "500 021 022",	"cast_punk",    0,      100,    0,      1,    //steel1 teamno1 shotgun
    "Thug",	        "500 021 022",	"cast_punk",    0,      120,    0,      1,    //steel2 teamno7 shotgun
    "Thug",	        "500 021 022",	"cast_punk",    64,     200,    0,      1,    //steel4 teamno1 tommygun
    "Thug",	        "500 021 022",	"cast_punk",    64,     100,    0,      1,    //steel1 teamno2 tommygun
    "Thug",	        "500 021 022",	"cast_punk",    64,     120,    0,      1,    //steel2 teamno7 tommygun
    "Thug",	        "136 021 022",	"cast_punk",    16,     100,    0,      1,    //steel3 teamno7 hmg
    "Thug",	        "700 021 022",	"cast_punk",    4,      100,    0,      3,    //steel3 teamno3 flamethrower
    "Thug",	        "700 021 022",	"cast_punk",    4,      120,    0,      3,    //steel1 teamno3 flamethrower
    "Thug",	        "700 021 022",	"cast_punk",    4,      150,    0,      3,    //grp3 flamethrower
    "Runt",	        "023 021 020",	"cast_runt",    0,      100,    0,      0,    //grp2 shotgun
    "Runt",	        "024 021 020",	"cast_runt",    0,      150,    0,      0,    //grp1 shotgun
    "Runt",     	"022 020 022",	"cast_runt",    0,      100,    0,      0,    //steel1 teamno3 pistol
    "Runt",	        "022 021 022",	"cast_shorty",  64,     150,    0,      0,    //steel3 teamno1 tommygun
    "Runt",	        "022 021 022",	"cast_shorty",  0,      120,    0,      0,    //steel3 teamno1 shotgun
    "Runt",	        "022 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno7 shotgun
    "Runt",	        "022 021 022",	"cast_shorty",  0,      200,    0,      0,    //steel4 teamno2 shotgun
    "Runt",	        "022 021 022",	"cast_shorty",  16,     100,    0,      0,    //steel3 teamno7 hmg
    "Runt",	        "022 021 022",	"cast_shorty",  64,     200,    0,      0,    //steel4 teamno2 tommygun
    "Bitch",        "020 021 021",	"cast_whore",   0,      100,    0,      0,    //grp1 shotgun
    "Bitch",     	"020 020 021",	"cast_whore",   64,     100,    0,      0,    //grp3 tommygun

	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
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
    "",	        "700 025 022",	"cast_punk",    4,      120,    0,      3,    //steel1 teamno3 flamethrower
    "",	        "022 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno3 shotgun
    "",	        "024 020 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno3 shotgun
    "",	        "700 025 022",	"cast_punk",    4,      100,    0,      3,    //steel3 teamno3 flamethrower
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
    "",	        "700 024 020",	"cast_punk",    4,      150,    0,      3,    //grp3 flamethrower
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
    "Kroker",	"023 020 020",  "cast_runt",	64,	    150,	1,		0,	//steel1 melee
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
	self->cast_info.scale = 1.25;//Or will give problems?
    self->health = 800 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Trainyard_skins(edict_t *self)//New skins mix of localteam trainyard to fix skin
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Thug",	        "051 051 052",	"cast_punk",    0,      100,    0,      0, //shotgun
    "Thug",	        "051 051 052",	"cast_punk",    64,     100,    0,      0,  //tommygun
	"Thug",	        "051 051 052",	"cast_punk",    64,     150,    0,      0,  //tommygun
    "Thug",	        "051 051 052",	"cast_punk",    128,    100,    0,      0,  //grenade
    "Thug",	        "507 081 054",	"cast_punk",    16,     100,    0,      1,  //hmg
    "Runt",	        "050 051 054",	"cast_shorty",  64,     100,    0,      0, //tommygun
    "Runt",	        "050 051 054",	"cast_shorty",  16,     100,    0,      0,  //hmg
    "Bitch",        "071 070 021",	"cast_whore",   64,     100,    0,      0, //tommygun
	"Bitch",        "072 073 021",	"cast_whore",   0,      100,    0,      0, //shotgun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
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
	self->cast_info.scale = 1.25;//Or will give problems?
    self->health = 500 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Radio_City_skins(edict_t *self)//New skins mix of localteam Radiocity to fix skin
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Runt",	    "058 055 058",	"cast_runt",     0,     100,    0,      0,  //shotgun
    "Runt",	    "057 055 058",	"cast_shorty",   0,     150,    0,      0,  //shotgun
    "Runt",	    "057 055 058",	"cast_shorty",  16,     100,    0,      0,  //hmg
    "Runt",	    "057 055 058",	"cast_shorty",   8,     125,    0,      0,  //bazooka
    "Thug",     "056 058 059",	"cast_punk",     0,     125,    0,      0,  //shotgun
    "Thug",     "056 058 059",	"cast_punk",    64,     125,    0,      0,  //tommygun
    "Thug",     "056 058 059",	"cast_punk",    16,     100,    0,      0,  //hmg
    "Thug",     "056 058 059",	"cast_punk",    16,     150,    0,      0,  //hmg
	"Thug",     "056 058 059",	"cast_punk",    16,      80,    0,      0,  //hmg
    "Thug",     "056 058 059",	"cast_punk",     8,     125,    0,      0,  //bazooka
    "Thug",     "056 058 059",	"cast_punk",     8,     100,    0,      0,  //bazooka
    "Bitch",	"055 057 056",	"cast_whore",    0,     175,    0,      0,  //shotgun
    "Bitch",	"055 057 056",	"cast_whore",   64,     100,    0,      0,  //tommygun
    "Bitch",	"055 057 056",	"cast_whore",   64,     125,    0,      0,  //tommygun
    "Bitch",	"055 057 056",	"cast_whore",   64,     150,    0,      0,  //tommygun
    "Bitch",	"058 056 056",	"cast_whore",    8,     150,    0,      0,  //bazooka
    "Bitch",	"058 056 056",	"cast_whore",    8,     250,    0,      0,  //bazooka
    "Bitch",    "057 057 056",	"cast_bitch",    0,     150,    0,      0,  //pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
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
	self->cast_info.scale = 1.25;//Or will give problems?
	self->health = 800 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}

void cast_TF_Crystal_Palace_boss_kingpin(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Kingpin");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "120 120 120");
	self->spawnflags = 0;//pistol
    self->acc = 5;
    self->cal = 5;
//	self->scale = 1.15;
    self->cast_info.scale = 1.25;//Or bigger?
	self->classname = "cast_runt";
	self->health = 1500 * currWave_plysCount;
}
void cast_TF_Crystal_Palace_boss_blunt(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Blunt");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "080 059 059");
	self->spawnflags = 16;//hmg //18 =triger spawned (>>2)
	self->classname = "cast_whore";
	self->cast_info.scale = 1.25;//Or bigger?
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

	//store boss ID.
	boss_entityID = spawn;
}
// end setup cast skins
///////////////////////

extern voice_table_t sr_jesus[];
extern voice_table_t lamont_random[];
extern voice_table_t nickiblanco[];
extern voice_table_t steeltown_moker[];
extern voice_table_t heilman[];
extern voice_table_t ty_tyrone[];
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
    case NAME_JESUS: tmpVoice = sr_jesus;	break;
    case NAME_LAMONT: tmpVoice = lamont_random;	break;//Maybe not really a boss?
	case NAME_NICKIBLANCO: tmpVoice = nickiblanco;	break;
    case NAME_HEILMAN: tmpVoice = heilman;	break;
    case NAME_MOKER: tmpVoice = steeltown_moker;	break;
	case NAME_TYRONE: tmpVoice = ty_tyrone;	break;
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

	if (level.waveEnemyCount_cur)
		level.waveEnemyCount_cur = 0; //reset spawned cast count
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
					if (boss_entityID && level.characters[i] == boss_entityID)
					{
						if (level.waveNum == currWave_length) //final wave
						{
							level.waveEnemyCount_cur = 1;
							level.num_characters = 1;
							level.waveEnemyCount_total = 1;
						}
						else
							boss_entityID = NULL; //any wave
					}

					//free any dead cast
					level.characters[i]->character_index = 0;
					level.characters[i] = NULL;
					level.num_characters--;
					level.waveEnemyCount_cur--;
					level.waveEnemyCount_total--;
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
			level.waveEnemyCount_total = level.waveEnemyCount_cur + 5 + (int)skill->value; //how many boss helpers to spawn??
			cast_boss_sounds();
		}
		else if (boss_called_help == 1 && boss_entityID && boss_entityID->health < (boss_maxHP *.5))
		{
			boss_called_help = 2;
			level.waveEnemyCount_total = level.waveEnemyCount_cur + 5 + (int)skill->value; //how many boss helpers to spawn??
			cast_boss_sounds();
		}
		else if (boss_called_help == 2 && boss_entityID && boss_entityID->health < (boss_maxHP *.25))
		{
			boss_called_help = 3;
			level.waveEnemyCount_total = level.waveEnemyCount_cur + 5 + (int)skill->value; //how many boss helpers to spawn??
			cast_boss_sounds();
		}
	}

	//free slot. add enemy
	if (count < level.waveEnemyCount_total && count < currWave_castMax)
		cast_TF_spawn();


	//return level.waveEnemyCount_cur;
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
        case 1:	cast_TF_Skidrow_names_melee(spawn);break;//Todo testing: cast_TF_Skidrow_melee
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
        case 1:	cast_TF_Skidrow_names(spawn);break;
        case 2:	cast_TF_Skidrow_names(spawn);break;
        case 3:	cast_TF_Skidrow_names(spawn);break;
        case 4:	cast_TF_Skidrow_names(spawn);break;
        case 5:	cast_TF_Skidrow_names(spawn);break;
        }
    }
}
void cast_TF_spawnWave2(edict_t *spawn)//Skidrow
{
	if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Skidrow_boss(spawn);//spawn boss when almost done
	else
	{
		switch (rand() % 7)
		{
		case 0:	cast_TF_dog(spawn);break;
		case 1:	cast_TF_dog(spawn);break;
		case 2:	cast_TF_rat(spawn);break;
        case 3:	cast_TF_Skidrow_courtyard(spawn);break;//Police
        case 4:	cast_TF_Skidrow_courtyard(spawn);break;//Police
        case 5:	cast_TF_Skidrow_courtyard(spawn);break;//Police
        case 6:	cast_TF_Skidrow_courtyard(spawn);break;//Police
//        case 7: cast_TF_Skidrow_names(spawn);break;
//		case 8: cast_TF_Skidrow_names(spawn);break;
/*		case 5:	cast_TF_Skidrow_courtyard2(spawn);break;
		case 6:	cast_TF_Skidrow_street(spawn);break;
		case 7:	cast_TF_Skidrow_hallway(spawn);break;*/
		}
	}
}
//Skidrow now
//4 bitch
//3 thug
//7 runt
//1 punk
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
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Poisonville_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 6)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Poisonville_skins(spawn);break;
        case 3:	cast_TF_Poisonville_skins(spawn);break;
        case 4:	cast_TF_Poisonville_skins(spawn);break;
        case 5:	cast_TF_Poisonville_skins(spawn);break;
 /*       case 2:	cast_TF_Poisonville_tanks(spawn);break;//Nikki skins
        case 3:	cast_TF_Poisonville_tanks(spawn);break;//Nikki skins
        case 4:	cast_TF_Poisonville_boob(spawn);break;//Nikki skins
        case 5:	cast_TF_Poisonville_boob(spawn);break;//Nikki skins*/
//        case 6:	cast_TF_Poisonville_names(spawn);break;
        }
	}
}
//Poisonville
//3 runt
//4 punk
//2 shorty
//1 bitch
//1 whore
//1 thug
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
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Shipyard_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 6)
        {
            case 0:	cast_TF_dog(spawn);break;
            case 1:	cast_TF_rat(spawn);break;
            case 2:	cast_TF_Shipyard_skins(spawn);break;//heilman skins
            case 3:	cast_TF_Shipyard_skins(spawn);break;//heilman skins
            case 4:	cast_TF_Shipyard_skins(spawn);break;//heilman skins
            case 5:	cast_TF_Shipyard_skins(spawn);break;//heilman skins
 /*           case 4:	cast_TF_Shipyard_deckmonkeys(spawn);break;//heilman skins
            case 5:	cast_TF_Shipyard_deckmonkeys(spawn);break;//heilman skins
            case 4:	cast_TF_Shipyard_goons(spawn);break; //heilman skins
            case 5:	cast_TF_Shipyard_goons(spawn);break; //heilman skins
            case 6:	cast_TF_Shipyard_swabs(spawn);break; //heilman skins
            case 7:	cast_TF_Shipyard_swabs(spawn);break; //heilman skins*/
        }
	}
}
//Shipyard
//5 shorty
//8 punk
//1 whore
//1 bitch
void cast_TF_spawnWave7(edict_t *spawn)//Steeltown
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Steeltown_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 6)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Steeltown_skins(spawn);break;
        case 3:	cast_TF_Steeltown_skins(spawn);break;
        case 4:	cast_TF_Steeltown_skins(spawn);break;
        case 5:	cast_TF_Steeltown_skins(spawn);break;
/*        case 2:	cast_TF_Steeltown_teamno(spawn);break; //Moker team skin
        case 3:	cast_TF_Steeltown_teamno(spawn);break; //Moker team skin
        case 4:	cast_TF_Steeltown_grp(spawn);break; //Moker team skin
        case 5:	cast_TF_Steeltown_grp(spawn);break;//Moker team skin
        case 6:	cast_TF_Steeltown_Kid_names(spawn);break;
        case 7:	cast_TF_Steeltown_names(spawn);break;*/
        }
	}
}
//Steeltown
//4 punk
//3 runt
//1 shorty
//2 whore
void cast_TF_spawnWave8(edict_t *spawn)//Trainyard
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Trainyard_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 6)
        { //hypov8 note: posibly a bad actor here or above.. any sfx not used on other models?
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Trainyard_skins(spawn);break;
        case 3:	cast_TF_Trainyard_skins(spawn);break;
        case 4:	cast_TF_Trainyard_skins(spawn);break;
        case 5:	cast_TF_Trainyard_skins(spawn);break;
/*        case 2:	cast_TF_Trainyard_trainwreck(spawn);break;
        case 3:	cast_TF_Trainyard_frontline(spawn);break;
        case 4:	cast_TF_Trainyard_trainteam(spawn);break;//Only team with gl
        case 5:	cast_TF_Trainyard_trainteam(spawn);break;//Only team with gl
        case 6:	cast_TF_Trainyard_poop(spawn);break;*/
//        case 7:	cast_TF_Trainyard_names(spawn);break;//Only 2 names and not really fitting skins
        }
	}
}
//Trainyard
//3 punk
//1 shorty
//2 whore
void cast_TF_spawnWave9(edict_t *spawn)//Radiocity
{
	switch (rand() % 7)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_rat(spawn);break;
    case 2:	cast_TF_Radio_City_skins(spawn);break;
    case 3:	cast_TF_Radio_City_skins(spawn);break;
//	case 2:	cast_TF_Radio_City_dragon(spawn);break;
//    case 3:	cast_TF_Radio_City_dragonalley(spawn);break;
	case 4:	cast_TF_Radio_City_names(spawn);break;
	case 5:	cast_TF_Radio_City_names(spawn);break;
    case 6:	cast_TF_Radio_City_names(spawn);break;
	}
}
void cast_TF_spawnWave10(edict_t *spawn)//Radiocity
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Radio_City_boss(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 7)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Radio_City_skins(spawn);break;
        case 3:	cast_TF_Radio_City_skins(spawn);break;
        case 4:	cast_TF_Radio_City_skins(spawn);break;
        case 5:	cast_TF_Radio_City_skins(spawn);break;
/*        case 2:	cast_TF_Radio_City_dragon_street(spawn);break;
        case 3:	cast_TF_Radio_City_dragon_hoppers(spawn);break;
        case 4:	cast_TF_Radio_City_dragon_rooftop(spawn);break;
        case 5:	cast_TF_Radio_City_dragon_building(spawn);break;
        case 6:	cast_TF_Radio_City_dragon_street(spawn);break;*/
    //    case 7:	cast_TF_Radio_City_dragon_office(spawn);break;
        }
	}
}
//RadioCity
//7 runt
//5 shorty
//3 punk
//2 whore
//3 bitch
void cast_TF_spawnWaveBoss(edict_t *spawn)
{
	//boss.. or his helpers?
	if (boss_called_help == 0)
		cast_TF_Crystal_Palace_boss(spawn);
	else if (boss_called_help == 1)
    {//		cast_TF_cast_melee(spawn);
        if (skill->value < 2)
            cast_TF_Steeltown_skins(spawn);
        else
            cast_TF_Skidrow_courtyard(spawn);
    }
	else if (boss_called_help == 2)
    {//		cast_TF_cast_pistol(spawn);
        if (skill->value < 2)
            cast_TF_Trainyard_skins(spawn);
        else
            cast_TF_Poisonville_skins(spawn);
    }
	else if (boss_called_help == 3)
    {//		cast_TF_cast_shotgun(spawn);
        if (skill->value < 2)
            cast_TF_Radio_City_skins(spawn);
        else
            cast_TF_Shipyard_skins(spawn);
    }
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
		case 0: cast_TF_spawnWave1(spawn);	break;
		case 1:	cast_TF_spawnWave2(spawn);	break;
		case 2: cast_TF_spawnWave3(spawn);	break;
		case 3: cast_TF_spawnWave4(spawn);	break;
		case 4: cast_TF_spawnWave5(spawn);	break;
		case 5: cast_TF_spawnWave6(spawn);	break;
		case 6: cast_TF_spawnWave7(spawn);	break;
		case 7: cast_TF_spawnWave8(spawn);	break;
		case 8: cast_TF_spawnWave9(spawn);	break;
		case 9: cast_TF_spawnWave10(spawn);	break;
		case 10: cast_TF_spawnWaveBoss(spawn); break; //wave 11
		}
	}
	else if (currWave_length == WAVELEN_MED - 1)//med wave
	{
		switch (level.waveNum)
		{
		case 0: cast_TF_spawnWave1(spawn);	break;
		case 1:	cast_TF_spawnWave2(spawn);	break;
		case 2: cast_TF_spawnWave4(spawn);	break;
		case 3: cast_TF_spawnWave5(spawn);	break;
		case 4: cast_TF_spawnWave6(spawn);	break;
		case 5: cast_TF_spawnWave8(spawn);	break;
		case 6: cast_TF_spawnWave10(spawn);	break;
		case 7: cast_TF_spawnWaveBoss(spawn); break; //wave 8
		}
	}
	else 	//WAVELEN_SHORT //short wave
	{
		switch (level.waveNum)
		{
		case 0: cast_TF_spawnWave2(spawn);	break; //Skidrow
		case 1: cast_TF_spawnWave4(spawn);	break; //Should be 5?
		case 2: cast_TF_spawnWave8(spawn);	break; //Should be 7?
		case 3: cast_TF_spawnWave10(spawn);	break; //Radiocity
		case 4: cast_TF_spawnWaveBoss(spawn); break; //wave 5
		}
	}
}

//spawn in a cast model
void cast_TF_spawn(void)
{
	edict_t *spawn, *spawnspot;

	{
		if (level.waveEnemyCount_cur >= MAX_CHARACTERS)
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

		spawn->max_health = spawn->health;

		//add healt after skil
		if (boss_entityID && spawn == boss_entityID)
		{
			spawn->maxs[2] = 62; //increase head height on boss
			boss_maxHP = spawn->health;
		}

		// stop cast health increasing slowly
		spawn->healspeed = -1;

		//set what player to attack
		cast_TF_setEnemyPlayer(spawn);

		//add enemy to counter
		level.waveEnemyCount_cur++;
	}
}

//total enemy counts per wave.
//this will be multiplied by player counts later
#if 0 //HYPODEBUG
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
			//AddCharacterToGame(self); //add player to level.characters
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
		level.waveEnemyCount_total =(level.waveNum == currWave_length)? 1 :  wave_shortGame[level.waveNum] * playerCount; //force 1 boss
	}
	else if ((int)wavetype->value == 1)
	{
		currWave_length = WAVELEN_MED - 1;//med wave
		level.waveEnemyCount_total =(level.waveNum == currWave_length)? 1 :  wave_medGame[level.waveNum] * playerCount; //force 1 boss
	}
	else //wavetype >= 2
	{
		currWave_length = WAVELEN_LONG - 1;//long wave
		level.waveEnemyCount_total = (level.waveNum == currWave_length)? 1 : wave_longGame[level.waveNum] * playerCount; //force 1 boss
	}

}

