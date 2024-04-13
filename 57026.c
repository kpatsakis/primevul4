tilde_map_2( httpd_conn* hc )
    {
    static char* temp;
    static size_t maxtemp = 0;
    static char* postfix = TILDE_MAP_2;
    char* cp;
    struct passwd* pw;
    char* alt;
    char* rest;

    /* Get the username. */
    httpd_realloc_str( &temp, &maxtemp, strlen( hc->expnfilename ) - 1 );
    (void) strcpy( temp, &hc->expnfilename[1] );
    cp = strchr( temp, '/' );
    if ( cp != (char*) 0 )
	*cp++ = '\0';
    else
	cp = "";

    /* Get the passwd entry. */
    pw = getpwnam( temp );
    if ( pw == (struct passwd*) 0 )
	return 0;

    /* Set up altdir. */
    httpd_realloc_str(
	&hc->altdir, &hc->maxaltdir,
	strlen( pw->pw_dir ) + 1 + strlen( postfix ) );
    (void) strcpy( hc->altdir, pw->pw_dir );
    if ( postfix[0] != '\0' )
	{
	(void) strcat( hc->altdir, "/" );
	(void) strcat( hc->altdir, postfix );
	}
    alt = expand_symlinks( hc->altdir, &rest, 0, 1 );
    if ( rest[0] != '\0' )
	return 0;
    httpd_realloc_str( &hc->altdir, &hc->maxaltdir, strlen( alt ) );
    (void) strcpy( hc->altdir, alt );

    /* And the filename becomes altdir plus the post-~ part of the original. */
    httpd_realloc_str(
	&hc->expnfilename, &hc->maxexpnfilename,
	strlen( hc->altdir ) + 1 + strlen( cp ) );
    (void) my_snprintf( hc->expnfilename, hc->maxexpnfilename,
	"%s/%s", hc->altdir, cp );

    /* For this type of tilde mapping, we want to defeat vhost mapping. */
    hc->tildemapped = 1;

    return 1;
    }
