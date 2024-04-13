psf_close_rsrc (SF_PRIVATE *psf)
{	psf_close_fd (psf->rsrc.filedes) ;
	psf->rsrc.filedes = -1 ;
	return 0 ;
} /* psf_close_rsrc */
