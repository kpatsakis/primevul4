write_bits(netdissect_options *ndo,
           unsigned int val, const char *fmt)
{
    const char *p = fmt;
    int i = 0;

    while ((p = strchr(fmt, '|'))) {
	size_t l = PTR_DIFF(p, fmt);
	if (l && (val & (1 << i)))
	    ND_PRINT((ndo, "%.*s ", (int)l, fmt));
	fmt = p + 1;
	i++;
    }
}
