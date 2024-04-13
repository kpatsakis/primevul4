expand_symlinks( char* path, char** restP, int no_symlink_check, int tildemapped )
    {
    static char* checked;
    static char* rest;
    char link[5000];
    static size_t maxchecked = 0, maxrest = 0;
    size_t checkedlen, restlen, linklen, prevcheckedlen, prevrestlen;
    int nlinks, i;
    char* r;
    char* cp1;
    char* cp2;

    if ( no_symlink_check )
	{
	/* If we are chrooted, we can actually skip the symlink-expansion,
	** since it's impossible to get out of the tree.  However, we still
	** need to do the pathinfo check, and the existing symlink expansion
	** code is a pretty reasonable way to do this.  So, what we do is
	** a single stat() of the whole filename - if it exists, then we
	** return it as is with nothing in restP.  If it doesn't exist, we
	** fall through to the existing code.
	**
	** One side-effect of this is that users can't symlink to central
	** approved CGIs any more.  The workaround is to use the central
	** URL for the CGI instead of a local symlinked one.
	*/
	struct stat sb;
	if ( stat( path, &sb ) != -1 )
	    {
	    checkedlen = strlen( path );
	    httpd_realloc_str( &checked, &maxchecked, checkedlen );
	    (void) strcpy( checked, path );
	    /* Trim trailing slashes. */
	    while ( checkedlen && checked[checkedlen - 1] == '/' )
		{
		checked[checkedlen - 1] = '\0';
		--checkedlen;
		}
	    httpd_realloc_str( &rest, &maxrest, 0 );
	    rest[0] = '\0';
	    *restP = rest;
	    return checked;
	    }
	}

    /* Start out with nothing in checked and the whole filename in rest. */
    httpd_realloc_str( &checked, &maxchecked, 1 );
    checked[0] = '\0';
    checkedlen = 0;
    restlen = strlen( path );
    httpd_realloc_str( &rest, &maxrest, restlen );
    (void) strcpy( rest, path );
    if ( restlen && rest[restlen - 1] == '/' )
	rest[--restlen] = '\0';         /* trim trailing slash */
    if ( ! tildemapped )
	/* Remove any leading slashes. */
	while ( rest[0] == '/' )
	    {
	    /*One more for '\0', one less for the eaten first*/
	    (void) memmove( rest, &(rest[1]), strlen(rest) );
	    --restlen;
	    }
    r = rest;
    nlinks = 0;

    /* While there are still components to check... */
    while ( restlen > 0 )
	{
	/* Save current checkedlen in case we get a symlink.  Save current
	** restlen in case we get a non-existant component.
	*/
	prevcheckedlen = checkedlen;
	prevrestlen = restlen;

	/* Grab one component from r and transfer it to checked. */
	cp1 = strchr( r, '/' );
	if ( cp1 != (char*) 0 )
	    {
	    i = cp1 - r;
	    if ( i == 0 )
		{
		/* Special case for absolute paths. */
		httpd_realloc_str( &checked, &maxchecked, checkedlen + 1 );
		(void) strncpy( &checked[checkedlen], r, 1 );
		checkedlen += 1;
		}
	    else if ( strncmp( r, "..", MAX( i, 2 ) ) == 0 )
		{
		/* Ignore ..'s that go above the start of the path. */
		if ( checkedlen != 0 )
		    {
		    cp2 = strrchr( checked, '/' );
		    if ( cp2 == (char*) 0 )
			checkedlen = 0;
		    else if ( cp2 == checked )
			checkedlen = 1;
		    else
			checkedlen = cp2 - checked;
		    }
		}
	    else
		{
		httpd_realloc_str( &checked, &maxchecked, checkedlen + 1 + i );
		if ( checkedlen > 0 && checked[checkedlen-1] != '/' )
		    checked[checkedlen++] = '/';
		(void) strncpy( &checked[checkedlen], r, i );
		checkedlen += i;
		}
	    checked[checkedlen] = '\0';
	    r += i + 1;
	    restlen -= i + 1;
	    }
	else
	    {
	    /* No slashes remaining, r is all one component. */
	    if ( strcmp( r, ".." ) == 0 )
		{
		/* Ignore ..'s that go above the start of the path. */
		if ( checkedlen != 0 )
		    {
		    cp2 = strrchr( checked, '/' );
		    if ( cp2 == (char*) 0 )
			checkedlen = 0;
		    else if ( cp2 == checked )
			checkedlen = 1;
		    else
			checkedlen = cp2 - checked;
		    checked[checkedlen] = '\0';
		    }
		}
	    else
		{
		httpd_realloc_str(
		    &checked, &maxchecked, checkedlen + 1 + restlen );
		if ( checkedlen > 0 && checked[checkedlen-1] != '/' )
		    checked[checkedlen++] = '/';
		(void) strcpy( &checked[checkedlen], r );
		checkedlen += restlen;
		}
	    r += restlen;
	    restlen = 0;
	    }

	/* Try reading the current filename as a symlink */
	if ( checked[0] == '\0' )
	    continue;
	linklen = readlink( checked, link, sizeof(link) - 1 );
	if ( linklen == -1 )
	    {
	    if ( errno == EINVAL )
		continue;               /* not a symlink */
	    if ( errno == EACCES || errno == ENOENT || errno == ENOTDIR )
		{
		/* That last component was bogus.  Restore and return. */
		*restP = r - ( prevrestlen - restlen );
		if ( prevcheckedlen == 0 )
		    (void) strcpy( checked, "." );
		else
		    checked[prevcheckedlen] = '\0';
		return checked;
		}
	    syslog( LOG_ERR, "readlink %.80s - %m", checked );
	    return (char*) 0;
	    }
	++nlinks;
	if ( nlinks > MAX_LINKS )
	    {
	    syslog( LOG_ERR, "too many symlinks in %.80s", path );
	    return (char*) 0;
	    }
	link[linklen] = '\0';
	if ( link[linklen - 1] == '/' )
	    link[--linklen] = '\0';     /* trim trailing slash */

	/* Insert the link contents in front of the rest of the filename. */
	if ( restlen != 0 )
	    {
	    (void) strcpy( rest, r );
	    httpd_realloc_str( &rest, &maxrest, restlen + linklen + 1 );
	    for ( i = restlen; i >= 0; --i )
		rest[i + linklen + 1] = rest[i];
	    (void) strcpy( rest, link );
	    rest[linklen] = '/';
	    restlen += linklen + 1;
	    r = rest;
	    }
	else
	    {
	    /* There's nothing left in the filename, so the link contents
	    ** becomes the rest.
	    */
	    httpd_realloc_str( &rest, &maxrest, linklen );
	    (void) strcpy( rest, link );
	    restlen = linklen;
	    r = rest;
	    }

	if ( rest[0] == '/' )
	    {
	    /* There must have been an absolute symlink - zero out checked. */
	    checked[0] = '\0';
	    checkedlen = 0;
	    }
	else
	    {
	    /* Re-check this component. */
	    checkedlen = prevcheckedlen;
	    checked[checkedlen] = '\0';
	    }
	}

    /* Ok. */
    *restP = r;
    if ( checked[0] == '\0' )
	(void) strcpy( checked, "." );
    return checked;
    }
