#include "g_local.h"
#include "g_cast_spawn.h"

#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT];// = {NULL, NULL, NULL}; //hypov8

void cast_TF_spawn(int ammount, int type);
static int currWave_castCount = 0; //enemy currently on map
static int currWave_plysCount = 0; //players
static int currWave_length = 0; //long, med, short
static int currWave_castMax = 0; //max enemy allowed on map


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

///////////////////////
// setup cast skins

void cast_TF_dog(edict_t *self)
{
	self->classname = "cast_dog";
}
void cast_TF_rat(edict_t *self)
{
	self->classname = "cast_rat";
}
void cast_TF_bitch_melee(edict_t *self)
{
	static cast_bitchskins_s skins[3] = {
		"Betty", "008 006 003",	//sr1
		"Beth", "009 007 004",	//sr1
		"Lisa", "012 015 012"	//sr1 Got normally 120 health
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].skin);
	self->spawnflags = 64;
	self->classname = "cast_bitch";
}
void cast_TF_bitch_pistol(edict_t *self)
{
	static cast_bitchskins_s skins[8] = {
		"Mona",			"014 012 003",	//sr2
		"yolanda",		"041 050 003",	//pv_h got normally 200 health
		"candy",		"015 017 013",	//bar_pv same name as in bar_rc
		"brittany",		"046 040 010",	//steel1
		"bambi",		"044 042 003",	//bar_st
		"lola",			"045 049 015", 	//rc3
		"candy",		"019 019 015",	//bar_rc same name as in bar_pv
		"selma",		"600 043 003", 	//bar_sy
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 8].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 8].skin);
	self->spawnflags = 0;
	self->classname = "cast_bitch";
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
	static cast_runtskins_s skins[13] = {
	"bubba",  "017 016 008", 9,	    //sr1
//	"louie", "011 011 005",	3,	    //sr1
//	"buttmunch", "001 001 001", 0,	//sr1
	"magicj", "020 011 005", 0,   	//sr1
	"kroker", "023 020 020", 1,	    //steel1 Got normally 150 health
	"kid_1", "134 132 132",	0,	    //steel2
	"kid_2", "132 132 132",	0,	    //steel2
	"kid_3", "133 132 132",	0,	    //steel2
	"popeye", "040 019 048", 0,	    //sy_h Got normaly health 300 and diffrent head?
	"harpo", "142 140 140",	0,	     //rc1 Got normally 200 health
	"bubba", "042 042 010",	9,	    //rc1
	"groucho", "140 141 046", 0,	     //rc1 Got normally 150 health
	"chico", "141 141 046", 0,		     //rc1 Got normally 150 health //same skin as grouncho
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].skin);
	self->count = (int)skins[rand() % 13].count;
	self->classname = "cast_runt";
	self->spawnflags = 64;
}
void cast_TF_runt_pistol(edict_t *self)
{
	static cast_runtskins_s skins[13] = {
//	"bernie", "011 012 004", "1",	//sr1 Got normally 150 health
//	"punky", "005 003 001",	"0",	//sr1 Got normally 50 health//sewer_rats

    "momo", "020 011 003", 0,		//sr2 //bouncer
    "momo", "013 045 006", 0,		//steel1 //bouncer
    "momo", "049 041 009", 0,		//rc4 //bouncer

    "moe", "047 065 041", 0,		//sy_h

//	"jesus", "021 017 010", 0,	//sr4 Got normally 400 health some kind of boss
	"mathew", "048 042 015", 0,	//steel1
	"luke", "049 046 046", 0,		//ty1
	"clarence", "047 013 003", 9,	//bar_pv
	"buster", "046 010 009", 9,	//bar_pv Got normally 120 health
	"louie", "043 041 009", 0,	//bar_pv
	"clarence", "045 024 005", 1,	//bar_rc
	"sluggo", "019 010 011", 0,	//bar_sr
	"lenny", "018 011 007",	0,	//bar_sr
	"dogg", "016 014 003",	0, 	//bar_sy
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].skin);
	self->count = (int)skins[rand() % 13].count;
	self->spawnflags = 0;
	self->classname = "cast_runt";
}
void cast_TF_runt_shotgun(edict_t *self)
{
	static cast_runtskins_s skins[4] = {
	"momo", "072 014 009", 0, 	//sr1 Got normally 150 health
    "butch", "131 130 010", 0,	//rc1
    "dude", "130 131 010", 9,     //rc3 Got normally 200 health
    "momo", "072 014 009", 0, //pv_h
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].skin);
	self->count = (int)skins[rand() % 4].count;
	self->spawnflags = 8192;//0; is also shotgun
	self->classname = "cast_shorty";
}
void cast_TF_runt_tommygun(edict_t *self)
{
	//name, skin, count
	static cast_runtskins_s skins[4] = {
        "sal", "064 047 047", 0,	    //bar_sy Got normally 250 health
        "mung", "013 010 006", 9,	    //pv_h
        "bubba", "130 131 010", 9,    //rc2
        "heilman", "121 121 121", 0,	//sy2 Got normally 250 health
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].skin);
	self->count = (int)skins[rand() % 4].count;
	self->spawnflags = 64;
	self->classname = "cast_shorty";
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
	static cast_thugskins_s skins[5] = {
		"leroy",	"010 010 003",	0,    //sr1
//		"johnny",	"011 007 004",	0,	//sr1 Got normally 80 health
		"brewster", "002 001 001",	0,	//sr1
//		"IntroGuy", "041 026 010",	0,	//sr1 player
        "kid_4",	"133 132 132",	0,	//steel3
        "kid_5",	"134 132 132",	0,	//steel3
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].skin);
	self->spawnflags = 64;
	self->classname = "cast_thug";
}
void cast_TF_thug_pistol(edict_t *self)
{
	static cast_thugskins_s skins[9] = {
		"fingers",	"025 009 009",//bar_pv Got normally 150 health
		"burt",	    "063 041 010",//bar_pv
		"adolf",	"047 042 009",//bar_rc
        "burt",	    "023 017 005",//bar_rc
        "scalper",	"103 044 009",//bar_rc
        "rocko",	"016 009 006",//bar_sr
//        "igmo",	    "003 002 001",//sr1 Got normally 200 health
        "lamont",	"024 017 010",//sr2 Got normally 300 health
        "hann",	    "029 006 010",//ty1
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 9].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 9].skin);
	self->spawnflags = 0;
	self->classname = "cast_thug";
}
void cast_TF_thug_shotgun(edict_t *self)
{
	static cast_thugskins_s skins[7] = {
		"bigwillie",	"109 046 047", "0",	//bar_sy Got normally 350 health
		"Laurel",		"504 032 031", "1",	//pv_h Got normally 200 health
		"Hardey",		"114 032 031", "0",	//pv_h Got normally 200 health
	//	"ToughGuy1",	"019 028 010", "0",	//sr1 intro guy
	//	"ToughGuy2",	"020 029 010", "0",	//sr1 intro guy
	//	"arnold",		"012 007 004", "0",	//sr1 Got normally 200 health
		"momo",			"107 044 010", "0",	//sy_h
		"larry",		"048 047 041", "0",	//sy_h Got normally 200 health
		"curly",		"100 047 041", "0",	//sy_h Got normally 200 health
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 7].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 7].skin);
	self->head = (int)skins[rand() % 7].head;
	self->spawnflags = 8192;//0; is also shotgun
	self->classname = "cast_punk";
}
void cast_TF_thug_tommygun(edict_t *self)
{
	//name, skin, count
	static cast_thugskins_s skins[7] = {
        "lefty", "064 046 047", "0",	  //bar_sy Got normally 250 health
        "dubs",  "108 009 006", "0",	  //pv_h  Got normally 220 health
        "patrick", "511 130 010", "1",    //rc1
        "donny", "512 131 010", "1",      //rc4 Got normally 200 health
        "oscar", "044 012 006", "0",      //steel2 Got normally 250 health
        "david", "132 132 132", "0",      //steel3 Got normally 300 health
        "tyrone", "507 082 054", "1",     //steel3 Got normally 500 health
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 7].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 7].skin);
	self->head = (int)skins[rand() % 7].head;
	self->spawnflags = 64;
	self->classname = "cast_punk";
}
void cast_TF_thug_bazooka(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "moker");//steel4 Got normally 800 health
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "111 027 023");
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
    //
	static localteam_skins_s skins[4] = {
		//name,		//skin,			classname		flags	HP		count	head
		"johnny",	"011 007 004",	"cast_thug",	64, 	80,	    0,       "0",
        "louie",    "011 011 005",	"cast_runt",	0,	    100,	3,  	 "0",//Not really police, more person they try to secure
        "bernie",   "011 012 004",  "cast_runt",	0,	    100,	1,	     "0",
        "arnold",	"012 007 004",	"cast_punk",	0,	    200,	0,	     "0",
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].classname);
    self->spawnflags = (int)skins[rand() % 4].spawnflags;
    self->health = (int)skins[rand() % 4].health;
    self->count = (int)skins[rand() % 4].count;
}
void cast_TF_Skidrow_sewer_rats(edict_t *self)//sr1 Sewer Rats ugly skins
{
    //
	static localteam_skins_s skins[3] = {
		//name,		//skin,			classname		flags	HP		count	head
		"buttmunch", "001 001 001",	"cast_runt",	64,	    100,	0,	    "0",
        "punky",     "005 003 001",	"cast_runt",	0,	    50,	    0,	    "0",
        "igmo",      "003 002 001", "cast_thug",	0,	    200,	0,	    "0",
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].classname);
    self->spawnflags = (int)skins[rand() % 3].spawnflags;
    self->health = (int)skins[rand() % 3].health;
}
void cast_TF_Skidrow_treehouse(edict_t *self)//sewer treehouse, ugly skins, no names
{
    //
	static localteam_skins_s skins[2] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",         "004 003 001",	"cast_runt",	0,	    100,	0,	    "0", //todo: indeed no name, also not really needed? expect for friendly characters:/
        "",         "004 001 007",	"cast_punk",	0,	    100,	0,	    "0",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 2].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 2].classname);
    self->spawnflags = 0;
}
void cast_TF_Skidrow_motards(edict_t *self)//sr2 motards, ugly skins all pistol, no names
{
    //
	static localteam_skins_s skins[3] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",          "003 003 001",	"cast_runt",	0,	    100,	0,	    "0",
        "",         "002 001 001",	"cast_bitch",	0,	    100,	0,	    "0",
        "",	        "005 001 001",  "cast_thug",	0,	    100,	0,	    "0",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].classname);
    self->spawnflags = 0;
}
//SR3 pack1 missing duo just 3 dogs
void cast_TF_Skidrow_courtyard2(edict_t *self)//sr3 courtyard2 and 1, no names
{
	static localteam_skins_s skins[5] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "514 003 002",	"cast_punk",	0,	    100,	0,	    "1",//courtyard2
        "",	        "010 006 002",	"cast_runt",	0,	    100,	0,	    "0",//courtyard1
        "",	        "514 004 002",  "cast_punk",	0,	    100,	0,	    "1",//courtyard1
        "",	        "513 004 002",  "cast_punk",	0,	    100,	0,	    "1",//courtyard1
        "",	        "006 005 002",	"cast_runt",	0,	    100,	0,	    "0",//courtyard2
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].classname);
    self->spawnflags = 0;
 //   self->head = (int)skins[rand() % 5].head;//FREDZ probably gives problems
}
void cast_TF_Skidrow_street(edict_t *self)//sr3 street1, no names
{
	static localteam_skins_s skins[3] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "006 006 002",	"cast_runt",	0,	    100,	0,	    "0",
        "",	        "008 008 002",	"cast_runt",	0,	    100,	0,	    "0",
        "",	        "010 004 002",  "cast_runt",	64,	    100,	0,	    "0",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].classname);
    self->spawnflags = (int)skins[rand() % 3].spawnflags;
}
void cast_TF_Skidrow_hallway(edict_t *self)//sr3 hallway1,  no names
{
	static localteam_skins_s skins[2] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",     	"006 006 002",	"cast_runt",	0,	    100,	0,	    "0",
        "",	        "514 003 002",	"cast_punk",	0,	    100,	0,	    "0",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 2].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 2].classname);
    self->spawnflags = 0;
}
void cast_TF_Skidrow_postbattery(edict_t *self)//sr3 postbattery1 and 2,mostly shotty,  no names
{
	static localteam_skins_s skins[5] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "004 001 001",	"cast_punk",    0,      120,  0,    "0",    //postbattery1
        "",	        "008 008 002",	"cast_runt",    0,      120,  0,    "0",    //postbattery2
        "",     	"514 003 002",	"cast_punk",    0,      120,  0,    "1",    //postbattery2
        "",	        "009 007 002",	"cast_runt",    0,      100,  0,    "0",    //postbattery2
        "",	        "004 001 001",	"cast_punk",    0,      80,   0,    "0",    //postbattery1
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].classname);
    self->spawnflags = 0;
    self->health = (int)skins[rand() % 5].health;
