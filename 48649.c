read_rsrc_marker (const SD2_RSRC *prsrc, int offset)
{	const unsigned char * data = prsrc->rsrc_data ;

	if (offset < 0 || offset + 3 >= prsrc->rsrc_len)
		return 0 ;

	if (CPU_IS_BIG_ENDIAN)
		return (((uint32_t) data [offset]) << 24) + (data [offset + 1] << 16) + (data [offset + 2] << 8) + data [offset + 3] ;
	if (CPU_IS_LITTLE_ENDIAN)
		return data [offset] + (data [offset + 1] << 8) + (data [offset + 2] << 16) + (((uint32_t) data [offset + 3]) << 24) ;

	return 0 ;
} /* read_rsrc_marker */
