f2flac8_clip_array (const float *src, int32_t *dest, int count, int normalize)
{	float normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10) : 1.0 ;

	while (--count >= 0)
	{	scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7F))
		{	dest [count] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10))
		{	dest [count] = 0x80 ;
			continue ;
			} ;
		dest [count] = lrintf (scaled_value) ;
		} ;

	return ;
} /* f2flac8_clip_array */
