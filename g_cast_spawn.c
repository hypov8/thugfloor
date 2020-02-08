#include "g_local.h"
#include "g_cast_spawn.h"

//FREDZ todo Some character no name maybe check "localteam" in maps for skins setup.

//edict_t *spawn_cast[MAX_CHARACTERS];//64 max like MAX_CHARACTERS

#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT];// = {NULL, NULL, NULL}; //hypov8

void cast_TF_spawn(int ammount, int type);
static int currWave_castCount = 0;
static int currWave_plysCount = 0;
static int currWave_length = 0;


void cast_pawn_o_matic_free()
{
	int i;
	for (i = 0; i < BUYGUY_COUNT; i++)
	{
		if (pawnGuy[i] && pawnGuy[i]->inuse && !Q_stricmp(pawnGuy[i]->classname,"cast_pawn_o_matic")) //just incase
			G_FreeEdict(pawnGuy[i]);
	}
}

void cast_pawn_o_matic_spawn ()
{
	edict_t *spawn,*spawnspot;
	int count = 0, i;

    //spawnspot = SelectDeathmatchSpawnPoint(spawn); //hypov8 wil use spawn furtherest DF_SPAWN_FARTHEST if set

	//random?
	//while ((spawnspot = G_Find(spawnspot, FOFS(classname), "info_player_deathmatch")) != NULL)

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

		//check for telfrag??
		//ai code will move them

		spawn = G_Spawn();

		VectorCopy (spawnspot->s.angles, spawn->s.angles);
		VectorCopy( spawnspot->s.origin, spawn->s.origin );
		spawn->s.origin[2] += 1;

		spawn->classname = "cast_pawn_o_matic";

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

#if 0
void dog_spawn(int ammount)
{
	edict_t *spawn, *spawnspot;
	int count = 0, i;

	//	spawn = G_Spawn();
	spawnspot = NULL;

	//   spawnspot = SelectDeathmatchSpawnPoint(spawn); //Will use spawn furtherest DF_SPAWN_FARTHEST if set

	   //random?
	while ((spawnspot = G_Find(spawnspot, FOFS(classname), "info_player_deathmatch")) != NULL)

		//find first 3 dm spawns in entity list. pawnGuy will be at same 3 location every time, unless custom kpded2 itemlist
		for (i = 0; i < globals.num_edicts; i++)
		{
			spawnspot = g_edicts + i;
			if (!spawnspot->inuse)
				continue;
			if (!spawnspot->classname)
				continue;
			if (Q_stricmp(spawnspot->classname, "info_player_deathmatch"))
				continue;

			//check for telfrag??
			//ai code will move them

			spawn = G_Spawn();

			VectorCopy(spawnspot->s.angles, spawn->s.angles);
			VectorCopy(spawnspot->s.origin, spawn->s.origin);
			spawn->s.origin[2] += 1;

			spawn->classname = "cast_dog";

			gi.linkentity(spawn);

			if (level.num_characters == MAX_CHARACTERS)
			{
				gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
				G_FreeEdict(spawn);
				return;
			}

			ED_CallSpawn(spawn);

			//copy entity our list so we can free later
			spawn_cast[count] = spawn;

			count++;
			//no more required
			if (count == ammount)
				return;
		}
}

void rat_spawn(int ammount)
{
	edict_t *spawn, *spawnspot;
	int count = 0, i;

	//	spawn = G_Spawn();
	spawnspot = NULL;

	//   spawnspot = SelectDeathmatchSpawnPoint(spawn); //Will use spawn furtherest DF_SPAWN_FARTHEST if set

	   //random?
	while ((spawnspot = G_Find(spawnspot, FOFS(classname), "info_player_deathmatch")) != NULL)

		//find first 3 dm spawns in entity list. pawnGuy will be at same 3 location every time, unless custom kpded2 itemlist
		for (i = 0; i < globals.num_edicts; i++)
		{
			spawnspot = g_edicts + i;
			if (!spawnspot->inuse)
				continue;
			if (!spawnspot->classname)
				continue;
			if (Q_stricmp(spawnspot->classname, "info_player_deathmatch"))
				continue;

			//check for telfrag??
			//ai code will move them

			spawn = G_Spawn();

			VectorCopy(spawnspot->s.angles, spawn->s.angles);
			VectorCopy(spawnspot->s.origin, spawn->s.origin);
			spawn->s.origin[2] += 1;

			spawn->classname = "cast_dog";

			gi.linkentity(spawn);

			if (level.num_characters == MAX_CHARACTERS)
			{
				gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
				G_FreeEdict(spawn);
				return;
			}

			ED_CallSpawn(spawn);

			//copy entity our list so we can free later
			spawn_cast[count] = spawn;

			count++;
			//no more required
			if (count == ammount)
				return;
		}
}
#endif

extern void SP_cast_bitch (edict_t *self);
extern void SP_cast_whore (edict_t *self);
#if 0
void SP_cast_bitch_melee (edict_t *self)
{
    //Doesn't use count
	switch (rand() % 3)
	{
		case 0:
		    {
                self->name = "Betty";//sr1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "008 006 003");
                break;
		    }
		case 1:
		    {
                self->name = "Beth";//sr1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "009 007 004");
                break;
		    }
        case 2:
		    {
                self->name = "Lisa";//sr1 Got normally 120 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "012 015 012");
                break;
		    }

	}

	self->classname = "cast_bitch";

    self->spawnflags = 64;

	SP_cast_bitch(self);
}
void SP_cast_bitch_pistol (edict_t *self)
{
    edict_t	*other = NULL;
    //Doesn't use count
	switch (rand() % 6)
	{
		case 0:
		    {
                self->name = "Mona";//sr2
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "014 012 003");
                break;
		    }
		case 1:
		    {
                self->name = "yolanda";//pv_h got normally 200 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "041 050 003");
                break;
		    }
        case 2:
		    {
                self->name = "candy";//bar_pv same name as in bar_rc
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "015 017 013");
                break;
		    }
       case 3:
		    {
                self->name = "brittany";//steel1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "046 040 010");
                break;
		    }
        case 4:
		    {
                self->name = "bambi";//bar_st
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "044 042 003");
                break;
		    }
        case 5:
		    {
                self->name = "lola";//rc3
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "045 049 015");
                break;
		    }
        case 6:
		    {
                self->name = "candy";//bar_rc same name as in bar_pv
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "019 019 015");
                break;
		    }
	}

    self->spawnflags = 0;

    self->classname = "cast_bitch";


	SP_cast_bitch(self);


	/*
	if (other->client)//FREDZ not tested and maybe other self need to be switch
    {
        while (self = findradius(self, other->s.origin, 512))//FREDZ need fix
        {
            if (self->svflags & SVF_MONSTER)
            {
                AI_MakeEnemy(self, other, 0 );  // hostile
            }
        }
    }*/

}
#endif

