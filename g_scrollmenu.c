#include "g_local.h"

//FREDZ Scroll Menu Source Code
extern void AutoLoadWeapon(gclient_t *client, gitem_t *weapon, gitem_t *ammo);
extern void SP_cast_runt(edict_t *self);
//extern void SP_coil_Skidrow(edict_t *self);
extern voice_table_t pawnomatic_specific[];
extern voice_table_t pawnomatic_funny[];

#define MAX_MENU_ITEMS	9 //+1.. maintain null at last index

#define MENU_LEFT_SIDE	0
#define MENU_RIGHT_SIDE	1

struct MENU_HEADER_NAME
{
	char *name[MAX_MENU_ITEMS];
} menu_header =

{
	{	"Weapons",
		"Mods",
		"Ammo",
		"Health",
		"Armor",
		"Packs"
	}
};

struct MENU_ITEM_NAMES
{
	char	*name[MAX_MENU_ITEMS];
	int		price[MAX_MENU_ITEMS];
} menu_item_names[MAX_MENU_ITEMS] =
{
	{ //Weapons
		{/*"Pistol", */"Crowbar", "Shotgun", "Tommygun", "Heavy machinegun", "Grenade Launcher", "Bazooka", "FlameThrower"}, 
		{/*50, */100, 150, 250, 750, 450, 500, 550} 
	},	
	{	//Mod's
		{"Pistol Silencer", "Pistol RoF Mod", "Pistol Magnum Mod", "Pistol Reload Mod","HMG Cooling Mod",
		"Hyper ShotGun"}, 
		{10, 25, 50, 25, 100, 250} 
	},	
    { //Ammo
		{"Bullets", "Shells", "308cal", "Grenades", "Rockets", "Gas"}, 
		{5, 25, 30, 50, 50, 50} 
	},	
	{ //Health
		{"Small Health", "Large Health"}, 
		{25, 50} 
	},
	{ //Armor
		{"Helmet Armor", "Jacket Armor", "Legs Armor", "Helmet Armor Heavy", "Jacket Armor Heavy", "Legs Armor Heavy"},
		{50, 75, 50, 100, 150, 100} 
	}, 
	{	//back pack
		{"Ammo Pack Small", "Ammo Pack Medium", "Ammo Pack Large"}, 
		{100, 150, 200} 
	},

};

//FREDZ For scroll menu init
/*
=================
Cmd_InitMenu_f
=================
*/
void Cmd_InitMenu_f (edict_t *ent)
{
	gclient_t	*cl;

	cl = ent->client;

	cl->showscores = NO_SCOREBOARD;
	cl->showhelp = false;
	cl->showinventory = false;

	if (cl->showscrollmenu)
	{
		cl->showscrollmenu = false;
		if (ent->pawnGuyID)
			Voice_Specific(ent->pawnGuyID, ent, pawnomatic_specific, 6+(rand()%2));
		//PawnSay(pawnomatic_specific, 6+(rand()%2));
		return;
	}
	else
	{
		if (ent->pawnGuyID)
		{
            if (ent->health < 5)
                Voice_Specific(ent->pawnGuyID, ent, pawnomatic_funny, 3);
            else if (ent->health < 25)
                Voice_Specific(ent->pawnGuyID, ent, pawnomatic_funny, 1);
            else if (ent->health < 50)
                Voice_Specific(ent->pawnGuyID, ent, pawnomatic_funny, 0);
            else if (ent->health < 75)
                Voice_Specific(ent->pawnGuyID, ent, pawnomatic_funny, 2);
            else
                Voice_Specific(ent->pawnGuyID, ent, pawnomatic_funny, 4);
			//Voice_Specific(ent->pawnGuyID, ent, pawnomatic_funny, (rand()%5));
		}

		cl->showscrollmenu = true;
	}

	ScrollMenuMessage (ent);
}

//hypov8 note: all players can hear him talk...
static void ScrollMenuSound_Sold(edict_t *ent)
{
	switch (rand()%3)
	{
	case 0:
		gi.centerprintf (ent, "It's all yours\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/sold1.wav"), 1, ATTN_STATIC, 0);
		break;
	case 1:
		gi.centerprintf (ent, "Here ya go\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/sold2.wav"), 1, ATTN_STATIC, 0);
		break;
	case 2:
		gi.centerprintf (ent, "Sold\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/sold3.wav"), 1, ATTN_STATIC, 0);
		break;
	}
}

