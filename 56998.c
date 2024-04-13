httpd_initialize(
    char* hostname, httpd_sockaddr* sa4P, httpd_sockaddr* sa6P,
    unsigned short port, char* cgi_pattern, int cgi_limit, char* charset,
    char* p3p, int max_age, char* cwd, int no_log, FILE* logfp,
    int no_symlink_check, int vhost, int global_passwd, char* url_pattern,
    char* local_pattern, int no_empty_referers )
    {
    httpd_server* hs;
    static char ghnbuf[256];
    char* cp;

    check_options();

    hs = NEW( httpd_server, 1 );
    if ( hs == (httpd_server*) 0 )
	{
	syslog( LOG_CRIT, "out of memory allocating an httpd_server" );
	return (httpd_server*) 0;
	}

    if ( hostname != (char*) 0 )
	{
	hs->binding_hostname = strdup( hostname );
	if ( hs->binding_hostname == (char*) 0 )
	    {
	    syslog( LOG_CRIT, "out of memory copying hostname" );
	    return (httpd_server*) 0;
	    }
	hs->server_hostname = hs->binding_hostname;
	}
    else
	{
	hs->binding_hostname = (char*) 0;
	hs->server_hostname = (char*) 0;
	if ( gethostname( ghnbuf, sizeof(ghnbuf) ) < 0 )
	    ghnbuf[0] = '\0';
#ifdef SERVER_NAME_LIST
	if ( ghnbuf[0] != '\0' )
	    hs->server_hostname = hostname_map( ghnbuf );
#endif /* SERVER_NAME_LIST */
	if ( hs->server_hostname == (char*) 0 )
	    {
#ifdef SERVER_NAME
	    hs->server_hostname = SERVER_NAME;
#else /* SERVER_NAME */
	    if ( ghnbuf[0] != '\0' )
		hs->server_hostname = ghnbuf;
#endif /* SERVER_NAME */
	    }
	}

    hs->port = port;
    if ( cgi_pattern == (char*) 0 )
	hs->cgi_pattern = (char*) 0;
    else
	{
	/* Nuke any leading slashes. */
	if ( cgi_pattern[0] == '/' )
	    ++cgi_pattern;
	hs->cgi_pattern = strdup( cgi_pattern );
	if ( hs->cgi_pattern == (char*) 0 )
	    {
	    syslog( LOG_CRIT, "out of memory copying cgi_pattern" );
	    return (httpd_server*) 0;
	    }
	/* Nuke any leading slashes in the cgi pattern. */
	while ( ( cp = strstr( hs->cgi_pattern, "|/" ) ) != (char*) 0 )
	    /* -2 for the offset, +1 for the '\0' */
	    (void) memmove( cp + 1, cp + 2, strlen( cp ) - 1 );
	}
    hs->cgi_limit = cgi_limit;
    hs->cgi_count = 0;
    hs->charset = strdup( charset );
    hs->p3p = strdup( p3p );
    hs->max_age = max_age;
    hs->cwd = strdup( cwd );
    if ( hs->cwd == (char*) 0 )
	{
	syslog( LOG_CRIT, "out of memory copying cwd" );
	return (httpd_server*) 0;
	}
    if ( url_pattern == (char*) 0 )
	hs->url_pattern = (char*) 0;
    else
	{
	hs->url_pattern = strdup( url_pattern );
	if ( hs->url_pattern == (char*) 0 )
	    {
	    syslog( LOG_CRIT, "out of memory copying url_pattern" );
	    return (httpd_server*) 0;
	    }
	}
    if ( local_pattern == (char*) 0 )
	hs->local_pattern = (char*) 0;
    else
	{
	hs->local_pattern = strdup( local_pattern );
	if ( hs->local_pattern == (char*) 0 )
	    {
	    syslog( LOG_CRIT, "out of memory copying local_pattern" );
	    return (httpd_server*) 0;
	    }
	}
    hs->no_log = no_log;
    hs->logfp = (FILE*) 0;
    httpd_set_logfp( hs, logfp );
    hs->no_symlink_check = no_symlink_check;
    hs->vhost = vhost;
    hs->global_passwd = global_passwd;
    hs->no_empty_referers = no_empty_referers;

    /* Initialize listen sockets.  Try v6 first because of a Linux peculiarity;
    ** like some other systems, it has magical v6 sockets that also listen for
    ** v4, but in Linux if you bind a v4 socket first then the v6 bind fails.
    */
    if ( sa6P == (httpd_sockaddr*) 0 )
	hs->listen6_fd = -1;
    else
	hs->listen6_fd = initialize_listen_socket( sa6P );
    if ( sa4P == (httpd_sockaddr*) 0 )
	hs->listen4_fd = -1;
    else
	hs->listen4_fd = initialize_listen_socket( sa4P );
    /* If we didn't get any valid sockets, fail. */
    if ( hs->listen4_fd == -1 && hs->listen6_fd == -1 )
	{
	free_httpd_server( hs );
	return (httpd_server*) 0;
	}

    init_mime();

    /* Done initializing. */
    if ( hs->binding_hostname == (char*) 0 )
	syslog(
	    LOG_NOTICE, "%.80s starting on port %d", SERVER_SOFTWARE,
	    (int) hs->port );
    else
	syslog(
	    LOG_NOTICE, "%.80s starting on %.80s, port %d", SERVER_SOFTWARE,
	    httpd_ntoa( hs->listen4_fd != -1 ? sa4P : sa6P ),
	    (int) hs->port );
    return hs;
    }
