flac_write_header (SF_PRIVATE *psf, int UNUSED (calc_length))
{	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;
	int err ;

	flac_write_strings (psf, pflac) ;

	if ((err = FLAC__stream_encoder_init_stream (pflac->fse, sf_flac_enc_write_callback, sf_flac_enc_seek_callback, sf_flac_enc_tell_callback, NULL, psf)) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
	{	psf_log_printf (psf, "Error : FLAC encoder init returned error : %s\n", FLAC__StreamEncoderInitStatusString [err]) ;
		return SFE_FLAC_INIT_DECODER ;
		} ;

	if (psf->error == 0)
		psf->dataoffset = psf_ftell (psf) ;
	pflac->encbuffer = calloc (ENC_BUFFER_SIZE, sizeof (int32_t)) ;

	return psf->error ;
} /* flac_write_header */
