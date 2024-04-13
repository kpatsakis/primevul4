flac_enc_init (SF_PRIVATE *psf)
{	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;
	unsigned bps ;

	/* To cite the flac FAQ at
	** http://flac.sourceforge.net/faq.html#general__samples
	**     "FLAC supports linear sample rates from 1Hz - 655350Hz in 1Hz
	**     increments."
	*/
	if (psf->sf.samplerate < 1 || psf->sf.samplerate > 655350)
	{	psf_log_printf (psf, "flac sample rate out of range.\n", psf->sf.samplerate) ;
		return SFE_FLAC_BAD_SAMPLE_RATE ;
		} ;

	psf_fseek (psf, 0, SEEK_SET) ;

	switch (SF_CODEC (psf->sf.format))
	{	case SF_FORMAT_PCM_S8 :
			bps = 8 ;
			break ;
		case SF_FORMAT_PCM_16 :
			bps = 16 ;
			break ;
		case SF_FORMAT_PCM_24 :
			bps = 24 ;
			break ;

		default :
			bps = 0 ;
			break ;
		} ;

	if (pflac->fse)
		FLAC__stream_encoder_delete (pflac->fse) ;
	if ((pflac->fse = FLAC__stream_encoder_new ()) == NULL)
		return SFE_FLAC_NEW_DECODER ;

	if (! FLAC__stream_encoder_set_channels (pflac->fse, psf->sf.channels))
	{	psf_log_printf (psf, "FLAC__stream_encoder_set_channels (%d) return false.\n", psf->sf.channels) ;
		return SFE_FLAC_INIT_DECODER ;
		} ;

	if (! FLAC__stream_encoder_set_sample_rate (pflac->fse, psf->sf.samplerate))
	{	psf_log_printf (psf, "FLAC__stream_encoder_set_sample_rate (%d) returned false.\n", psf->sf.samplerate) ;
		return SFE_FLAC_BAD_SAMPLE_RATE ;
		} ;

	if (! FLAC__stream_encoder_set_bits_per_sample (pflac->fse, bps))
	{	psf_log_printf (psf, "FLAC__stream_encoder_set_bits_per_sample (%d) return false.\n", bps) ;
		return SFE_FLAC_INIT_DECODER ;
		} ;

	if (! FLAC__stream_encoder_set_compression_level (pflac->fse, pflac->compression))
	{	psf_log_printf (psf, "FLAC__stream_encoder_set_compression_level (%d) return false.\n", pflac->compression) ;
		return SFE_FLAC_INIT_DECODER ;
		} ;

	return 0 ;
} /* flac_enc_init */
