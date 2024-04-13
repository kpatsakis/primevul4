unsigned char *ser_string(char *s, int *slen)
{
	size_t len = strlen(s);
	unsigned char *ret;

	ret = malloc(1 + len + 8); // Leave room for largest size
	if (unlikely(!ret))
		quit(1, "Failed to malloc ret in ser_string");
	if (len < 253) {
		ret[0] = len;
		memcpy(ret + 1, s, len);
		*slen = len + 1;
	} else if (len < 0x10000) {
		uint16_t *u16 = (uint16_t *)&ret[1];

		ret[0] = 253;
		*u16 = htobe16(len);
		memcpy(ret + 3, s, len);
		*slen = len + 3;
	} else {
		/* size_t is only 32 bit on many platforms anyway */
		uint32_t *u32 = (uint32_t *)&ret[1];

		ret[0] = 254;
		*u32 = htobe32(len);
		memcpy(ret + 5, s, len);
		*slen = len + 5;
	}
	return ret;
}
