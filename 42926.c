ppp_print(netdissect_options *ndo,
          register const u_char *p, u_int length)
{
	u_int proto,ppp_header;
        u_int olen = length; /* _o_riginal length */
	u_int hdr_len = 0;

	/*
	 * Here, we assume that p points to the Address and Control
	 * field (if they present).
	 */
	if (length < 2)
		goto trunc;
	ND_TCHECK2(*p, 2);
        ppp_header = EXTRACT_16BITS(p);

        switch(ppp_header) {
        case (PPP_WITHDIRECTION_IN  << 8 | PPP_CONTROL):
            if (ndo->ndo_eflag) ND_PRINT((ndo, "In  "));
            p += 2;
            length -= 2;
            hdr_len += 2;
            break;
        case (PPP_WITHDIRECTION_OUT << 8 | PPP_CONTROL):
            if (ndo->ndo_eflag) ND_PRINT((ndo, "Out "));
            p += 2;
            length -= 2;
            hdr_len += 2;
            break;
        case (PPP_ADDRESS << 8 | PPP_CONTROL):
            p += 2;			/* ACFC not used */
            length -= 2;
            hdr_len += 2;
            break;

        default:
            break;
        }

	if (length < 2)
		goto trunc;
	ND_TCHECK(*p);
	if (*p % 2) {
		proto = *p;		/* PFC is used */
		p++;
		length--;
		hdr_len++;
	} else {
		ND_TCHECK2(*p, 2);
		proto = EXTRACT_16BITS(p);
		p += 2;
		length -= 2;
		hdr_len += 2;
	}

	if (ndo->ndo_eflag)
		ND_PRINT((ndo, "%s (0x%04x), length %u: ",
		          tok2str(ppptype2str, "unknown", proto),
		          proto,
		          olen));

	handle_ppp(ndo, proto, p, length);
	return (hdr_len);
trunc:
	ND_PRINT((ndo, "[|ppp]"));
	return (0);
}