static void ScrollMenuSound_Have(edict_t *ent)
{
	if (rand() % 2 == 0)
	{
		gi.centerprintf(ent, "you already got that\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/alreadyhave1.wav"), 1, ATTN_NORM, 0);
	}
	else
	{
		gi.centerprintf(ent, "hey, you don't need two... you already got one\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/alreadyhave2.wav"), 1, ATTN_NORM, 0);
	}
}

static void ScrollMenuSound_NoStock(edict_t *ent)
{
	if (rand() % 2 == 0)
	{
		gi.centerprintf(ent, "I'm outta that");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/stock_out1.wav"), 1, ATTN_NORM, 0);
	}
	else
	{
		gi.centerprintf(ent, "sorry, we ain't got any");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/stock_out2.wav"), 1, ATTN_NORM, 0);
	}
}

static void ScrollMenuSound_Full(edict_t *ent, int useSpecific)
{
	int i = useSpecific-1;

	if (!useSpecific)
		i = (rand() % 4);

	switch (i)
	{
	case 0: //full
		gi.centerprintf(ent, "you're all full up...\nyou can't hold any more\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/fullup1.wav"), 1, ATTN_NORM, 0);
		break;

	case 1://health
		gi.centerprintf (ent, "you don't need any more health...\nyou're full up\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/fullup2.wav"), 1, ATTN_NORM, 0);
		break;

	case 2: //armor
		gi.centerprintf(ent, "hey you don't need any more...\nyou're all patched up\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/fullup3.wav"), 1, ATTN_NORM, 0);
		break;

	case 3: //health
		gi.centerprintf (ent, "hey you're fine..\nyou don't need any more health\n");
		gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/fullup4.wav"), 1, ATTN_NORM, 0);
		break;

	}
}

static void ScrollMenuSound_Maxed(edict_t *ent)
{
	switch (rand() % 3)
	{
	case 0: //full //duplicate
		gi.centerprintf(ent, "you're all full up...\nyou can't hold any more\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/fullup1.wav"), 1, ATTN_NORM, 0);
		break;

	case 1://health
		gi.centerprintf(ent, "hey you can't carry any more\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/maxed1.wav"), 1, ATTN_NORM, 0);
		break;

	case 2: //armor
		gi.centerprintf(ent, "you're maxed out\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/maxed2.wav"), 1, ATTN_NORM, 0);
		break;
	}
}

static qboolean ScrollMenuHasCash(edict_t *ent)
{
	if (ent->client->pers.currentcash < menu_item_names[ent->current_menu_left].price[ent->current_menu_right])
	{
		if (rand() % 2 == 0)
		{
			gi.centerprintf(ent, "you ain't got enough money\n");
			gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/stock_cash2.wav"), 1, ATTN_NORM, 0);
		}
		else
		{
			gi.centerprintf(ent, "hey, you don't have enough cash...\n");
			gi.sound(ent, CHAN_VOICE, gi.soundindex("world/pawnomatic/stock_cash1.wav"), 1, ATTN_NORM, 0);
		}
		return false;
	}

	return true;
}




static void ScrollMenuSetMaxAmmo(edict_t *ent, int size)
{
	static int maxAmmoVal[6][3] = {
#if 0
		//S ,   M,   L
		{300, 400, 500},//max_bullets
		{200, 300, 400},//max_shells
		{50, 100, 200},//max_rockets
		{15, 30, 50},//max_grenades
		{300, 400, 500},//max_gas
		{90, 120, 150},//max_308cal
#else
	//killa's values
	   //S , M, L
		{100, 200, 400},//max_bullets
		{24, 32, 64},//max_shells
		{12, 25, 50},//max_rockets
		{10, 15, 20},//max_grenades
		{50, 150, 200},//max_gas
		{60, 120, 180},//max_308cal
#endif
	};

	ent->client->pers.max_bullets = maxAmmoVal[0][size];
	ent->client->pers.max_shells = maxAmmoVal[1][size];
	ent->client->pers.max_rockets = maxAmmoVal[2][size];
	ent->client->pers.max_grenades = maxAmmoVal[3][size];
	ent->client->pers.max_gas = maxAmmoVal[4][size];
	ent->client->pers.max_308cal = maxAmmoVal[5][size];
}


