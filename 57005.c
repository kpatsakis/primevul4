httpd_start_request( httpd_conn* hc, struct timeval* nowP )
    {
    int r;

    /* Really start the request. */
    r = really_start_request( hc, nowP );

    /* And return the status. */
    return r;
    }
