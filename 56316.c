isonsap_string(netdissect_options *ndo, const u_char *nsap,
	       register u_int nsap_length)
{
	register u_int nsap_idx;
	register char *cp;
	register struct enamemem *tp;

	if (nsap_length < 1 || nsap_length > ISONSAP_MAX_LENGTH)
		return ("isonsap_string: illegal length");

	tp = lookup_nsap(ndo, nsap, nsap_length);
	if (tp->e_name)
		return tp->e_name;

	tp->e_name = cp = (char *)malloc(sizeof("xx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xx"));
	if (cp == NULL)
		(*ndo->ndo_error)(ndo, "isonsap_string: malloc");

	for (nsap_idx = 0; nsap_idx < nsap_length; nsap_idx++) {
		*cp++ = hex[*nsap >> 4];
		*cp++ = hex[*nsap++ & 0xf];
		if (((nsap_idx & 1) == 0) &&
		     (nsap_idx + 1 < nsap_length)) {
		     	*cp++ = '.';
		}
	}
	*cp = '\0';
	return (tp->e_name);
}
