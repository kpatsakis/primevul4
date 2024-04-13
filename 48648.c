read_rsrc_char (const SD2_RSRC *prsrc, int offset)
{	const unsigned char * data = prsrc->rsrc_data ;
	if (offset < 0 || offset >= prsrc->rsrc_len)
		return 0 ;
	return data [offset] ;
} /* read_rsrc_char */
