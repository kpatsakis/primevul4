httpd_set_logfp( httpd_server* hs, FILE* logfp )
    {
    if ( hs->logfp != (FILE*) 0 )
	(void) fclose( hs->logfp );
    hs->logfp = logfp;
    }
