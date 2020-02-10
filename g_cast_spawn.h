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

typedef struct
{
	char *name;
	char *skin;
	char *classname;
	char *spawnflags;
    char *health;
//    char *moral;
//    char *acc;
//    char *cal;
    char *count;
    char *head;
} localteam_skins_s;

//local
void cast_pawn_o_matic_spawn ();
void cast_pawn_o_matic_free();
int cast_TF_checkEnemyState(void);
void cast_TF_setupEnemyCounters(void);


//external kingpin functions
void ED_CallSpawn(edict_t *ent);
edict_t *cast_SelectRandomDeathmatchSpawnPoint(edict_t *ent);