//    self->head = (int)skins[rand() % 5].head;//FREDZ probably gives problems
}
void cast_TF_Skidrow_names(edict_t *self)
{
	static localteam_skins_s skins[13] = {
		//name,		//skin,			classname		flags	HP		count	head
		"Betty",	"008 006 003",	"cast_bitch",	64,	    100,	0,  	"0",    //sr1
		"Beth",		"009 007 004",	"cast_bitch",	64,	    100,	0,  	"0",    //sr1
		"Lisa",		"012 015 012",	"cast_bitch",	64,	    120,	0,  	"0",    //sr1
        "Mona",		"014 012 003",	"cast_bitch",	0,	    100,	0,  	"0",    //sr2
        "bubba",	"017 016 008",	"cast_runt",	64,	    100,	9,  	"0",    //sr1
        "magicj",	"020 011 005",	"cast_runt",	64,	    100,	0,  	"0",   	//sr1
        "momo",		"020 011 003",	"cast_runt",	0,	    100,	0,  	"0",	//sr2 //bouncer
    	"sluggo",	"019 010 011",	"cast_runt",	0,	    100,	0,  	"0",    //bar_sr
        "lenny",	"018 011 007",	"cast_runt",	0,	    100,	0,  	"0",	//bar_sr
        "momo",		"072 014 009",	"cast_shorty",	8192,   150,	0,  	"0", 	//sr1
        "leroy",	"010 010 003",	"cast_thug",	64,	    100,	0,  	"0",    //sr1
		"brewster", "002 001 001",	"cast_thug",	64,	    100,	0,  	"0",	//sr1
        "rocko",	"016 009 006",	"cast_thug",	0,	    100,	0,  	"0",	//bar_sr
	};

    self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].classname);
    self->spawnflags = (int)skins[rand() % 3].spawnflags;
    self->health = (int)skins[rand() % 13].health;
    self->count = (int)skins[rand() % 13].count;
}
void cast_TF_Skidrow_boss_lamont(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "lamont");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "024 017 010");
	self->spawnflags = 0;//pistol
	self->classname = "cast_thug";
	self->moral = 6;
