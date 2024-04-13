read_rsrc_str (const SD2_RSRC *prsrc, int offset, char * buffer, int buffer_len)
{	const unsigned char * data = prsrc->rsrc_data ;
	int k ;

	memset (buffer, 0, buffer_len) ;

	if (offset < 0 || offset + buffer_len >= prsrc->rsrc_len)
		return ;

	for (k = 0 ; k < buffer_len - 1 ; k++)
	{	if (psf_isprint (data [offset + k]) == 0)
			return ;
		buffer [k] = data [offset + k] ;
		} ;
	return ;
} /* read_rsrc_str */
