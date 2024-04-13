parse_7zip_uint64(struct archive_read *a, uint64_t *val)
{
	const unsigned char *p;
	unsigned char avail, mask;
	int i;

	if ((p = header_bytes(a, 1)) == NULL)
		return (-1);
	avail = *p;
	mask = 0x80;
	*val = 0;
	for (i = 0; i < 8; i++) {
		if (avail & mask) {
			if ((p = header_bytes(a, 1)) == NULL)
				return (-1);
			*val |= ((uint64_t)*p) << (8 * i);
			mask >>= 1;
			continue;
		}
		*val += ((uint64_t)(avail & (mask -1))) << (8 * i);
		break;
	}
	return (0);
}