//	self->scale = 1.05;
	self->health = 300 * currWave_plysCount;
}
void cast_TF_Skidrow_boss_jesus(edict_t *self)//sr4
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "jesus");
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
	static localteam_skins_s skins[3] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "030 031 032",	"cast_shorty",	0,	    100,	0,	    "0",
		"",	        "503 033 032",	"cast_punk",	0,	    100,	0,	    "0",
		"",	        "033 032 032",	"cast_shorty",	0,	    100,	0,	    "0",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].classname);
    self->spawnflags = 0;
}
void cast_TF_Poisonville_tanks(edict_t *self)//pv_b tanks, all tommygun, no names
{
	static localteam_skins_s skins[4] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "506 036 032",	"cast_punk",    64,     160,    0,      "1",
		"",     	"504 031 031",	"cast_punk",    64,     140,    0,      "1",
		"",	        "030 032 033",	"cast_whore",   64,     120,    0,      "0",
		"",	        "035 030 031",	"cast_shorty",  64,     140,    0,      "0",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 4].classname);
    self->spawnflags = 64;
    self->health = (int)skins[rand() % 4].health;
//    self->head = (int)skins[rand() % 4].head;//FREDZ probably gives problems
}
void cast_TF_Poisonville_names(edict_t *self)
{
	static localteam_skins_s skins[11] = {
		//name,		//skin,			classname		flags	HP		count	head
        "yolanda",	"041 050 003",	"cast_bitch",   0,      200,    0,      "0", 	//pv_h
        "Mung",     "013 010 006",	"cast_runt",    64,     100,    9,      "0",	//pv_h
        "Fingers",	"025 009 009",	"cast_thug",    0,      150,    0,      "0",	//bar_pv
		"Burt",	    "063 041 010",	"cast_thug",    0,      100,    0,      "0",	//bar_pv
        "Clarence", "047 013 003",  "cast_runt",    0,      100,    9,      "0",	//bar_pv
        "Candy",	"015 017 013",  "cast_bitch",   0,      100,    0,      "0",	//bar_pv
        "Louie",    "043 041 009",  "cast_runt",    0,      100,    0,      "0",	//bar_pv
        "Laurel",	"504 032 031",	"cast_punk",    8192,   200,    0,      "1",	//pv_h
		"Hardey",	"114 032 031",	"cast_punk",    8192,   200,    0,      "0",	//pv_h
        "Dubs",     "108 009 006",  "cast_punk",    64,     220,    0,      "0",	//pv_h
        "Momo",     "072 014 009",  "cast_shorty",  0,      100,    0,      "0",	//pv_h
	};

    self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 11].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 11].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 11].classname);
    self->spawnflags = (int)skins[rand() % 11].spawnflags;
    self->health = (int)skins[rand() % 11].health;
    self->count = (int)skins[rand() % 11].count;
}
void cast_TF_Poisonville_boss(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "nikkiblanco");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "122 122 122");
	self->spawnflags = 16;//Flametrhower
	self->classname = "cast_punk";
	self->moral = 5;
