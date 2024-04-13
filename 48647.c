parse_str_rsrc (SF_PRIVATE *psf, SD2_RSRC * rsrc)
{	char name [32], value [32] ;
	int k, str_offset, rsrc_id, data_offset = 0, data_len = 0 ;

	psf_log_printf (psf, "Finding parameters :\n") ;

	str_offset = rsrc->string_offset ;
	psf_log_printf (psf, "  Offset    RsrcId    dlen    slen    Value\n") ;

	for (k = 0 ; data_offset + data_len < rsrc->rsrc_len ; k++)
	{	int slen ;

		slen = read_rsrc_char (rsrc, str_offset) ;
		read_rsrc_str (rsrc, str_offset + 1, name, SF_MIN (SIGNED_SIZEOF (name), slen + 1)) ;
		str_offset += slen + 1 ;

		rsrc_id = read_rsrc_short (rsrc, rsrc->item_offset + k * 12) ;

		data_offset = rsrc->data_offset + read_rsrc_int (rsrc, rsrc->item_offset + k * 12 + 4) ;
		if (data_offset < 0 || data_offset > rsrc->rsrc_len)
		{	psf_log_printf (psf, "Exiting parser on data offset of %d.\n", data_offset) ;
			break ;
			} ;

		data_len = read_rsrc_int (rsrc, data_offset) ;
		if (data_len < 0 || data_len > rsrc->rsrc_len)
		{	psf_log_printf (psf, "Exiting parser on data length of %d.\n", data_len) ;
			break ;
			} ;

		slen = read_rsrc_char (rsrc, data_offset + 4) ;
		read_rsrc_str (rsrc, data_offset + 5, value, SF_MIN (SIGNED_SIZEOF (value), slen + 1)) ;

		psf_log_printf (psf, "  0x%04x     %4d     %4d     %3d    '%s'\n", data_offset, rsrc_id, data_len, slen, value) ;

		if (rsrc_id == 1000 && rsrc->sample_size == 0)
			rsrc->sample_size = strtol (value, NULL, 10) ;
		else if (rsrc_id == 1001 && rsrc->sample_rate == 0)
			rsrc->sample_rate = strtol (value, NULL, 10) ;
		else if (rsrc_id == 1002 && rsrc->channels == 0)
			rsrc->channels = strtol (value, NULL, 10) ;
		} ;

	psf_log_printf (psf, "Found Parameters :\n") ;
	psf_log_printf (psf, "  sample-size : %d\n", rsrc->sample_size) ;
	psf_log_printf (psf, "  sample-rate : %d\n", rsrc->sample_rate) ;
	psf_log_printf (psf, "  channels    : %d\n", rsrc->channels) ;

	if (rsrc->sample_rate <= 4 && rsrc->sample_size > 4)
	{	int temp ;

		psf_log_printf (psf, "Geez!! Looks like sample rate and sample size got switched.\nCorrecting this screw up.\n") ;
		temp = rsrc->sample_rate ;
		rsrc->sample_rate = rsrc->sample_size ;
		rsrc->sample_size = temp ;
		} ;

	if (rsrc->sample_rate < 0)
	{	psf_log_printf (psf, "Bad sample rate (%d)\n", rsrc->sample_rate) ;
		return SFE_SD2_BAD_RSRC ;
		} ;

	if (rsrc->channels < 0)
	{	psf_log_printf (psf, "Bad channel count (%d)\n", rsrc->channels) ;
		return SFE_SD2_BAD_RSRC ;
		} ;

	psf->sf.samplerate = rsrc->sample_rate ;
	psf->sf.channels = rsrc->channels ;
	psf->bytewidth = rsrc->sample_size ;

	switch (rsrc->sample_size)
	{	case 1 :
			psf->sf.format = SF_FORMAT_SD2 | SF_FORMAT_PCM_S8 ;
			break ;

		case 2 :
			psf->sf.format = SF_FORMAT_SD2 | SF_FORMAT_PCM_16 ;
			break ;

		case 3 :
			psf->sf.format = SF_FORMAT_SD2 | SF_FORMAT_PCM_24 ;
			break ;

		case 4 :
			psf->sf.format = SF_FORMAT_SD2 | SF_FORMAT_PCM_32 ;
			break ;

		default :
			psf_log_printf (psf, "Bad sample size (%d)\n", rsrc->sample_size) ;
			return SFE_SD2_BAD_SAMPLE_SIZE ;
		} ;

	psf_log_printf (psf, "ok\n") ;

	return 0 ;
} /* parse_str_rsrc */
