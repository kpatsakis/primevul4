send_dirredirect( httpd_conn* hc )
    {
    static char* location;
    static char* header;
    static size_t maxlocation = 0, maxheader = 0;
    static char headstr[] = "Location: ";

    if ( hc->query[0] != '\0')
	{
	char* cp = strchr( hc->encodedurl, '?' );
	if ( cp != (char*) 0 )	/* should always find it */
	    *cp = '\0';
	httpd_realloc_str(
	    &location, &maxlocation,
	    strlen( hc->encodedurl ) + 2 + strlen( hc->query ) );
	(void) my_snprintf( location, maxlocation,
	    "%s/?%s", hc->encodedurl, hc->query );
	}
    else
	{
	httpd_realloc_str(
	    &location, &maxlocation, strlen( hc->encodedurl ) + 1 );
	(void) my_snprintf( location, maxlocation,
	    "%s/", hc->encodedurl );
	}
    httpd_realloc_str(
	&header, &maxheader, sizeof(headstr) + strlen( location ) );
    (void) my_snprintf( header, maxheader,
	"%s%s\015\012", headstr, location );
    send_response( hc, 302, err302title, header, err302form, location );
    }
