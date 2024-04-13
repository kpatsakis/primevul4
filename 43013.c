getu32(int swap, uint32_t value)
{
	union {
		uint32_t ui;
		char c[4];
	} retval, tmpval;

	if (swap) {
		tmpval.ui = value;

		retval.c[0] = tmpval.c[3];
		retval.c[1] = tmpval.c[2];
		retval.c[2] = tmpval.c[1];
		retval.c[3] = tmpval.c[0];
		
		return retval.ui;
	} else
		return value;
}
