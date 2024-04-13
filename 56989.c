figure_mime( httpd_conn* hc )
    {
    char* prev_dot;
    char* dot;
    char* ext;
    int me_indexes[100], n_me_indexes;
    size_t ext_len, encodings_len;
    int i, top, bot, mid;
    int r;
    char* default_type = "application/octet-stream";

    /* Peel off encoding extensions until there aren't any more. */
    n_me_indexes = 0;
    hc->type = default_type;
    for ( prev_dot = &hc->expnfilename[strlen(hc->expnfilename)]; ; prev_dot = dot )
	{
	for ( dot = prev_dot - 1; dot >= hc->expnfilename && *dot != '.'; --dot )
	    ;
	if ( dot < hc->expnfilename )
	    {
	    /* No dot found.  No more extensions.  */
	    goto done;
	    }
	ext = dot + 1;
	ext_len = prev_dot - ext;
	/* Search the encodings table.  Linear search is fine here, there
	** are only a few entries.
	*/
	for ( i = 0; i < n_enc_tab; ++i )
	    {
	    if ( ext_len == enc_tab[i].ext_len && strncasecmp( ext, enc_tab[i].ext, ext_len ) == 0 )
		{
		if ( n_me_indexes < sizeof(me_indexes)/sizeof(*me_indexes) )
		    {
		    me_indexes[n_me_indexes] = i;
		    ++n_me_indexes;
		    }
		break;
		}
	    }
	/* Binary search for a matching type extension. */
	top = n_typ_tab - 1;
	bot = 0;
	while ( top >= bot )
	    {
	    mid = ( top + bot ) / 2;
	    r = strncasecmp( ext, typ_tab[mid].ext, ext_len );
	    if ( r < 0 )
		top = mid - 1;
	    else if ( r > 0 )
		bot = mid + 1;
	    else
		if ( ext_len < typ_tab[mid].ext_len )
		    top = mid - 1;
		else if ( ext_len > typ_tab[mid].ext_len )
		    bot = mid + 1;
		else
		    {
		    hc->type = typ_tab[mid].val;
		    goto done;
		    }
	    }
	}

    done:

    /* The last thing we do is actually generate the mime-encoding header. */
    hc->encodings[0] = '\0';
    encodings_len = 0;
    for ( i = n_me_indexes - 1; i >= 0; --i )
	{
	httpd_realloc_str(
	    &hc->encodings, &hc->maxencodings,
	    encodings_len + enc_tab[me_indexes[i]].val_len + 1 );
	if ( hc->encodings[0] != '\0' )
	    {
	    (void) strcpy( &hc->encodings[encodings_len], "," );
	    ++encodings_len;
	    }
	(void) strcpy( &hc->encodings[encodings_len], enc_tab[me_indexes[i]].val );
	encodings_len += enc_tab[me_indexes[i]].val_len;
	}

    }
