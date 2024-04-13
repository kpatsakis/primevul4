get_loop_mode (int16_t mode)
{	switch (mode)
	{	case 0 : return SF_LOOP_NONE ;
		case 1 : return SF_LOOP_FORWARD ;
		case 2 : return SF_LOOP_BACKWARD ;
		} ;

	return SF_LOOP_NONE ;
} /* get_loop_mode */
