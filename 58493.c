i2flac8_array (const int *src, int32_t *dest, int count)
{	while (--count >= 0)
		dest [count] = src [count] >> 24 ;
} /* i2flac8_array */