void ScrollMenuMessage (edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	char	string2[1400];
	int		stringlength=0, stringlength2=0;
	int		i, yofs= 0; //y start from centre

	char	leftname[32];
	char	rightname[32];
	string[0] = string2[0] = leftname[0] = rightname[0] = 0;

	//build non hilighted first, left aligned
	strcpy(string, "xm -150 ");
	stringlength += strlen(string);

	if (ent->client->showscrollmenu)
	{
		for (i=0; i<MAX_MENU_ITEMS; i++)
		{	//active menu
			int		leftActive=0;
			int		rightActive=0;
			char	*lefttag = "764";
			char	*righttag = "764";

			if (menu_item_names[ent->current_menu_left].name[i] == NULL
				&& menu_header.name[i] == NULL)
				continue;

			if (i == ent->current_menu_left && !ent->current_menu_side)
			{
				lefttag = "999";//color select
				leftActive = 1;
			}

			if (i == ent->current_menu_right && ent->current_menu_side)
			{
				righttag = "999";//color select
				rightActive = 1;
			}

			//header
			if (menu_header.name[i] != NULL)
			{
				strcpy( leftname, menu_header.name[i]);
			}
			else
			{
				strcpy( leftname, "");
			}
			//items
			if (menu_item_names[ent->current_menu_left].name[i] != NULL)
			{
				sprintf( rightname, "%3i - %s", 
					menu_item_names[ent->current_menu_left].price[i],
					menu_item_names[ent->current_menu_left].name[i]);
			}
			else
			{
				strcpy( rightname, "");
			}

			if (leftActive || rightActive)
			{
				Com_sprintf(entry, sizeof(entry),
					"yv %i "
					"xm -150 dmstr %s \"%s\" "
					"xm -70 dmstr %s \"%s\" "
					, yofs
					, lefttag, leftname
					, righttag, rightname);

				strcpy (string2 + stringlength2, entry);
				stringlength2 += strlen(entry);
			}
			else
			{	
				int sIdx, len = strlen(leftname);
				//align right margin
				for (sIdx = len; sIdx < 8; sIdx++)	{
					leftname[sIdx] = ' ';
				}
				leftname[8] = '\0';

				Com_sprintf(entry, sizeof(entry),
					"yv %i dmstr %s \"%s%s\" " //combined. reduce SZ_GetSpace: overflow
					, yofs , lefttag, leftname, rightname);

				strcpy (string + stringlength, entry);
				stringlength += strlen(entry);
			}
			yofs+=20;
		}

		//merge strings
		strcpy (string + stringlength, string2);

		gi.WriteByte (svc_layout);
		gi.WriteString (string);
		gi.unicast (ent, false);//true);
	}
}

