tenbytefloat2int (uint8_t *bytes)
{	int val = 3 ;

	if (bytes [0] & 0x80)	/* Negative number. */
		return 0 ;

	if (bytes [0] <= 0x3F)	/* Less than 1. */
		return 1 ;

	if (bytes [0] > 0x40)	/* Way too big. */
		return 0x4000000 ;

	if (bytes [0] == 0x40 && bytes [1] > 0x1C) /* Too big. */
		return 800000000 ;

	/* Ok, can handle it. */

	val = (bytes [2] << 23) | (bytes [3] << 15) | (bytes [4] << 7) | (bytes [5] >> 1) ;

	val >>= (29 - bytes [1]) ;

	return val ;
} /* tenbytefloat2int */
