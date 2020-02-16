#include "g_local.h"
#include "g_cast_spawn.h"

#define ARYSIZE(x) (sizeof(x) / sizeof(x[0])) //get dynamic size of array

#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT];// = {NULL, NULL, NULL}; //hypov8

void cast_TF_spawn(int ammount, int type);
static int currWave_castCount = 0; //enemy currently on map
static int currWave_plysCount = 0; //players
static int currWave_length = 0; //long, med, short
static int currWave_castMax = 0; //max enemy allowed on map

#define TEST_NEWSKIN

void cast_pawn_o_matic_free()
{
	int i;
	for (i = 0; i < BUYGUY_COUNT; i++)
	{
		if (pawnGuy[i] && pawnGuy[i]->inuse && !Q_stricmp(pawnGuy[i]->classname, "cast_pawn_o_matic")) //just incase
		{
			AI_UnloadCastMemory(pawnGuy[i]);
			G_FreeEdict(pawnGuy[i]);
		}
	}
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
		//no more required
		if (count == BUYGUY_COUNT)
			return;
	}
}


//apply skins.. consider skill
void cast_TF_applyRandSkin(edict_t *self, localteam_skins_s *skins, int skinCount)
{
#ifdef TEST_NEWSKIN //only 2 waves can be stested. crashes on 3rd

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

#ifdef TEST_NEWSKIN //example skin method
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
}
void cast_TF_bitch_melee(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Betty",	"008 006 003",  "cast_bitch",	64,	    0,		0,		0,	//sr1
	"Beth",		"009 007 004",  "cast_bitch",	64,	    0,		0,		0,	//sr1
	"Lisa",		"012 015 012",  "cast_bitch",	64,	    0,		0,		0,	//sr1 Got normally 120 health
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_bitch_pistol(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Mona",		"014 012 003",  "cast_bitch",	0,	    0,		0,		0,	//sr2
	"yolanda",	"041 050 003",  "cast_bitch",	0,	    0,		0,		0,	//pv_h got normally 200 health
	"candy",	"015 017 013",  "cast_bitch",	0,	    0,		0,		0,	//bar_pv same name as in bar_rc
	"brittany",	"046 040 010",  "cast_bitch",	0,	    0,		0,		0,	//steel1
	"bambi",	"044 042 003",  "cast_bitch",	0,	    0,		0,		0,	//bar_st
	"lola",		"045 049 015",  "cast_bitch",	0,	    0,		0,		0, 	//rc3
	"candy",	"019 019 015",  "cast_bitch",	0,	    0,		0,		0,	//bar_rc same name as in bar_pv
	"selma",	"600 043 003",  "cast_bitch",	0,	    0,		0,		0, 	//bar_sy
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_bitch_shotgun(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Rochelle");//bar_st
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "601 009 012");
	self->spawnflags = 8192;//0; is also shotgun
	self->classname = "cast_whore";
}
void cast_TF_runt_melee(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,			//skin,			classname		flags	HP		count	head
	"bubba",		"017 016 008",  "cast_runt",	64,	    100,	9,		0,	    //sr1
//	"louie",		"011 011 005",  "cast_runt",	64,	    100,	3,		0,	    //sr1
//	"buttmunch",	"001 001 001",  "cast_runt",	64,	    100,	0,		0,	//sr1
	"magicj",		"020 011 005",  "cast_runt",	64,	    100,	0,		0,   	//sr1
	"kroker",		"023 020 020",  "cast_runt",	64,	    100,	1,		0,	//steel1 Got normally 150 health
	"kid_1",		"134 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2
	"kid_2",		"132 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2
	"kid_3",		"133 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2
	"popeye",		"040 019 048",  "cast_runt",	64,	    100,	0,		0,	//sy_h Got normaly health 300 and diffrent head?
	"harpo",		"142 140 140",  "cast_runt",	64,	    100,	0,		0,	//rc1 Got normally 200 health
	"bubba",		"042 042 010",  "cast_runt",	64,	    100,	9,		0,	//rc1
	"groucho",		"140 141 046",  "cast_runt",	64,	    100,	0,		0,	//rc1 Got normally 150 health
	"chico",		"141 141 046",  "cast_runt",	64,	    100,	0,		0,	 //rc1 Got normally 150 health //same skin as grouncho
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_runt_pistol(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
//	"bernie",	"011 012 004",  "cast_runt",	0,	    0,		1,		0,	//sr1 Got normally 150 health
//	"punky",	"005 003 001",  "cast_runt",	0,	    0,		0,		0,	//sr1 Got normally 50 health//sewer_rats
	"momo",		"020 011 003",  "cast_runt",	0,	    0,		0,		0,		//sr2 //bouncer
	"momo",		"013 045 006",  "cast_runt",	0,	    0,		0,		0,		//steel1 //bouncer
	"momo",		"049 041 009",  "cast_runt",	0,	    0,		0,		0,	//rc4 //bouncer
	"moe",		"047 065 041",  "cast_runt",	0,	    0,		0,		0,		//sy_h
//	"jesus",	"021 017 010",  "cast_runt",	0,	    0,		0,		0,	//sr4 Got normally 400 health some kind of boss
	"mathew",	"048 042 015",  "cast_runt",	0,	    0,		0,		0,	//steel1
	"luke",		"049 046 046",  "cast_runt",	0,	    0,		0,		0,		//ty1
	"clarence", "047 013 003",  "cast_runt",	0,	    0,		9,		0,	//bar_pv
	"buster",	"046 010 009",  "cast_runt",	0,	    0,		9,		0,	//bar_pv Got normally 120 health
	"louie",	"043 041 009",  "cast_runt",	0,	    0,		0,		0,	//bar_pv
	"clarence", "045 024 005",  "cast_runt",	0,	    0,		1,		0,	//bar_rc
	"sluggo",	"019 010 011",  "cast_runt",	0,	    0,		0,		0,	//bar_sr
	"lenny",	"018 011 007",  "cast_runt",	0,	    0,		0,		0,	//bar_sr
	"dogg",		"016 014 003",  "cast_runt",	0,	    0,		0,		0, 	//bar_sy
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_runt_shotgun(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"momo",		"072 014 009",	"cast_shorty",	8192,	0,		 0,		0, 	//sr1 Got normally 150 health
	"butch",	"131 130 010",	"cast_shorty",	8192,	0,		 0,		0,	//rc1
	"dude",		"130 131 010",	"cast_shorty",	8192,	0,		 9,		0,     //rc3 Got normally 200 health
	"momo",		"072 014 009",	"cast_shorty",	8192,	0,		 0,		0, //pv_h
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_runt_tommygun(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "sal",		"064 047 047",	"cast_shorty",	64,		0,		 0,		0,	//bar_sy Got normally 250 health
    "mung",		"013 010 006",	"cast_shorty",	64,		0,		 9,		0,	//pv_h
    "bubba",	"130 131 010",	"cast_shorty",	64,		0,		 9,		0,	//rc2
    "heilman",	"121 121 121",	"cast_shorty",	64,		0,		 0,		0,	//sy2 Got normally 250 health
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_runt_hmg(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL),  "walter");//bar_rc Got normally 350 health
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "130 130 010");
	self->spawnflags = 16;
	self->classname = "cast_shorty";
}
void cast_TF_thug_melee(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"leroy",	"010 010 003", "cast_thug",		64,		0,		0,		 0,    //sr1
//	"johnny",	"011 007 004", "cast_thug",		64,		0,		0,		 0,	//sr1 Got normally 80 health
	"brewster", "002 001 001", "cast_thug",		64,		0,		0,		 0,	//sr1
//	"IntroGuy", "041 026 010", "cast_thug",		64,		0,		0,		 0,	//sr1 player
    "kid_4",	"133 132 132", "cast_thug",		64,		0,		0,		 0,	//steel3
    "kid_5",	"134 132 132", "cast_thug",		64,		0,		0,		 0,	//steel3
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_thug_pistol(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"fingers",	"025 009 009", "cast_thug",		0,		0,		0,		 0,		 //bar_pv Got normally 150 health
	"burt",	    "063 041 010", "cast_thug",		0,		0,		0,		 0,		 //bar_pv
	"adolf",	"047 042 009", "cast_thug",		0,		0,		0,		 0,		 //bar_rc
    "burt",	    "023 017 005", "cast_thug",		0,		0,		0,		 0,		 //bar_rc
    "scalper",	"103 044 009", "cast_thug",		0,		0,		0,		 0,		 //bar_rc
    "rocko",	"016 009 006", "cast_thug",		0,		0,		0,		 0,		 //bar_sr
//	"igmo",	    "003 002 001", "cast_punk",		0,		0,		0,		 0,		 //sr1 Got normally 200 health
    "lamont",	"024 017 010", "cast_thug",		0,		0,		0,		 0,		 //sr2 Got normally 300 health
    "hann",	    "029 006 010", "cast_thug",		0,		0,		0,		 0,		 //ty1
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_thug_shotgun(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,			//skin,			classname		flags	HP		count	head
	"bigwillie",	"109 046 047", "cast_punk",		8192,	0,		0,		 0,	//bar_sy Got normally 350 health
	"Laurel",		"504 032 031", "cast_punk",		8192,	0,		0,		 1,	//pv_h Got normally 200 health
	"Hardey",		"114 032 031", "cast_punk",		8192,	0,		0,		 0,	//pv_h Got normally 200 health
//	"ToughGuy1",	"019 028 010", "cast_punk",		8192,	0,		0,		 0,	//sr1 intro guy
//	"ToughGuy2",	"020 029 010", "cast_punk",		8192,	0,		0,		 0,	//sr1 intro guy
//	"arnold",		"012 007 004", "cast_punk",		8192,	0,		0,		 0,	//sr1 Got normally 200 health
	"momo",			"107 044 010", "cast_punk",		8192,	0,		0,		 0,	//sy_h
	"larry",		"048 047 041", "cast_punk",		8192,	0,		0,		 0,	//sy_h Got normally 200 health
	"curly",		"100 047 041", "cast_punk",		8192,	0,		0,		 0,	//sy_h Got normally 200 health
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
	//self->spawnflags = 8192;//0; is also shotgun
}
void cast_TF_thug_tommygun(edict_t *self)
{	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "lefty",	"064 046 047", "cast_punk",		64,		0,		0,		0,	  //bar_sy Got normally 250 health
    "dubs",		"108 009 006", "cast_punk",		64,		0,		0,		0,	  //pv_h  Got normally 220 health
    "patrick",	"511 130 010", "cast_punk",		64,		0,		0,		1,    //rc1
    "donny",	"512 131 010", "cast_punk",		64,		0,		0,		1,      //rc4 Got normally 200 health
    "oscar",	"044 012 006", "cast_punk",		64,		0,		0,		0,      //steel2 Got normally 250 health
    "david",	"132 132 132", "cast_punk",		64,		0,		0,		0,      //steel3 Got normally 300 health
 //   "tyrone",	"507 082 054", "cast_punk",		64,		0,		0,		1,     //steel3 Got normally 500 health
	};
int idx = rand() % ARYSIZE(skins);
cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_thug_bazooka(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "moker");//steel4 Got normally 800 health
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "111 027 023"); //steel4 teamno2
	self->spawnflags = 8;
	self->classname = "cast_punk";
}
void cast_TF_thug_flamethrower(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "nikkiblanco");//pv_boss and rcboss1 Got normally 450 and 800 health boss
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "122 122 122");
	self->spawnflags = 16;
	self->classname = "cast_punk";
}

