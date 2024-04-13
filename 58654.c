static int prepend_name(char **buffer, int *buflen, const struct qstr *name)
{
	const char *dname = ACCESS_ONCE(name->name);
	u32 dlen = ACCESS_ONCE(name->len);
	char *p;

	smp_read_barrier_depends();

	*buflen -= dlen + 1;
	if (*buflen < 0)
		return -ENAMETOOLONG;
	p = *buffer -= dlen + 1;
	*p++ = '/';
	while (dlen--) {
		char c = *dname++;
		if (!c)
			break;
		*p++ = c;
	}
	return 0;
}
