write_marker (unsigned char * data, int offset, int value)
{
	if (CPU_IS_BIG_ENDIAN)
	{	data [offset] = value >> 24 ;
		data [offset + 1] = value >> 16 ;
		data [offset + 2] = value >> 8 ;
		data [offset + 3] = value ;
		}
	else
	{	data [offset] = value ;
		data [offset + 1] = value >> 8 ;
		data [offset + 2] = value >> 16 ;
		data [offset + 3] = value >> 24 ;
		} ;
} /* write_marker */
