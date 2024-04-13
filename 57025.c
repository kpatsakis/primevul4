tilde_map_1( httpd_conn* hc )
    {
    static char* temp;
    static size_t maxtemp = 0;
    int len;
    static char* prefix = TILDE_MAP_1;

    len = strlen( hc->expnfilename ) - 1;
    httpd_realloc_str( &temp, &maxtemp, len );
    (void) strcpy( temp, &hc->expnfilename[1] );
    httpd_realloc_str(
	&hc->expnfilename, &hc->maxexpnfilename, strlen( prefix ) + 1 + len );
    (void) strcpy( hc->expnfilename, prefix );
    if ( prefix[0] != '\0' )
	(void) strcat( hc->expnfilename, "/" );
    (void) strcat( hc->expnfilename, temp );
    return 1;
    }