qboolean Pickup_Armor_Scroll(char *name, edict_t *other)
{
	if (strcmp(name, "item_armor_helmet") == 0)
	{
		gitem_t *itemh = FindItem("Helmet Armor");
		gitem_t *itemhh = FindItem("Helmet Armor Heavy");

		if ((other->client->pers.inventory[ITEM_INDEX(itemh)] == 100) ||
			(other->client->pers.inventory[ITEM_INDEX(itemhh)] == 100))
			return false;

		if (other->client->pers.inventory[ITEM_INDEX(itemhh)])
		{
			//			other->client->pers.inventory[ITEM_INDEX(itemhh)] += 25;
			other->client->pers.inventory[ITEM_INDEX(itemhh)] += 50;//FREDZ nobody like this part that upgrade is only 25
			if (other->client->pers.inventory[ITEM_INDEX(itemhh)] > 100)
				other->client->pers.inventory[ITEM_INDEX(itemhh)] = 100;
		}
		else
		{
			other->client->pers.inventory[ITEM_INDEX(itemh)] += 50;
			if (other->client->pers.inventory[ITEM_INDEX(itemh)] > 100)
				other->client->pers.inventory[ITEM_INDEX(itemh)] = 100;
		}
	}
	else if (strcmp(name, "item_armor_jacket") == 0)
	{
		gitem_t *itemj = FindItem("Jacket Armor");
		gitem_t *itemjh = FindItem("Jacket Armor Heavy");

		if ((other->client->pers.inventory[ITEM_INDEX(itemj)] == 100) ||
			(other->client->pers.inventory[ITEM_INDEX(itemjh)] == 100))
			return false;

		if (other->client->pers.inventory[ITEM_INDEX(itemjh)])
		{
			//			other->client->pers.inventory[ITEM_INDEX(itemjh)] += 25;
			other->client->pers.inventory[ITEM_INDEX(itemjh)] += 50;//FREDZ nobody like this part that upgrade is only 25
			if (other->client->pers.inventory[ITEM_INDEX(itemjh)] > 100)
				other->client->pers.inventory[ITEM_INDEX(itemjh)] = 100;
		}
		else
		{
			other->client->pers.inventory[ITEM_INDEX(itemj)] += 50;
			if (other->client->pers.inventory[ITEM_INDEX(itemj)] > 100)
				other->client->pers.inventory[ITEM_INDEX(itemj)] = 100;
		}
	}
	else if (strcmp(name, "item_armor_legs") == 0)
	{
		gitem_t *iteml = FindItem("Legs Armor");
		gitem_t *itemlh = FindItem("Legs Armor Heavy");

		if ((other->client->pers.inventory[ITEM_INDEX(iteml)] == 100) ||
			(other->client->pers.inventory[ITEM_INDEX(itemlh)] == 100))
			return false;

		if (other->client->pers.inventory[ITEM_INDEX(itemlh)])
		{
			//			other->client->pers.inventory[ITEM_INDEX(itemlh)] += 25;
			other->client->pers.inventory[ITEM_INDEX(itemlh)] += 50;//FREDZ nobody like this part that upgrade is only 25
			if (other->client->pers.inventory[ITEM_INDEX(itemlh)] > 100)
				other->client->pers.inventory[ITEM_INDEX(itemlh)] = 100;
		}
		else
		{
			other->client->pers.inventory[ITEM_INDEX(iteml)] += 50;
			if (other->client->pers.inventory[ITEM_INDEX(iteml)] > 100)
				other->client->pers.inventory[ITEM_INDEX(iteml)] = 100;
		}
	}
	else if (strcmp(name, "item_armor_helmet_heavy") == 0)
	{
		gitem_t *itemh = FindItem("Helmet Armor");
		gitem_t *itemhh = FindItem("Helmet Armor Heavy");

		if (other->client->pers.inventory[ITEM_INDEX(itemhh)] == 100)
			return false;

		if (other->client->pers.inventory[ITEM_INDEX(itemh)])
			other->client->pers.inventory[ITEM_INDEX(itemh)] = 0;

		other->client->pers.inventory[ITEM_INDEX(itemhh)] = 100;
	}
	else if (strcmp(name, "item_armor_jacket_heavy") == 0)
	{
		gitem_t *itemj = FindItem("Jacket Armor");
		gitem_t *itemjh = FindItem("Jacket Armor Heavy");

		if (other->client->pers.inventory[ITEM_INDEX(itemjh)] == 100)
			return false;

		if (other->client->pers.inventory[ITEM_INDEX(itemj)])
			other->client->pers.inventory[ITEM_INDEX(itemj)] = 0;

		other->client->pers.inventory[ITEM_INDEX(itemjh)] = 100;
	}
	else if (strcmp(name, "item_armor_legs_heavy") == 0)
	{
		gitem_t *iteml = FindItem("Legs Armor");
		gitem_t *itemlh = FindItem("Legs Armor Heavy");

		if (other->client->pers.inventory[ITEM_INDEX(itemlh)] == 100)
			return false;

		if (other->client->pers.inventory[ITEM_INDEX(iteml)])
			other->client->pers.inventory[ITEM_INDEX(iteml)] = 0;

		other->client->pers.inventory[ITEM_INDEX(itemlh)] = 100;
	}
	return true;
}

void SP_pistol_mod_damage(edict_t *self);
void SP_pistol_mod_reload(edict_t *self);
void SP_pistol_mod_rof(edict_t *self);
void SP_hmg_mod_cooling(edict_t *self);
void SP_hyper_shotgun_mod(edict_t *self);


