// g_cast_spawn.h

typedef struct
{
	char *name;
	char *skin;
} cast_bitchskins_s;

typedef struct
{
	char *name;
	char *skin;
	char *count;
} cast_runtskins_s;

typedef struct
{
	char *name;
	char *skin;
	//has count options but not used
	char *head;
} cast_thugskins_s;

//local
void cast_pawn_o_matic_spawn ();
void cast_pawn_o_matic_free();
void dog_spawn (int ammount);
void bitch_spawn (int ammount);
int cast_TF_checkEnemyState(void);
void cast_TF_setupEnemyCounters(void);


//external kingpin functions
void ED_CallSpawn(edict_t *ent);
edict_t *cast_SelectRandomDeathmatchSpawnPoint(edict_t *ent);