/*
void SP_cast_bitch_shotgun (edict_t *self)
{
    //Doesn't use count
    self->name = "Rochelle";//bar_st
    self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "601 009 012");

	self->classname = "cast_bitch";

    self->spawnflags = 8192;
    //self->spawnflags = 0; is also shotgun

	SP_cast_whore(self);
}
void SP_cast_bitch_tommygun (edict_t *self)
{
    //Doesn't use count
    //No character names with this weapon

	self->classname = "cast_bitch";

    self->spawnflags = 64;

	SP_cast_whore(self);
}
void SP_cast_bitch_hmg (edict_t *self)
{
    //Doesn't use count
    //No character names with this weapon
    //Expect endboss on rc_boss2 named blunt

	self->classname = "cast_bitch";

    self->spawnflags = 16;

	SP_cast_whore(self);
}
void SP_cast_bitch_grenade (edict_t *self)
{
    //Doesn't use count
    //No character names with this weapon

	self->classname = "cast_bitch";

    self->spawnflags = 128;

	SP_cast_whore(self);
}
void SP_cast_bitch_bazooka (edict_t *self)
{
    //Doesn't use count
    //No character names with this weapon

	self->classname = "cast_bitch";

    self->spawnflags = 8;

	SP_cast_whore(self);
}
void SP_cast_bitch_flamethrower (edict_t *self)
{
    //Doesn't use count
    //No character names with this weapon

	self->classname = "cast_bitch";

    self->spawnflags = 4;

	SP_cast_whore(self);
}
*/


