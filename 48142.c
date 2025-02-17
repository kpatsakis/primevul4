psf_get_filelen_fd (int fd)
{
#if (SIZEOF_OFF_T == 4 && SIZEOF_SF_COUNT_T == 8 && HAVE_FSTAT64)
	struct stat64 statbuf ;

	if (fstat64 (fd, &statbuf) == -1)
		return (sf_count_t) -1 ;

	return statbuf.st_size ;
#else
	struct stat statbuf ;

	if (fstat (fd, &statbuf) == -1)
		return (sf_count_t) -1 ;

	return statbuf.st_size ;
#endif
} /* psf_get_filelen_fd */
