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
		if (!spawnspot) {
			gi.dprintf("\nNo spawn points for cast.\n\n");
			G_FreeEdict(spawn);
			return;
		}
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

//localteam
void cast_TF_Skidrow_melee(edict_t *self)//New skins scorpions
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	head
    "thug",	        "513 004 002",  "cast_thug",	64,	    100,		0,	    1,
    "thug",	        "514 003 002",  "cast_thug",	64,	    100,		0,	    1,
    "runt",	        "010 006 002",	"cast_runt",	64,	    100,		0,	    0,
    "runt",	        "006 005 002",	"cast_runt",	64,	    100,		0,	    0,
    "runt",	        "008 008 002",	"cast_runt",	64,	    100,		0,	    0,
    "runt",	        "010 004 002",  "cast_runt",	64,	    100,		0,	    0,
    "runt",     	"006 006 002",	"cast_runt",	64,	    100,		0,	    0,
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Skidrow_skins(edict_t *self)//New skins scorpions
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	head
    "thug",	        "513 004 002",  "cast_punk",	0,	    100,		0,	    1,//shotgun
	"thug",	        "514 003 002",	"cast_punk",	0,	    100,		0,	    1,  //shotgun
    "runt",	        "010 006 002",	"cast_runt",	0,	    100,		0,	    0, //pistol
    "thug",	        "006 005 002",	"cast_runt",	0,	    100,		0,	    0, //pistol
    "thug",	        "008 008 002",	"cast_runt",	0,	    100,		0,	    0,  //pistol
    "thug",	        "010 004 002",  "cast_runt",	64,	    100,		0,	    0,  //melee
    "thug",     	"006 006 002",	"cast_runt",	0,	    100,		0,	    0,  //pistol
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
//	self->health = 400 * currWave_plysCount;//originale
    self->health = 300 * currWave_plysCount;//wass to high, maybe still shoot to fast

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
void cast_TF_Poisonville_skins1(edict_t *self)//New skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Runt",	        "030 030 031",	"cast_runt",	0,	    100,	0,	    0,//pistol
    "Runt",	        "035 031 031",	"cast_shorty",	0,	    100,	0,	    0,//Shotgun
    "Runt",	        "035 031 031",	"cast_shorty",  64,     100,    0,      0,//Tommygun
    "Thug",	        "503 031 031",	"cast_thug",	0,	    100,	0,	    1,//pistol
    "Thug",	        "504 031 031",	"cast_punk",	0,	    100,	0,	    1,//Shotgun
	"Thug",     	"504 031 031",	"cast_punk",    64,     100,    0,      1,//Tommygun
	"Bitch",	    "031 031 033",	"cast_bitch",    0,     100,    0,      0,//pistol
	"Bitch",	    "032 032 033",	"cast_whore",    0,     100,    0,      0,//Shotgun
	"Bitch",	    "032 032 033",	"cast_whore",   64,     100,    0,      0,//Tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Poisonville_skins2(edict_t *self)//New skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
	"Runt",	        "030 030 031",	"cast_runt",	0,	    150,	0,	    0,//pistol
    "Runt",	        "035 031 031",	"cast_shorty",	0,	    150,	0,	    0,//Shotgun
    "Runt",	        "035 031 031",	"cast_shorty",  64,     150,    0,      0,//Tommygun
    "Thug",	        "503 031 031",	"cast_thug",	0,	    150,	0,	    1,//pistol
    "Thug",	        "504 031 031",	"cast_punk",	0,	    150,	0,	    1,//Shotgun
	"Thug",     	"504 031 031",	"cast_punk",    64,     150,    0,      1,//Tommygun
	"Bitch",	    "031 031 033",	"cast_bitch",    0,     150,    0,      0,//pistol
	"Bitch",	    "032 032 033",	"cast_whore",    0,     150,    0,      0,//Shotgun
	"Bitch",	    "032 032 033",	"cast_whore",   64,     150,    0,      0,//Tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Poisonville_boss_nikkiblanco(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Nikkiblanco");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "122 122 122");
	self->spawnflags = 4;//Flamethrower
	self->classname = "cast_punk";
	self->moral = 5;