#if 0
void bitch_spawn(int ammount, int weapon)
{
	edict_t *spawn, *spawnspot;
	int count = 0, i;

	//	spawn = G_Spawn();
	spawnspot = NULL;

	//   spawnspot = SelectDeathmatchSpawnPoint(spawn); //Will use spawn furtherest DF_SPAWN_FARTHEST if set

	   //random?
	while ((spawnspot = G_Find(spawnspot, FOFS(classname), "info_player_deathmatch")) != NULL)

		//find first 3 dm spawns in entity list. pawnGuy will be at same 3 location every time, unless custom kpded2 itemlist
		for (i = 0; i < globals.num_edicts; i++)
		{
			spawnspot = g_edicts + i;
			if (!spawnspot->inuse)
				continue;
			if (!spawnspot->classname)
				continue;
			if (Q_stricmp(spawnspot->classname, "info_player_deathmatch"))
				continue;

			//check for telfrag??
			//ai code will move them

			spawn = G_Spawn();

			VectorCopy(spawnspot->s.angles, spawn->s.angles);
			VectorCopy(spawnspot->s.origin, spawn->s.origin);
			spawn->s.origin[2] += 1;

			if (weapon == 0)
				SP_cast_bitch_melee(spawn);
			else if (weapon == 1)
				SP_cast_bitch_pistol(spawn);
			else
				spawn->classname = "cast_bitch";

			gi.linkentity(spawn);

			if (level.num_characters == MAX_CHARACTERS)
			{
				gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
				G_FreeEdict(spawn);
				return;
			}

			ED_CallSpawn(spawn);

			//copy entity our list so we can free later
			spawn_cast[count] = spawn;

			count++;
			//no more required
			if (count == ammount)
				return;
		}
}
#endif

