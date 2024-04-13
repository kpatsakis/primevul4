httpd_get_conn( httpd_server* hs, int listen_fd, httpd_conn* hc )
    {
    httpd_sockaddr sa;
    socklen_t sz;

    if ( ! hc->initialized )
	{
	hc->read_size = 0;
	httpd_realloc_str( &hc->read_buf, &hc->read_size, 500 );
	hc->maxdecodedurl =
	    hc->maxorigfilename = hc->maxexpnfilename = hc->maxencodings =
	    hc->maxpathinfo = hc->maxquery = hc->maxaccept =
	    hc->maxaccepte = hc->maxreqhost = hc->maxhostdir =
	    hc->maxremoteuser = hc->maxresponse = 0;
#ifdef TILDE_MAP_2
	hc->maxaltdir = 0;
#endif /* TILDE_MAP_2 */
	httpd_realloc_str( &hc->decodedurl, &hc->maxdecodedurl, 1 );
	httpd_realloc_str( &hc->origfilename, &hc->maxorigfilename, 1 );
	httpd_realloc_str( &hc->expnfilename, &hc->maxexpnfilename, 0 );
	httpd_realloc_str( &hc->encodings, &hc->maxencodings, 0 );
	httpd_realloc_str( &hc->pathinfo, &hc->maxpathinfo, 0 );
	httpd_realloc_str( &hc->query, &hc->maxquery, 0 );
	httpd_realloc_str( &hc->accept, &hc->maxaccept, 0 );
	httpd_realloc_str( &hc->accepte, &hc->maxaccepte, 0 );
	httpd_realloc_str( &hc->reqhost, &hc->maxreqhost, 0 );
	httpd_realloc_str( &hc->hostdir, &hc->maxhostdir, 0 );
	httpd_realloc_str( &hc->remoteuser, &hc->maxremoteuser, 0 );
	httpd_realloc_str( &hc->response, &hc->maxresponse, 0 );
#ifdef TILDE_MAP_2
	httpd_realloc_str( &hc->altdir, &hc->maxaltdir, 0 );
#endif /* TILDE_MAP_2 */
	hc->initialized = 1;
	}

    /* Accept the new connection. */
    sz = sizeof(sa);
    hc->conn_fd = accept( listen_fd, &sa.sa, &sz );
    if ( hc->conn_fd < 0 )
	{
	if ( errno == EWOULDBLOCK )
	    return GC_NO_MORE;
	syslog( LOG_ERR, "accept - %m" );
	return GC_FAIL;
	}
    if ( ! sockaddr_check( &sa ) )
	{
	syslog( LOG_ERR, "unknown sockaddr family" );
	close( hc->conn_fd );
	hc->conn_fd = -1;
	return GC_FAIL;
	}
    (void) fcntl( hc->conn_fd, F_SETFD, 1 );
    hc->hs = hs;
    (void) memset( &hc->client_addr, 0, sizeof(hc->client_addr) );
    (void) memmove( &hc->client_addr, &sa, sockaddr_len( &sa ) );
    hc->read_idx = 0;
    hc->checked_idx = 0;
    hc->checked_state = CHST_FIRSTWORD;
    hc->method = METHOD_UNKNOWN;
    hc->status = 0;
    hc->bytes_to_send = 0;
    hc->bytes_sent = 0;
    hc->encodedurl = "";
    hc->decodedurl[0] = '\0';
    hc->protocol = "UNKNOWN";
    hc->origfilename[0] = '\0';
    hc->expnfilename[0] = '\0';
    hc->encodings[0] = '\0';
    hc->pathinfo[0] = '\0';
    hc->query[0] = '\0';
    hc->referer = "";
    hc->useragent = "";
    hc->accept[0] = '\0';
    hc->accepte[0] = '\0';
    hc->acceptl = "";
    hc->cookie = "";
    hc->contenttype = "";
    hc->reqhost[0] = '\0';
    hc->hdrhost = "";
    hc->hostdir[0] = '\0';
    hc->authorization = "";
    hc->remoteuser[0] = '\0';
    hc->response[0] = '\0';
#ifdef TILDE_MAP_2
    hc->altdir[0] = '\0';
#endif /* TILDE_MAP_2 */
    hc->responselen = 0;
    hc->if_modified_since = (time_t) -1;
    hc->range_if = (time_t) -1;
    hc->contentlength = -1;
    hc->type = "";
    hc->hostname = (char*) 0;
    hc->mime_flag = 1;
    hc->one_one = 0;
    hc->got_range = 0;
    hc->tildemapped = 0;
    hc->first_byte_index = 0;
    hc->last_byte_index = -1;
    hc->keep_alive = 0;
    hc->should_linger = 0;
    hc->file_address = (char*) 0;
    return GC_OK;
    }
