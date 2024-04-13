psf_fopen (SF_PRIVATE *psf)
{
	psf->error = 0 ;
	psf->file.filedes = psf_open_fd (&psf->file) ;

	if (psf->file.filedes == - SFE_BAD_OPEN_MODE)
	{	psf->error = SFE_BAD_OPEN_MODE ;
		psf->file.filedes = -1 ;
		return psf->error ;
		} ;

	if (psf->file.filedes == -1)
		psf_log_syserr (psf, errno) ;

	return psf->error ;
} /* psf_fopen */
