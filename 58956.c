aiff_open (SF_PRIVATE *psf)
{	COMM_CHUNK comm_fmt ;
	int error, subformat ;

	memset (&comm_fmt, 0, sizeof (comm_fmt)) ;

	subformat = SF_CODEC (psf->sf.format) ;

	if ((psf->container_data = calloc (1, sizeof (AIFF_PRIVATE))) == NULL)
		return SFE_MALLOC_FAILED ;

	if (psf->file.mode == SFM_READ || (psf->file.mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = aiff_read_header (psf, &comm_fmt)))
			return error ;

		psf->next_chunk_iterator = aiff_next_chunk_iterator ;
		psf->get_chunk_size = aiff_get_chunk_size ;
		psf->get_chunk_data = aiff_get_chunk_data ;

		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		} ;

	if (psf->file.mode == SFM_WRITE || psf->file.mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		if ((SF_CONTAINER (psf->sf.format)) != SF_FORMAT_AIFF)
			return SFE_BAD_OPEN_FORMAT ;

		if (psf->file.mode == SFM_WRITE && (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE))
		{	if ((psf->peak_info = peak_info_calloc (psf->sf.channels)) == NULL)
				return SFE_MALLOC_FAILED ;
			psf->peak_info->peak_loc = SF_PEAK_START ;
			} ;

		if (psf->file.mode != SFM_RDWR || psf->filelength < 40)
		{	psf->filelength = 0 ;
			psf->datalength = 0 ;
			psf->dataoffset = 0 ;
			psf->sf.frames = 0 ;
			} ;

		psf->strings.flags = SF_STR_ALLOW_START | SF_STR_ALLOW_END ;

		if ((error = aiff_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header	= aiff_write_header ;
		psf->set_chunk		= aiff_set_chunk ;
		} ;

	psf->container_close = aiff_close ;
	psf->command = aiff_command ;

	switch (SF_CODEC (psf->sf.format))
	{	case SF_FORMAT_PCM_U8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_S8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_ULAW :
				error = ulaw_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :
				error = alaw_init (psf) ;
				break ;

		/* Lite remove start */
		case SF_FORMAT_FLOAT :
				error = float32_init (psf) ;
				break ;

		case SF_FORMAT_DOUBLE :
				error = double64_init (psf) ;
				break ;

		case SF_FORMAT_DWVW_12 :
				if (psf->sf.frames > comm_fmt.numSampleFrames)
					psf->sf.frames = comm_fmt.numSampleFrames ;
				break ;

		case SF_FORMAT_DWVW_16 :
				error = dwvw_init (psf, 16) ;
				if (psf->sf.frames > comm_fmt.numSampleFrames)
					psf->sf.frames = comm_fmt.numSampleFrames ;
				break ;

		case SF_FORMAT_DWVW_24 :
				error = dwvw_init (psf, 24) ;
				if (psf->sf.frames > comm_fmt.numSampleFrames)
					psf->sf.frames = comm_fmt.numSampleFrames ;
				break ;

		case SF_FORMAT_DWVW_N :
				if (psf->file.mode != SFM_READ)
				{	error = SFE_DWVW_BAD_BITWIDTH ;
					break ;
					} ;
				if (comm_fmt.sampleSize >= 8 && comm_fmt.sampleSize < 24)
				{	error = dwvw_init (psf, comm_fmt.sampleSize) ;
					if (psf->sf.frames > comm_fmt.numSampleFrames)
						psf->sf.frames = comm_fmt.numSampleFrames ;
					break ;
					} ;
				psf_log_printf (psf, "AIFC/DWVW : Bad bitwidth %d\n", comm_fmt.sampleSize) ;
				error = SFE_DWVW_BAD_BITWIDTH ;
				break ;

		case SF_FORMAT_IMA_ADPCM :
				/*
				**	IMA ADPCM encoded AIFF files always have a block length
				**	of 34 which decodes to 64 samples.
				*/
				error = aiff_ima_init (psf, AIFC_IMA4_BLOCK_LEN, AIFC_IMA4_SAMPLES_PER_BLOCK) ;
				break ;
		/* Lite remove end */

		case SF_FORMAT_GSM610 :
				error = gsm610_init (psf) ;
				if (psf->sf.frames > comm_fmt.numSampleFrames)
					psf->sf.frames = comm_fmt.numSampleFrames ;
				break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	if (psf->file.mode != SFM_WRITE && psf->sf.frames - comm_fmt.numSampleFrames != 0)
	{	psf_log_printf (psf,
			"*** Frame count read from 'COMM' chunk (%u) not equal to frame count\n"
			"*** calculated from length of 'SSND' chunk (%u).\n",
			comm_fmt.numSampleFrames, (uint32_t) psf->sf.frames) ;
		} ;

	return error ;
} /* aiff_open */
