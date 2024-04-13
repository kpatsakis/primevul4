getname6(netdissect_options *ndo, const u_char *ap)
{
	register struct hostent *hp;
	union {
		struct in6_addr addr;
		struct for_hash_addr {
			char fill[14];
			uint16_t d;
		} addra;
	} addr;
	struct h6namemem *p;
	register const char *cp;
	char ntop_buf[INET6_ADDRSTRLEN];

	memcpy(&addr, ap, sizeof(addr));
	p = &h6nametable[addr.addra.d & (HASHNAMESIZE-1)];
	for (; p->nxt; p = p->nxt) {
		if (memcmp(&p->addr, &addr, sizeof(addr)) == 0)
			return (p->name);
	}
	p->addr = addr.addr;
	p->nxt = newh6namemem(ndo);

	/*
	 * Do not print names if -n was given.
	 */
	if (!ndo->ndo_nflag) {
#ifdef HAVE_CASPER
		if (capdns != NULL) {
			hp = cap_gethostbyaddr(capdns, (char *)&addr,
			    sizeof(addr), AF_INET6);
		} else
#endif
			hp = gethostbyaddr((char *)&addr, sizeof(addr),
			    AF_INET6);
		if (hp) {
			char *dotp;

			p->name = strdup(hp->h_name);
			if (p->name == NULL)
				(*ndo->ndo_error)(ndo,
						  "getname6: strdup(hp->h_name)");
			if (ndo->ndo_Nflag) {
				/* Remove domain qualifications */
				dotp = strchr(p->name, '.');
				if (dotp)
					*dotp = '\0';
			}
			return (p->name);
		}
	}
	cp = addrtostr6(ap, ntop_buf, sizeof(ntop_buf));
	p->name = strdup(cp);
	if (p->name == NULL)
		(*ndo->ndo_error)(ndo, "getname6: strdup(cp)");
	return (p->name);
}
