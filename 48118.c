psf_fclose (SF_PRIVATE *psf)
{	int retval ;

	if (psf->virtual_io)
		return 0 ;

	if (psf->file.do_not_close_descriptor)
	{	psf->file.handle = NULL ;
		return 0 ;
		} ;

	if ((retval = psf_close_handle (psf->file.handle)) == -1)
		psf_log_syserr (psf, GetLastError ()) ;

	psf->file.handle = NULL ;

	return retval ;
} /* psf_fclose */
