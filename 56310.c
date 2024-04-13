init_eprotoarray(netdissect_options *ndo)
{
	register int i;
	register struct hnamemem *table;

	for (i = 0; eproto_db[i].s; i++) {
		int j = htons(eproto_db[i].p) & (HASHNAMESIZE-1);
		table = &eprototable[j];
		while (table->name)
			table = table->nxt;
		table->name = eproto_db[i].s;
		table->addr = htons(eproto_db[i].p);
		table->nxt = newhnamemem(ndo);
	}
}
