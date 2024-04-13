psf_is_pipe (SF_PRIVATE *psf)
{
	if (psf->virtual_io)
		return SF_FALSE ;

	if (GetFileType (psf->file.handle) == FILE_TYPE_DISK)
		return SF_FALSE ;

	/* Default to maximum safety. */
	return SF_TRUE ;
} /* psf_is_pipe */
