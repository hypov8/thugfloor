#include "g_local.h"

//FREDZ Scroll Menu Source Code

#define MAX_MENU_ITEMS	8

#define MENU_LEFT_SIDE	0
#define MENU_RIGHT_SIDE	1

struct MENU_HEADER_NAME
{
	char *name[8];
} menu_header =

{
	{	"Weapons",
		"Mods",
		"Ammo",
		"Health",
		"Armor"
	}
};

struct MENU_ITEM_NAMES
{
	char	*name[8];
	int		price[8];
} menu_item_names[MAX_MENU_ITEMS] =
{
	//Weapons
	{ {"Pistol", "Shotgun", "Tommygun", "Heavy machinegun", "Grenade Launcher", "Bazooka", "FlameThrower"}, {50, 150, 250, 750, 200, 350, 500} },
	//Mod's
//	{ {"Pistol Silencer"}, {10} },
	{ {"Pistol Silencer", "Pistol RoF Mod", "Pistol Magnum Mod", "Pistol Reload Mod","HMG Cooling Mod"}, {10, 25, 50, 25, 50} },
	//Ammo
	{ {"Bullets", "Shells", "308cal", "Grenades", "Rockets", "Gas"}, {5, 25, 20, 100, 100, 25} },
	//Health
//	{ {"Small Health", "Large Health", "Adrenaline"}, {25, 50, 100} },
	{ {"Small Health", "Large Health"}, {25, 50} },
	//Armor
	{ {"Helmet Armor", "Jacket Armor", "Legs Armor", "Helmet Armor Heavy", "Jacket Armor Heavy", "Legs Armor Heavy"}, {100, 150, 100, 200, 300, 200} }
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
		return;
	}
	else
	{
		cl->showscrollmenu = true;
	}

	ScrollMenuMessage (ent);
}

void ScrollMenuMessage (edict_t *ent)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j, yofs=0;

	char	leftname[32];
	char	rightname[32];
	char	*lefttag;
	char	*righttag;

	string[0] = 0;
	stringlength = 0;

	if (ent->client->showscrollmenu)
	{

		for (i=0; i<MAX_MENU_ITEMS; i++)
		{
			if (i == ent->current_menu_left && !ent->current_menu_side)
			{
				lefttag = "999";
			}
			else
			{
				lefttag = "444";
			}

			if (i == ent->current_menu_right && ent->current_menu_side)
			{
				righttag = "999";
			}
			else
			{
				righttag = "444";
			}


			if (menu_header.name[i])
			{
				strcpy( leftname, menu_header.name[i]);
			}
			else
			{
				strcpy( leftname, "");
			}
			if (menu_item_names[ent->current_menu_left].name[i])
			{
				sprintf( rightname, "%i - ", menu_item_names[ent->current_menu_left].price[i]);
				strcat( rightname, menu_item_names[ent->current_menu_left].name[i]);
			}
			else
			{
				strcpy( rightname, "");
			}

			Com_sprintf(entry, sizeof(entry), "xl 20 yv %i dmstr %s \"%s\" xl 150 dmstr %s \"%s\" ", yofs-109, lefttag, leftname, righttag, rightname);
			j = strlen(entry);
			strcpy (string + stringlength, entry);
			stringlength += j;
			yofs+=20;
		}

	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
	gi.unicast (ent, false);//true);
}