//	self->scale = 1.06;
	self->health = 450 * currWave_plysCount;
}
void cast_TF_Shipyard_deckmonkeys(edict_t *self)//sy1 deck_monkeys and 2, shotty and tommyguns, no names
{
	static localteam_skins_s skins[6] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "061 062 060",	"cast_shorty",  0,      300,    0,      "0",    //deck_monkeys2
		"",     	"060 060 060",	"cast_shorty",  0,      250,    0,      "0",    //deck_monkeys
		"",	        "060 060 060",	"cast_whore",   64,     100,    0,      "0",    //deck_monkeys2
		"",	        "508 060 060",	"cast_punk",    0,      300,    0,      "1",    //deck_monkeys2
        "",	        "515 062 060",	"cast_punk",    64,     250,    0,      "1",    //deck_monkeys
		"",	        "515 060 060",	"cast_punk",    64,     250,    0,      "1",    //deck_monkeys
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 6].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 6].classname);
    self->spawnflags = (int)skins[rand() % 6].spawnflags;
    self->health = (int)skins[rand() % 6].health;
//    self->head = (int)skins[rand() % 6].head;//FREDZ probably gives problems
}
void cast_TF_Shipyard_goons(edict_t *self)//sy2 goons, shotty and tommyguns, no names
{
	static localteam_skins_s skins[8] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "515 063 060",	"cast_punk",    0,      250,    0,      "1",
		"",     	"060 060 060",	"cast_whore",   0,      250,    0,      "0",
		"",	        "515 063 060",	"cast_punk",    0,       20,    0,      "1",//Health not a typer error or xatrix did that :p
		"",	        "515 063 060",	"cast_punk",    0,      250,    0,      "1",
        "",	        "063 064 060",	"cast_shorty",  64,     225,    0,      "0",
		"",	        "101 065 060",	"cast_punk",    64,     225,    0,      "0",
        "",	        "113 065 060",	"cast_punk",    64,     300,    0,      "0",
        "",	        "064 066 060",	"cast_shorty",  64,     300,    0,      "0",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 8].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 8].classname);
    self->spawnflags = (int)skins[rand() % 8].spawnflags;
    self->health = (int)skins[rand() % 8].health;
