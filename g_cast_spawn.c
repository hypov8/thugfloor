#include "g_local.h"

edict_t *SelectDeathmatchSpawnPoint (edict_t *ent);
void ED_CallSpawn (edict_t *ent);

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
void SP_cast_bitch_melee( edict_t *self )
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

    if (!self->health)
		self->health = 100;

    self->spawnflags = 64;

	SP_cast_bitch(self);
}
void SP_cast_bitch_pistol( edict_t *self )
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
                self->name = "candy";//bar_rc same name as in bar_pv
                self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "019 019 015");
                break;
		    }
	}

    self->spawnflags = 0;

    self->classname = "cast_bitch";

    if (!self->health)
		self->health = 100;

	SP_cast_bitch(self);

	if (self->client)//FREDZ not tested and maybe other self need to be switch
    {
 //	while (self = findradius(self, other->s.origin, 512))
    while (other = findradius(other, self->s.origin, 512))//FREDZ need fix should not attack own folks
    {
        if (other->svflags & SVF_MONSTER)
        {
            AI_MakeEnemy(other, self, 0 );  // hostile
        }
    }
    }

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
 //           spawn->classname = "cast_bitch_melee";
        else if (weapon==1)
            SP_cast_bitch_pistol (spawn);
 //           spawn->classname = "cast_bitch_pistol";
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
/*//FREDZ still need better fix
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
}
*/
