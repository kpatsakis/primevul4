getname(netdissect_options *ndo, const u_char *ap)
{
	register struct hostent *hp;
	uint32_t addr;
	struct hnamemem *p;

	memcpy(&addr, ap, sizeof(addr));
	p = &hnametable[addr & (HASHNAMESIZE-1)];
	for (; p->nxt; p = p->nxt) {
		if (p->addr == addr)
			return (p->name);
	}
	p->addr = addr;
	p->nxt = newhnamemem(ndo);

	/*
	 * Print names unless:
	 *	(1) -n was given.
	 *      (2) Address is foreign and -f was given. (If -f was not
	 *	    given, f_netmask and f_localnet are 0 and the test
	 *	    evaluates to true)
	 */
	if (!ndo->ndo_nflag &&
	    (addr & f_netmask) == f_localnet) {
#ifdef HAVE_CASPER
		if (capdns != NULL) {
			hp = cap_gethostbyaddr(capdns, (char *)&addr, 4,
			    AF_INET);
		} else
#endif
			hp = gethostbyaddr((char *)&addr, 4, AF_INET);
		if (hp) {
			char *dotp;

			p->name = strdup(hp->h_name);
			if (p->name == NULL)
				(*ndo->ndo_error)(ndo,
						  "getname: strdup(hp->h_name)");
			if (ndo->ndo_Nflag) {
				/* Remove domain qualifications */
				dotp = strchr(p->name, '.');
				if (dotp)
					*dotp = '\0';
			}
			return (p->name);
		}
	}
	p->name = strdup(intoa(addr));
	if (p->name == NULL)
		(*ndo->ndo_error)(ndo, "getname: strdup(intoa(addr))");
	return (p->name);
}
