flac_seek (SF_PRIVATE *psf, int UNUSED (mode), sf_count_t offset)
{	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;

	if (pflac == NULL)
		return 0 ;

	if (psf->dataoffset < 0)
	{	psf->error = SFE_BAD_SEEK ;
		return ((sf_count_t) -1) ;
		} ;

	pflac->frame = NULL ;

	if (psf->file.mode == SFM_READ)
	{	if (FLAC__stream_decoder_seek_absolute (pflac->fsd, offset))
			return offset ;

		if (offset == psf->sf.frames)
		{	/*
			** If we've been asked to seek to the very end of the file, libFLAC
			** will return an error. However, we know the length of the file so
			** instead of returning an error, we can return the offset.
			*/
			return offset ;
			} ;

		psf->error = SFE_BAD_SEEK ;
		return ((sf_count_t) -1) ;
		} ;

	/* Seeking in write mode not yet supported. */
	psf->error = SFE_BAD_SEEK ;

	return ((sf_count_t) -1) ;
} /* flac_seek */