#if 0
extern void SP_cast_runt(edict_t *self);
extern void SP_cast_shorty (edict_t *self);
void SP_cast_runt_melee (edict_t *self)
{
	switch (rand() % 14)
	{
		case 0:
		    {
                self->name = "bubba";//sr1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "017 016 008");
                self->count = 9;
                break;
		    }

        case 2:
		    {
                self->name = "louie";//sr1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "011 011 005");
                self->count = 3;
                break;
		    }
        case 3:
		    {
                self->name = "buttmunch";//sr1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "001 001 001");//sewer_rats
                break;
		    }
        case 4:
		    {
                self->name = "magicj";//sr1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "020 011 005");
                break;
		    }
        case 5:
		    {
                self->name = "kroker";//steel1 Got normally 150 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "023 020 020");
                self->count = 1;
                break;
		    }
        case 6:
		    {
                self->name = "kid_1";//steel2
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "134 132 132");
                break;
		    }
       case 7:
		    {
                self->name = "kid_2";//steel2
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "132 132 132");
                break;
		    }
        case 8:
		    {
                self->name = "kid_3";//steel2
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "133 132 132");
                break;
		    }
        case 9:
		    {
                self->name = "popeye";//sy_h Got normaly health 300 and diffrent head?
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "040 019 048");
                break;
		    }
        case 10:
		    {
                self->name = "harpo"; //rc1 Got normally 200 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "142 140 140");
                break;
		    }
        case 11:
		    {
                self->name = "bubba"; //rc1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "042 042 010");
                self->count = 9;
                break;
		    }
        case 12:
		    {
                self->name = "groucho"; //rc1 Got normally 150 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "140 141 046");
                break;
		    }
        case 13:
		    {
                self->name = "chico"; //rc1 Got normally 150 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "141 141 046"); //same skin as grouncho
                break;
		    }
	}

	self->classname = "cast_runt";

    self->spawnflags = 64;

	SP_cast_runt(self);
}
void SP_cast_runt_pistol (edict_t *self)
{
    edict_t	*other = NULL;

	switch (rand() % 15)
	{
		case 0:
		    {
                self->name = "bernie";//sr1 Got normally 150 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "011 012 004");//courtyard
                self->count = 1;
                break;
		    }
        case 1:
		    {
                self->name = "punky";//sr1 Got normally 50 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "005 003 001");//sewer_rats
                break;
		    }
        case 2:
		    {
                self->name = "momo";//sr2
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "020 011 003");
                break;
		    }
        case 3:
		    {
                self->name = "jesus";//sr4 Got normally 400 health some kind of boss
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "021 017 010");
                break;
		    }
        case 4:
		    {
                self->name = "momo";//steel1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "013 045 006");
                break;
		    }
        case 5:
		    {
                self->name = "mathew";//steel1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "048 042 015");
                break;
		    }
        case 6:
		    {
                self->name = "luke";//ty1
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "049 046 046");
                break;
		    }
        case 7:
		    {
                self->name = "clarence";//bar_pv
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "047 013 003");
                self->count = 9;
                break;
		    }
        case 8:
		    {
                self->name = "buster";//bar_pv Got normally 120 health
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "046 010 009");
                self->count = 9;
                break;
		    }
        case 9:
		    {
                self->name = "louie";//bar_pv
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "043 041 009");
                break;
		    }
        case 10:
		    {
                self->name = "clarence";//bar_rc
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "045 024 005");
                self->count = 1;
                break;
		    }
        case 11:
		    {
                self->name = "sluggo";//bar_sr
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "019 010 011");
                break;
		    }
        case 12:
		    {
                self->name = "lenny";//bar_sr
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "018 011 007");
                break;
		    }
        case 13:
		    {
                self->name = "dogg";//bar_sy
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "016 014 003");
                break;
		    }
        case 14:
		    {
                self->name = "momo";//rc4
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "049 041 009");
                break;
		    }


	}

    self->spawnflags = 0;

    self->classname = "cast_runt";


	SP_cast_runt(self);
}
//FREDZ still need todo more other characters
#endif
#if 0
void runt_spawn (int ammount, int weapon)
{
	edict_t *spawn,*spawnspot;
	int count = 0, i;

//	spawn = G_Spawn();
	spawnspot = NULL;

 //   spawnspot = SelectDeathmatchSpawnPoint(spawn); //Will use spawn furtherest DF_SPAWN_FARTHEST if set

	//random?
	while ((spawnspot = G_Find(spawnspot, FOFS(classname), "info_player_deathmatch")) != NULL)

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

		//check for telfrag??
		//ai code will move them

		spawn = G_Spawn();

		VectorCopy (spawnspot->s.angles, spawn->s.angles);
		VectorCopy( spawnspot->s.origin, spawn->s.origin );
		spawn->s.origin[2] += 1;

		spawn->classname = "cast_runt";

		gi.linkentity (spawn);

		if (level.num_characters == MAX_CHARACTERS)
		{
			gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
			G_FreeEdict(spawn);
			return ;
		}

		ED_CallSpawn(spawn);

		//copy entity our list so we can free later
		spawn_cast[count] = spawn;

		count++;
		//no more required
		if (count == ammount)
			return;
	}
}
#endif
/*
void thug_spawn (int ammount, int weapon)
{
	edict_t *spawn,*spawnspot;
	int count = 0, i;

//	spawn = G_Spawn();
	spawnspot = NULL;

 //   spawnspot = SelectDeathmatchSpawnPoint(spawn); //Will use spawn furtherest DF_SPAWN_FARTHEST if set

	//random?
	while ((spawnspot = G_Find(spawnspot, FOFS(classname), "info_player_deathmatch")) != NULL)

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

		//check for telfrag??
		//ai code will move them

		spawn = G_Spawn();

		VectorCopy (spawnspot->s.angles, spawn->s.angles);
		VectorCopy( spawnspot->s.origin, spawn->s.origin );
		spawn->s.origin[2] += 1;

		spawn->classname = "cast_thug";

		gi.linkentity (spawn);

		if (level.num_characters == MAX_CHARACTERS)
		{
			gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
			G_FreeEdict(spawn);
			return ;
		}

		ED_CallSpawn(spawn);

		//copy entity our list so we can free later
		spawn_cast[count] = spawn;

		count++;
		//no more required
		if (count == ammount)
			return;
	}
}*/

//hypov8 most of the above can be deleted


