write_int (unsigned char * data, int offset, int value)
{	data [offset] = value >> 24 ;
	data [offset + 1] = value >> 16 ;
	data [offset + 2] = value >> 8 ;
	data [offset + 3] = value ;
} /* write_int */