//localteam
void cast_TF_Skidrow_courtyard(edict_t *self)//sr1 Police/Security
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"johnny",	"011 007 004",	"cast_thug",	64, 	80,	    0,       0, //melee
    "bernie",   "011 012 004",  "cast_runt",	0,	    100,	1,	     0, //pistol
    "arnold",	"012 007 004",	"cast_punk",	0,	    200,	0,	     0, //Shotgun
    "louie",    "011 011 005",	"cast_runt",	0,	    100,	3,  	 0, //Not really police, more person they try to secure //pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_sewer_rats(edict_t *self)//sr1 Sewer Rats ugly skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"buttmunch", "001 001 001",	"cast_runt",	64,	    100,	0,	    0, //melee
    "punky",     "005 003 001",	"cast_runt",	0,	    50,	    0,	    0, //pistol
    "igmo",      "003 002 001", "cast_thug",	0,	    200,	0,	    0, //pistol
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
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
}
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
}
void cast_TF_Skidrow_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Betty",	"008 006 003",	"cast_bitch",	64,	    100,	0,  	0,    //sr1 melee
	"Beth",		"009 007 004",	"cast_bitch",	64,	    100,	0,  	0,    //sr1 melee
	"Lisa",		"012 015 012",	"cast_bitch",	64,	    120,	0,  	0,    //sr1 melee
	"Momo",		"072 014 009",	"cast_shorty",	8192,   150,	0,  	0, 	  //sr1 shogun
	"Leroy",	"010 010 003",	"cast_thug",	64,	    100,	0,  	0,    //sr1 melee
	"Brewster", "002 001 001",	"cast_thug",	64,	    100,	0,  	0,	  //sr1 melee
	"Bubba",	"017 016 008",	"cast_runt",	64,	    100,	9,  	0,    //sr1 melee
	"Magicj",	"020 011 005",	"cast_runt",	64,	    100,	0,  	0,    //sr1 melee
	"Momo",		"020 011 003",	"cast_runt",	0,	    100,	0,  	0,	  //sr2 //bouncer pistol
    "Mona",		"014 012 003",	"cast_bitch",	0,	    100,	0,  	0,    //sr2 pistol
	"Sluggo",	"019 010 011",	"cast_runt",	0,	    100,	0,  	0,    //bar_sr pistol
	"Lenny",	"018 011 007",	"cast_runt",	0,	    100,	0,  	0,	  //bar_sr pistol
	"Rocko",	"016 009 006",	"cast_thug",	0,	    100,	0,  	0,	  //bar_sr pistol
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
void cast_TF_Poisonville_boob(edict_t *self)//pv_1 boob, all pistol, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "030 031 032",	"cast_shorty",	0,	    100,	0,	    0,
	"",	        "503 033 032",	"cast_punk",	0,	    100,	0,	    0,
	"",	        "033 032 032",	"cast_shorty",	0,	    100,	0,	    0,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Poisonville_tanks(edict_t *self)//pv_b tanks, all tommygun, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "506 036 032",	"cast_punk",    64,     160,    0,      1,
	"",     	"504 031 031",	"cast_punk",    64,     140,    0,      1,
	"",	        "030 032 033",	"cast_whore",   64,     120,    0,      0,
	"",	        "035 030 031",	"cast_shorty",  64,     140,    0,      0,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