///////////////////////
// setup cast skins
void cast_TF_boss_kingpin(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "kingpin");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "120 120 120");
	self->spawnflags = 0;//pistol
	self->classname = "cast_runt";
	self->health = 1500 * currWave_plysCount;

}
void cast_TF_boss_blunt(edict_t *self)
{
	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), "blunt");
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "080 059 059");
	self->spawnflags = 18;//hmg
	self->classname = "cast_whore";
	self->health = 1500 * currWave_plysCount;//150000 normally
}
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
	static cast_bitchskins_s skins[7] = {
		"Mona",			"014 012 003",	//sr2
		"yolanda",		"041 050 003",	//pv_h got normally 200 health
		"candy",		"015 017 013",	//bar_pv same name as in bar_rc
		"brittany",		"046 040 010",	//steel1
		"bambi",		"044 042 003",	//bar_st
		"lola",			"045 049 015", 	//rc3
		"candy",		"019 019 015"	//bar_rc same name as in bar_pv
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 7].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 7].skin);
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
	"bubba",  "017 016 008", "9",	    //sr1
	"louie", "011 011 005",	"3",	    //sr1
	"buttmunch", "001 001 001", "0",	//sr1
	"magicj", "020 011 005", "0",   	//sr1
	"kroker", "023 020 020", "1",	    //steel1 Got normally 150 health
	"kid_1", "134 132 132",	"0",	    //steel2
	"kid_2", "132 132 132",	"0",	    //steel2
	"kid_3", "133 132 132",	"0",	    //steel2
	"popeye", "040 019 048", "0",	    //sy_h Got normaly health 300 and diffrent head?
	"harpo", "142 140 140",	"0",	     //rc1 Got normally 200 health
	"bubba", "042 042 010",	"9",	    //rc1
	"groucho", "140 141 046", "0",	     //rc1 Got normally 150 health
	"chico", "141 141 046", "0"		     //rc1 Got normally 150 health //same skin as grouncho
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 13].skin);
	self->count = (int)skins[rand() % 13].count;
	self->classname = "cast_runt";
	self->spawnflags = 64;
}

void cast_TF_runt_pistol(edict_t *self)
{
	static cast_runtskins_s skins[15] = {
	"bernie", "011 012 004", "1",	//sr1 Got normally 150 health
	"punky", "005 003 001",	"0",	//sr1 Got normally 50 health//sewer_rats

    "momo", "020 011 003", "0",		//sr2 //bouncer
    "momo", "013 045 006", "0",		//steel1 //bouncer
    "momo", "049 041 009", "0",		//rc4 //bouncer

	"jesus", "021 017 010", "0",	//sr4 Got normally 400 health some kind of boss
	"mathew", "048 042 015", "0",	//steel1
	"luke", "049 046 046", "0",		//ty1
	"clarence", "047 013 003", "9",	//bar_pv
	"buster", "046 010 009", "9",	//bar_pv Got normally 120 health
	"louie", "043 041 009", "0",	//bar_pv
	"clarence", "045 024 005", "1",	//bar_rc
	"sluggo", "019 010 011", "0",	//bar_sr
	"lenny", "018 011 007",	"0",	//bar_sr
	"dogg", "016 014 003",	"0" 	//bar_sy
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 15].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 15].skin);
	self->count = (int)skins[rand() % 15].count;
	self->spawnflags = 0;
	self->classname = "cast_runt";
}
void cast_TF_runt_shotgun(edict_t *self)
{
	static cast_runtskins_s skins[4] = {
	"momo", "072 014 009", "0", 	//sr1 Got normally 150 health
    "butch", "131 130 010", "0",	//rc1
    "dude", "130 131 010", "9",     //rc3 Got normally 200 health
    "heilman", "121 121 121", "0"	//rc1 Got normally 250 health
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
	static cast_runtskins_s skins[3] = {
        "sal", "064 047 047", "0",	    //bar_sy Got normally 250 health
        "mung", "013 010 006", "9",	    //pv_h
        "bubba", "130 131 010", "9"    //rc2
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 3].skin);
	self->count = (int)skins[rand() % 3].count;
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
		"leroy",	"010 010 003",	"0",    //sr1
		"johnny",	"011 007 004",	"0",	//sr1 Got normally 80 health
		"brewster", "002 001 001",	"0",	//sr1
//		"IntroGuy", "041 026 010",	"0",	//sr1 player
        "kid_4",	"133 132 132",	"0",	//steel3
        "kid_5",	"134 132 132",	"0",	//steel3
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 5].skin);
	self->spawnflags = 64;
	self->classname = "cast_thug";
}

