// g_cast_spawn.h

typedef struct
{
	char *name;
	char *skin;
	char *classname;
	int spawnflags;
    int health;
//    int moral;
//    int acc;
//    int cal;
    int count;
    char head;
} localteam_skins_s;//Need to be same as edict_s in g_local.h

//local
void cast_pawn_o_matic_spawn ();
void cast_pawn_o_matic_free();
int cast_TF_checkEnemyState(void);
void cast_TF_setupEnemyCounters(void);


//external kingpin functions
void ED_CallSpawn(edict_t *ent);
edict_t *cast_SelectRandomDeathmatchSpawnPoint(edict_t *ent);