void ScrollMenuBuy(edict_t *ent)
{
	gitem_t		*it;
	int			index;
	edict_t		*it_ent;
	int oldcost = 0;

	//FREDZ Must be on right side to have something selected
	if (!ent->current_menu_side)
		return;

#if 0 //def HYPODEBUG
	ent->client->pers.currentcash = 9999;
#endif

	it = FindItem (menu_item_names[ent->current_menu_left].name[ent->current_menu_right]);
	if (!it)
	{
		gi.cprintf (ent, PRINT_HIGH, "not a valid item\n");
		return;
	}

	if (!it->pickup)
	{
		gi.cprintf (ent, PRINT_HIGH, "non-pickup item\n");
		return;
	}

	if (ent->timestamp > level.time)//FREDZ 
		return;

	//enough time to talk.. but not to delay client to much..
	ent->timestamp = level.time + 1.5f; //seconds

	index = ITEM_INDEX(it);

	if (it->flags & IT_KEY)
	{
		if (ent->health >= ent->max_health)
		{	//play "full" sound
			ScrollMenuSound_Full(ent, 0); //was full_2.wav & full_4.wav
			return;
		}

		//refuse sound with no cash
		if (!ScrollMenuHasCash(ent))
			return;
		
		//play "sold" sound
		ScrollMenuSound_Sold(ent);

		if (ent->current_menu_left == 3) //health
		{
			it_ent = G_Spawn();
			it_ent->classname = it->classname;
			SpawnItem(it_ent, it);
			Touch_Item(it_ent, ent, NULL, NULL);
			if (it_ent->inuse)
				G_FreeEdict(it_ent);
		}
		else
			oldcost = oldcost; //debug
	}
	else if (it->flags & IT_AMMO)//Ammo
	{
		//refuse sound with no cash
		if (!ScrollMenuHasCash(ent))
			return;

		if (!Add_Ammo(ent, it, it->quantity))
		{	//play "full" sound
			ScrollMenuSound_Maxed(ent);
			//ScrollMenuSound_Full(ent, 1); //was full_1.wav
			return;
		}

		//play "sold" sound
		ScrollMenuSound_Sold(ent);
	}
	else if (it->flags & IT_ARMOR)//Armor
	{
		//refuse sound with no cash
		if (!ScrollMenuHasCash(ent))
			return;

		if (!Pickup_Armor_Scroll(it->classname, ent))
		{	//play "full" sound
			ScrollMenuSound_Full(ent, 3); //was full_3.wav
			return;
		}

		//play "sold" sound
		ScrollMenuSound_Sold(ent);
	}
	else if (it->flags & IT_WEAPON)//Weapons
	{
		//refuse sound with no cash
		if (!ScrollMenuHasCash(ent))
			return;

		if (ent->client->pers.inventory[index])
		{
			ScrollMenuSound_Have(ent); //have mods
			return;
		}
		if (ent->current_menu_left == 1)//FREDZ mods
		{
			//make new item to drop on client head
			it_ent = G_Spawn();
			it_ent->classname = it->classname;

			if (ent->current_menu_right == 1) //"Pistol RoF Mod"
				SP_pistol_mod_rof(it_ent);
			else if (ent->current_menu_right == 2) //"Pistol Magnum Mod"
				SP_pistol_mod_damage(it_ent);
			else if (ent->current_menu_right == 3) //"Pistol Reload Mod"
				SP_pistol_mod_reload(it_ent);
			else if (ent->current_menu_right == 4) //HMG Cooling Mod
				SP_hmg_mod_cooling(it_ent);
			else if (ent->current_menu_right == 5) //Hyper ShotGun
				SP_hyper_shotgun_mod(it_ent);
			else
				SpawnItem (it_ent, it); // silencer

			Touch_Item (it_ent, ent, NULL, NULL);
			if (it_ent->inuse)
				G_FreeEdict(it_ent);
		}
		else//Weapons
		{
			//drop item on client
			it_ent = G_Spawn();
			it_ent->classname = it->classname;
			SpawnItem (it_ent, it);
			Touch_Item (it_ent, ent, NULL, NULL);
			if (it_ent->inuse)
				G_FreeEdict(it_ent);

			//ent->client->newweapon = it;
			//ChangeWeapon(ent);
		}

		ent->client->pers.inventory[index] = 1;
		//play "sold" sound
		ScrollMenuSound_Sold(ent);
	}
	else if (it->flags & IT_PACK)//back pack
	{
		//refuse sound with no cash
		if (!ScrollMenuHasCash(ent))
			return;

		//have item
		if (ent->client->pers.inventory[index] /*|| ent->client->pers.max_bullets >= maxAmmoVal[0][it->quantity-1]*/)
		{
			ScrollMenuSound_Have(ent); //have pack
			return;
		}

		if (it->quantity == 3 && level.waveNum < 7) //pack large
		{			//cant have this!!!
			ScrollMenuSound_NoStock(ent);
			return;
		}
		else if (it->quantity == 2 && level.waveNum < 3) //pack medium
		{	//cant have this!!!
			ScrollMenuSound_NoStock(ent);
			return; 
		}

		//give max ammo counts
		ScrollMenuSetMaxAmmo(ent, it->quantity-1);
		ent->client->pers.inventory[index] = 1;
		//play "sold" sound
		ScrollMenuSound_Sold(ent);
	}

	ent->client->pers.currentcash -= (menu_item_names[ent->current_menu_left].price[ent->current_menu_right] - oldcost);
	gi.cprintf(ent, PRINT_HIGH, "Bought: %s\n", it->pickup_name);
}

