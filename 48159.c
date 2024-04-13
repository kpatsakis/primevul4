psf_use_rsrc (SF_PRIVATE *psf, int on_off)
{
	if (on_off)
	{	if (psf->file.filedes != psf->rsrc.filedes)
		{	psf->file.savedes = psf->file.filedes ;
			psf->file.filedes = psf->rsrc.filedes ;
			} ;
		}
	else if (psf->file.filedes == psf->rsrc.filedes)
		psf->file.filedes = psf->file.savedes ;

	return ;
} /* psf_use_rsrc */
