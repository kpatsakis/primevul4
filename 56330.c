name_ptr(netdissect_options *ndo,
         const u_char *buf, int ofs, const u_char *maxbuf)
{
    const u_char *p;
    u_char c;

    p = buf + ofs;
    if (p >= maxbuf)
	return(NULL);	/* name goes past the end of the buffer */
    ND_TCHECK2(*p, 1);

    c = *p;

    /* XXX - this should use the same code that the DNS dissector does */
    if ((c & 0xC0) == 0xC0) {
	uint16_t l;

	ND_TCHECK2(*p, 2);
	if ((p + 1) >= maxbuf)
	    return(NULL);	/* name goes past the end of the buffer */
	l = EXTRACT_16BITS(p) & 0x3FFF;
	if (l == 0) {
	    /* We have a pointer that points to itself. */
	    return(NULL);
	}
	p = buf + l;
	if (p >= maxbuf)
	    return(NULL);	/* name goes past the end of the buffer */
	ND_TCHECK2(*p, 1);
    }
    return(p);

trunc:
    return(NULL);	/* name goes past the end of the buffer */
}