//	self->scale = 1.06;
    self->cast_info.scale = 1.25;//Or will give problems?
//	self->health = 450 * currWave_plysCount;//originale
    self->health = 400 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Shipyard_skins1(edict_t *self)//New skins
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	head
	"Runt",     	"063 060 060",	"cast_shorty",   0,     200,    0,      0, //shotgun
    "Runt",	        "063 064 060",	"cast_shorty",  64,     200,    0,      0, //tommygun
    "Thug",	        "515 060 060",	"cast_punk",     0,     200,    0,      1, //shotgun
    "Thug",	        "045 064 060",	"cast_punk",    64,     200,    0,      0, //tommygun
	"Bitch",     	"060 060 060",	"cast_whore",    0,     200,    0,      0, //shotgun
    "Bitch",	    "060 060 060",	"cast_whore",   64,     200,    0,      0, //tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_skins2(edict_t *self)//New skins
{
	static localteam_skins_s skins[] = {
	//name,		    //skin,			classname		flags	HP		count	head
	"Runt",     	"063 060 060",	"cast_shorty",   0,     250,    0,      0, //shotgun
    "Runt",	        "063 064 060",	"cast_shorty",  64,     250,    0,      0, //tommygun
    "Thug",	        "515 060 060",	"cast_punk",     0,     250,    0,      1, //shotgun
    "Thug",	        "045 064 060",	"cast_punk",    64,     250,    0,      0, //tommygun
	"Bitch",     	"060 060 060",	"cast_whore",    0,     250,    0,      0, //shotgun
    "Bitch",	    "060 060 060",	"cast_whore",   64,     250,    0,      0, //tommygun
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Shipyard_boss_popeye(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Popeye");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "040 019 048");
	self->spawnflags = 64;//Melee
	self->classname = "cast_runt";
	self->moral = 4;
    self->acc = 2;
    self->head = 1;
    self->cast_info.scale = 1.25;//Or will give problems?
//	self->health = 300 * currWave_plysCount;//originale
    self->health = 200 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Shipyard_boss_heilman(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Heilman");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "121 121 121");
//	self->spawnflags = 64;//Tommygun originale
    self->spawnflags = 0;//shogun
	self->classname = "cast_shorty";
	self->moral = 5;
    self->acc = 4;
    self->cast_info.scale = 1.25;//Or will give problems?
//	self->health = 650 * currWave_plysCount;//to strong compare to tyrone
    self->health = 500 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Steeltown_skins(edict_t *self)//New skins, maybe higher health?
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Thug",	        "500 022 020",	"cast_punk",    0,      100,    0,      1,    //shotgun
    "Thug",	        "500 022 020",	"cast_punk",    64,     100,    0,      1,    //tommygun
    "Thug",	        "024 021 022",	"cast_punk",    16,     100,    0,      0,    //hmg
    "Thug",	        "700 021 022",	"cast_punk",    4,      150,    0,      3,    //flamethrower
    "Thug",	        "700 021 022",	"cast_punk",    4,      150,    0,      3,    //flamethrower
    "Thug",	        "700 021 022",	"cast_punk",    4,      150,    0,      3,    //flamethrower
    "Runt",	        "024 022 020",	"cast_runt",    0,      100,    0,      0,    //pistol
    "Runt",	        "024 022 020",	"cast_runt",    0,      100,    0,      0,    //shotgun
    "Runt",	        "024 022 020",	"cast_shorty",  64,     100,    0,      0,    //tommygun
    "Runt",	        "023 021 022",	"cast_shorty",  16,     100,    0,      0,    //hmg
    "Bitch",        "021 021 021",	"cast_whore",   0,      100,    0,      0,    //shotgun
    "Bitch",     	"021 021 021",	"cast_whore",   64,     100,    0,      0,    //tommygun
    "Bitch",     	"020 020 021",	"cast_whore",   16,     100,    0,      0,    //hmg

	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Steeltown_boss_moker(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Moker");//steel4
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "111 027 023");
	self->spawnflags = 8;//bazooka
    self->moral = 5;
    self->acc = 5;
//    self->localteam = teamno2;
	self->classname = "cast_punk";
	self->cast_info.scale = 1.25;//Or will give problems?
//    self->health = 800 * currWave_plysCount;//originale
    self->health = 600 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Trainyard_skins(edict_t *self)//New skins mix of localteam trainyard to fix skin
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Thug",	        "051 051 052",	"cast_thug",    0,      150,    0,      0, //pistol
    "Thug",	        "051 051 052",	"cast_punk",    0,      150,    0,      0, //shotgun
	"Thug",	        "051 051 052",	"cast_punk",    64,     150,    0,      0, //tommygun
    "Thug",	        "507 081 054",	"cast_punk",    16,     150,    0,      1, //hmg
    "Thug",	        "507 051 052",	"cast_punk",    128,    150,    0,      1, //gl
    "Runt",	        "051 081 052",	"cast_runt",    0,      150,    0,      0, //pistol
    "Runt",	        "051 081 052",	"cast_shorty",  0,      150,    0,      0, //shotgun
    "Runt",	        "051 081 052",	"cast_shorty",  64,     150,    0,      0, //tommygun
    "Runt",	        "050 051 054",	"cast_shorty",  16,     150,    0,      0, //hmg
    "Runt",	        "050 051 054",	"cast_shorty", 128,     150,    0,      0, //gl
    "Bitch",        "071 070 021",	"cast_bitch",    0,     150,    0,      0, //pistol
    "Bitch",        "071 070 021",	"cast_whore",    0,     150,    0,      0, //shotgun
    "Bitch",        "071 073 021",	"cast_whore",   64,     150,    0,      0, //tommygun
    "Bitch",        "072 073 021",	"cast_whore",   16,     150,    0,      0, //hmg
    "Bitch",        "072 073 021",	"cast_whore",  128,     150,    0,      0, //gl
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Trainyard_boss_tyrone(edict_t *self)
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
//    self->health = 500 * currWave_plysCount;//Week compare to heilman
    self->health = 700 * currWave_plysCount;

	//store boss ID.
	boss_entityID = self;
}
void cast_TF_Radio_City_skins1(edict_t *self)//New skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Runt",	    "058 056 058",	"cast_runt",     0,     200,    0,      0,  //pistol
    "Runt",	    "058 056 058",	"cast_shorty",   0,     200,    0,      0,  //shotgun
    "Runt",	    "058 056 058",	"cast_shorty",  64,     200,    0,      0,  //tommygun
    "Runt",	    "057 055 059",	"cast_shorty",  16,     200,    0,      0,  //hmg
    "Runt",	    "057 055 059",	"cast_shorty", 128,     200,    0,      0,  //gl
    "Runt",	    "057 055 059",	"cast_shorty",   8,     200,    0,      0,  //bazooka
    "Runt",	    "057 055 059",	"cast_shorty",   4,     200,    0,      0,  //flamethrower
    "Thug",     "057 056 058",	"cast_thug",     0,     200,    0,      0,  //pistol
    "Thug",     "057 056 058",	"cast_punk",     0,     200,    0,      0,  //shotgun
    "Thug",     "057 056 058",	"cast_punk",    64,     200,    0,      0,  //tommygun
	"Thug",     "056 058 059",	"cast_punk",    16,     200,    0,      0,  //hmg
    "Thug",     "056 058 059",	"cast_punk",   128,     200,    0,      0,  //gl
    "Thug",     "056 058 059",	"cast_punk",     8,     200,    0,      0,  //bazooka
    "Thug",     "056 058 059",	"cast_punk",     4,     200,    0,      0,  //flamethrower
    "Bitch",    "057 057 056",	"cast_bitch",    0,     200,    0,      0,  //pistol
    "Bitch",	"057 057 056",	"cast_whore",    0,     200,    0,      0,  //shotgun
    "Bitch",	"057 057 056",	"cast_whore",   64,     200,    0,      0,  //tommygun
    "Bitch",	"058 056 056",	"cast_whore",   16,     200,    0,      0,  //hmg
    "Bitch",	"058 056 056",	"cast_whore",  128,     200,    0,      0,  //gl
    "Bitch",	"058 056 056",	"cast_whore",    8,     200,    0,      0,  //bazooka
    "Bitch",	"058 056 056",	"cast_whore",    4,     200,    0,      0,  //flamethrower
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_skins2(edict_t *self)//New skins
{
	static localteam_skins_s skins[] = {
	//name,		//skin,			classname		flags	HP		count	head
    "Runt",	    "058 056 058",	"cast_runt",     0,     250,    0,      0,  //pistol
    "Runt",	    "058 056 058",	"cast_shorty",   0,     250,    0,      0,  //shotgun
    "Runt",	    "058 056 058",	"cast_shorty",  64,     250,    0,      0,  //tommygun
    "Runt",	    "057 055 059",	"cast_shorty",  16,     250,    0,      0,  //hmg
    "Runt",	    "057 055 059",	"cast_shorty", 128,     250,    0,      0,  //gl
    "Runt",	    "057 055 059",	"cast_shorty",   8,     250,    0,      0,  //bazooka
    "Runt",	    "057 055 059",	"cast_shorty",   4,     250,    0,      0,  //flamethrower
    "Thug",     "057 056 058",	"cast_thug",     0,     250,    0,      0,  //pistol
    "Thug",     "057 056 058",	"cast_punk",     0,     250,    0,      0,  //shotgun
    "Thug",     "057 056 058",	"cast_punk",    64,     250,    0,      0,  //tommygun
	"Thug",     "056 058 059",	"cast_punk",    16,     250,    0,      0,  //hmg
    "Thug",     "056 058 059",	"cast_punk",   128,     250,    0,      0,  //gl
    "Thug",     "056 058 059",	"cast_punk",     8,     250,    0,      0,  //bazooka
    "Thug",     "056 058 059",	"cast_punk",     4,     250,    0,      0,  //flamethrower
    "Bitch",    "057 057 056",	"cast_bitch",    0,     250,    0,      0,  //pistol
    "Bitch",	"057 057 056",	"cast_whore",    0,     250,    0,      0,  //shotgun
    "Bitch",	"057 057 056",	"cast_whore",   64,     250,    0,      0,  //tommygun
    "Bitch",	"058 056 056",	"cast_whore",   16,     250,    0,      0,  //hmg
    "Bitch",	"058 056 056",	"cast_whore",  128,     250,    0,      0,  //gl
    "Bitch",	"058 056 056",	"cast_whore",    8,     250,    0,      0,  //bazooka
    "Bitch",	"058 056 056",	"cast_whore",    4,     250,    0,      0,  //flamethrower
	};
	int idx = rand() % ARYSIZE(skins);
	cast_TF_applyRandSkin(self, skins, idx);
}
void cast_TF_Radio_City_boss_nikkiblanco(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "nikkiblanco");//rcboss1
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "122 122 122");
//	self->spawnflags = 64;//Tommygun
	self->spawnflags = 8;//bazooka
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
	//Normal but not really fair? spawn also with hmg
/*	self->spawnflags = 0;//pistol
    self->acc = 5;
    self->cal = 5;
	self->classname = "cast_runt";*/
    self->spawnflags = 16;//hmg
    self->classname = "cast_shorty";
	self->cast_info.scale = 1.25;
	self->health = 1500 * currWave_plysCount;
}
void cast_TF_Crystal_Palace_boss_blunt(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "Blunt");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "080 059 059");
	self->spawnflags = 16;//hmg //18 =triger spawned (>>2)
	self->classname = "cast_whore";
	self->cast_info.scale = 1.25;
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

