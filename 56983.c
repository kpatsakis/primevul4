cgi_kill( ClientData client_data, struct timeval* nowP )
    {
    pid_t pid;

    pid = (pid_t) client_data.i;
    if ( kill( pid, SIGINT ) == 0 )
	{
	syslog( LOG_ERR, "killed CGI process %d", pid );
	/* In case this isn't enough, schedule an uncatchable kill. */
	if ( tmr_create( nowP, cgi_kill2, client_data, 5 * 1000L, 0 ) == (Timer*) 0 )
	    {
	    syslog( LOG_CRIT, "tmr_create(cgi_kill2) failed" );
	    exit( 1 );
	    }
	}
    }
