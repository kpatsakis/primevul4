psf_set_file (SF_PRIVATE *psf, int fd)
{	HANDLE handle ;
	intptr_t osfhandle ;

	osfhandle = _get_osfhandle (fd) ;
	handle = (HANDLE) osfhandle ;

	psf->file.handle = handle ;
} /* psf_set_file */
