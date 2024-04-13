psf_fclose (SF_PRIVATE *psf)
{	int retval ;

	if (psf->virtual_io)
		return 0 ;

	if (psf->file.do_not_close_descriptor)
	{	psf->file.filedes = -1 ;
		return 0 ;
		} ;

	if ((retval = psf_close_fd (psf->file.filedes)) == -1)
		psf_log_syserr (psf, errno) ;

	psf->file.filedes = -1 ;

	return retval ;
} /* psf_fclose */
