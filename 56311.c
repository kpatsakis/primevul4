init_etherarray(netdissect_options *ndo)
{
	register const struct etherlist *el;
	register struct enamemem *tp;
#ifdef USE_ETHER_NTOHOST
	char name[256];
#else
	register struct pcap_etherent *ep;
	register FILE *fp;

	/* Suck in entire ethers file */
	fp = fopen(PCAP_ETHERS_FILE, "r");
	if (fp != NULL) {
		while ((ep = pcap_next_etherent(fp)) != NULL) {
			tp = lookup_emem(ndo, ep->addr);
			tp->e_name = strdup(ep->name);
			if (tp->e_name == NULL)
				(*ndo->ndo_error)(ndo,
						  "init_etherarray: strdup(ep->addr)");
		}
		(void)fclose(fp);
	}
#endif

	/* Hardwire some ethernet names */
	for (el = etherlist; el->name != NULL; ++el) {
		tp = lookup_emem(ndo, el->addr);
		/* Don't override existing name */
		if (tp->e_name != NULL)
			continue;

#ifdef USE_ETHER_NTOHOST
		/*
		 * Use YP/NIS version of name if available.
		 */
		if (ether_ntohost(name, (const struct ether_addr *)el->addr) == 0) {
			tp->e_name = strdup(name);
			if (tp->e_name == NULL)
				(*ndo->ndo_error)(ndo,
						  "init_etherarray: strdup(name)");
			continue;
		}
#endif
		tp->e_name = el->name;
	}
}
