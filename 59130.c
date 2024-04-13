data_left (FILE* input_file)
{
    int retval = 1;
    
    if (feof(input_file)) retval = 0;
    else if (input_file != stdin)
    {
	/* check if there is enough data left */
	struct stat statbuf;
	size_t pos, data_left;
	fstat (fileno(input_file), &statbuf);
	pos = ftell(input_file);
	data_left = (statbuf.st_size - pos);

	if (data_left > 0 && data_left < MINIMUM_ATTR_LENGTH) 
	{
	    if ( CRUFT_SKIP )
	    {
		/* look for specific flavor of cruft -- trailing "\r\n" */

		if ( data_left == 2 )
		{
		    int c = fgetc( input_file );

		    if ( c < 0 )	/* this should never happen */
		    {
			fprintf( stderr, "ERROR: confused beyond all redemption.\n" );
			exit (1);
		    }

		    ungetc( c, input_file );

		    if ( c == 0x0d )		/* test for "\r" part of "\r\n" */
		    {
			/* "trust" that next char is 0x0a and ignore this cruft */

			if ( VERBOSE_ON )
			    fprintf( stderr, "WARNING: garbage at end of file (ignored)\n" );

			if ( DEBUG_ON )
			    debug_print( "!!garbage at end of file (ignored)\n" );
		    }
		    else
		    {
			fprintf( stderr, "ERROR: garbage at end of file.\n" );
		    }
		}
		else
		{
		    fprintf (stderr, "ERROR: garbage at end of file.\n");
		}
	    }
	    else
	    {
		fprintf (stderr, "ERROR: garbage at end of file.\n");
	    }

	    retval = 0;
	}
    }
    return retval;
}
