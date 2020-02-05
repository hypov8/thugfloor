#include "g_local.h"

edict_t *SelectDeathmatchSpawnPoint (edict_t *ent);
void ED_CallSpawn (edict_t *ent);

#define BUYGUY_COUNT 3 //3 pawnOmatic guys?
edict_t	*pawnGuy[BUYGUY_COUNT] = {NULL, NULL, NULL}; //hypov8

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
void dog_spawn (edict_t *ent)
{
	edict_t *spawn, *spawnspot;

	spawn = G_Spawn();

	spawnspot = NULL;
    spawnspot = SelectDeathmatchSpawnPoint(ent);

    VectorCopy (spawnspot->s.angles, spawn->s.angles);
    VectorCopy( spawnspot->s.origin, spawn->s.origin );
    spawn->s.origin[2] += 1;

	spawn->classname = "cast_dog";

	gi.linkentity (spawn);

	if (level.num_characters == MAX_CHARACTERS)
	{
		gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
		G_FreeEdict(spawn);
		return;
	}

	ED_CallSpawn(spawn);
}
