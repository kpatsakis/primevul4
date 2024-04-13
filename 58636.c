static inline int dentry_string_cmp(const unsigned char *cs, const unsigned char *ct, unsigned tcount)
{
	do {
		if (*cs != *ct)
			return 1;
		cs++;
		ct++;
		tcount--;
	} while (tcount);
	return 0;
}
