flac_close	(SF_PRIVATE *psf)
{	FLAC_PRIVATE* pflac ;
	int k ;

	if ((pflac = (FLAC_PRIVATE*) psf->codec_data) == NULL)
		return 0 ;

	if (pflac->metadata != NULL)
		FLAC__metadata_object_delete (pflac->metadata) ;

	if (psf->file.mode == SFM_WRITE)
	{	FLAC__stream_encoder_finish (pflac->fse) ;
		FLAC__stream_encoder_delete (pflac->fse) ;
		free (pflac->encbuffer) ;
		} ;

	if (psf->file.mode == SFM_READ)
	{	FLAC__stream_decoder_finish (pflac->fsd) ;
		FLAC__stream_decoder_delete (pflac->fsd) ;
		} ;

	for (k = 0 ; k < ARRAY_LEN (pflac->rbuffer) ; k++)
		free (pflac->rbuffer [k]) ;

	free (pflac) ;
	psf->codec_data = NULL ;

	return 0 ;
} /* flac_close */
