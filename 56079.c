decode_labeled_prefix4(netdissect_options *ndo,
                       const u_char *pptr, u_int itemlen, char *buf, u_int buflen)
{
	struct in_addr addr;
	u_int plen, plenbytes;

	/* prefix length and label = 4 bytes */
	ND_TCHECK2(pptr[0], 4);
	ITEMCHECK(4);
	plen = pptr[0];   /* get prefix length */

        /* this is one of the weirdnesses of rfc3107
           the label length (actually the label + COS bits)
           is added to the prefix length;
           we also do only read out just one label -
           there is no real application for advertisement of
           stacked labels in a single BGP message
        */

	if (24 > plen)
		return -1;

        plen-=24; /* adjust prefixlen - labellength */

	if (32 < plen)
		return -1;
	itemlen -= 4;

	memset(&addr, 0, sizeof(addr));
	plenbytes = (plen + 7) / 8;
	ND_TCHECK2(pptr[4], plenbytes);
	ITEMCHECK(plenbytes);
	memcpy(&addr, &pptr[4], plenbytes);
	if (plen % 8) {
		((u_char *)&addr)[plenbytes - 1] &=
			((0xff00 >> (plen % 8)) & 0xff);
	}
        /* the label may get offsetted by 4 bits so lets shift it right */
	snprintf(buf, buflen, "%s/%d, label:%u %s",
                 ipaddr_string(ndo, &addr),
                 plen,
                 EXTRACT_24BITS(pptr+1)>>4,
                 ((pptr[3]&1)==0) ? "(BOGUS: Bottom of Stack NOT set!)" : "(bottom)" );

	return 4 + plenbytes;

trunc:
	return -2;

badtlv:
	return -3;
}
