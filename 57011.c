initialize_listen_socket( httpd_sockaddr* saP )
    {
    int listen_fd;
    int on, flags;

    /* Check sockaddr. */
    if ( ! sockaddr_check( saP ) )
	{
	syslog( LOG_CRIT, "unknown sockaddr family on listen socket" );
	return -1;
	}

    /* Create socket. */
    listen_fd = socket( saP->sa.sa_family, SOCK_STREAM, 0 );
    if ( listen_fd < 0 )
	{
	syslog( LOG_CRIT, "socket %.80s - %m", httpd_ntoa( saP ) );
	return -1;
	}
    (void) fcntl( listen_fd, F_SETFD, 1 );

    /* Allow reuse of local addresses. */
    on = 1;
    if ( setsockopt(
	     listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &on,
	     sizeof(on) ) < 0 )
	syslog( LOG_CRIT, "setsockopt SO_REUSEADDR - %m" );

    /* Bind to it. */
    if ( bind( listen_fd, &saP->sa, sockaddr_len( saP ) ) < 0 )
	{
	syslog(
	    LOG_CRIT, "bind %.80s - %m", httpd_ntoa( saP ) );
	(void) close( listen_fd );
	return -1;
	}

    /* Set the listen file descriptor to no-delay / non-blocking mode. */
    flags = fcntl( listen_fd, F_GETFL, 0 );
    if ( flags == -1 )
	{
	syslog( LOG_CRIT, "fcntl F_GETFL - %m" );
	(void) close( listen_fd );
	return -1;
	}
    if ( fcntl( listen_fd, F_SETFL, flags | O_NDELAY ) < 0 )
	{
	syslog( LOG_CRIT, "fcntl O_NDELAY - %m" );
	(void) close( listen_fd );
	return -1;
	}

    /* Start a listen going. */
    if ( listen( listen_fd, LISTEN_BACKLOG ) < 0 )
	{
	syslog( LOG_CRIT, "listen - %m" );
	(void) close( listen_fd );
	return -1;
	}

    /* Use accept filtering, if available. */
#ifdef SO_ACCEPTFILTER
    {
#if ( __FreeBSD_version >= 411000 )
#define ACCEPT_FILTER_NAME "httpready"
#else
#define ACCEPT_FILTER_NAME "dataready"
#endif
    struct accept_filter_arg af;
    (void) bzero( &af, sizeof(af) );
    (void) strcpy( af.af_name, ACCEPT_FILTER_NAME );
    (void) setsockopt(
	listen_fd, SOL_SOCKET, SO_ACCEPTFILTER, (char*) &af, sizeof(af) );
    }
#endif /* SO_ACCEPTFILTER */

    return listen_fd;
    }
