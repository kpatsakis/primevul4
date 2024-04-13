post_post_garbage_hack( httpd_conn* hc )
    {
    char buf[2];

    /* If we are in a sub-process, turn on no-delay mode in case we
    ** previously cleared it.
    */
    if ( sub_process )
	httpd_set_ndelay( hc->conn_fd );
    /* And read up to 2 bytes. */
    (void) read( hc->conn_fd, buf, sizeof(buf) );
    }
