send_err_file( httpd_conn* hc, int status, char* title, char* extraheads, char* filename )
    {
    FILE* fp;
    char buf[1000];
    size_t r;

    fp = fopen( filename, "r" );
    if ( fp == (FILE*) 0 )
	return 0;
    send_mime(
	hc, status, title, "", extraheads, "text/html; charset=%s", (off_t) -1,
	(time_t) 0 );
    for (;;)
	{
	r = fread( buf, 1, sizeof(buf) - 1, fp );
	if ( r == 0 )
	    break;
	buf[r] = '\0';
	add_response( hc, buf );
	}
    (void) fclose( fp );

#ifdef ERR_APPEND_SERVER_INFO
    send_response_tail( hc );
#endif /* ERR_APPEND_SERVER_INFO */

    return 1;
    }
