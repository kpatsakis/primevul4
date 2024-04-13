sd2_write_rsrc_fork (SF_PRIVATE *psf, int UNUSED (calc_length))
{	SD2_RSRC rsrc ;
	STR_RSRC str_rsrc [] =
	{	{ RSRC_STR, 1000, "_sample-size", "", 0 },
		{ RSRC_STR, 1001, "_sample-rate", "", 0 },
		{ RSRC_STR, 1002, "_channels", "", 0 },
		{ RSRC_BIN, 1000, "_Markers", "", 8 }
		} ;

	int k, str_offset, data_offset, next_str ;

	psf_use_rsrc (psf, SF_TRUE) ;

	memset (&rsrc, 0, sizeof (rsrc)) ;

	rsrc.sample_rate = psf->sf.samplerate ;
	rsrc.sample_size = psf->bytewidth ;
	rsrc.channels = psf->sf.channels ;

	rsrc.rsrc_data = psf->header ;
	rsrc.rsrc_len = sizeof (psf->header) ;
	memset (rsrc.rsrc_data, 0xea, rsrc.rsrc_len) ;

	snprintf (str_rsrc [0].value, sizeof (str_rsrc [0].value), "_%d", rsrc.sample_size) ;
	snprintf (str_rsrc [1].value, sizeof (str_rsrc [1].value), "_%d.000000", rsrc.sample_rate) ;
	snprintf (str_rsrc [2].value, sizeof (str_rsrc [2].value), "_%d", rsrc.channels) ;

	for (k = 0 ; k < ARRAY_LEN (str_rsrc) ; k++)
	{	if (str_rsrc [k].value_len == 0)
		{	str_rsrc [k].value_len = strlen (str_rsrc [k].value) ;
			str_rsrc [k].value [0] = str_rsrc [k].value_len - 1 ;
			} ;

		/* Turn name string into a pascal string. */
		str_rsrc [k].name [0] = strlen (str_rsrc [k].name) - 1 ;
		} ;

	rsrc.data_offset = 0x100 ;

	/*
	** Calculate data length :
	**		length of strings, plus the length of the sdML chunk.
	*/
	rsrc.data_length = 0 ;
	for (k = 0 ; k < ARRAY_LEN (str_rsrc) ; k++)
		rsrc.data_length += str_rsrc [k].value_len + 4 ;

	rsrc.map_offset = rsrc.data_offset + rsrc.data_length ;

	/* Very start of resource fork. */
	write_int (rsrc.rsrc_data, 0, rsrc.data_offset) ;
	write_int (rsrc.rsrc_data, 4, rsrc.map_offset) ;
	write_int (rsrc.rsrc_data, 8, rsrc.data_length) ;

	write_char (rsrc.rsrc_data, 0x30, strlen (psf->file.name.c)) ;
	write_str (rsrc.rsrc_data, 0x31, psf->file.name.c, strlen (psf->file.name.c)) ;

	write_short (rsrc.rsrc_data, 0x50, 0) ;
	write_marker (rsrc.rsrc_data, 0x52, Sd2f_MARKER) ;
	write_marker (rsrc.rsrc_data, 0x56, lsf1_MARKER) ;

	/* Very start of resource map. */
	write_int (rsrc.rsrc_data, rsrc.map_offset + 0, rsrc.data_offset) ;
	write_int (rsrc.rsrc_data, rsrc.map_offset + 4, rsrc.map_offset) ;
	write_int (rsrc.rsrc_data, rsrc.map_offset + 8, rsrc.data_length) ;

	/* These I don't currently understand. */
	if (1)
	{	write_char (rsrc.rsrc_data, rsrc.map_offset+ 16, 1) ;
		/* Next resource map. */
		write_int (rsrc.rsrc_data, rsrc.map_offset + 17, 0x12345678) ;
		/* File ref number. */
		write_short (rsrc.rsrc_data, rsrc.map_offset + 21, 0xabcd) ;
		/* Fork attributes. */
		write_short (rsrc.rsrc_data, rsrc.map_offset + 23, 0) ;
		} ;

	/* Resource type offset. */
	rsrc.type_offset = rsrc.map_offset + 30 ;
	write_short (rsrc.rsrc_data, rsrc.map_offset + 24, rsrc.type_offset - rsrc.map_offset - 2) ;

	/* Type index max. */
	rsrc.type_count = 2 ;
	write_short (rsrc.rsrc_data, rsrc.map_offset + 28, rsrc.type_count - 1) ;

	rsrc.item_offset = rsrc.type_offset + rsrc.type_count * 8 ;

	rsrc.str_count = ARRAY_LEN (str_rsrc) ;
	rsrc.string_offset = rsrc.item_offset + (rsrc.str_count + 1) * 12 - rsrc.map_offset ;
	write_short (rsrc.rsrc_data, rsrc.map_offset + 26, rsrc.string_offset) ;

	/* Write 'STR ' resource type. */
	rsrc.str_count = 3 ;
	write_marker (rsrc.rsrc_data, rsrc.type_offset, STR_MARKER) ;
	write_short (rsrc.rsrc_data, rsrc.type_offset + 4, rsrc.str_count - 1) ;
	write_short (rsrc.rsrc_data, rsrc.type_offset + 6, 0x12) ;

	/* Write 'sdML' resource type. */
	write_marker (rsrc.rsrc_data, rsrc.type_offset + 8, sdML_MARKER) ;
	write_short (rsrc.rsrc_data, rsrc.type_offset + 12, 0) ;
	write_short (rsrc.rsrc_data, rsrc.type_offset + 14, 0x36) ;

	str_offset = rsrc.map_offset + rsrc.string_offset ;
	next_str = 0 ;
	data_offset = rsrc.data_offset ;
	for (k = 0 ; k < ARRAY_LEN (str_rsrc) ; k++)
	{	write_str (rsrc.rsrc_data, str_offset, str_rsrc [k].name, strlen (str_rsrc [k].name)) ;

		write_short (rsrc.rsrc_data, rsrc.item_offset + k * 12, str_rsrc [k].id) ;
		write_short (rsrc.rsrc_data, rsrc.item_offset + k * 12 + 2, next_str) ;

		str_offset += strlen (str_rsrc [k].name) ;
		next_str += strlen (str_rsrc [k].name) ;

		write_int (rsrc.rsrc_data, rsrc.item_offset + k * 12 + 4, data_offset - rsrc.data_offset) ;

		write_int (rsrc.rsrc_data, data_offset, str_rsrc [k].value_len) ;
		write_str (rsrc.rsrc_data, data_offset + 4, str_rsrc [k].value, str_rsrc [k].value_len) ;
		data_offset += 4 + str_rsrc [k].value_len ;
		} ;

	/* Finally, calculate and set map length. */
	rsrc.map_length = str_offset - rsrc.map_offset ;
	write_int (rsrc.rsrc_data, 12, rsrc.map_length) ;
	write_int (rsrc.rsrc_data, rsrc.map_offset + 12, rsrc.map_length) ;

	rsrc.rsrc_len = rsrc.map_offset + rsrc.map_length ;

	psf_fwrite (rsrc.rsrc_data, rsrc.rsrc_len, 1, psf) ;

	psf_use_rsrc (psf, SF_FALSE) ;

	if (psf->error)
		return psf->error ;

	return 0 ;
} /* sd2_write_rsrc_fork */
