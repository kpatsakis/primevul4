httpd_close_conn( httpd_conn* hc, struct timeval* nowP )
    {
    make_log_entry( hc, nowP );

    if ( hc->file_address != (char*) 0 )
	{
	mmc_unmap( hc->file_address, &(hc->sb), nowP );
	hc->file_address = (char*) 0;
	}
    if ( hc->conn_fd >= 0 )
	{
	(void) close( hc->conn_fd );
	hc->conn_fd = -1;
	}
    }