extern void AutoLoadWeapon( gclient_t *client, gitem_t *weapon, gitem_t *ammo );
void ScrollMenuBuy(edict_t *ent)
{
	gitem_t		*it;
	int			index;
	edict_t		*it_ent;
	int oldcost = 0;
	int		i;

	//FREDZ Must be on right side to have something selected
	if (!ent->current_menu_side)
		return;

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

	index = ITEM_INDEX(it);

	if (it->flags & IT_KEY)
	{
		gitem_t		*it_health;
		int			index_health;

		if (ent->health >= ent->max_health)
		{
			gi.centerprintf (ent, "you don't need any more health...\nyou're full up\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/fullup2.wav"), 1, ATTN_NORM, 0);
			ent->timestamp = level.time + 2;
			return;
		}
		if (ent->client->pers.currentcash < menu_item_names[ent->current_menu_left].price[ent->current_menu_right])
		{
			i = rand()%2;
			switch (i)
			{
			case 0:
				gi.centerprintf (ent, "you ain't got enough money\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash2.wav"), 1, ATTN_NORM, 0);
				break;
			case 1:
				gi.centerprintf (ent, "hey, you don't have enough cash...\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash1.wav"), 1, ATTN_NORM, 0);
				break;
			}
			ent->timestamp = level.time + 2;
			return;
		}

		i = rand()%3;
		switch (i)
		{
		case 0:
			gi.centerprintf (ent, "It's all yours\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold1.wav"), 1, ATTN_NORM, 0);
			break;
		case 1:
			gi.centerprintf (ent, "Here ya go\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold2.wav"), 1, ATTN_NORM, 0);
			break;
		case 2:
			gi.centerprintf (ent, "Sold\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold3.wav"), 1, ATTN_NORM, 0);
			break;
		}
		ent->timestamp = level.time + 2;


		if (ent->current_menu_left == 3)
		{
			if (ent->current_menu_right == 0)
			{
				it_health = FindItem ("Small Health");
			}
			else if (ent->current_menu_right == 1)
			{
				it_health = FindItem ("Large Health");
			}
/*			else if (ent->current_menu_right == 2)
			{
				it_health = FindItem ("Adrenaline");
			}*/

			index_health = ITEM_INDEX(it_health);

			it_ent = G_Spawn();
			it_ent->classname = it_health->classname;
			SpawnItem (it_ent, it_health);
			Touch_Item (it_ent, ent, NULL, NULL);
			if (it_ent->inuse)
					G_FreeEdict(it_ent);

			ent->timestamp = level.time + 2;
		}
