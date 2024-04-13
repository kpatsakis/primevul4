cgi_interpose_output( httpd_conn* hc, int rfd )
    {
    int r;
    char buf[1024];
    size_t headers_size, headers_len;
    char* headers;
    char* br;
    int status;
    char* title;
    char* cp;

    /* Make sure the connection is in blocking mode.  It should already
    ** be blocking, but we might as well be sure.
    */
    httpd_clear_ndelay( hc->conn_fd );

    /* Slurp in all headers. */
    headers_size = 0;
    httpd_realloc_str( &headers, &headers_size, 500 );
    headers_len = 0;
    for (;;)
	{
	r = read( rfd, buf, sizeof(buf) );
	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
	    {
	    sleep( 1 );
	    continue;
	    }
	if ( r <= 0 )
	    {
	    br = &(headers[headers_len]);
	    break;
	    }
	httpd_realloc_str( &headers, &headers_size, headers_len + r );
	(void) memmove( &(headers[headers_len]), buf, r );
	headers_len += r;
	headers[headers_len] = '\0';
	if ( ( br = strstr( headers, "\015\012\015\012" ) ) != (char*) 0 ||
	     ( br = strstr( headers, "\012\012" ) ) != (char*) 0 )
	    break;
	}

    /* If there were no headers, bail. */
    if ( headers[0] == '\0' )
	return;

    /* Figure out the status.  Look for a Status: or Location: header;
    ** else if there's an HTTP header line, get it from there; else
    ** default to 200.
    */
    status = 200;
    if ( strncmp( headers, "HTTP/", 5 ) == 0 )
	{
	cp = headers;
	cp += strcspn( cp, " \t" );
	status = atoi( cp );
	}
    if ( ( cp = strstr( headers, "Status:" ) ) != (char*) 0 &&
	 cp < br &&
	 ( cp == headers || *(cp-1) == '\012' ) )
	{
	cp += 7;
	cp += strspn( cp, " \t" );
	status = atoi( cp );
	}
    else if ( ( cp = strstr( headers, "Location:" ) ) != (char*) 0 &&
	 cp < br &&
	 ( cp == headers || *(cp-1) == '\012' ) )
	status = 302;

    /* Write the status line. */
    switch ( status )
	{
	case 200: title = ok200title; break;
	case 302: title = err302title; break;
	case 304: title = err304title; break;
	case 400: title = httpd_err400title; break;
#ifdef AUTH_FILE
	case 401: title = err401title; break;
#endif /* AUTH_FILE */
	case 403: title = err403title; break;
	case 404: title = err404title; break;
	case 408: title = httpd_err408title; break;
	case 500: title = err500title; break;
	case 501: title = err501title; break;
	case 503: title = httpd_err503title; break;
	default: title = "Something"; break;
	}
    (void) my_snprintf( buf, sizeof(buf), "HTTP/1.0 %d %s\015\012", status, title );
    (void) httpd_write_fully( hc->conn_fd, buf, strlen( buf ) );

    /* Write the saved headers. */
    (void) httpd_write_fully( hc->conn_fd, headers, headers_len );

    /* Echo the rest of the output. */
    for (;;)
	{
	r = read( rfd, buf, sizeof(buf) );
	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
	    {
	    sleep( 1 );
	    continue;
	    }
	if ( r <= 0 )
	    break;
	if ( httpd_write_fully( hc->conn_fd, buf, r ) != r )
	    break;
	}
    shutdown( hc->conn_fd, SHUT_WR );
    }
