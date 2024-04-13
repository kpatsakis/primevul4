read_Bools(struct archive_read *a, unsigned char *data, size_t num)
{
	const unsigned char *p;
	unsigned i, mask = 0, avail = 0;

	for (i = 0; i < num; i++) {
		if (mask == 0) {
			if ((p = header_bytes(a, 1)) == NULL)
				return (-1);
			avail = *p;
			mask = 0x80;
		}
		data[i] = (avail & mask)?1:0;
		mask >>= 1;
	}
	return (0);
}
