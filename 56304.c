dnaddr_string(netdissect_options *ndo, u_short dnaddr)
{
	register struct hnamemem *tp;

	for (tp = &dnaddrtable[dnaddr & (HASHNAMESIZE-1)]; tp->nxt != NULL;
	     tp = tp->nxt)
		if (tp->addr == dnaddr)
			return (tp->name);

	tp->addr = dnaddr;
	tp->nxt = newhnamemem(ndo);
	if (ndo->ndo_nflag)
		tp->name = dnnum_string(ndo, dnaddr);
	else
		tp->name = dnname_string(ndo, dnaddr);

	return(tp->name);
}
