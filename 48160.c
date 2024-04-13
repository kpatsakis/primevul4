psf_use_rsrc (SF_PRIVATE *psf, int on_off)
{
	if (on_off)
	{	if (psf->file.handle != psf->rsrc.handle)
		{	psf->file.hsaved = psf->file.handle ;
			psf->file.handle = psf->rsrc.handle ;
			} ;
		}
	else if (psf->file.handle == psf->rsrc.handle)
		psf->file.handle = psf->file.hsaved ;

	return ;
} /* psf_use_rsrc */
