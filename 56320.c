protoid_string(netdissect_options *ndo, register const u_char *pi)
{
	register u_int i, j;
	register char *cp;
	register struct protoidmem *tp;
	char buf[sizeof("00:00:00:00:00")];

	tp = lookup_protoid(ndo, pi);
	if (tp->p_name)
		return tp->p_name;

	cp = buf;
	if ((j = *pi >> 4) != 0)
		*cp++ = hex[j];
	*cp++ = hex[*pi++ & 0xf];
	for (i = 4; (int)--i >= 0;) {
		*cp++ = ':';
		if ((j = *pi >> 4) != 0)
			*cp++ = hex[j];
		*cp++ = hex[*pi++ & 0xf];
	}
	*cp = '\0';
	tp->p_name = strdup(buf);
	if (tp->p_name == NULL)
		(*ndo->ndo_error)(ndo, "protoid_string: strdup(buf)");
	return (tp->p_name);
}
