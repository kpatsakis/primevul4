psf_close_rsrc (SF_PRIVATE *psf)
{	psf_close_handle (psf->rsrc.handle) ;
	psf->rsrc.handle = NULL ;
	return 0 ;
} /* psf_close_rsrc */
