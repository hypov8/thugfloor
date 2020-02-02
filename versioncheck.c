#include "g_local.h"

#define CLIENTVER "1.00b" // MH: required client version

// mod version check
void set_version(edict_t *ent)
{
	gi.WriteByte(13);
	gi.WriteString(va("exec thugfloor; wait; %s $thugfloor_ver\n", ver_check)); // MH: request version in same message
	gi.unicast(ent, true);
	// MH: kick after 5 seconds if there is no reply
	KICKENT(ent,"%s was kicked for apparently having no client-side files.\n");
	ent->kickdelay = 50;
}

// MH: read_version removed (not needed any more)

void Cmd_VersionCheck_f (edict_t *ent)
{
	char *ver = gi.argv(1); // MH: added
	if (*ver) // MH: changed
	{   // got client info
		if (strcmp(ver, CLIENTVER))
		{
			char buf[200];
			cvar_t	*game;

			game = gi.cvar("game", "", 0);

            if (kpded2)//FREDZ
                sprintf(buf,"error \"You have old client files. Delete the PAK1.PAK file from your %s folder and then reconnect to get the correct version.\"\n", game->string);//Easy to long
            else
                sprintf(buf,"error \"You have old " GAMEVERSION " (v%s) client files. Get the v" CLIENTVER " client files at: www.kingpin.info\"\n", ver);
			gi.WriteByte(13);
			gi.WriteString(buf);
			gi.unicast(ent, true);

			KICKENT(ent,"%s was kicked for having old client-side files.\n");
		}
		else
			ent->kickdelay = 0; // MH: cancel the set_version kick
	}
	else
	{ // no client info
		char buf[128];

		sprintf(buf,"error \"You need the version" CLIENTVER " client files to play on this server for " GAMEVERSION ". Get them at: www.kingpin.info\"\n" );
		gi.WriteByte(13);
		gi.WriteString(buf);
		gi.unicast(ent, true);

		KICKENT(ent,"%s was kicked for having no client-side files.\n");
	}
}
// add additional Cmd_ functions here.
qboolean ModCommands (edict_t *ent)
{
	if (!Q_stricmp (gi.argv(0),ver_check))
	{
		Cmd_VersionCheck_f (ent);
		return true;
	}
	// add additional binds here

	else return false;
}

