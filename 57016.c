really_start_request( httpd_conn* hc, struct timeval* nowP )
    {
    static char* indexname;
    static size_t maxindexname = 0;
    static const char* index_names[] = { INDEX_NAMES };
    int i;
#ifdef AUTH_FILE
    static char* dirname;
    static size_t maxdirname = 0;
#endif /* AUTH_FILE */
    size_t expnlen, indxlen;
    char* cp;
    char* pi;

    expnlen = strlen( hc->expnfilename );

    if ( hc->method != METHOD_GET && hc->method != METHOD_HEAD &&
	 hc->method != METHOD_POST )
	{
	httpd_send_err(
	    hc, 501, err501title, "", err501form, httpd_method_str( hc->method ) );
	return -1;
	}

    /* Stat the file. */
    if ( stat( hc->expnfilename, &hc->sb ) < 0 )
	{
	httpd_send_err( hc, 500, err500title, "", err500form, hc->encodedurl );
	return -1;
	}

    /* Is it world-readable or world-executable?  We check explicitly instead
    ** of just trying to open it, so that no one ever gets surprised by
    ** a file that's not set world-readable and yet somehow is
    ** readable by the HTTP server and therefore the *whole* world.
    */
    if ( ! ( hc->sb.st_mode & ( S_IROTH | S_IXOTH ) ) )
	{
	syslog(
	    LOG_INFO,
	    "%.80s URL \"%.80s\" resolves to a non world-readable file",
	    httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	httpd_send_err(
	    hc, 403, err403title, "",
	    ERROR_FORM( err403form, "The requested URL '%.80s' resolves to a file that is not world-readable.\n" ),
	    hc->encodedurl );
	return -1;
	}

    /* Is it a directory? */
    if ( S_ISDIR(hc->sb.st_mode) )
	{
	/* If there's pathinfo, it's just a non-existent file. */
	if ( hc->pathinfo[0] != '\0' )
	    {
	    httpd_send_err( hc, 404, err404title, "", err404form, hc->encodedurl );
	    return -1;
	    }

	/* Special handling for directory URLs that don't end in a slash.
	** We send back an explicit redirect with the slash, because
	** otherwise many clients can't build relative URLs properly.
	*/
	if ( strcmp( hc->origfilename, "" ) != 0 &&
	     strcmp( hc->origfilename, "." ) != 0 &&
	     hc->origfilename[strlen( hc->origfilename ) - 1] != '/' )
	    {
	    send_dirredirect( hc );
	    return -1;
	    }

	/* Check for an index file. */
	for ( i = 0; i < sizeof(index_names) / sizeof(char*); ++i )
	    {
	    httpd_realloc_str(
		&indexname, &maxindexname,
		expnlen + 1 + strlen( index_names[i] ) );
	    (void) strcpy( indexname, hc->expnfilename );
	    indxlen = strlen( indexname );
	    if ( indxlen == 0 || indexname[indxlen - 1] != '/' )
		(void) strcat( indexname, "/" );
	    if ( strcmp( indexname, "./" ) == 0 )
		indexname[0] = '\0';
	    (void) strcat( indexname, index_names[i] );
	    if ( stat( indexname, &hc->sb ) >= 0 )
		goto got_one;
	    }

	/* Nope, no index file, so it's an actual directory request. */
#ifdef GENERATE_INDEXES
	/* Directories must be readable for indexing. */
	if ( ! ( hc->sb.st_mode & S_IROTH ) )
	    {
	    syslog(
		LOG_INFO,
		"%.80s URL \"%.80s\" tried to index a directory with indexing disabled",
		httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	    httpd_send_err(
		hc, 403, err403title, "",
		ERROR_FORM( err403form, "The requested URL '%.80s' resolves to a directory that has indexing disabled.\n" ),
		hc->encodedurl );
	    return -1;
	    }
#ifdef AUTH_FILE
	/* Check authorization for this directory. */
	if ( auth_check( hc, hc->expnfilename ) == -1 )
	    return -1;
#endif /* AUTH_FILE */
	/* Referer check. */
	if ( ! check_referer( hc ) )
	    return -1;
	/* Ok, generate an index. */
	return ls( hc );
#else /* GENERATE_INDEXES */
	syslog(
	    LOG_INFO, "%.80s URL \"%.80s\" tried to index a directory",
	    httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	httpd_send_err(
	    hc, 403, err403title, "",
	    ERROR_FORM( err403form, "The requested URL '%.80s' is a directory, and directory indexing is disabled on this server.\n" ),
	    hc->encodedurl );
	return -1;
#endif /* GENERATE_INDEXES */

	got_one: ;
	/* Got an index file.  Expand symlinks again.  More pathinfo means
	** something went wrong.
	*/
	cp = expand_symlinks( indexname, &pi, hc->hs->no_symlink_check, hc->tildemapped );
	if ( cp == (char*) 0 || pi[0] != '\0' )
	    {
	    httpd_send_err( hc, 500, err500title, "", err500form, hc->encodedurl );
	    return -1;
	    }
	expnlen = strlen( cp );
	httpd_realloc_str( &hc->expnfilename, &hc->maxexpnfilename, expnlen );
	(void) strcpy( hc->expnfilename, cp );

	/* Now, is the index version world-readable or world-executable? */
	if ( ! ( hc->sb.st_mode & ( S_IROTH | S_IXOTH ) ) )
	    {
	    syslog(
		LOG_INFO,
		"%.80s URL \"%.80s\" resolves to a non-world-readable index file",
		httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	    httpd_send_err(
		hc, 403, err403title, "",
		ERROR_FORM( err403form, "The requested URL '%.80s' resolves to an index file that is not world-readable.\n" ),
		hc->encodedurl );
	    return -1;
	    }
	}

#ifdef AUTH_FILE
    /* Check authorization for this directory. */
    httpd_realloc_str( &dirname, &maxdirname, expnlen );
    (void) strcpy( dirname, hc->expnfilename );
    cp = strrchr( dirname, '/' );
    if ( cp == (char*) 0 )
	(void) strcpy( dirname, "." );
    else
	*cp = '\0';
    if ( auth_check( hc, dirname ) == -1 )
	return -1;

    /* Check if the filename is the AUTH_FILE itself - that's verboten. */
    if ( expnlen == sizeof(AUTH_FILE) - 1 )
	{
	if ( strcmp( hc->expnfilename, AUTH_FILE ) == 0 )
	    {
	    syslog(
		LOG_NOTICE,
		"%.80s URL \"%.80s\" tried to retrieve an auth file",
		httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	    httpd_send_err(
		hc, 403, err403title, "",
		ERROR_FORM( err403form, "The requested URL '%.80s' is an authorization file, retrieving it is not permitted.\n" ),
		hc->encodedurl );
	    return -1;
	    }
	}
    else if ( expnlen >= sizeof(AUTH_FILE) &&
	      strcmp( &(hc->expnfilename[expnlen - sizeof(AUTH_FILE) + 1]), AUTH_FILE ) == 0 &&
	      hc->expnfilename[expnlen - sizeof(AUTH_FILE)] == '/' )
	{
	syslog(
	    LOG_NOTICE,
	    "%.80s URL \"%.80s\" tried to retrieve an auth file",
	    httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	httpd_send_err(
	    hc, 403, err403title, "",
	    ERROR_FORM( err403form, "The requested URL '%.80s' is an authorization file, retrieving it is not permitted.\n" ),
	    hc->encodedurl );
	return -1;
	}
#endif /* AUTH_FILE */

    /* Referer check. */
    if ( ! check_referer( hc ) )
	return -1;

    /* Is it world-executable and in the CGI area? */
    if ( hc->hs->cgi_pattern != (char*) 0 &&
	 ( hc->sb.st_mode & S_IXOTH ) &&
	 match( hc->hs->cgi_pattern, hc->expnfilename ) )
	return cgi( hc );

    /* It's not CGI.  If it's executable or there's pathinfo, someone's
    ** trying to either serve or run a non-CGI file as CGI.   Either case
    ** is prohibited.
    */
    if ( hc->sb.st_mode & S_IXOTH )
	{
	syslog(
	    LOG_NOTICE, "%.80s URL \"%.80s\" is executable but isn't CGI",
	    httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	httpd_send_err(
	    hc, 403, err403title, "",
	    ERROR_FORM( err403form, "The requested URL '%.80s' resolves to a file which is marked executable but is not a CGI file; retrieving it is forbidden.\n" ),
	    hc->encodedurl );
	return -1;
	}
    if ( hc->pathinfo[0] != '\0' )
	{
	syslog(
	    LOG_INFO, "%.80s URL \"%.80s\" has pathinfo but isn't CGI",
	    httpd_ntoa( &hc->client_addr ), hc->encodedurl );
	httpd_send_err(
	    hc, 403, err403title, "",
	    ERROR_FORM( err403form, "The requested URL '%.80s' resolves to a file plus CGI-style pathinfo, but the file is not a valid CGI file.\n" ),
	    hc->encodedurl );
	return -1;
	}

    /* Fill in last_byte_index, if necessary. */
    if ( hc->got_range &&
	 ( hc->last_byte_index == -1 || hc->last_byte_index >= hc->sb.st_size ) )
	hc->last_byte_index = hc->sb.st_size - 1;

    figure_mime( hc );

    if ( hc->method == METHOD_HEAD )
	{
	send_mime(
	    hc, 200, ok200title, hc->encodings, "", hc->type, hc->sb.st_size,
	    hc->sb.st_mtime );
	}
    else if ( hc->if_modified_since != (time_t) -1 &&
	 hc->if_modified_since >= hc->sb.st_mtime )
	{
	send_mime(
	    hc, 304, err304title, hc->encodings, "", hc->type, (off_t) -1,
	    hc->sb.st_mtime );
	}
    else
	{
	hc->file_address = mmc_map( hc->expnfilename, &(hc->sb), nowP );
	if ( hc->file_address == (char*) 0 )
	    {
	    httpd_send_err( hc, 500, err500title, "", err500form, hc->encodedurl );
	    return -1;
	    }
	send_mime(
	    hc, 200, ok200title, hc->encodings, "", hc->type, hc->sb.st_size,
	    hc->sb.st_mtime );
	}

    return 0;
    }
