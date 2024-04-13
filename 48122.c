psf_file_valid (SF_PRIVATE *psf)
{	if (psf->file.handle == NULL)
		return SF_FALSE ;
	if (psf->file.handle == INVALID_HANDLE_VALUE)
		return SF_FALSE ;
	return SF_TRUE ;
} /* psf_set_file */
