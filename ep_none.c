

/******************************************************************************

  NONE - Episode specific code

*******************************************************************************/

#include "g_local.h"

#include "voice_punk.h"
#include "voice_bitch.h"


qboolean EP_None_EventSpeech (edict_t *self, edict_t *other, int saywhat)
{
//	cast_memory_t	*mem;

//	mem = level.global_cast_memory[ self->character_index ][ other->character_index ];


	switch (saywhat)
	{

	case say_neutral://Should not be used everybody is agrresive :p, but maybe handy for other mods

        //Skidrow start
		//............................................................................
		// BETH
		if ( self->name_index == NAME_BETH && other->client)
		{
            Voice_Random(self, other, beth_specific, 6);
			return true;
		}

		//............................................................................
		// MONA
		if ( self->name_index == NAME_MONA && other->client)
		{
            Voice_Random (self, other, &mona_specific[4], 6);
			return true;
		}
		//............................................................................
		// MAGICJ
		if	(self->name_index == NAME_MAGICJ&& other->client)
		{

            Voice_Random ( self, other, mj_random, 4);
            return true;
		}
		//............................................................................
		//Skidrow end

		//Poisonville start
		if (self->name_index == NAME_YOLANDA)
		{
			if (other->client)
			{
                Voice_Random (self, other, &yolanda_specific[0], 5);
				return true;
			}
			else
			{
				Voice_Random (self, other, &yolanda_specific[12], 5);
				return true;
			}
		}
        //Poisonville end
        //Shipyard start
		if (self->name_index == NAME_BWILLIE && other->client)
		{
			Voice_Random (self, other, sy_bigwillie, 5);
			return true;
		}

		if (self->name_index == NAME_DOGG && other->client)
		{
			Voice_Random (self, other, sy_dogg, 5);
			return true;
		}
		if (self->name_index == NAME_POPEYE && other->client)
		{
            Voice_Random (self, other, &sy_popeye[3], 6);
			return true;
		}
		//Shipyard end
		//Trainyard start
		if (self->name_index == NAME_LUKE && other->client)
		{
            Voice_Random (self, other, &ty_luke[2], 4);
			return true;
		}

		if (self->name_index == NAME_HANN && other->client)
		{
            Voice_Random (self, other, &ty_hann[2], 4);
			return true;
		}
		//Trainyard end
		//Radiocity start
		if (self->name_index == NAME_LOLA && other->client)
		{
            Voice_Random (self, other, &rc_lola[1], 3);
			return true;
		}
		//Radiocity end
		return false;
		break;

	case say_hostile:

        //Skidrow start
		if ( self->name_index == NAME_BETH && other->client)
		{
			// Rafael: we need more negative sounds
			if (rand()%100 > 75)
				Voice_Random (self, other, &f_fightsounds[0],3);
			else
                Voice_Specific (self, other, beth_specific, 7);
			return true;
		}

		//............................................................................
		// MONA
		if ( self->name_index == NAME_MONA && other->client)
		{
            Voice_Random (self, other, &mona_specific[15], 5);
//			self->cast_info.aiflags |= AI_NO_TALK;
			return true;
		}

		if	(self->name_index == NAME_MAGICJ&& other->client)
		{
			{
				if ((rand()%10) < 4)
					Voice_Specific (self, other, specific, 29);
				else
                    Voice_Random ( self, other, &specific[23], 2);
				return true;
			}
		}
		//Skidrow end
		//Poisonville start
		if (self->name_index == NAME_YOLANDA && other->client)
		{
            Voice_Random (self, other, &yolanda_specific[17], 8);
			return true;
		}
		//Poisonville end
		//Shipyard start
		if (self->name_index == NAME_BWILLIE && other->client)
		{
			Voice_Random (self, other, sy_bigwillie, 5);
			return true;
		}
		if (self->name_index == NAME_DOGG && other->client)
		{
			Voice_Random (self, other, sy_dogg, 5);
			return true;
		}
		if (self->name_index == NAME_POPEYE && other->client)
		{
            // just do a random speech
            Voice_Random (self, other, &sy_popeye[16], 3);
            return true;
		}
		//Shipyard end
		//Trainyard start
		if (self->name_index == NAME_LUKE && other->client)
		{
            // just do a random curse at the player
            Voice_Random (self, other, &ty_luke[6], 3);
            return true;
		}

		if (self->name_index == NAME_HANN && other->client)
		{
            // just do a random curse at the player
            Voice_Random (self, other, &ty_hann[6], 3);
            return true;
		}
		//Trainyard end
		//Radiocity start
		if	(self->name_index == NAME_SCALPER && (other->client))
		{
				Voice_Random (self, other, &rc_scalper[9], 4);
				return true;
		}

		if (self->name_index == NAME_LOLA && other->client)
		{
            // just do a random curse at the player
            Voice_Random (self, other, &rc_lola[4], 3);
			return true;
		}
		//Radiocity end
		return false;
		break;
	}
	return false;
}
