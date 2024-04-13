httpd_read_fully( int fd, void* buf, size_t nbytes )
    {
    int nread;

    nread = 0;
    while ( nread < nbytes )
	{
	int r;

	r = read( fd, (char*) buf + nread, nbytes - nread );
	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
	    {
	    sleep( 1 );
	    continue;
	    }
	if ( r < 0 )
	    return r;
	if ( r == 0 )
	    break;
	nread += r;
	}

    return nread;
    }
