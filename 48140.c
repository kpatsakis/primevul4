psf_get_filelen (SF_PRIVATE *psf)
{	sf_count_t	filelen ;

	if (psf->virtual_io)
		return psf->vio.get_filelen (psf->vio_user_data) ;

	filelen = psf_get_filelen_handle (psf->file.handle) ;

	if (filelen == -1)
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	if (filelen == -SFE_BAD_STAT_SIZE)
	{	psf->error = SFE_BAD_STAT_SIZE ;
		return (sf_count_t) -1 ;
		} ;

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
			/* Shouldn't be here, so return error. */
			filelen = -1 ;
		} ;

	return filelen ;
} /* psf_get_filelen */
