file_write (File *file, const char* directory)
{
    char *path = NULL;

    assert (file);
    if (!file) return;

    if (file->name == NULL)
    {
	file->name = strdup( TNEF_DEFAULT_FILENAME );
	debug_print ("No file name specified, using default %s.\n", TNEF_DEFAULT_FILENAME);
    }

    if ( file->path == NULL )
    {
	file->path = munge_fname( file->name );

	if (file->path == NULL)
	{
	    file->path = strdup( TNEF_DEFAULT_FILENAME );
	    debug_print ("No path name available, using default %s.\n", TNEF_DEFAULT_FILENAME);
	}
    }

    path = concat_fname( directory, file->path );

    if (path == NULL)
    {
	path = strdup( TNEF_DEFAULT_FILENAME );
	debug_print ("No path generated, using default %s.\n", TNEF_DEFAULT_FILENAME);
    }

    debug_print ("%sWRITING\t|\t%s\t|\t%s\n",
		 ((LIST_ONLY==0)?"":"NOT "), file->name, path);

    if (!LIST_ONLY)
    {
	FILE *fp = NULL;

	if (!confirm_action ("extract %s?", file->name)) return;
	if (!OVERWRITE_FILES)
	{
	    if (file_exists (path))
	    {
		if (!NUMBER_FILES)
		{
		    fprintf (stderr,
			     "tnef: %s: Could not create file: File exists\n",
			     path);
		    return;
		}
		else
		{
		    char *tmp = find_free_number (path);
		    debug_print ("Renaming %s to %s\n", path, tmp);
		    XFREE (path);
		    path = tmp;
		}
	    }
	}

	fp = fopen (path, "wb");
	if (fp == NULL)
	{
	    perror (path);
	    exit (1);
	}
	if (fwrite (file->data, 1, file->len, fp) != file->len)
	{
	    perror (path);
	    exit (1);
	}
	fclose (fp);
    }

    if (LIST_ONLY || VERBOSE_ON)
    {
	if (LIST_ONLY && VERBOSE_ON)
	{
	    /* FIXME: print out date and stuff */
	    const char *date_str = date_to_str(&file->dt);
	    fprintf (stdout, "%11lu\t|\t%s\t|\t%s\t|\t%s", 
		     (unsigned long)file->len,
		     date_str+4, /* skip the day of week */
		     file->name,
		     path);
	}
	else
	{
            fprintf (stdout, "%s\t|\t%s", file->name, path);
	}
	if ( SHOW_MIME )
	{
	    fprintf (stdout, "\t|\t%s", file->mime_type ? file->mime_type : "unknown");
            fprintf (stdout, "\t|\t%s", file->content_id ? file->content_id : "");
	}
        fprintf (stdout, "\n");
    }
    XFREE(path);
}
