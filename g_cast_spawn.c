#include "g_local.h"
#include "g_cast_spawn.h"

//FREDZ todo Some character no name maybe check "localteam" in maps for skins setup.

edict_t *spawn_cast[MAX_CHARACTERS];//64 max like MAX_CHARACTERS

#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT];// = {NULL, NULL, NULL}; //hypov8

static int cast_count = 0;


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

#if 0
extern void SP_cast_bitch (edict_t *self);
extern void SP_cast_whore (edict_t *self);
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


void cast_TF_dog(edict_t *self)
{
	self->classname = "cast_dog";
}

void cast_TF_bitch_malee(edict_t *self)
{
	static cast_skins_s bitchMelee_Skins[6] = {
		"Betty", "008 006 003",	//sr1
		"Beth", "009 007 004",	//sr1
		"Lisa", "012 015 012"	//sr1 Got normally 120 health
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), bitchMelee_Skins[rand() % 3].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), bitchMelee_Skins[rand() % 3].skin);
	self->spawnflags = 64;
	self->classname = "cast_bitch";
}

void cast_TF_bitch_pistol(edict_t *self)
{
	static cast_skins_s bitchPistol_Skins[7] = {
		"Mona",			"014 012 003",	//sr2
		"yolanda",		"041 050 003",	//pv_h got normally 200 health
		"candy",		"015 017 013",	//bar_pv same name as in bar_rc
		"brittany",		"046 040 010",	//steel1
		"bambi",		"044 042 003",	//bar_st
		"lola",			"045 049 015", 	//rc3
		"candy",		"019 019 015"	//bar_rc same name as in bar_pv
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), bitchPistol_Skins[rand() % 7].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), bitchPistol_Skins[rand() % 7].skin);
	self->classname = "cast_bitch";

}

void cast_TF_runt_melee(edict_t *self)
{
	static cast_skins_s skins[13] = {
	"bubba",  "017 016 008",	//sr1
	"louie", "011 011 005",		//sr1
	"buttmunch", "001 001 001",	//sr1
	"magicj", "020 011 005",	//sr1
	"kroker", "023 020 020",	//steel1 Got normally 150 health
	"kid_1", "134 132 132",		//steel2
	"kid_2", "132 132 132",		//steel2
	"kid_3", "133 132 132",		//steel2
	"popeye", "040 019 048",	//sy_h Got normaly health 300 and diffrent head?
	"harpo", "142 140 140",		 //rc1 Got normally 200 health
	"bubba", "042 042 010",		 //rc1
	"groucho", "140 141 046",	 //rc1 Got normally 150 health
	"chico", "141 141 046"		 //rc1 Got normally 150 health //same skin as grouncho
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 14].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 14].skin);
	self->classname = "cast_runt";
	self->spawnflags = 64;
}

void cast_TF_runt_pistol(edict_t *self)
{
	edict_t	*other = NULL;

	static cast_skins_s skins[15] = {
	"bernie", "011 012 004",	//sr1 Got normally 150 health
	"punky", "005 003 001",		//sr1 Got normally 50 health//sewer_rats
	 "momo", "020 011 003",		//sr2
	"jesus", "021 017 010",		//sr4 Got normally 400 health some kind of boss
	 "momo", "013 045 006",		//steel1
	"mathew", "048 042 015",		//steel1
	"luke", "049 046 046",		//ty1
	"clarence", "047 013 003",	//bar_pv
	"buster", "046 010 009",	//bar_pv Got normally 120 health
	"louie", "043 041 009",		//bar_pv
	"clarence", "045 024 005",	//bar_rc
	"sluggo", "019 010 011",	//bar_sr
	"lenny", "018 011 007",		//bar_sr
	"dogg", "016 014 003",		//bar_sy
	"momo", "049 041 009"		//rc4
	};

	self->name = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 14].name);
	self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), skins[rand() % 14].skin);
	self->spawnflags = 0;
	self->classname = "cast_runt";
}
//FREDZ still need todo more other characters



//check if all enemy have died
int cast_TF_checkEnemyState()
{
	int i, count = 0; //hypov8 todo: clean this up

	for (i = 0; i < cast_count; i++)
	{
		if (spawn_cast[i] && spawn_cast[i]->inuse)
		{
			if (level.modeset != WAVE_ACTIVE)
				G_FreeEdict(spawn_cast[i]);
			else if (spawn_cast[i]->health > 0)
				count++;
		}
	}

	level.waveEnemyCount = count;

	if (level.modeset != WAVE_ACTIVE)
		cast_count = 0; //reset spawned cast count

	return count;
}


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


void cast_TF_spawn(int ammount, int type)
{
	edict_t *spawn, *spawnspot;
	int i;

	for (i = 0; i < ammount; i++)
	{
		if (cast_count >= MAX_CHARACTERS)
			return;

		spawn = G_Spawn();
		spawnspot = cast_SelectRandomDeathmatchSpawnPoint(spawn);
		VectorCopy(spawnspot->s.angles, spawn->s.angles);
		VectorCopy(spawnspot->s.origin, spawn->s.origin);
		spawn->s.origin[2] += 1;

		switch (type)
		{
		default:
		case 1:
			cast_TF_dog(spawn);break;
		case 2:
			cast_TF_bitch_malee(spawn);break;
		case 3:
			cast_TF_bitch_pistol(spawn);break;
		case 4:
			cast_TF_runt_pistol(spawn);break;

		}

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

		//set player to attack
		cast_TF_setEnemyPlayer(spawn);

		//copy entity our list so we can free later
		spawn_cast[cast_count] = spawn;
		cast_count++;
	}
}

/*
==========
cast_TF_spawn_allEnemy
this is where we generate the eneny spawn counts
hypov8 note; this may entity overflow clients
==========
*/
void cast_TF_spawn_allEnemy(void)
{
	int playerCount = 0;
	int numDog = level.waveNum + 8;		//max 11+8=19
	int numBitch = level.waveNum + 2;	//max 11+2=13
	//int numThug = level.waveNum + 2;	//max 11+2=13
	//int numRat = level.waveNum + 2;	//max 11+2=13
	//hypov8 todo: player count?

	//hypov8 todo: rand
	cast_TF_spawn(numDog, 1); //1 = dog
	cast_TF_spawn(numBitch, 2); //2 = bitch melee
	cast_TF_spawn(numBitch, 3); //3 = bitch pistol
	cast_TF_spawn(numBitch, 4); //4 = runt pistol 

}