//    self->head = (int)skins[rand() % 8].head;//FREDZ probably gives problems
}
void cast_TF_Shipyard_swabs(edict_t *self)//sy2 swabs, mostly shotty and low health, no names
{
	static localteam_skins_s skins[5] = {
		//name,		//skin,			classname		flags	HP		count	head
		"",	        "045 064 060",	"cast_punk",    64,      75,    0,      "0",
		"",     	"060 060 060",	"cast_shorty",   0,      75,    0,      "0",
		"",	        "060 060 060",	"cast_whore",    0,      75,    0,      "0",
		"",	        "066 064 060",	"cast_punk",     0,      75,    0,      "0",
        "",	        "515 063 060",	"cast_punk",     0,      75,    0,      "1",
	};

	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].classname);
    self->spawnflags = (int)skins[rand() % 5].spawnflags;
    self->health = (int)skins[rand() % 5].health;
//    self->head = (int)skins[rand() % 5].head;//FREDZ probably gives problems
}
void cast_TF_Shipyard_names(edict_t *self)
{
	static localteam_skins_s skins[10] = {
		//name,		//skin,			classname		flags	HP		count	head
        "Dogg",     "016 014 003",  "cast_runt",    0,      100,    0,      "0",    //bar_sy
        "Selma",	"600 043 003",  "cast_bitch",   0,      100,    0,      "2", 	//bar_sy
        "BigWillie","109 046 047",  "cast_punk",    0,      350,    0,      "0",    //bar_sy
        "Sal",      "064 047 047",  "cast_shorty",  64,     250,    0,      "0",    //bar_sy
        "Lefty",    "064 046 047",  "cast_punk",    64,     250,    0,      "0",    //bar_sy
        "Moe",      "047 065 041",  "cast_runt",    0,       50,    0,      "0",    //sy_h
        "Popeye",   "040 019 048",  "cast_runt",    64,     300,    0,      "1",    //sy_h
        "Momo",		"107 044 010",  "cast_punk",    0,      100,    0,      "0",    //sy_h
		"Larry",	"048 047 041",  "cast_punk",    0,      200,    0,      "0",    //sy_h
		"Curly",	"100 047 041",  "cast_punk",    0,      200,    0,      "0",    //sy_h
	};

    self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 10].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 10].skin);
	self->classname = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 10].classname);
    self->spawnflags = (int)skins[rand() % 10].spawnflags;
    self->health = (int)skins[rand() % 10].health;
    self->count = (int)skins[rand() % 10].count;
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
//FREDZ todo
//Steeltown
//Trainyard
//Radio City
void cast_TF_Crystal_Palace_boss_kingpin(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "kingpin");
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
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "blunt");
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
	switch (rand() % 6)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_rat(spawn);break;
    case 2:	cast_TF_rat(spawn);break;
    case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_Skidrow_treehouse(spawn);break;
	case 5:	cast_TF_Skidrow_sewer_rats(spawn);break;
