safeputchar(netdissect_options *ndo,
            const u_char c)
{
	ND_PRINT((ndo, (c < 0x80 && ND_ISPRINT(c)) ? "%c" : "\\0x%02x", c));
}
