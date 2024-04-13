bufgets( httpd_conn* hc )
    {
    int i;
    char c;

    for ( i = hc->checked_idx; hc->checked_idx < hc->read_idx; ++hc->checked_idx )
	{
	c = hc->read_buf[hc->checked_idx];
	if ( c == '\012' || c == '\015' )
	    {
	    hc->read_buf[hc->checked_idx] = '\0';
	    ++hc->checked_idx;
	    if ( c == '\015' && hc->checked_idx < hc->read_idx &&
		 hc->read_buf[hc->checked_idx] == '\012' )
		{
		hc->read_buf[hc->checked_idx] = '\0';
		++hc->checked_idx;
		}
	    return &(hc->read_buf[i]);
	    }
	}
    return (char*) 0;
    }
