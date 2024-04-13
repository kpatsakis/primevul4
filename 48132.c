psf_fsync (SF_PRIVATE *psf)
{
#if HAVE_FSYNC
	if (psf->file.mode == SFM_WRITE || psf->file.mode == SFM_RDWR)
		fsync (psf->file.filedes) ;
#else
	psf = NULL ;
#endif
} /* psf_fsync */
