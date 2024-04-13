getu16(int swap, uint16_t value)
{
	union {
		uint16_t ui;
		char c[2];
	} retval, tmpval;

	if (swap) {
		tmpval.ui = value;

		retval.c[0] = tmpval.c[1];
		retval.c[1] = tmpval.c[0];
		
		return retval.ui;
	} else
		return value;
}
