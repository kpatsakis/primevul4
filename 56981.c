cgi_interpose_input( httpd_conn* hc, int wfd )
    {
    size_t c;
    ssize_t r;
    char buf[1024];

    c = hc->read_idx - hc->checked_idx;
    if ( c > 0 )
	{
	if ( httpd_write_fully( wfd, &(hc->read_buf[hc->checked_idx]), c ) != c )
	    return;
	}
    while ( c < hc->contentlength )
	{
	r = read( hc->conn_fd, buf, MIN( sizeof(buf), hc->contentlength - c ) );
	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
	    {
	    sleep( 1 );
	    continue;
	    }
	if ( r <= 0 )
	    return;
	if ( httpd_write_fully( wfd, buf, r ) != r )
	    return;
	c += r;
	}
    post_post_garbage_hack( hc );
    }
