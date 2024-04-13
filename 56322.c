udpport_string(netdissect_options *ndo, register u_short port)
{
	register struct hnamemem *tp;
	register uint32_t i = port;
	char buf[sizeof("00000")];

	for (tp = &uporttable[i & (HASHNAMESIZE-1)]; tp->nxt; tp = tp->nxt)
		if (tp->addr == i)
			return (tp->name);

	tp->addr = i;
	tp->nxt = newhnamemem(ndo);

	(void)snprintf(buf, sizeof(buf), "%u", i);
	tp->name = strdup(buf);
	if (tp->name == NULL)
		(*ndo->ndo_error)(ndo, "udpport_string: strdup(buf)");
	return (tp->name);
}
