ls( httpd_conn* hc )
    {
    DIR* dirp;
    struct dirent* de;
    int namlen;
    static int maxnames = 0;
    int nnames;
    static char* names;
    static char** nameptrs;
    static char* name;
    static size_t maxname = 0;
    static char* rname;
    static size_t maxrname = 0;
    static char* encrname;
    static size_t maxencrname = 0;
    FILE* fp;
    int i, r;
    struct stat sb;
    struct stat lsb;
    char modestr[20];
    char* linkprefix;
    char link[MAXPATHLEN+1];
    int linklen;
    char* fileclass;
    time_t now;
    char* timestr;
    ClientData client_data;

    dirp = opendir( hc->expnfilename );
    if ( dirp == (DIR*) 0 )
	{
	syslog( LOG_ERR, "opendir %.80s - %m", hc->expnfilename );
	httpd_send_err( hc, 404, err404title, "", err404form, hc->encodedurl );
	return -1;
	}

    if ( hc->method == METHOD_HEAD )
	{
	closedir( dirp );
	send_mime(
	    hc, 200, ok200title, "", "", "text/html; charset=%s", (off_t) -1,
	    hc->sb.st_mtime );
	}
    else if ( hc->method == METHOD_GET )
	{
	if ( hc->hs->cgi_limit != 0 && hc->hs->cgi_count >= hc->hs->cgi_limit )
	    {
	    closedir( dirp );
	    httpd_send_err(
		hc, 503, httpd_err503title, "", httpd_err503form,
		hc->encodedurl );
	    return -1;
	    }
	++hc->hs->cgi_count;
	r = fork( );
	if ( r < 0 )
	    {
	    syslog( LOG_ERR, "fork - %m" );
	    closedir( dirp );
	    httpd_send_err(
		hc, 500, err500title, "", err500form, hc->encodedurl );
	    return -1;
	    }
	if ( r == 0 )
	    {
	    /* Child process. */
	    sub_process = 1;
	    httpd_unlisten( hc->hs );
	    send_mime(
		hc, 200, ok200title, "", "", "text/html; charset=%s",
		(off_t) -1, hc->sb.st_mtime );
	    httpd_write_response( hc );

#ifdef CGI_NICE
	    /* Set priority. */
	    (void) nice( CGI_NICE );
#endif /* CGI_NICE */

	    /* Open a stdio stream so that we can use fprintf, which is more
	    ** efficient than a bunch of separate write()s.  We don't have
	    ** to worry about double closes or file descriptor leaks cause
	    ** we're in a subprocess.
	    */
	    fp = fdopen( hc->conn_fd, "w" );
	    if ( fp == (FILE*) 0 )
		{
		syslog( LOG_ERR, "fdopen - %m" );
		httpd_send_err(
		    hc, 500, err500title, "", err500form, hc->encodedurl );
		httpd_write_response( hc );
		closedir( dirp );
		exit( 1 );
		}

	    (void) fprintf( fp, "\
<HTML>\n\
<HEAD><TITLE>Index of %.80s</TITLE></HEAD>\n\
<BODY BGCOLOR=\"#99cc99\" TEXT=\"#000000\" LINK=\"#2020ff\" VLINK=\"#4040cc\">\n\
<H2>Index of %.80s</H2>\n\
<PRE>\n\
mode  links  bytes  last-changed  name\n\
<HR>",
		hc->encodedurl, hc->encodedurl );

	    /* Read in names. */
	    nnames = 0;
	    while ( ( de = readdir( dirp ) ) != 0 )     /* dirent or direct */
		{
		if ( nnames >= maxnames )
		    {
		    if ( maxnames == 0 )
			{
			maxnames = 100;
			names = NEW( char, maxnames * ( MAXPATHLEN + 1 ) );
			nameptrs = NEW( char*, maxnames );
			}
		    else
			{
			maxnames *= 2;
			names = RENEW( names, char, maxnames * ( MAXPATHLEN + 1 ) );
			nameptrs = RENEW( nameptrs, char*, maxnames );
			}
		    if ( names == (char*) 0 || nameptrs == (char**) 0 )
			{
			syslog( LOG_ERR, "out of memory reallocating directory names" );
			exit( 1 );
			}
		    for ( i = 0; i < maxnames; ++i )
			nameptrs[i] = &names[i * ( MAXPATHLEN + 1 )];
		    }
		namlen = NAMLEN(de);
		(void) strncpy( nameptrs[nnames], de->d_name, namlen );
		nameptrs[nnames][namlen] = '\0';
		++nnames;
		}
	    closedir( dirp );

	    /* Sort the names. */
	    qsort( nameptrs, nnames, sizeof(*nameptrs), name_compare );

	    /* Generate output. */
	    for ( i = 0; i < nnames; ++i )
		{
		httpd_realloc_str(
		    &name, &maxname,
		    strlen( hc->expnfilename ) + 1 + strlen( nameptrs[i] ) );
		httpd_realloc_str(
		    &rname, &maxrname,
		    strlen( hc->origfilename ) + 1 + strlen( nameptrs[i] ) );
		if ( hc->expnfilename[0] == '\0' ||
		     strcmp( hc->expnfilename, "." ) == 0 )
		    {
		    (void) strcpy( name, nameptrs[i] );
		    (void) strcpy( rname, nameptrs[i] );
		    }
		else
		    {
		    (void) my_snprintf( name, maxname,
			"%s/%s", hc->expnfilename, nameptrs[i] );
		    if ( strcmp( hc->origfilename, "." ) == 0 )
			(void) my_snprintf( rname, maxrname,
			    "%s", nameptrs[i] );
		    else
			(void) my_snprintf( rname, maxrname,
			    "%s%s", hc->origfilename, nameptrs[i] );
		    }
		httpd_realloc_str(
		    &encrname, &maxencrname, 3 * strlen( rname ) + 1 );
		strencode( encrname, maxencrname, rname );

		if ( stat( name, &sb ) < 0 || lstat( name, &lsb ) < 0 )
		    continue;

		linkprefix = "";
		link[0] = '\0';
		/* Break down mode word.  First the file type. */
		switch ( lsb.st_mode & S_IFMT )
		    {
		    case S_IFIFO:  modestr[0] = 'p'; break;
		    case S_IFCHR:  modestr[0] = 'c'; break;
		    case S_IFDIR:  modestr[0] = 'd'; break;
		    case S_IFBLK:  modestr[0] = 'b'; break;
		    case S_IFREG:  modestr[0] = '-'; break;
		    case S_IFSOCK: modestr[0] = 's'; break;
		    case S_IFLNK:  modestr[0] = 'l';
		    linklen = readlink( name, link, sizeof(link) - 1 );
		    if ( linklen != -1 )
			{
			link[linklen] = '\0';
			linkprefix = " -&gt; ";
			}
		    break;
		    default:       modestr[0] = '?'; break;
		    }
		/* Now the world permissions.  Owner and group permissions
		** are not of interest to web clients.
		*/
		modestr[1] = ( lsb.st_mode & S_IROTH ) ? 'r' : '-';
		modestr[2] = ( lsb.st_mode & S_IWOTH ) ? 'w' : '-';
		modestr[3] = ( lsb.st_mode & S_IXOTH ) ? 'x' : '-';
		modestr[4] = '\0';

		/* We also leave out the owner and group name, they are
		** also not of interest to web clients.  Plus if we're
		** running under chroot(), they would require a copy
		** of /etc/passwd and /etc/group, which we want to avoid.
		*/

		/* Get time string. */
		now = time( (time_t*) 0 );
		timestr = ctime( &lsb.st_mtime );
		timestr[ 0] = timestr[ 4];
		timestr[ 1] = timestr[ 5];
		timestr[ 2] = timestr[ 6];
		timestr[ 3] = ' ';
		timestr[ 4] = timestr[ 8];
		timestr[ 5] = timestr[ 9];
		timestr[ 6] = ' ';
		if ( now - lsb.st_mtime > 60*60*24*182 )        /* 1/2 year */
		    {
		    timestr[ 7] = ' ';
		    timestr[ 8] = timestr[20];
		    timestr[ 9] = timestr[21];
		    timestr[10] = timestr[22];
		    timestr[11] = timestr[23];
		    }
		else
		    {
		    timestr[ 7] = timestr[11];
		    timestr[ 8] = timestr[12];
		    timestr[ 9] = ':';
		    timestr[10] = timestr[14];
		    timestr[11] = timestr[15];
		    }
		timestr[12] = '\0';

		/* The ls -F file class. */
		switch ( sb.st_mode & S_IFMT )
		    {
		    case S_IFDIR:  fileclass = "/"; break;
		    case S_IFSOCK: fileclass = "="; break;
		    case S_IFLNK:  fileclass = "@"; break;
		    default:
		    fileclass = ( sb.st_mode & S_IXOTH ) ? "*" : "";
		    break;
		    }

		/* And print. */
		(void)  fprintf( fp,
		   "%s %3ld  %10lld  %s  <A HREF=\"/%.500s%s\">%.80s</A>%s%s%s\n",
		    modestr, (long) lsb.st_nlink, (int64_t) lsb.st_size,
		    timestr, encrname, S_ISDIR(sb.st_mode) ? "/" : "",
		    nameptrs[i], linkprefix, link, fileclass );
		}

	    (void) fprintf( fp, "</PRE></BODY>\n</HTML>\n" );
	    (void) fclose( fp );
	    exit( 0 );
	    }

	/* Parent process. */
	closedir( dirp );
	syslog( LOG_INFO, "spawned indexing process %d for directory '%.200s'", r, hc->expnfilename );
#ifdef CGI_TIMELIMIT
	/* Schedule a kill for the child process, in case it runs too long */
	client_data.i = r;
	if ( tmr_create( (struct timeval*) 0, cgi_kill, client_data, CGI_TIMELIMIT * 1000L, 0 ) == (Timer*) 0 )
	    {
	    syslog( LOG_CRIT, "tmr_create(cgi_kill ls) failed" );
	    exit( 1 );
	    }
#endif /* CGI_TIMELIMIT */
	hc->status = 200;
	hc->bytes_sent = CGI_BYTECOUNT;
	hc->should_linger = 0;
	}
    else
	{
	closedir( dirp );
	httpd_send_err(
	    hc, 501, err501title, "", err501form, httpd_method_str( hc->method ) );
	return -1;
	}

    return 0;
    }
