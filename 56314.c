init_servarray(netdissect_options *ndo)
{
	struct servent *sv;
	register struct hnamemem *table;
	register int i;
	char buf[sizeof("0000000000")];

	while ((sv = getservent()) != NULL) {
		int port = ntohs(sv->s_port);
		i = port & (HASHNAMESIZE-1);
		if (strcmp(sv->s_proto, "tcp") == 0)
			table = &tporttable[i];
		else if (strcmp(sv->s_proto, "udp") == 0)
			table = &uporttable[i];
		else
			continue;

		while (table->name)
			table = table->nxt;
		if (ndo->ndo_nflag) {
			(void)snprintf(buf, sizeof(buf), "%d", port);
			table->name = strdup(buf);
		} else
			table->name = strdup(sv->s_name);
		if (table->name == NULL)
			(*ndo->ndo_error)(ndo, "init_servarray: strdup");

		table->addr = port;
		table->nxt = newhnamemem(ndo);
	}
	endservent();
}
