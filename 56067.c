as_printf(netdissect_options *ndo,
          char *str, int size, u_int asnum)
{
	if (!ndo->ndo_bflag || asnum <= 0xFFFF) {
		snprintf(str, size, "%u", asnum);
	} else {
		snprintf(str, size, "%u.%u", asnum >> 16, asnum & 0xFFFF);
	}
	return str;
}
