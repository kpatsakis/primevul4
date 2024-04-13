auth_check( httpd_conn* hc, char* dirname  )
    {
    if ( hc->hs->global_passwd )
	{
	char* topdir;
	if ( hc->hs->vhost && hc->hostdir[0] != '\0' )
	    topdir = hc->hostdir;
	else
	    topdir = ".";
	switch ( auth_check2( hc, topdir ) )
	    {
	    case -1:
	    return -1;
	    case 1:
	    return 1;
	    }
	}
    return auth_check2( hc, dirname );
    }