void cast_TF_thug_pistol(edict_t *self)
{
	static cast_thugskins_s skins[9] = {
		"fingers",	"025 009 009",	"0",	//bar_pv Got normally 150 health
		"burt",	    "063 041 010",	"0",	//bar_pv
		"adolf",	"047 042 009",	"0",	//bar_rc
        "burt",	    "023 017 005",	"0",	//bar_rc
        "scalper",	"103 044 009",	"0",	//bar_rc
        "rocko",	"016 009 006",	"0",	//bar_sr
        "igmo",	    "003 002 001",	"0",	//sr1 Got normally 200 health
        "lamont",	"024 017 010",	"0",	//sr2 Got normally 300 health
        "hann",	    "029 006 010",	"0",	//ty1
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 9].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 9].skin);
	self->spawnflags = 0;
	self->classname = "cast_thug";
}
//FREDZ still need todo more other characters
void cast_TF_thug_shotgun(edict_t *self)
{
	static cast_thugskins_s skins[7] = {
		"bigwillie",	"109 046 047", "0",	//bar_sy Got normally 350 health
		"Laurel",		"504 032 031", "1",	//pv_h Got normally 200 health
		"Hardey",		"114 032 031", "0",	//pv_h Got normally 200 health
	//	"ToughGuy1",	"019 028 010", "0",	//sr1 intro guy
	//	"ToughGuy2",	"020 029 010", "0",	//sr1 intro guy
		"arnold",		"012 007 004", "0",	//sr1 Got normally 200 health
		"momo",			"107 044 010", "0",	//sy_h
		"larry",		"048 047 041", "0",	//sy_h Got normally 200 health
		"curcly",		"100 047 041", "0"	//sy_h Got normally 200 health
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
// end setup cast skins
///////////////////////


//check if enemy has died or kill them for endWave/endGame
int cast_TF_checkEnemyState()
{
	int i, count = 0;

	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (level.characters[i] && level.characters[i]->inuse && !level.characters[i]->client)
		{
			if (level.modeset != WAVE_ACTIVE|| level.characters[i]->health <= 0) //timelimit reached?
			{
				// free cast array //hypov8 todo: this is wrong
				if (level.characters[i]->character_index)
				{
					AI_UnloadCastMemory(level.characters[i]);
					G_FreeEdict(level.characters[i]);
					currWave_castCount--;
					level.waveEnemyCount--;
				}
			}
			else if (level.characters[i]->health > 0)
				count++;
		}
	}

	//free slot. add enemy
	if (level.modeset == WAVE_ACTIVE && count < level.waveEnemyCount && count <  level.waveEnemyCount_sym)
		cast_TF_spawn(0,0);

	if (level.modeset != WAVE_ACTIVE)
		currWave_castCount = 0; //reset spawned cast count

	return count;
}

// find the closest player to pick on
void cast_TF_setEnemyPlayer(edict_t *spawn)
{
	int j, best = 1;
	float v, dist = 99999;
	edict_t *player;

	//attack closest player. todo rand?? depends on player count?
	for_each_player(player, j)
	{
		if (player->health <= 0 || player->solid == SOLID_NOT) // MH: exclude spectators
			continue;
		v = VectorDistance(spawn->s.origin, player->s.origin);
		if (v < dist) {
			dist = v;
			best = j;
		}
	}

	AI_MakeEnemy(spawn, &g_edicts[best], 0);  // hostile to closest enemy
	//hypov8 todo: what if enemy is dead??
}

//////////////////
// rand spawn types
void cast_TF_spawnWave1(edict_t *spawn)
{
	switch (rand() % 6)
	{
	case 0:	cast_TF_dog(spawn);break;	
	case 1:	cast_TF_dog(spawn);break;
	case 2:	cast_TF_rat(spawn);break;
	case 3:	cast_TF_rat(spawn);break;
	case 4:	cast_TF_bitch_melee(spawn);break;
	case 5:	cast_TF_thug_melee(spawn);break;
	}
}
void cast_TF_spawnWave2(edict_t *spawn)
{
	switch (rand() % 5)
	{
	case 0:		cast_TF_dog(spawn);break;
	case 1:		cast_TF_dog(spawn);break;
	case 2:		cast_TF_bitch_melee(spawn);break;
	case 3:		cast_TF_bitch_pistol(spawn);break;
	case 4:		cast_TF_runt_pistol(spawn);break;
	}
}
void cast_TF_spawnWave3(edict_t *spawn)
{
	//todo.. fredz!!!!!!
}
void cast_TF_spawnWave4(edict_t *spawn)
{
	//todo
}
void cast_TF_spawnWave5(edict_t *spawn)
{
	//todo
}
void cast_TF_spawnWave6(edict_t *spawn)
{
	//todo
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
	switch (rand() % 2)
	{
	case 0:		cast_TF_boss_kingpin(spawn);break;
	case 1:		cast_TF_boss_blunt(spawn);break;
	}
}
//end rand spawn types
//////////////////////


//generate types based on game length
void cast_TF_spawnTypes(edict_t *spawn)
{
	//todo:  short. med


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
		case 2: cast_TF_spawnWave3(spawn);	break; //wave 3
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
		case 0: cast_TF_spawnWave1(spawn);	break; //wave 1
		case 1: cast_TF_spawnWave3(spawn);	break; //wave 2
		case 2: cast_TF_spawnWave5(spawn);	break; //wave 3
		case 3: cast_TF_spawnWave8(spawn);	break; //wave 4
		case 4: cast_TF_spawnWaveBoss(spawn); break; //wave 5
		}
	}
}

