flac_write_d2flac (SF_PRIVATE *psf, const double *ptr, sf_count_t len)
{	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;
	void (*convert) (const double *, int32_t *, int, int) ;
	int bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	int32_t* buffer = pflac->encbuffer ;

	switch (SF_CODEC (psf->sf.format))
	{	case SF_FORMAT_PCM_S8 :
			convert = (psf->add_clipping) ? d2flac8_clip_array : d2flac8_array ;
			break ;
		case SF_FORMAT_PCM_16 :
			convert = (psf->add_clipping) ? d2flac16_clip_array : d2flac16_array ;
			break ;
		case SF_FORMAT_PCM_24 :
			convert = (psf->add_clipping) ? d2flac24_clip_array : d2flac24_array ;
			break ;
		default :
			return -1 ;
		} ;

	bufferlen = ENC_BUFFER_SIZE / (sizeof (int32_t) * psf->sf.channels) ;
	bufferlen *= psf->sf.channels ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, buffer, writecount, psf->norm_double) ;
		if (FLAC__stream_encoder_process_interleaved (pflac->fse, buffer, writecount / psf->sf.channels))
			thiswrite = writecount ;
		else
			break ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;

		len -= thiswrite ;
		} ;

	return total ;
} /* flac_write_d2flac */
