flac_read_flac2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;
	sf_count_t total = 0, current ;
	unsigned readlen ;

	pflac->pcmtype = PFLAC_PCM_SHORT ;

	while (total < len)
	{	pflac->ptr = ptr + total ;
		readlen = (len - total > 0x1000000) ? 0x1000000 : (unsigned) (len - total) ;
		current = flac_read_loop (psf, readlen) ;
		if (current == 0)
			break ;
		total += current ;
		} ;

	return total ;
} /* flac_read_flac2s */
