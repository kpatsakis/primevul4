check_referer( httpd_conn* hc )
    {
    int r;
    char* cp;

    /* Are we doing referer checking at all? */
    if ( hc->hs->url_pattern == (char*) 0 )
	return 1;

    r = really_check_referer( hc );

    if ( ! r )
	{
	if ( hc->hs->vhost && hc->hostname != (char*) 0 )
	    cp = hc->hostname;
	else
	    cp = hc->hs->server_hostname;
	if ( cp == (char*) 0 )
	    cp = "";
	syslog(
	    LOG_INFO, "%.80s non-local referer \"%.80s%.80s\" \"%.80s\"",
	    httpd_ntoa( &hc->client_addr ), cp, hc->encodedurl, hc->referer );
	httpd_send_err(
	    hc, 403, err403title, "",
	    ERROR_FORM( err403form, "You must supply a local referer to get URL '%.80s' from this server.\n" ),
	    hc->encodedurl );
	}
    return r;
    }
