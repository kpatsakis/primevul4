ospf6_decode_at(netdissect_options *ndo,
                const u_char *cp, const u_int len)
{
	uint16_t authdatalen;

	if (len == 0)
		return 0;
	if (len < OSPF6_AT_HDRLEN)
		goto trunc;
	/* Authentication Type */
	ND_TCHECK2(*cp, 2);
	ND_PRINT((ndo, "\n\tAuthentication Type %s", tok2str(ospf6_auth_type_str, "unknown (0x%04x)", EXTRACT_16BITS(cp))));
	cp += 2;
	/* Auth Data Len */
	ND_TCHECK2(*cp, 2);
	authdatalen = EXTRACT_16BITS(cp);
	ND_PRINT((ndo, ", Length %u", authdatalen));
	if (authdatalen < OSPF6_AT_HDRLEN || authdatalen > len)
		goto trunc;
	cp += 2;
	/* Reserved */
	ND_TCHECK2(*cp, 2);
	cp += 2;
	/* Security Association ID */
	ND_TCHECK2(*cp, 2);
	ND_PRINT((ndo, ", SAID %u", EXTRACT_16BITS(cp)));
	cp += 2;
	/* Cryptographic Sequence Number (High-Order 32 Bits) */
	ND_TCHECK2(*cp, 4);
	ND_PRINT((ndo, ", CSN 0x%08x", EXTRACT_32BITS(cp)));
	cp += 4;
	/* Cryptographic Sequence Number (Low-Order 32 Bits) */
	ND_TCHECK2(*cp, 4);
	ND_PRINT((ndo, ":%08x", EXTRACT_32BITS(cp)));
	cp += 4;
	/* Authentication Data */
	ND_TCHECK2(*cp, authdatalen - OSPF6_AT_HDRLEN);
	if (ndo->ndo_vflag > 1)
		print_unknown_data(ndo,cp, "\n\tAuthentication Data ", authdatalen - OSPF6_AT_HDRLEN);
	return 0;

trunc:
	return 1;
}
