httpd_unlisten( httpd_server* hs )
    {
    if ( hs->listen4_fd != -1 )
	{
	(void) close( hs->listen4_fd );
	hs->listen4_fd = -1;
	}
    if ( hs->listen6_fd != -1 )
	{
	(void) close( hs->listen6_fd );
	hs->listen6_fd = -1;
	}
    }
