i2flac16_array (const int *src, int32_t *dest, int count)
{
	while (--count >= 0)
		dest [count] = src [count] >> 16 ;
} /* i2flac16_array */