//	case 5:	cast_TF_Skidrow_names(spawn);break;//FREDZ abit hard first round, maybe need fix
//  case 6:	cast_TF_Skidrow_names(spawn);break;
//  case 7:	cast_TF_Skidrow_names(spawn);break;
//  case 8:	cast_TF_Skidrow_names(spawn);break;
	}
}
void cast_TF_spawnWave2(edict_t *spawn)
{
	switch (rand() % 15)
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
    case 14: cast_TF_Skidrow_boss(spawn);break;//FREDZ probably should only spawn ones
	}
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
	//todo
}
void cast_TF_spawnWave8(edict_t *spawn)
{
	//todo
}
void cast_TF_spawnWave9(edict_t *spawn)
{
	//todo
}
void cast_TF_spawnWave10(edict_t *spawn)
{
	//todo
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

		if(!Q_strcasecmp(spawn->classname, "cast_dog"))
			spawn->cast_group = 23; //hypov8 does this work?
		else
			spawn->cast_group = 22; //hypov8 does this work?

		if (!spawn->health)
			spawn->health = 100;

		//set what player to attack
		cast_TF_setEnemyPlayer(spawn);

		//add enemy to counter
		currWave_castCount++;
	}
}

//FREDZ tweak probably devide this to 2 and to up divide by 2? makes sence with the 2.5 before with money?
//wave total enemy counts
//originale:
//static int wave_shortGame[5] = { 25, 32, 35, 42, 1 };
//Devide by 5:
//static int wave_shortGame[5] = { 5, 6, 7, 8, 1 };
//Devide by 2:
static int wave_shortGame[5] = { 13, 16, 17, 21, 1 };

