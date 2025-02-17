psf_log_syserr (SF_PRIVATE *psf, int error)
{
	/* Only log an error if no error has been set yet. */
	if (psf->error == 0)
	{	psf->error = SFE_SYSTEM ;
		snprintf (psf->syserr, sizeof (psf->syserr), "System error : %s.", strerror (error)) ;
		} ;

	return ;
} /* psf_log_syserr */
