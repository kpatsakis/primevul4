psf_is_pipe (SF_PRIVATE *psf)
{	struct stat statbuf ;

	if (psf->virtual_io)
		return SF_FALSE ;

	/* Not sure if this works. */
	if (fstat (psf->file.filedes, &statbuf) == -1)
	{	psf_log_syserr (psf, errno) ;
		/* Default to maximum safety. */
		return SF_TRUE ;
		} ;

	/* These macros are defined in Win32/unistd.h. */
	if (S_ISFIFO (statbuf.st_mode) || S_ISSOCK (statbuf.st_mode))
		return SF_TRUE ;

	return SF_FALSE ;
} /* psf_checkpipe */