//originale:
//static int wave_medGame[8] = { 25, 28, 32, 35, 35, 40, 42, 1 };
//Devide by 5:
//static int wave_medGame[8] = { 5, 6, 7, 7, 7, 8, 8, 1 };
//Devide by 2:
static int wave_medGame[8] = { 13, 14, 16, 17, 17, 20, 21, 1 };

//originale:
//static int wave_longGame[11] = { 25, 28, 32, 32, 35, 35, 35, 40, 42, 42, 1 };
//Devide by 5:
//static int wave_longGame[11] = { 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 1 };
//Devide by 2:
static int wave_longGame[11] = { 2, 14, 16, 17, 17, 17, 17, 20, 21, 21, 1 };//testing 13, 14, 16, 17, 17, 17, 17, 20, 21, 21, 1

//wave skill. number of enemy allowed in level at 1 time. max 8 players
//originale:
/*
static int wave_skill[5][9] = {
	{ 0, 10, 14, 32, 32, 32, 32, 32, 32},   //novice
	{ 0, 11, 18, 32, 32, 32, 32, 32, 32},   //easy
	{ 0, 12, 18, 32, 32, 32, 32, 32, 32 },  //medium
	{ 0, 12, 18, 32, 32, 32, 32, 32, 32 },  //hard
	{ 0, 12, 18, 32, 32, 32, 32, 32, 32 }   //real
};*/
//Devide by 2:
static int wave_skill[5][9] = {
//num players
//0, 1, 2, 3, 4, 5, 6. 7, 8
	{ 0, 5, 7, 16, 16, 16, 16, 16, 16},     //novice
	{ 0, 5, 9, 16, 16, 16, 16, 16, 16},     //easy
	{ 0, 6, 9, 16, 16, 16, 16, 16, 16 },    //medium
	{ 0, 6, 9, 16, 16, 16, 16, 16, 16 },    //hard
	{ 0, 6, 9, 16, 16, 16, 16, 16, 16 }     //real
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
		level.waveEnemyCount = wave_longGame[level.waveNum] * playerCount;
	}
}