//    self->head = (int)skins[rand() % 4].head;//FREDZ probably gives problems
}
void cast_TF_Poisonville_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Yolanda",	"041 050 003",	"cast_bitch",   0,      200,    0,      0, 	//pv_h pistol
    "Mung",     "013 010 006",	"cast_runt",    64,     100,    9,      0,	//pv_h melee
    "Laurel",	"504 032 031",	"cast_punk",    0,      200,    0,      1,	//pv_h shotgun
	"Hardey",	"114 032 031",	"cast_punk",    0,      200,    0,      0,	//pv_h shotgun
    "Dubs",     "108 009 006",  "cast_punk",    64,     220,    0,      0,	//pv_h tommygun
    "Momo",     "072 014 009",  "cast_shorty",  0,      100,    0,      0,	//pv_h shotgun
    "Fingers",	"025 009 009",	"cast_thug",    0,      150,    0,      0,	//bar_pv pistol
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
	self->spawnflags = 16;//Flamethrower
	self->classname = "cast_punk";
	self->moral = 5;
//	self->scale = 1.06;
	self->health = 450 * currWave_plysCount;
}
void cast_TF_Shipyard_deckmonkeys(edict_t *self)//sy1 deck_monkeys and 2, shotty and tommyguns, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",     	"060 060 060",	"cast_shorty",  0,      250,    0,      0,    //deck_monkeys
    "",	        "515 062 060",	"cast_punk",    64,     250,    0,      1,    //deck_monkeys
	"",	        "515 060 060",	"cast_punk",    64,     250,    0,      1,    //deck_monkeys
    "",	        "061 062 060",	"cast_shorty",  0,      300,    0,      0,    //deck_monkeys2
    "",	        "060 060 060",	"cast_whore",   64,     100,    0,      0,    //deck_monkeys2
	"",	        "508 060 060",	"cast_punk",    0,      300,    0,      1,    //deck_monkeys2
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_goons(edict_t *self)//sy2 goons, shotty and tommyguns, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "515 063 060",	"cast_punk",    0,      250,    0,      1,
	"",     	"060 060 060",	"cast_whore",   0,      250,    0,      0,
	"",	        "515 063 060",	"cast_punk",    0,       20,    0,      1,//Health not a typer error or Xatrix did that :p
	"",	        "515 063 060",	"cast_punk",    0,      250,    0,      1,
    "",	        "063 064 060",	"cast_shorty",  64,     225,    0,      0,
	"",	        "101 065 060",	"cast_punk",    64,     225,    0,      0,
    "",	        "113 065 060",	"cast_punk",    64,     300,    0,      0,
    "",	        "064 066 060",	"cast_shorty",  64,     300,    0,      0,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
