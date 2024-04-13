newhnamemem(netdissect_options *ndo)
{
	register struct hnamemem *p;
	static struct hnamemem *ptr = NULL;
	static u_int num = 0;

	if (num  <= 0) {
		num = 64;
		ptr = (struct hnamemem *)calloc(num, sizeof (*ptr));
		if (ptr == NULL)
			(*ndo->ndo_error)(ndo, "newhnamemem: calloc");
	}
	--num;
	p = ptr++;
	return (p);
}
