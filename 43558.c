char *bin2hex(const unsigned char *p, size_t len)
{
	ssize_t slen;
	char *s;

	slen = len * 2 + 1;
	if (slen % 4)
		slen += 4 - (slen % 4);
	s = calloc(slen, 1);
	if (unlikely(!s))
		quithere(1, "Failed to calloc");

	__bin2hex(s, p, len);

	return s;
}
