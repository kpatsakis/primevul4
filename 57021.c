send_response_tail( httpd_conn* hc )
    {
    char buf[1000];

    (void) my_snprintf( buf, sizeof(buf), "\
<HR>\n\
<ADDRESS><A HREF=\"%s\">%s</A></ADDRESS>\n\
</BODY>\n\
</HTML>\n",
	SERVER_ADDRESS, EXPOSED_SERVER_SOFTWARE );
    add_response( hc, buf );
    }
