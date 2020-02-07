// g_cast_spawn.h

typedef struct
{
	char *name;
	char *skin;

} cast_skins_s;

//local
void cast_pawn_o_matic_spawn ();
void cast_pawn_o_matic_free();
void dog_spawn (int ammount);
void bitch_spawn (int ammount);
int cast_TF_checkEnemyState(void);
void cast_TF_spawn_allEnemy(void);


//external kingpin functions
void ED_CallSpawn(edict_t *ent);
edict_t *cast_SelectRandomDeathmatchSpawnPoint(edict_t *ent);