/*		if (it->classname, "item_health_lg")
		{
			ent->health += 25;
		}
		else//FREDZ never happends???
			ent->health += 15;*/
	}

	if (it->flags & IT_AMMO)
	{
		if (ent->client->pers.currentcash < menu_item_names[ent->current_menu_left].price[ent->current_menu_right])
		{
			i = rand()%2;
			switch (i)
			{
			case 0:
				gi.centerprintf (ent, "you ain't got enough money\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash2.wav"), 1, ATTN_NORM, 0);
				break;
			case 1:
				gi.centerprintf (ent, "hey, you don't have enough cash...\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash1.wav"), 1, ATTN_NORM, 0);
				break;
			}
			ent->timestamp = level.time + 2;
			return;
		}

		i = rand()%3;
		switch (i)
		{
		case 0:
			gi.centerprintf (ent, "It's all yours\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold1.wav"), 1, ATTN_NORM, 0);
			break;
		case 1:
			gi.centerprintf (ent, "Here ya go\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold2.wav"), 1, ATTN_NORM, 0);
			break;
		case 2:
			gi.centerprintf (ent, "Sold\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold3.wav"), 1, ATTN_NORM, 0);
			break;
		}
		ent->timestamp = level.time + 2;

		if ( !Add_Ammo( ent, it, it->quantity ) )
		{
			gi.centerprintf (ent, "you're all full up... you can't hold any more\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/fullup1.wav"), 1, ATTN_NORM, 0);
			ent->timestamp = level.time + 2;
			return;
		}
	}
	else if (it->flags & IT_ARMOR)
	{
		if (ent->client->pers.currentcash < menu_item_names[ent->current_menu_left].price[ent->current_menu_right])
		{
			i = rand()%2;
			switch (i)
			{
			case 0:
				gi.centerprintf (ent, "you ain't got enough money\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash2.wav"), 1, ATTN_NORM, 0);
				break;
			case 1:
				gi.centerprintf (ent, "hey, you don't have enough cash...\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash1.wav"), 1, ATTN_NORM, 0);
				break;
			}
			ent->timestamp = level.time + 2;
			return;
		}

		i = rand()%3;
		switch (i)
		{
		case 0:
			gi.centerprintf (ent, "It's all yours\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold1.wav"), 1, ATTN_NORM, 0);
			break;
		case 1:
			gi.centerprintf (ent, "Here ya go\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold2.wav"), 1, ATTN_NORM, 0);
			break;
		case 2:
			gi.centerprintf (ent, "Sold\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold3.wav"), 1, ATTN_NORM, 0);
			break;
		}
		ent->timestamp = level.time + 2;

		ent->client->pers.inventory[ITEM_INDEX(it)] = it->quantity;
	}
	else if (it->flags & IT_WEAPON)
	{
		gitem_t		*it_weapon;
		int			index_weapon;
		gitem_t *ammo;

		if (ent->client->pers.currentcash < menu_item_names[ent->current_menu_left].price[ent->current_menu_right])
		{
			i = rand()%2;
			switch (i)
			{
			case 0:
				gi.centerprintf (ent, "you ain't got enough money\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash2.wav"), 1, ATTN_NORM, 0);
				break;
			case 1:
				gi.centerprintf (ent, "hey, you don't have enough cash...\n");
				gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/stock_cash1.wav"), 1, ATTN_NORM, 0);
				break;
			}
			ent->timestamp = level.time + 2;
			return;
		}

		i = rand()%3;
		switch (i)
		{
		case 0:
			gi.centerprintf (ent, "It's all yours\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold1.wav"), 1, ATTN_NORM, 0);
			break;
		case 1:
			gi.centerprintf (ent, "Here ya go\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold2.wav"), 1, ATTN_NORM, 0);
			break;
		case 2:
			gi.centerprintf (ent, "Sold\n");
			gi.sound(ent, CHAN_ITEM, gi.soundindex("world/pawnomatic/sold3.wav"), 1, ATTN_NORM, 0);
			break;
		}

		if (ent->current_menu_left == 1)//FREDZ mods
		{
			if (ent->current_menu_right == 0)
			{
				it_weapon = FindItem ("Pistol Silencer");
			}
			else if (ent->current_menu_right == 1)
			{
				it_weapon = FindItem ("Pistol RoF Mod");
			}
			else if (ent->current_menu_right == 2)
			{
				it_weapon = FindItem ("Pistol Magnum Mod");
			}
			else if (ent->current_menu_right == 3)
			{
				it_weapon = FindItem ("Pistol Reload Mod");
			}
			else if (ent->current_menu_right == 4)
			{
				it_weapon = FindItem ("HMG Cooling Mod");
			}

			index_weapon = ITEM_INDEX(it_weapon);

			it_ent = G_Spawn();
			it_ent->classname = it_weapon->classname;
			SpawnItem (it_ent, it_weapon);
			Touch_Item (it_ent, ent, NULL, NULL);
			if (it_ent->inuse)
					G_FreeEdict(it_ent);

			ent->client->pers.hmg_shots = 30;//FREDZ fix
			ent->client->pers.silencer_shots = 20;//FREDZ fix

			ent->timestamp = level.time + 2;
		}
		else
		{
			ent->timestamp = level.time + 2;
			it_ent = G_Spawn();
			it_ent->classname = it->classname;
			SpawnItem (it_ent, it);
			ent->client->pers.inventory[index] = 1;

			if (it_ent->inuse)
				G_FreeEdict(it_ent);

			if (it->ammo)
			{
				ammo = FindItem(it->ammo);

				if (!ent->client->pers.inventory[ITEM_INDEX(ammo)])
					Add_Ammo( ent, ammo, ammo->quantity );

				AutoLoadWeapon(ent->client, it, ammo);
			}
			ent->client->newweapon = it;
			ChangeWeapon(ent);

			ent->client->ps.stats[STAT_PICKUP_ICON] = gi.imageindex(it->icon);
			ent->client->ps.stats[STAT_PICKUP_STRING] = CS_ITEMS+ITEM_INDEX(it);
			// JOSEPH 25-JAN-99
			ent->client->pickup_msg_time = level.time + 5.5;

			if (it->use)
				ent->client->pers.selected_item = ent->client->ps.stats[STAT_SELECTED_ITEM] = ITEM_INDEX(it);
		}
	}

	gi.sound(ent, CHAN_ITEM, gi.soundindex(it->pickup_sound), 1, ATTN_NORM, 0);
	ent->client->pers.currentcash -= (menu_item_names[ent->current_menu_left].price[ent->current_menu_right] - oldcost);
	gi.cprintf(ent, PRINT_HIGH, "Bought: %s\n", it->classname );
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
	if (!ent->current_menu_side)
	{
		if (menu_header.name[ent->current_menu_left - 1])
		{
			ent->current_menu_left--;
			ent->current_menu_right = 0;
		}
		else
		{
			ent->current_menu_left = 0;//FREDZ fix
		}
	}
	else
	{
		if (menu_item_names[ent->current_menu_left].name[ent->current_menu_right - 1])
			ent->current_menu_right--;
		else
			ent->current_menu_right = 0;
	}

	ScrollMenuMessage( ent );
}

void ScrollMenuNext(edict_t *ent)
{
	if (!ent->current_menu_side)
	{
		if (menu_header.name[ent->current_menu_left + 1])
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
		if (menu_item_names[ent->current_menu_left].name[ent->current_menu_right + 1])
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
extern void SP_cast_runt (edict_t *self);
extern void SP_coil_Skidrow (edict_t *self);
void SP_cast_pawn_o_matic( edict_t *self )
{
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

	self->flags |= FL_GODMODE;

	SP_cast_runt(self);
}