//Full list with diffrent pain sound see self->onfiretime
extern voice_table_t sr_jesus[];
extern voice_table_t sy_popeye[];
//extern voice_table_t lamont_random[];//No diffrent pain sounds
extern voice_table_t nickiblanco[];
extern voice_table_t steeltown_moker[];
extern voice_table_t heilman[];
extern voice_table_t ty_tyrone[];
extern voice_table_t blunt[];//No diffrent pain sounds
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
    case NAME_POPEYE: tmpVoice = sy_popeye;	break;
//    case NAME_LAMONT: tmpVoice = lamont_random;	break;//Maybe not really a boss?
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
        if (level.waveEnemyCount_total == currWave_castMax)
            cast_TF_Shipyard_boss_popeye(spawn);//use or not not use?
        else
        {
            switch (rand() % 5)
            {
            case 0:	cast_TF_dog(spawn);break;
            case 1:	cast_TF_Skidrow_melee(spawn);break;
            case 2:	cast_TF_Skidrow_melee(spawn);break;
            case 3:	cast_TF_Skidrow_melee(spawn);break;
            case 4:	cast_TF_Skidrow_melee(spawn);break;
    /*        case 1:	cast_TF_Skidrow_names_melee(spawn);break;
            case 2:	cast_TF_Skidrow_names_melee(spawn);break;
            case 3:	cast_TF_Skidrow_names_melee(spawn);break;
            case 4:	cast_TF_Skidrow_names_melee(spawn);break;
            case 5:	cast_TF_Skidrow_names_melee(spawn);break;*/
            }
        }
    }
    else
    {
        if (level.waveEnemyCount_total == currWave_castMax)
            cast_TF_Shipyard_boss_popeye(spawn);
        else
        {
            switch (rand() % 5)
            {
            case 0:	cast_TF_dog(spawn);break;
            case 1:	cast_TF_Skidrow_skins(spawn);break;
            case 2:	cast_TF_Skidrow_skins(spawn);break;
            case 3:	cast_TF_Skidrow_skins(spawn);break;
            case 4:	cast_TF_Skidrow_skins(spawn);break;
     /*       case 1:	cast_TF_Skidrow_names(spawn);break;
            case 2:	cast_TF_Skidrow_names(spawn);break;
            case 3:	cast_TF_Skidrow_names(spawn);break;
            case 4:	cast_TF_Skidrow_names(spawn);break;
            case 5:	cast_TF_Skidrow_names(spawn);break;*/
            }
        }
    }
}
void cast_TF_spawnWave2(edict_t *spawn)//Skidrow
{
	if (level.waveEnemyCount_total == currWave_castMax)
		//cast_TF_Skidrow_boss(spawn);//spawn boss when almost done
		cast_TF_Skidrow_boss_jesus(spawn);
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
void cast_TF_spawnWave3(edict_t *spawn)//Poisonville
{
	switch (rand() % 6)
	{
	case 0:	cast_TF_dog(spawn);break;
	case 1:	cast_TF_rat(spawn);break;
    case 2:	cast_TF_Poisonville_skins1(spawn);break;
    case 3:	cast_TF_Poisonville_skins1(spawn);break;
    case 4:	cast_TF_Poisonville_skins1(spawn);break;
    case 5:	cast_TF_Poisonville_skins1(spawn);break;
/*	case 2:	cast_TF_Poisonville_names(spawn);break;
	case 3:	cast_TF_Poisonville_names(spawn);break;
	case 4:	cast_TF_Poisonville_names(spawn);break;
    case 5:	cast_TF_Poisonville_names(spawn);break;
	case 6:	cast_TF_Poisonville_names(spawn);break;*/
	}
}
void cast_TF_spawnWave4(edict_t *spawn)//Poisonville
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Poisonville_boss_nikkiblanco(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 6)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Poisonville_skins2(spawn);break;
        case 3:	cast_TF_Poisonville_skins2(spawn);break;
        case 4:	cast_TF_Poisonville_skins2(spawn);break;
        case 5:	cast_TF_Poisonville_skins2(spawn);break;
 /*       case 2:	cast_TF_Poisonville_tanks(spawn);break;//Nikki skins
        case 3:	cast_TF_Poisonville_tanks(spawn);break;//Nikki skins
        case 4:	cast_TF_Poisonville_boob(spawn);break;//Nikki skins
        case 5:	cast_TF_Poisonville_boob(spawn);break;//Nikki skins*/
//        case 6:	cast_TF_Poisonville_names(spawn);break;
        }
	}
}
void cast_TF_spawnWave5(edict_t *spawn)//Shipyard
{
	switch (rand() % 6)
	{
    case 0:	cast_TF_dog(spawn);break;
	case 1:	cast_TF_rat(spawn);break;
    case 2:	cast_TF_Shipyard_skins1(spawn);break;//heilman skins
    case 3:	cast_TF_Shipyard_skins1(spawn);break;//heilman skins
    case 4:	cast_TF_Shipyard_skins1(spawn);break;//heilman skins
    case 5:	cast_TF_Shipyard_skins1(spawn);break;//heilman skins
/*	case 2:	cast_TF_Shipyard_names(spawn);break;
	case 3:	cast_TF_Shipyard_names(spawn);break;
	case 4:	cast_TF_Shipyard_names(spawn);break;
    case 5:	cast_TF_Shipyard_names(spawn);break;
	case 6:	cast_TF_Shipyard_names(spawn);break;*/
	}
}
void cast_TF_spawnWave6(edict_t *spawn)//Shipyard
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Shipyard_boss_heilman(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 6)
        {
            case 0:	cast_TF_dog(spawn);break;
            case 1:	cast_TF_rat(spawn);break;
            case 2:	cast_TF_Shipyard_skins2(spawn);break;//heilman skins
            case 3:	cast_TF_Shipyard_skins2(spawn);break;//heilman skins
            case 4:	cast_TF_Shipyard_skins2(spawn);break;//heilman skins
            case 5:	cast_TF_Shipyard_skins2(spawn);break;//heilman skins
 /*           case 4:	cast_TF_Shipyard_deckmonkeys(spawn);break;//heilman skins
            case 5:	cast_TF_Shipyard_deckmonkeys(spawn);break;//heilman skins
            case 4:	cast_TF_Shipyard_goons(spawn);break; //heilman skins
            case 5:	cast_TF_Shipyard_goons(spawn);break; //heilman skins
            case 6:	cast_TF_Shipyard_swabs(spawn);break; //heilman skins
            case 7:	cast_TF_Shipyard_swabs(spawn);break; //heilman skins*/
        }
	}
}
void cast_TF_spawnWave7(edict_t *spawn)//Steeltown
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Steeltown_boss_moker(spawn);//spawn boss when almost done
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
void cast_TF_spawnWave8(edict_t *spawn)//Trainyard
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Trainyard_boss_tyrone(spawn);//spawn boss when almost done
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
void cast_TF_spawnWave9(edict_t *spawn)//Radiocity
{
	switch (rand() % 6)
	{
	case 0:	cast_TF_dog(spawn);break;
    case 1:	cast_TF_rat(spawn);break;
    case 2:	cast_TF_Radio_City_skins1(spawn);break;
    case 3:	cast_TF_Radio_City_skins1(spawn);break;
    case 4:	cast_TF_Radio_City_skins1(spawn);break;
    case 5:	cast_TF_Radio_City_skins1(spawn);break;
/*    case 2:	cast_TF_Radio_City_skins(spawn);break;
    case 3:	cast_TF_Radio_City_skins(spawn);break;
//	case 2:	cast_TF_Radio_City_dragon(spawn);break;
//    case 3:	cast_TF_Radio_City_dragonalley(spawn);break;
	case 4:	cast_TF_Radio_City_names(spawn);break;
	case 5:	cast_TF_Radio_City_names(spawn);break;
    case 6:	cast_TF_Radio_City_names(spawn);break;*/
	}
}
void cast_TF_spawnWave10(edict_t *spawn)//Radiocity
{
    if (level.waveEnemyCount_total == currWave_castMax)
		cast_TF_Radio_City_boss_nikkiblanco(spawn);//spawn boss when almost done
	else
	{
        switch (rand() % 7)
        {
        case 0:	cast_TF_dog(spawn);break;
        case 1:	cast_TF_rat(spawn);break;
        case 2:	cast_TF_Radio_City_skins2(spawn);break;
        case 3:	cast_TF_Radio_City_skins2(spawn);break;
        case 4:	cast_TF_Radio_City_skins2(spawn);break;
        case 5:	cast_TF_Radio_City_skins2(spawn);break;
/*        case 2:	cast_TF_Radio_City_dragon_street(spawn);break;
        case 3:	cast_TF_Radio_City_dragon_hoppers(spawn);break;
        case 4:	cast_TF_Radio_City_dragon_rooftop(spawn);break;
        case 5:	cast_TF_Radio_City_dragon_building(spawn);break;
        case 6:	cast_TF_Radio_City_dragon_street(spawn);break;*/
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
    {//		cast_TF_cast_melee(spawn);
        if (skill->value < 2)
            cast_TF_Steeltown_skins(spawn);
        else
            cast_TF_Skidrow_skins(spawn);
    }
	else if (boss_called_help == 2)
    {//		cast_TF_cast_pistol(spawn);
        if (skill->value < 2)
            cast_TF_Trainyard_skins(spawn);
        else
            cast_TF_Poisonville_skins2(spawn);
    }
	else if (boss_called_help == 3)
    {//		cast_TF_cast_shotgun(spawn);
        if (skill->value < 2)
            cast_TF_Radio_City_skins2(spawn);
        else
            cast_TF_Shipyard_skins2(spawn);
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
		case 0: cast_TF_spawnWave1(spawn);	break;//Skidrow
		case 1:	cast_TF_spawnWave2(spawn);	break;//Skidrow
		case 2: cast_TF_spawnWave4(spawn);	break;//Poisonville
		case 3: cast_TF_spawnWave5(spawn);	break;//Shipyard
		case 4: cast_TF_spawnWave6(spawn);	break;//Shipyard
		case 5: cast_TF_spawnWave8(spawn);	break;//Trainyard
		case 6: cast_TF_spawnWave10(spawn);	break;//Radiocity
		case 7: cast_TF_spawnWaveBoss(spawn); break; //wave 8
		}
	}
	else 	//WAVELEN_SHORT //short wave
	{
		switch (level.waveNum)
		{
		case 0: cast_TF_spawnWave2(spawn);	break; //Skidrow
		case 1: cast_TF_spawnWave4(spawn);	break; //Poisonville
		case 2: cast_TF_spawnWave7(spawn);	break; //Steeltown
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
		if (!spawnspot)	{
			//debug
			gi.dprintf("\nNo spawn points for cast.\n\n");
			G_FreeEdict(spawn);
			return;
		}
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

			//Sound
            gi.WriteByte(svc_stufftext);
            if (spawn->name_index == NAME_KINGPIN)
                gi.WriteString("play actors/male/kingpin/funny3.wav\n");
            else if (spawn->name_index == NAME_BLUNT)
                gi.WriteString("play actors/female/blunt/funny1.wav\n");
            else if (spawn->name_index == NAME_POPEYE)
                gi.WriteString("play actors/male/popeye/whistle.wav\n");
            else if (spawn->name_index == NAME_JESUS)
                gi.WriteString("play actors/male/jesus/funny1.wav\n");
            else if (spawn->name_index == NAME_LAMONT)
                gi.WriteString("play actors/male/lamont/fuckyou.wav\n");
            else if (spawn->name_index == NAME_NICKIBLANCO)
                gi.WriteString("play actors/male/nicki/funny6.wav\n");
            else if (spawn->name_index == NAME_HEILMAN)
                gi.WriteString("play actors/male/heilman/taunt1.wav\n");
            else if (spawn->name_index == NAME_MOKER)
                gi.WriteString("play actors/male/moker/fight4.wav\n");
            else if (spawn->name_index == NAME_TYRONE)
                gi.WriteString("play actors/male/tyrone/funny3.wav\n");
            else
                gi.WriteString("play world/alarm.wav\n");
            gi.multicast(vec3_origin, MULTICAST_ALL);
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

