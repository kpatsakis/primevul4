d2flac16_array (const double *src, int32_t *dest, int count, int normalize)
{	double normfact = normalize ? (1.0 * 0x7FFF) : 1.0 ;

	while (--count >= 0)
		dest [count] = lrint (src [count] * normfact) ;
} /* d2flac16_array */
