//FREDZ second numbers means walking speed if it's 0.000 it doesn't move on that frame.
mframe_t	rat_frames_run[] =
{
	ai_run,	 11.155, NULL,	// frame 0
	ai_run,	 25.162, NULL,	// frame 1
	ai_run,	 27.589, NULL,	// frame 2
	ai_run,	 21.692, NULL,	// frame 3
};
mmove_t	rat_move_run = {FRAME_run_01, FRAME_run_04, rat_frames_run, AI_EndRun};

mframe_t	rat_frames_stand[] =
{
	ai_stand,	  0.000, rat_talk_think,	// frame 0
	ai_stand,	  0.000, rat_talk_think,	// frame 1
	ai_stand,	  0.000, rat_talk_think,	// frame 2
	ai_stand,	  0.000, rat_talk_think,	// frame 3
};
mmove_t	rat_move_stand = {FRAME_stand1_01, FRAME_stand1_04, rat_frames_stand, rat_end_stand};

mframe_t	rat_frames_sit_up[] =
{
    NULL,	  0.004, NULL,	        // frame 0
	NULL,	  5.304, rat_pounce,	// frame 1
	NULL,	  0.102, rat_pounce,	// frame 2
	NULL,	  8.523, rat_pounce,	// frame 3
	NULL,	 19.589, rat_bite,	    // frame 4
	NULL,	  3.179, rat_pounce,	// frame 5
	NULL,	 18.590, rat_bite,	    // frame 6
	NULL,	  4.629, rat_pounce,	// frame 7
	NULL,	 -4.434, rat_bite,	    // frame 8
	NULL,	  3.514, rat_bite,	    // frame 9
};
mmove_t	rat_move_sit_up = {FRAME_situp_01, FRAME_situp_10, rat_frames_sit_up, AI_EndAttack};

mframe_t	rat_frames_reverse_run[] =
{
	ai_turn,	 21.692, NULL,	// frame 0
	ai_turn,	 27.589, NULL,	// frame 1
	ai_turn,	 25.162, NULL,	// frame 2
	ai_turn,	 11.155, NULL,	// frame 3
};
mmove_t	rat_move_reverse_run = {FRAME_run_04, FRAME_run_01, rat_frames_reverse_run, AI_EndAttack};

