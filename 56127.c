ipN_print(netdissect_options *ndo, register const u_char *bp, register u_int length)
{
	if (length < 1) {
		ND_PRINT((ndo, "truncated-ip %d", length));
		return;
	}

	ND_TCHECK(*bp);
	switch (*bp & 0xF0) {
	case 0x40:
		ip_print (ndo, bp, length);
		break;
	case 0x60:
		ip6_print (ndo, bp, length);
		break;
	default:
		ND_PRINT((ndo, "unknown ip %d", (*bp & 0xF0) >> 4));
		break;
	}
	return;

trunc:
	ND_PRINT((ndo, "%s", tstr));
	return;
}
