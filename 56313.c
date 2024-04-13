init_protoidarray(netdissect_options *ndo)
{
	register int i;
	register struct protoidmem *tp;
	const struct protoidlist *pl;
	u_char protoid[5];

	protoid[0] = 0;
	protoid[1] = 0;
	protoid[2] = 0;
	for (i = 0; eproto_db[i].s; i++) {
		u_short etype = htons(eproto_db[i].p);

		memcpy((char *)&protoid[3], (char *)&etype, 2);
		tp = lookup_protoid(ndo, protoid);
		tp->p_name = strdup(eproto_db[i].s);
		if (tp->p_name == NULL)
			(*ndo->ndo_error)(ndo,
					  "init_protoidarray: strdup(eproto_db[i].s)");
	}
	/* Hardwire some SNAP proto ID names */
	for (pl = protoidlist; pl->name != NULL; ++pl) {
		tp = lookup_protoid(ndo, pl->protoid);
		/* Don't override existing name */
		if (tp->p_name != NULL)
			continue;

		tp->p_name = pl->name;
	}
}
