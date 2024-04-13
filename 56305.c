etheraddr_string(netdissect_options *ndo, register const u_char *ep)
{
	register int i;
	register char *cp;
	register struct enamemem *tp;
	int oui;
	char buf[BUFSIZE];

	tp = lookup_emem(ndo, ep);
	if (tp->e_name)
		return (tp->e_name);
#ifdef USE_ETHER_NTOHOST
	if (!ndo->ndo_nflag) {
		char buf2[BUFSIZE];

		if (ether_ntohost(buf2, (const struct ether_addr *)ep) == 0) {
			tp->e_name = strdup(buf2);
			if (tp->e_name == NULL)
				(*ndo->ndo_error)(ndo,
						  "etheraddr_string: strdup(buf2)");
			return (tp->e_name);
		}
	}
#endif
	cp = buf;
	oui = EXTRACT_24BITS(ep);
	*cp++ = hex[*ep >> 4 ];
	*cp++ = hex[*ep++ & 0xf];
	for (i = 5; --i >= 0;) {
		*cp++ = ':';
		*cp++ = hex[*ep >> 4 ];
		*cp++ = hex[*ep++ & 0xf];
	}

	if (!ndo->ndo_nflag) {
		snprintf(cp, BUFSIZE - (2 + 5*3), " (oui %s)",
		    tok2str(oui_values, "Unknown", oui));
	} else
		*cp = '\0';
	tp->e_name = strdup(buf);
	if (tp->e_name == NULL)
		(*ndo->ndo_error)(ndo, "etheraddr_string: strdup(buf)");
	return (tp->e_name);
}
