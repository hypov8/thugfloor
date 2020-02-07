#include "g_local.h"

edict_t *SelectDeathmatchSpawnPoint (edict_t *ent);
void ED_CallSpawn (edict_t *ent);

//FREDZ todo Some character no name maybe check "localteam" in maps for skins setup.

edict_t *spawn_cast[100];//64 max like MAX_CHARACTERS

#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT];// = {NULL, NULL, NULL}; //hypov8

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
void dog_spawn (int ammount)
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

		spawn->classname = "cast_dog";

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
void rat_spawn (int ammount)
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

		spawn->classname = "cast_dog";

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
void bitch_spawn (int ammount, int weapon)
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

		if (weapon==0)
            SP_cast_bitch_melee (spawn);
        else if (weapon==1)
            SP_cast_bitch_pistol (spawn);
        else
            spawn->classname = "cast_bitch";

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
extern void SP_cast_runt (edict_t *self);
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

