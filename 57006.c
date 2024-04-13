httpd_terminate( httpd_server* hs )
    {
    httpd_unlisten( hs );
    if ( hs->logfp != (FILE*) 0 )
	(void) fclose( hs->logfp );
    free_httpd_server( hs );
    }
