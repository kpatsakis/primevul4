httpd_write_response( httpd_conn* hc )
    {
    /* If we are in a sub-process, turn off no-delay mode. */
    if ( sub_process )
	httpd_clear_ndelay( hc->conn_fd );
    /* Send the response, if necessary. */
    if ( hc->responselen > 0 )
	{
	(void) httpd_write_fully( hc->conn_fd, hc->response, hc->responselen );
	hc->responselen = 0;
	}
    }
