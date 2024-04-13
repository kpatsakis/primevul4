chdlc_slarp_print(netdissect_options *ndo, const u_char *cp, u_int length)
{
	const struct cisco_slarp *slarp;
        u_int sec,min,hrs,days;

	ND_PRINT((ndo, "SLARP (length: %u), ",length));
	if (length < SLARP_MIN_LEN)
		goto trunc;

	slarp = (const struct cisco_slarp *)cp;
	ND_TCHECK2(*slarp, SLARP_MIN_LEN);
	switch (EXTRACT_32BITS(&slarp->code)) {
	case SLARP_REQUEST:
		ND_PRINT((ndo, "request"));
		/*
		 * At least according to William "Chops" Westfield's
		 * message in
		 *
		 *	http://www.nethelp.no/net/cisco-hdlc.txt
		 *
		 * the address and mask aren't used in requests -
		 * they're just zero.
		 */
		break;
	case SLARP_REPLY:
		ND_PRINT((ndo, "reply %s/%s",
			ipaddr_string(ndo, &slarp->un.addr.addr),
			ipaddr_string(ndo, &slarp->un.addr.mask)));
		break;
	case SLARP_KEEPALIVE:
		ND_PRINT((ndo, "keepalive: mineseen=0x%08x, yourseen=0x%08x, reliability=0x%04x",
                       EXTRACT_32BITS(&slarp->un.keep.myseq),
                       EXTRACT_32BITS(&slarp->un.keep.yourseq),
                       EXTRACT_16BITS(&slarp->un.keep.rel)));

                if (length >= SLARP_MAX_LEN) { /* uptime-stamp is optional */
                        cp += SLARP_MIN_LEN;
                        ND_TCHECK2(*cp, 4);
                        sec = EXTRACT_32BITS(cp) / 1000;
                        min = sec / 60; sec -= min * 60;
                        hrs = min / 60; min -= hrs * 60;
                        days = hrs / 24; hrs -= days * 24;
                        ND_PRINT((ndo, ", link uptime=%ud%uh%um%us",days,hrs,min,sec));
                }
		break;
	default:
		ND_PRINT((ndo, "0x%02x unknown", EXTRACT_32BITS(&slarp->code)));
                if (ndo->ndo_vflag <= 1)
                    print_unknown_data(ndo,cp+4,"\n\t",length-4);
		break;
	}

	if (SLARP_MAX_LEN < length && ndo->ndo_vflag)
		ND_PRINT((ndo, ", (trailing junk: %d bytes)", length - SLARP_MAX_LEN));
        if (ndo->ndo_vflag > 1)
            print_unknown_data(ndo,cp+4,"\n\t",length-4);
	return;

trunc:
	ND_PRINT((ndo, "[|slarp]"));
}