//    self->head = (int)skins[rand() % 8].head;//FREDZ probably gives problems
}
void cast_TF_Shipyard_swabs(edict_t *self)//sy2 swabs, mostly shotty and low health, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "045 064 060",	"cast_punk",    64,      75,    0,      0,
	"",     	"060 060 060",	"cast_shorty",   0,      75,    0,      0,
	"",	        "060 060 060",	"cast_whore",    0,      75,    0,      0,
	"",	        "066 064 060",	"cast_punk",     0,      75,    0,      0,
    "",	        "515 063 060",	"cast_punk",     0,      75,    0,      1,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Dogg",     "016 014 003",  "cast_runt",    0,      100,    0,      0,    //bar_sy
    "Selma",	"600 043 003",  "cast_bitch",   0,      100,    0,      2, 	  //bar_sy
    "BigWillie","109 046 047",  "cast_punk",    0,      350,    0,      0,    //bar_sy
    "Sal",      "064 047 047",  "cast_shorty",  64,     250,    0,      0,    //bar_sy
    "Lefty",    "064 046 047",  "cast_punk",    64,     250,    0,      0,    //bar_sy
    "Moe",      "047 065 041",  "cast_runt",    0,       50,    0,      0,    //sy_h
    "Popeye",   "040 019 048",  "cast_runt",    64,     300,    0,      1,    //sy_h
    "Momo",		"107 044 010",  "cast_punk",    0,      100,    0,      0,    //sy_h
	"Larry",	"048 047 041",  "cast_punk",    0,      200,    0,      0,    //sy_h
	"Curly",	"100 047 041",  "cast_punk",    0,      200,    0,      0,    //sy_h
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

