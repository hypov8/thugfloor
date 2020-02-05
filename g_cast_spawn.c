#include "g_local.h"

edict_t *SelectDeathmatchSpawnPoint (edict_t *ent);
void ED_CallSpawn (edict_t *ent);

void cast_pawn_o_matic_spawn (edict_t *ent)
{
	edict_t *spawn, *spawnspot;

	spawn = G_Spawn();

	spawnspot = NULL;
    spawnspot = SelectDeathmatchSpawnPoint(ent);

    VectorCopy (spawnspot->s.angles, spawn->s.angles);
    VectorCopy( spawnspot->s.origin, spawn->s.origin );
    spawn->s.origin[2] += 1;

	spawn->classname = "cast_pawn_o_matic";

	gi.linkentity (spawn);

	if (level.num_characters == MAX_CHARACTERS)
	{
		gi.dprintf("\nMAX_CHARACTERS exceeded!!!!!.\n\n");
		G_FreeEdict(spawn);
		return;
	}

	ED_CallSpawn(spawn);
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
