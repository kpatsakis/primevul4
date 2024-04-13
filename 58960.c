aiff_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t		current ;
	AIFF_PRIVATE	*paiff ;
	uint8_t	comm_sample_rate [10], comm_zero_bytes [2] = { 0, 0 } ;
	uint32_t	comm_type, comm_size, comm_encoding, comm_frames = 0, uk ;
	int				k, endian, has_data = SF_FALSE ;
	int16_t			bit_width ;

	if ((paiff = psf->container_data) == NULL)
		return SFE_INTERNAL ;

	current = psf_ftell (psf) ;

	if (current > psf->dataoffset)
		has_data = SF_TRUE ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		if (psf->bytewidth > 0)
			psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	if (psf->file.mode == SFM_RDWR && psf->dataoffset > 0 && psf->rchunks.count > 0)
	{	aiff_rewrite_header (psf) ;
		if (current > 0)
			psf_fseek (psf, current, SEEK_SET) ;
		return 0 ;
		} ;

	endian = SF_ENDIAN (psf->sf.format) ;
	if (CPU_IS_LITTLE_ENDIAN && endian == SF_ENDIAN_CPU)
		endian = SF_ENDIAN_LITTLE ;

	/* Standard value here. */
	bit_width = psf->bytewidth * 8 ;
	comm_frames = (psf->sf.frames > 0xFFFFFFFF) ? 0xFFFFFFFF : psf->sf.frames ;

	switch (SF_CODEC (psf->sf.format) | endian)
	{	case SF_FORMAT_PCM_S8 | SF_ENDIAN_BIG :
			psf->endian = SF_ENDIAN_BIG ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = twos_MARKER ;
			break ;

		case SF_FORMAT_PCM_S8 | SF_ENDIAN_LITTLE :
			psf->endian = SF_ENDIAN_LITTLE ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = sowt_MARKER ;
			break ;

		case SF_FORMAT_PCM_16 | SF_ENDIAN_BIG :
			psf->endian = SF_ENDIAN_BIG ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = twos_MARKER ;
			break ;

		case SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE :
			psf->endian = SF_ENDIAN_LITTLE ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = sowt_MARKER ;
			break ;

		case SF_FORMAT_PCM_24 | SF_ENDIAN_BIG :
			psf->endian = SF_ENDIAN_BIG ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = in24_MARKER ;
			break ;

		case SF_FORMAT_PCM_24 | SF_ENDIAN_LITTLE :
			psf->endian = SF_ENDIAN_LITTLE ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = ni24_MARKER ;
			break ;

		case SF_FORMAT_PCM_32 | SF_ENDIAN_BIG :
			psf->endian = SF_ENDIAN_BIG ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = in32_MARKER ;
			break ;

		case SF_FORMAT_PCM_32 | SF_ENDIAN_LITTLE :
			psf->endian = SF_ENDIAN_LITTLE ;
			comm_type = AIFC_MARKER ;
			comm_size = SIZEOF_AIFC_COMM ;
			comm_encoding = ni32_MARKER ;
			break ;

		case SF_FORMAT_PCM_S8 :			/* SF_ENDIAN_FILE */
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
			psf->endian = SF_ENDIAN_BIG ;
			comm_type = AIFF_MARKER ;
			comm_size = SIZEOF_AIFF_COMM ;
			comm_encoding = 0 ;
			break ;

		case SF_FORMAT_FLOAT :					/* Big endian floating point. */
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = FL32_MARKER ;	/* Use 'FL32' because its easier to read. */
				break ;

		case SF_FORMAT_DOUBLE :					/* Big endian double precision floating point. */
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = FL64_MARKER ;	/* Use 'FL64' because its easier to read. */
				break ;

		case SF_FORMAT_ULAW :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = ulaw_MARKER ;
				break ;

		case SF_FORMAT_ALAW :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = alaw_MARKER ;
				break ;

		case SF_FORMAT_PCM_U8 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = raw_MARKER ;
				break ;

		case SF_FORMAT_DWVW_12 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 12 ;
				break ;

		case SF_FORMAT_DWVW_16 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				break ;

		case SF_FORMAT_DWVW_24 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 24 ;
				break ;

		case SF_FORMAT_GSM610 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = GSM_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				break ;

		case SF_FORMAT_IMA_ADPCM :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = ima4_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				comm_frames = psf->sf.frames / AIFC_IMA4_SAMPLES_PER_BLOCK ;
				break ;

		default : return SFE_BAD_OPEN_FORMAT ;
		} ;

	/* Reset the current header length to zero. */
	psf->header.ptr [0] = 0 ;
	psf->header.indx = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	psf_binheader_writef (psf, "Etm8", FORM_MARKER, psf->filelength - 8) ;

	/* Write AIFF/AIFC marker and COM chunk. */
	if (comm_type == AIFC_MARKER)
		/* AIFC must have an FVER chunk. */
		psf_binheader_writef (psf, "Emm44", comm_type, FVER_MARKER, 4, 0xA2805140) ;
	else
		psf_binheader_writef (psf, "Em", comm_type) ;

	paiff->comm_offset = psf->header.indx - 8 ;

	memset (comm_sample_rate, 0, sizeof (comm_sample_rate)) ;
	uint2tenbytefloat (psf->sf.samplerate, comm_sample_rate) ;

	psf_binheader_writef (psf, "Em42t42", COMM_MARKER, comm_size, psf->sf.channels, comm_frames, bit_width) ;
	psf_binheader_writef (psf, "b", comm_sample_rate, sizeof (comm_sample_rate)) ;

	/* AIFC chunks have some extra data. */
	if (comm_type == AIFC_MARKER)
		psf_binheader_writef (psf, "mb", comm_encoding, comm_zero_bytes, sizeof (comm_zero_bytes)) ;

	if (psf->channel_map && paiff->chanmap_tag)
		psf_binheader_writef (psf, "Em4444", CHAN_MARKER, 12, paiff->chanmap_tag, 0, 0) ;

	/* Check if there's a INST chunk to write */
	if (psf->instrument != NULL && psf->cues != NULL)
	{	/* Huge chunk of code removed here because it had egregious errors that were
		** not detected by either the compiler or the tests. It was found when updating
		** the way psf_binheader_writef works.
		*/
		}
	else if (psf->instrument == NULL && psf->cues != NULL)
	{	/* There are cues but no loops */
		uint32_t idx ;
		int totalStringLength = 0, stringLength ;

		/* Here we count how many bytes will the pascal strings need */
		for (idx = 0 ; idx < psf->cues->cue_count ; idx++)
		{	stringLength = strlen (psf->cues->cue_points [idx].name) + 1 ; /* We'll count the first byte also of every pascal string */
			totalStringLength += stringLength + (stringLength % 2 == 0 ? 0 : 1) ;
			} ;

		psf_binheader_writef (psf, "Em42",
			MARK_MARKER, 2 + psf->cues->cue_count * (2 + 4) + totalStringLength, psf->cues->cue_count) ;

		for (idx = 0 ; idx < psf->cues->cue_count ; idx++)
			psf_binheader_writef (psf, "E24p", psf->cues->cue_points [idx].indx, psf->cues->cue_points [idx].sample_offset, psf->cues->cue_points [idx].name) ;
		} ;

	if (psf->strings.flags & SF_STR_LOCATE_START)
		aiff_write_strings (psf, SF_STR_LOCATE_START) ;

	if (psf->peak_info != NULL && psf->peak_info->peak_loc == SF_PEAK_START)
	{	psf_binheader_writef (psf, "Em4", PEAK_MARKER, AIFF_PEAK_CHUNK_SIZE (psf->sf.channels)) ;
		psf_binheader_writef (psf, "E44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "Eft8", (float) psf->peak_info->peaks [k].value, psf->peak_info->peaks [k].position) ;
		} ;

	/* Write custom headers. */
	for (uk = 0 ; uk < psf->wchunks.used ; uk++)
		psf_binheader_writef (psf, "Em4b", psf->wchunks.chunks [uk].mark32, psf->wchunks.chunks [uk].len, psf->wchunks.chunks [uk].data, make_size_t (psf->wchunks.chunks [uk].len)) ;

	/* Write SSND chunk. */
	paiff->ssnd_offset = psf->header.indx ;
	psf_binheader_writef (psf, "Etm844", SSND_MARKER, psf->datalength + SIZEOF_SSND_CHUNK, 0, 0) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header.ptr, psf->header.indx, 1, psf) ;

	if (psf->error)
		return psf->error ;

	if (has_data && psf->dataoffset != psf->header.indx)
		return psf->error = SFE_INTERNAL ;

	psf->dataoffset = psf->header.indx ;

	if (! has_data)
		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
	else if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* aiff_write_header */