void cast_TF_Steeltown_teamno(edict_t *self)//steel1,steel2,steel3,steel4 teamno1, 2, 3, 7 no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"",	        "017 044 010",	"cast_bitch",   0,      100,    0,      0,    //steel1 teamno1
    "",	        "030 024 022",	"cast_punk",    0,      100,    0,      0,    //steel1 teamno1
    "",	        "022 020 022",	"cast_shorty",  64,     150,    0,      0,    //steel3 teamno1
    "",	        "500 021 022",	"cast_punk",    0,      100,    0,      1,    //steel3 teamno1
    "",	        "023 021 022",	"cast_shorty",  0,      120,    0,      0,    //steel3 teamno1
    "",	        "500 021 022",	"cast_punk",    64,     200,    0,      1,    //steel4 teamno1
    "",	        "501 024 022",	"cast_punk",    64,     100,    0,      1,    //steel1 teamno2
	"",	        "135 025 022",	"cast_punk",    64,     100,    0,      0,    //steel1 teamno2
    "",	        "501 021 022",	"cast_punk",    0,      120,    0,      1,    //steel3 teamno2
    "",	        "025 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno2
    "",	        "501 024 022",	"cast_punk",    0,      120,    0,      0,    //steel3 teamno2
    "",	        "023 021 021",	"cast_shorty",  0,      200,    0,      0,    //steel4 teamno2
    "",	        "022 020 020",	"cast_shorty",  64,     200,    0,      0,    //steel4 teamno2
	"",	        "500 021 022",	"cast_punk",    0,      100,    0,      1,    //steel1 teamno3
    "",     	"022 020 022",	"cast_runt",    0,      100,    0,      0,    //steel1 teamno3
    "",	        "700 025 022",	"cast_punk",    4,      120,    0,      0,    //steel1 teamno3 flamethrower
    "",	        "022 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno3
    "",	        "024 020 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno3
    "",	        "700 025 022",	"cast_punk",    4,      100,    0,      0,    //steel3 teamno3 flamethrower
    "",	        "500 021 022",	"cast_punk",    64,     120,    0,      1,    //steel2 teamno7
    "",	        "500 022 020",	"cast_punk",    0,      120,    0,      1,    //steel2 teamno7
    "",	        "024 021 022",	"cast_shorty",  0,      100,    0,      0,    //steel3 teamno7
    "",	        "022 020 022",	"cast_shorty",  16,     100,    0,      0,    //steel3 teamno7 hmg
    "",	        "136 024 022",	"cast_punk",    16,     100,    0,      0,    //steel3 teamno7 hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Steeltown_grp(edict_t *self)//steel2 grp1, 2 and 3, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "024 021 020",	"cast_runt",    0,      150,    0,      0,    //grp1
	"",	        "020 021 021",	"cast_whore",   0,      100,    0,      0,    //grp1
	"",	        "500 021 020",	"cast_punk",    0,      150,    0,      1,    //grp2
    "",	        "023 021 020",	"cast_runt",    0,      100,    0,      0,    //grp2
	"",	        "020 020 021",	"cast_whore",   0,      100,    0,      0,    //grp3
    "",     	"020 020 021",	"cast_whore",   64,     100,    0,      0,    //grp3
    "",	        "700 024 020",	"cast_punk",    4,      150,    0,      0,    //grp3 flamethrower
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Steeltown_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Bambi",	"044 042 003",  "cast_bitch",	0,	    100,	0,		0,	//bar_st
    "Rochelle",	"601 009 012",  "cast_whore",	0,	    300,	0,		2,	//bar_st
    "Brittany",	"046 040 010",  "cast_bitch",	0,	    100,	0,		0,	//steel1
    "Kroker",	"023 020 020",  "cast_runt",	64,	    150,	1,		0,	//steel1
    "Momo",		"013 045 006",  "cast_runt",	0,	    100,	0,		0,	//steel1 //bouncer
    "Mathew",	"048 042 015",  "cast_runt",	0,	    100,	0,		0,	//steel1
	"Kid_1",	"134 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2
	"Kid_2",	"132 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2
	"Kid_3",	"133 132 132",  "cast_runt",	64,	    100,	0,		0,	//steel2
    "Kid_4",	"133 132 132",  "cast_thug",	64,		100,	0,		0,	//steel3
    "Kid_5",	"134 132 132",  "cast_thug",	64,		100,	0,		0,	//steel3
    "David",	"132 132 132",  "cast_punk",	64,		300,	0,		0,  //steel3 //Basicly a Kid_ just much health
    "Oscar",	"044 012 006",  "cast_punk",	64,		250,	0,		0,  //steel2
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

