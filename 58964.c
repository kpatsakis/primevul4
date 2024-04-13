get_loop_mode_str (int16_t mode)
{	switch (mode)
	{	case 0 : return "none" ;
		case 1 : return "forward" ;
		case 2 : return "backward" ;
		} ;

	return "*** unknown" ;
} /* get_loop_mode_str */
