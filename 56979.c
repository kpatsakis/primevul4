build_env( char* fmt, char* arg )
    {
    char* cp;
    size_t size;
    static char* buf;
    static size_t maxbuf = 0;

    size = strlen( fmt ) + strlen( arg );
    if ( size > maxbuf )
	httpd_realloc_str( &buf, &maxbuf, size );
    (void) my_snprintf( buf, maxbuf, fmt, arg );
    cp = strdup( buf );
    if ( cp == (char*) 0 )
	{
	syslog( LOG_ERR, "out of memory copying environment variable" );
	exit( 1 );
	}
    return cp;
    }
