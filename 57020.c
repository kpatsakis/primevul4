send_response( httpd_conn* hc, int status, char* title, char* extraheads, char* form, char* arg )
    {
    char defanged_arg[1000], buf[2000];

    send_mime(
	hc, status, title, "", extraheads, "text/html; charset=%s", (off_t) -1,
	(time_t) 0 );
    (void) my_snprintf( buf, sizeof(buf), "\
<HTML>\n\
<HEAD><TITLE>%d %s</TITLE></HEAD>\n\
<BODY BGCOLOR=\"#cc9999\" TEXT=\"#000000\" LINK=\"#2020ff\" VLINK=\"#4040cc\">\n\
<H2>%d %s</H2>\n",
	status, title, status, title );
    add_response( hc, buf );
    defang( arg, defanged_arg, sizeof(defanged_arg) );
    (void) my_snprintf( buf, sizeof(buf), form, defanged_arg );
    add_response( hc, buf );
    if ( match( "**MSIE**", hc->useragent ) )
	{
	int n;
	add_response( hc, "<!--\n" );
	for ( n = 0; n < 6; ++n )
	    add_response( hc, "Padding so that MSIE deigns to show this error instead of its own canned one.\n");
	add_response( hc, "-->\n" );
	}
    send_response_tail( hc );
    }