//spawn in a cast model
void cast_TF_spawn(int ammount, int type)
{
	edict_t *spawn, *spawnspot;
	//int i;

	//for (i = 0; i < ammount; i++)
	{
		if (currWave_castCount >= MAX_CHARACTERS)
			return;

		spawn = G_Spawn();
		spawnspot = cast_SelectRandomDeathmatchSpawnPoint(spawn);
		VectorCopy(spawnspot->s.angles, spawn->s.angles);
		VectorCopy(spawnspot->s.origin, spawn->s.origin);
		spawn->s.origin[2] += 1;

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
		if (!spawn->health)
			spawn->health = 100;

		//set what player to attack
		cast_TF_setEnemyPlayer(spawn);

		//add enemy to counter
		currWave_castCount++;
	}
}

//wave total enemy counts
static int wave_shortGame[5] = { 25, 32, 35, 42, 1 };
static int wave_medGame[8] = { 25, 28, 32, 25, 35, 40, 42, 1 };
static int wave_longGame[11] = { 25, 28, 32, 32, 35, 35, 35, 40, 42, 42, 1 };

//wave skill. number of enemy allowed in level at 1 time. max 8 players
static int wave_skill[5][9] = {
	{ 0, 10, 14, 32, 32, 32, 32, 32, 32}, //easy
	{ 0, 11, 18, 32, 32, 32, 32, 32, 32}, //med
	{ 0, 12, 18, 32, 32, 32, 32, 32, 32 }, //hard
	{ 0, 12, 18, 32, 32, 32, 32, 32, 32 }, //harder
	{ 0, 12, 18, 32, 32, 32, 32, 32, 32 } //harderest
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
	int playerCount = 0;
	int i, sk;


	if (skill->value == 0)			sk = 0;
	else if (skill->value == 1)		sk = 1;
	else if (skill->value == 2)		sk = 2;
	else if (skill->value == 3)		sk = 3;
	else							sk = 4;

	//count players that will enter the wave
	for_each_player(self, i)
	{
		if (self->client->pers.spectator != SPECTATING)
			playerCount++;
	}
	currWave_plysCount = playerCount;
	level.waveEnemyCount_sym = wave_skill[sk][playerCount]; //skill based enemy limits

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

