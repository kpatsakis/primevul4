psf_fopen (SF_PRIVATE *psf)
{
	psf->error = 0 ;
	psf->file.handle = psf_open_handle (&psf->file) ;

	if (psf->file.handle == NULL)
		psf_log_syserr (psf, GetLastError ()) ;

	return psf->error ;
} /* psf_fopen */
