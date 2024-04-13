gre_print_1(netdissect_options *ndo, const u_char *bp, u_int length)
{
	u_int len = length;
	uint16_t flags, prot;

	/* 16 bits ND_TCHECKed in gre_print() */
	flags = EXTRACT_16BITS(bp);
	len -= 2;
	bp += 2;

	if (ndo->ndo_vflag)
            ND_PRINT((ndo, ", Flags [%s]",
                   bittok2str(gre_flag_values,"none",flags)));

	ND_TCHECK2(*bp, 2);
	if (len < 2)
		goto trunc;
	prot = EXTRACT_16BITS(bp);
	len -= 2;
	bp += 2;


	if (flags & GRE_KP) {
		uint32_t k;

		ND_TCHECK2(*bp, 4);
		if (len < 4)
			goto trunc;
		k = EXTRACT_32BITS(bp);
		ND_PRINT((ndo, ", call %d", k & 0xffff));
		len -= 4;
		bp += 4;
	}

	if (flags & GRE_SP) {
		ND_TCHECK2(*bp, 4);
		if (len < 4)
			goto trunc;
		ND_PRINT((ndo, ", seq %u", EXTRACT_32BITS(bp)));
		bp += 4;
		len -= 4;
	}

	if (flags & GRE_AP) {
		ND_TCHECK2(*bp, 4);
		if (len < 4)
			goto trunc;
		ND_PRINT((ndo, ", ack %u", EXTRACT_32BITS(bp)));
		bp += 4;
		len -= 4;
	}

	if ((flags & GRE_SP) == 0)
		ND_PRINT((ndo, ", no-payload"));

        if (ndo->ndo_eflag)
            ND_PRINT((ndo, ", proto %s (0x%04x)",
                   tok2str(ethertype_values,"unknown",prot),
                   prot));

        ND_PRINT((ndo, ", length %u",length));

        if ((flags & GRE_SP) == 0)
            return;

        if (ndo->ndo_vflag < 1)
            ND_PRINT((ndo, ": ")); /* put in a colon as protocol demarc */
        else
            ND_PRINT((ndo, "\n\t")); /* if verbose go multiline */

	switch (prot) {
	case ETHERTYPE_PPP:
		ppp_print(ndo, bp, len);
		break;
	default:
		ND_PRINT((ndo, "gre-proto-0x%x", prot));
		break;
	}
	return;

trunc:
	ND_PRINT((ndo, "%s", tstr));
}
