print_lladdr(netdissect_options *ndo, const uint8_t *p, size_t l)
{
	const uint8_t *ep, *q;

	q = p;
	ep = p + l;
	while (l > 0 && q < ep) {
		if (q > p)
                        ND_PRINT((ndo,":"));
		ND_PRINT((ndo,"%02x", *q++));
		l--;
	}
}
