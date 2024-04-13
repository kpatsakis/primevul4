httpd_write_fully( int fd, const void* buf, size_t nbytes )
    {
    int nwritten;

    nwritten = 0;
    while ( nwritten < nbytes )
	{
	int r;

	r = write( fd, (char*) buf + nwritten, nbytes - nwritten );
	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
	    {
	    sleep( 1 );
	    continue;
	    }
	if ( r < 0 )
	    return r;
	if ( r == 0 )
	    break;
	nwritten += r;
	}

    return nwritten;
    }