void ScrollMenuLeft(edict_t *ent)
{
	if (ent->current_menu_side)
	{
		ent->current_menu_side = 0;
	}

	ScrollMenuMessage( ent );
}

void ScrollMenuRight(edict_t *ent)
{
	if (!ent->current_menu_side)
	{
		ent->current_menu_side = 1;
	}

	ScrollMenuMessage( ent );
}

void ScrollMenuPrev(edict_t *ent)
{
	if (!ent->current_menu_side) //using left menu
	{
		ent->current_menu_left--;
		ent->current_menu_right = 0;

		if (ent->current_menu_left < 0)
		{
			ent->current_menu_left = MAX_MENU_ITEMS-1;
			while (menu_header.name[ent->current_menu_left] == NULL)
				ent->current_menu_left -= 1;
		}
	}
	else //using a right menu
	{
		ent->current_menu_right--;

		if (ent->current_menu_right < 0)
		{
			ent->current_menu_right = MAX_MENU_ITEMS-1;
			while (menu_item_names[ent->current_menu_left].name[ent->current_menu_right] == NULL)
				ent->current_menu_right -= 1;
		}
	}

	ScrollMenuMessage( ent );
}

void ScrollMenuNext(edict_t *ent)
{
	if (!ent->current_menu_side)
	{
		if (menu_header.name[ent->current_menu_left + 1] != NULL)
		{
			ent->current_menu_left++;
			ent->current_menu_right = 0;
		}
		else
		{
			ent->current_menu_left = 0;
		}
	}
	else
	{
		if (menu_item_names[ent->current_menu_left].name[ent->current_menu_right + 1] != NULL)
			ent->current_menu_right++;
		else
			ent->current_menu_right = 0;
	}

	ScrollMenuMessage( ent );
}


/*QUAKED cast_pawn_o_matic (1 .5 0) (-16 -16 -24) (16 16 48) x TRIGGERED_START x x x IMMEDIATE_FOLLOW_PATH MELEE NOSHADOW

One handed weapon runt

cast_group 0	neutral

cast_group 1	friendly

cast_group 2+	enemy

model="models\actors\runt\"

*/

void SP_cast_pawn_o_matic( edict_t *self )
{
//	edict_t *other;
//	int i;

	//FREDZ only map rc got diffrent skin
	if (!self->art_skins)
	{
		if (strstr (level.mapname, "rc3"))
		{
			self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "011 041 003");
		}
		else
		{
			// use default skins
			self->art_skins = strcpy(gi.TagMalloc(12, TAG_LEVEL), "011 011 005");
		}
	}

	//FREDZ only map st and rc got diffrent count
	if (!self->count)
	{
		if ((strstr (level.mapname, "steel1")) || (strstr (level.mapname, "rc3")))
		{
			self->count = 3;
		}
		else
		{
			// use default count
			self->count = 1;
		}
	}

	if (!self->name)
	{
		self->name = "Sharky";
	}
	if (!self->moral)
	{
		self->moral = 3;
	}

//FREDZ will fuck up name
/*	if (!self->cast_group)
	{
		self->cast_group = 0;
	}
*/
	//FREDZ not killable
	level.total_monsters--;

	//FREDZ
//	level.killed_monsters--;

/*
	for_each_player(other, i)
	{
		if (other->client->pers.spectator == PLAYING)
			 Voice_Specific(self, other, pawnomatic_specific, 19);
	}*/


	self->flags |= FL_GODMODE;

	SP_cast_runt(self);
}

//scroll menu, update keys using id movement code
void ScrollMenuKeyLogger(edict_t *ent)
{
	//stop overflows
	if (ent->client->scrollmenu_timestamp > level.time)
		return;

	if (ent->client->scrollmenu_fwd_do == 1) {
		if (ent->client->scrollmenu_forward > 0)
			ScrollMenuPrev(ent);
		else
			ScrollMenuNext(ent);
		ent->client->scrollmenu_fwd_do = 2;
		ent->client->scrollmenu_timestamp = level.time + 0.2f;
	}
	if (ent->client->scrollmenu_side_do ==1) {
		if (ent->client->scrollmenu_side > 0)
			ScrollMenuRight(ent);
		else
			ScrollMenuLeft(ent);
		ent->client->scrollmenu_side_do = 2;
		ent->client->scrollmenu_timestamp = level.time + 0.2f;
	}
}

