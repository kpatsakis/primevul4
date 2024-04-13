name_interpret(netdissect_options *ndo,
               const u_char *in, const u_char *maxbuf, char *out)
{
    int ret;
    int len;

    if (in >= maxbuf)
	return(-1);	/* name goes past the end of the buffer */
    ND_TCHECK2(*in, 1);
    len = (*in++) / 2;

    *out=0;

    if (len > 30 || len < 1)
	return(0);

    while (len--) {
	ND_TCHECK2(*in, 2);
	if (in + 1 >= maxbuf)
	    return(-1);	/* name goes past the end of the buffer */
	if (in[0] < 'A' || in[0] > 'P' || in[1] < 'A' || in[1] > 'P') {
	    *out = 0;
	    return(0);
	}
	*out = ((in[0] - 'A') << 4) + (in[1] - 'A');
	in += 2;
	out++;
    }
    *out = 0;
    ret = out[-1];

    return(ret);

trunc:
    return(-1);
}
