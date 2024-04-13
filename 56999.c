httpd_logstats( long secs )
    {
    if ( str_alloc_count > 0 )
	syslog( LOG_INFO,
	    "  libhttpd - %d strings allocated, %lu bytes (%g bytes/str)",
	    str_alloc_count, (unsigned long) str_alloc_size,
	    (float) str_alloc_size / str_alloc_count );
    }
