newh6namemem(netdissect_options *ndo)
{
	register struct h6namemem *p;
	static struct h6namemem *ptr = NULL;
	static u_int num = 0;

	if (num  <= 0) {
		num = 64;
		ptr = (struct h6namemem *)calloc(num, sizeof (*ptr));
		if (ptr == NULL)
			(*ndo->ndo_error)(ndo, "newh6namemem: calloc");
	}
	--num;
	p = ptr++;
	return (p);
}
