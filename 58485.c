flac_read_header (SF_PRIVATE *psf)
{	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;

	psf_fseek (psf, 0, SEEK_SET) ;
	if (pflac->fsd)
		FLAC__stream_decoder_delete (pflac->fsd) ;
	if ((pflac->fsd = FLAC__stream_decoder_new ()) == NULL)
		return SFE_FLAC_NEW_DECODER ;

	FLAC__stream_decoder_set_metadata_respond_all (pflac->fsd) ;

	if (FLAC__stream_decoder_init_stream (pflac->fsd, sf_flac_read_callback, sf_flac_seek_callback, sf_flac_tell_callback, sf_flac_length_callback, sf_flac_eof_callback, sf_flac_write_callback, sf_flac_meta_callback, sf_flac_error_callback, psf) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
		return SFE_FLAC_INIT_DECODER ;

	FLAC__stream_decoder_process_until_end_of_metadata (pflac->fsd) ;

	psf_log_printf (psf, "End\n") ;

	if (psf->error == 0)
	{	FLAC__uint64 position ;

		FLAC__stream_decoder_get_decode_position (pflac->fsd, &position) ;
		psf->dataoffset = position ;
		} ;

	return psf->error ;
} /* flac_read_header */