void cast_TF_Trainyard_trainwreck(edict_t *self)//ty1 trainwreck, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "507 081 054",	"cast_punk",    64,     100,    0,      1,
	"",	        "051 051 052",	"cast_punk",    64,     150,    0,      0,
	"",	        "050 053 054",	"cast_shorty",  16,     100,    0,      0,  //hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_frontline(edict_t *self)//ty2 frontline, all tommygun no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "054 051 054",	"cast_shorty",  64,     100,    0,      0,
	"",	        "507 051 052",	"cast_punk",    64,     100,    0,      1,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_trainteam(edict_t *self)//ty3 trainteam, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "071 070 021",	"cast_whore",   64,     100,    0,      0,
    "",	        "050 051 054",	"cast_shorty",  16,     100,    0,      0,  //hmg
	"",	        "054 052 052",	"cast_punk",    128,    100,    0,      0,  //grenade
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_poop(edict_t *self)//ty4 poop, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "507 052 052",	"cast_punk",    0,      100,    0,      1,
    "",	        "507 081 054",	"cast_punk",    16,     100,    0,      1,  //hmg
	"",	        "072 073 021",	"cast_whore",   0,      100,    0,      0,
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
void cast_TF_Radio_City_dragon(edict_t *self)//rc2 dragon1 and 2, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "057 055 058",	"cast_shorty",  0,      150,    0,      0,  //dragon1 shotgun
    "",	        "056 091 059",	"cast_punk",    64,     125,    0,      0,  //dragon2 tommygun
	"",	        "057 058 057",	"cast_punk",     8,     100,    0,      0,  //dragon2 bazooka
    "",	        "055 056 056",	"cast_whore",   64,     100,    0,      0,  //dragon1 tommygun
    "",	        "056 091 059",	"cast_whore",    0,     175,    0,      0,  //dragon2 shotgun
    "",	        "057 055 058",	"cast_shorty",  16,     100,    0,      0,  //dragon1 hmg
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragonalley(edict_t *self)//rc2 dragonalley, no names
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
void cast_TF_Radio_City_dragon_streets(edict_t *self)//rc3 dragon_streets, no names
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
void cast_TF_Radio_City_dragon_hoppers(edict_t *self)//rc3 dragon_hoppers, no names
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
void cast_TF_Radio_City_dragon_rooftop(edict_t *self)//rc3 dragon_rooftop, all bazooka, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "057 058 057",	"cast_punk",      8,     125,    0,      0,  //bazooka
    "",	        "058 057 056",	"cast_whore",     8,     150,    0,      0,  //bazooka
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_building(edict_t *self)//rc3 dragon_building, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "058 059 057",	"cast_punk",     16,     150,    0,      0,  //hmg
    "",	        "057 058 056",	"cast_whore",     8,     250,    0,      0,  //bazooka
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_street(edict_t *self)//rc4 and rc5 street1, no names
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "",	        "058 055 058",	"cast_runt",     0,     100,    0,      0,   //rc4 shotgun
    "",	        "057 058 056",	"cast_whore",    64,    100,    0,      0,   //rc4 tommygun
    "",	        "056 059 057",	"cast_punk",     0,     125,    0,      0,   //rc4 shotgun
    "",	        "058 090 059",	"cast_punk",     8,     100,    0,      0,   //rc5 bazooka
    "",	        "058 090 059",	"cast_punk",     8,     100,    0,      0,   //rc5 bazooka double name
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_dragon_office(edict_t *self)//rc5 office, allot of double skins :/ no names
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
}
void cast_TF_Radio_City_names(edict_t *self)
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Clarence", "045 024 005",  "cast_runt",	0,	    100,	1,		0,      //bar_rc pistol
	"Adolf",	"047 042 009",  "cast_thug",	0,		100,	0,		0,		//bar_rc pistol
    "Burt",	    "023 017 005",  "cast_thug",	0,		100,	0,		0,		//bar_rc pistol
    "Candy",	"019 019 015",  "cast_bitch",	0,	    100,	0,		0,	    //bar_rc pistol
    "Scalper",	"103 044 009",  "cast_thug",	0,		100,	0,		0,		//bar_rc pistol
    "Walter",	"130 130 010",  "cast_shorty",	16,		350,	0,		0,		//bar_rc hmg
    "Harpo",	"142 140 140",  "cast_runt",	64,	    200,	0,		0,	    //rc1 tommygun
    "Butch",	"131 130 010",	"cast_shorty",	0,	    100,	0,		0,	    //rc1 shotgun
    "Bubba",	"042 042 010",  "cast_runt",	64,	    100,	9,		0,	    //rc1 tommygun
    "Patrick",	"511 130 010",  "cast_punk",	64,		100,	0,		1,      //rc1 tommygun
  	"Groucho",	"140 141 046",  "cast_runt",	64,	    150,	0,		0,	    //rc1 tommygun
	"Chico",	"141 141 046",  "cast_runt",	64,	    150,	0,		0,	    //rc1 tommygun //same skin as grouncho
	"Bubba",	"130 131 010",	"cast_shorty",	64,		100,    9,		0,	    //rc2 tommygun
    "Lola",		"045 049 015",  "cast_bitch",	0,	    100,	0,		0, 	    //rc3 pistol
	"Dude",		"130 131 010",	"cast_shorty",	0,	    200,	9,		0,      //rc3 pistol
    "Momo",		"049 041 009",  "cast_runt",	0,	    100,	0,		0,	    //rc4 //bouncer pistol
    "Donny",	"512 131 010",  "cast_punk",	64,		200,	0,		1,      //rc4 tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
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
	self->spawnflags = 18;//hmg
	self->classname = "cast_whore";
	self->health = 1500 * currWave_plysCount;//150000 normally
}
void cast_TF_Crystal_Palace_boss(edict_t *spawn)
{
	switch (rand() % 2)
	{
	case 0:		cast_TF_Crystal_Palace_boss_kingpin(spawn);break;
	case 1:		cast_TF_Crystal_Palace_boss_blunt(spawn);break;
	}
}
// end setup cast skins
///////////////////////

