s2flac24_array (const short *src, int32_t *dest, int count)
{	while (--count >= 0)
		dest [count] = src [count] << 8 ;
} /* s2flac24_array */
