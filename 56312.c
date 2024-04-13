init_ipxsaparray(netdissect_options *ndo)
{
	register int i;
	register struct hnamemem *table;

	for (i = 0; ipxsap_db[i].s != NULL; i++) {
		int j = htons(ipxsap_db[i].v) & (HASHNAMESIZE-1);
		table = &ipxsaptable[j];
		while (table->name)
			table = table->nxt;
		table->name = ipxsap_db[i].s;
		table->addr = htons(ipxsap_db[i].v);
		table->nxt = newhnamemem(ndo);
	}
}
