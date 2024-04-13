fn_print_char(netdissect_options *ndo, u_char c)
{
	if (!ND_ISASCII(c)) {
		c = ND_TOASCII(c);
		ND_PRINT((ndo, "M-"));
	}
	if (!ND_ISPRINT(c)) {
		c ^= 0x40;	/* DEL to ?, others to alpha */
		ND_PRINT((ndo, "^"));
	}
	ND_PRINT((ndo, "%c", c));
}
