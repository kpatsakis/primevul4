strappend_base64(struct xar *xar,
    struct archive_string *as, const char *s, size_t l)
{
	unsigned char buff[256];
	unsigned char *out;
	const unsigned char *b;
	size_t len;

	(void)xar; /* UNUSED */
	len = 0;
	out = buff;
	b = (const unsigned char *)s;
	while (l > 0) {
		int n = 0;

		if (l > 0) {
			if (base64[b[0]] < 0 || base64[b[1]] < 0)
				break;
			n = base64[*b++] << 18;
			n |= base64[*b++] << 12;
			*out++ = n >> 16;
			len++;
			l -= 2;
		}
		if (l > 0) {
			if (base64[*b] < 0)
				break;
			n |= base64[*b++] << 6;
			*out++ = (n >> 8) & 0xFF;
			len++;
			--l;
		}
		if (l > 0) {
			if (base64[*b] < 0)
				break;
			n |= base64[*b++];
			*out++ = n & 0xFF;
			len++;
			--l;
		}
		if (len+3 >= sizeof(buff)) {
			archive_strncat(as, (const char *)buff, len);
			len = 0;
			out = buff;
		}
	}
	if (len > 0)
		archive_strncat(as, (const char *)buff, len);
}