//check if enemy has died or kill them for endWave/endGame
int cast_TF_checkEnemyState()
{
	int i, count = 0;

	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (level.characters[i])
		{
			if (level.modeset != WAVE_ACTIVE) //free characters
			{
				if (!level.characters[i]->client)
				{
					AI_UnloadCastMemory(level.characters[i]);
					G_FreeEdict(level.characters[i]);
				}
				level.characters[i] = NULL;
			}
			else //active wave
			{
				if (level.characters[i]->inuse && !level.characters[i]->client)
				{
					if (level.characters[i]->health <= 0
						|| level.characters[i]->deadflag == DEAD_DEAD
						|| level.characters[i]->s.origin[2] < -4096 //fallen in void!!!
						) //timelimit reached?
					{
						// free cast array //hypov8 todo: this is wrong
						//if (level.characters[i]->character_index)
						//AI_UnloadCastMemory(level.characters[i]);
						//G_FreeEdict(level.characters[i]);
						level.characters[i]->character_index = 0;
						level.characters[i] = NULL;
						level.num_characters--;
						currWave_castCount--;
						level.waveEnemyCount--;
					}
					else if (level.characters[i]->health > 0)
						count++;
				}
			}
		}
	}

	//free slot. add enemy
	if (level.modeset == WAVE_ACTIVE)
	{
		if (count < level.waveEnemyCount && count < currWave_castMax)
			cast_TF_spawn(0, 0);
	}
	else
	{
		//level.num_characters = 0;
		currWave_castCount = 0; //reset spawned cast count
	}

	return currWave_castCount;
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
void cast_TF_spawnWave1(edict_t *spawn)
{
#ifdef TEST_NEWSKIN
	switch (rand() % 10)
	{
	case 0:	cast_TF_dog(spawn);break;
	case 1:	cast_TF_rat(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
	case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Wave1_Skidrow(spawn);break;
	case 5:	cast_TF_Wave1_Skidrow(spawn);break;
	case 6:	cast_TF_Wave1_Skidrow(spawn);break;
	case 7:	cast_TF_Wave1_Skidrow(spawn);break;
	case 8:	cast_TF_Wave1_Skidrow(spawn);break;
	case 9:	cast_TF_Wave1_Skidrow(spawn);break;
	}
#else
	switch (rand() % 10)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_rat(spawn);break;
    case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Skidrow_treehouse(spawn);break;
	case 5:	cast_TF_Skidrow_sewer_rats(spawn);break;
	case 6:	cast_TF_Skidrow_names(spawn);break;//FREDZ abit hard first round, maybe need fix
    case 7:	cast_TF_Skidrow_names(spawn);break;
    case 8:	cast_TF_Skidrow_names(spawn);break;
    case 9:	cast_TF_Skidrow_names(spawn);break;
	}
#endif
}
void cast_TF_spawnWave2(edict_t *spawn)
{
	if (level.waveEnemyCount == currWave_castMax)
		cast_TF_Skidrow_boss(spawn);//spawn boss when almost done
	else
#ifdef TEST_NEWSKIN
	{
		switch (rand() % 14)
		{
		case 0:	cast_TF_dog(spawn);break;
		case 1:	cast_TF_dog(spawn);break;
		case 2:	cast_TF_rat(spawn);break;
		case 3:	cast_TF_rat(spawn);break;
		case 4:	cast_TF_Wave2_Skidrow(spawn);break;
		case 5:	cast_TF_Wave2_Skidrow(spawn);break;
		case 6:	cast_TF_Wave2_Skidrow(spawn);break;
		case 7:	cast_TF_Wave2_Skidrow(spawn);break;
		case 8:	cast_TF_Wave2_Skidrow(spawn);break;
		case 9:	cast_TF_Wave2_Skidrow(spawn);break;
		case 10: cast_TF_Wave2_Skidrow(spawn);break;
		case 11: cast_TF_Wave2_Skidrow(spawn);break;
		case 12: cast_TF_Wave2_Skidrow(spawn);break;
		case 13: cast_TF_Wave2_Skidrow(spawn);break;
		}
	}
#else
	{
		switch (rand() % 14)
		{
		case 0:	cast_TF_dog(spawn);break;
		case 1:	cast_TF_dog(spawn);break;
		case 2:	cast_TF_rat(spawn);break;
		case 3:	cast_TF_rat(spawn);break;
		case 4:	cast_TF_Skidrow_courtyard(spawn);break;
		case 5:	cast_TF_Skidrow_motards(spawn);break;
		case 6:	cast_TF_Skidrow_courtyard2(spawn);break;
		case 7:	cast_TF_Skidrow_street(spawn);break;
		case 8:	cast_TF_Skidrow_hallway(spawn);break;
		case 9:	cast_TF_Skidrow_postbattery(spawn);break;
		case 10: cast_TF_Skidrow_names(spawn);break;
		case 11: cast_TF_Skidrow_names(spawn);break;
		case 12: cast_TF_Skidrow_names(spawn);break;
		case 13: cast_TF_Skidrow_names(spawn);break;
		}
	}
#endif
}
void cast_TF_spawnWave3(edict_t *spawn)
{
	switch (rand() % 9)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Poisonville_boob(spawn);break;
	case 5:	cast_TF_Poisonville_names(spawn);break;
	case 6:	cast_TF_Poisonville_names(spawn);break;
    case 7:	cast_TF_Poisonville_names(spawn);break;
	case 8:	cast_TF_Poisonville_names(spawn);break;
	}
}
void cast_TF_spawnWave4(edict_t *spawn)
{
    switch (rand() % 10)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Poisonville_tanks(spawn);break;
	case 5:	cast_TF_Poisonville_names(spawn);break;
	case 6:	cast_TF_Poisonville_names(spawn);break;
    case 7:	cast_TF_Poisonville_names(spawn);break;
	case 8:	cast_TF_Poisonville_names(spawn);break;
	case 9:	cast_TF_Poisonville_boss(spawn);break;//FREDZ probably should only spawn ones
	}
}
void cast_TF_spawnWave5(edict_t *spawn)
{
	switch (rand() % 9)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Shipyard_deckmonkeys(spawn);break;
	case 5:	cast_TF_Shipyard_names(spawn);break;
	case 6:	cast_TF_Shipyard_names(spawn);break;
    case 7:	cast_TF_Shipyard_names(spawn);break;
	case 8:	cast_TF_Shipyard_names(spawn);break;
	}
}
void cast_TF_spawnWave6(edict_t *spawn)
{
	switch (rand() % 11)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Shipyard_goons(spawn);break;
    case 5:	cast_TF_Shipyard_swabs(spawn);break;
	case 6:	cast_TF_Shipyard_names(spawn);break;
	case 7:	cast_TF_Shipyard_names(spawn);break;
    case 8:	cast_TF_Shipyard_names(spawn);break;
	case 9:	cast_TF_Shipyard_names(spawn);break;
    case 10: cast_TF_Shipyard_boss(spawn);break;//FREDZ probably should only spawn ones
	}
}
void cast_TF_spawnWave7(edict_t *spawn)
{
	switch (rand() % 11)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Steeltown_teamno(spawn);break;
    case 5:	cast_TF_Steeltown_teamno(spawn);break;
	case 6:	cast_TF_Steeltown_grp(spawn);break;
	case 7:	cast_TF_Steeltown_names(spawn);break;
    case 8:	cast_TF_Steeltown_names(spawn);break;
	case 9:	cast_TF_Steeltown_names(spawn);break;
    case 10: cast_TF_Steeltown_boss(spawn);break;//FREDZ probably should only spawn ones
	}
}
void cast_TF_spawnWave8(edict_t *spawn)//Trainyard
{
	switch (rand() % 10)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Trainyard_trainwreck(spawn);break;
    case 5:	cast_TF_Trainyard_frontline(spawn);break;
	case 6:	cast_TF_Trainyard_trainteam(spawn);break;
	case 7:	cast_TF_Trainyard_poop(spawn);break;
	case 8:	cast_TF_Trainyard_names(spawn);break;
    case 9: cast_TF_Trainyard_boss(spawn);break;//FREDZ probably should only spawn ones
	}
}
void cast_TF_spawnWave9(edict_t *spawn)//Radiocity
{
	switch (rand() % 8)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Radio_City_dragon(spawn);break;
    case 5:	cast_TF_Radio_City_dragonalley(spawn);break;
	case 6:	cast_TF_Radio_City_names(spawn);break;
	case 7:	cast_TF_Radio_City_names(spawn);break;
	}
}
void cast_TF_spawnWave10(edict_t *spawn)//Radiocity
{
	switch (rand() % 11)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
    case 4:	cast_TF_Radio_City_dragon_street(spawn);break;
    case 5:	cast_TF_Radio_City_dragon_hoppers(spawn);break;
    case 6:	cast_TF_Radio_City_dragon_rooftop(spawn);break;
    case 7:	cast_TF_Radio_City_dragon_building(spawn);break;
	case 8:	cast_TF_Radio_City_dragon_street(spawn);break;
//    case 9:	cast_TF_Radio_City_dragon_office(spawn);break;
	case 9:	cast_TF_Radio_City_names(spawn);break;
	case 10:	cast_TF_Radio_City_names(spawn);break;
	}
}
void cast_TF_spawnWaveBoss(edict_t *spawn)
{
	cast_TF_Crystal_Palace_boss(spawn);
}
//end rand spawn types
//////////////////////


