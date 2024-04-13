psf_get_filelen (SF_PRIVATE *psf)
{
#if 0
	/*
	** Windoze is SOOOOO FUCKED!!!!!!!
	** This code should work but doesn't. Why?
	** Code below does work.
	*/
	struct _stati64 statbuf ;

	if (_fstati64 (psf->file.filedes, &statbuf))
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	return statbuf.st_size ;
#else
	sf_count_t current, filelen ;

	if (psf->virtual_io)
		return psf->vio.get_filelen (psf->vio_user_data) ;

	if ((current = _telli64 (psf->file.filedes)) < 0)
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	/*
	** Lets face it, windoze if FUBAR!!!
	**
	** For some reason, I have to call _lseeki64() TWICE to get to the
	** end of the file.
	**
	** This might have been avoided if windows had implemented the POSIX
	** standard function fsync() but NO, that would have been too easy.
	**
	** I am VERY close to saying that windoze will no longer be supported
	** by libsndfile and changing the license to GPL at the same time.
	*/

	_lseeki64 (psf->file.filedes, 0, SEEK_END) ;

	if ((filelen = _lseeki64 (psf->file.filedes, 0, SEEK_END)) < 0)
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	if (filelen > current)
		_lseeki64 (psf->file.filedes, current, SEEK_SET) ;

	switch (psf->file.mode)
	{	case SFM_WRITE :
			filelen = filelen - psf->fileoffset ;
			break ;

		case SFM_READ :
			if (psf->fileoffset > 0 && psf->filelength > 0)
				filelen = psf->filelength ;
			break ;

		case SFM_RDWR :
			/*
			** Cannot open embedded files SFM_RDWR so we don't need to
			** subtract psf->fileoffset. We already have the answer we
			** need.
			*/
			break ;

		default :
			filelen = 0 ;
		} ;

	return filelen ;
#endif
} /* psf_get_filelen */