//generate types based on game length
void cast_TF_spawnTypes(edict_t *spawn)
{
	if (currWave_length == 11)	// long wave
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
	else if (currWave_length == 8)//med wave
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
	else 	//short wave
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
void cast_TF_spawn(int ammount, int type)
{
	edict_t *spawn, *spawnspot;

	{
		if (currWave_castCount >= MAX_CHARACTERS)
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

		if (!spawn->health)	spawn->health = 100;

		//make enemy health varable
		if ((int)maxwaves->value == 2)//11 waves
			spawn->health =  (int)((float)spawn->health * (0.5f + (float)level.waveNum / 22.0f)); //50% to 100%
		else if ((int)maxwaves->value == 1)//8 waves
			spawn->health =  (int)((float)spawn->health * (0.5f + (float)level.waveNum / 16.0f)); //50% to 100%
		else//5 waves
			spawn->health =  (int)((float)spawn->health * (0.5f + (float)level.waveNum / 10.0f)); //50% to 100%

		if (spawn->health< 10)	spawn->health = 10;

		//set what player to attack
		cast_TF_setEnemyPlayer(spawn);

		//add enemy to counter
		currWave_castCount++;
	}
}

//total enemy counts per wave.
//this will be multiplied by player counts later
#if 0 // HYPODEBUG
static int wave_shortGame[5] = { 2, 2, 2, 2, 1 };
static int wave_medGame[8] = { 2, 2, 2, 2, 2, 2, 2, 1 };
static int wave_longGame[11] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 };
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
		if (self->client->pers.spectator != SPECTATING)
		{
			playerCount++;
			AddCharacterToGame(self); //add player to level.characters
			if (!firstPlayer)
			{
				firstPlayer = 1;
				self->nav_TF_isFirstPayer = 1; //hypov8 nav
			}
			self->cast_group = 1;
		}
	}
	currWave_plysCount = playerCount;
	currWave_castMax = wave_skill[sk][playerCount]; //skill based enemy limits

	//make sure we have enough spawns(level size)
	if (level.dmSpawnPointCount < currWave_castMax)
		currWave_castMax = (int)( (float)level.dmSpawnPointCount * .75);
	if (currWave_castMax < 4)
		currWave_castMax = 4; //force 4 as min


	//get wave count
	if ((int)maxwaves->value == 2)
	{
		currWave_length = 11;//long wave
		level.waveEnemyCount = wave_longGame[level.waveNum] * playerCount;
	}
	else if ((int)maxwaves->value == 1)
	{
		currWave_length = 8;//med wave
		level.waveEnemyCount = wave_medGame[level.waveNum] * playerCount;
	}
	else
	{
		currWave_length = 5;//short wave
		level.waveEnemyCount = wave_shortGame[level.waveNum] * playerCount;
	}
}

