static struct nd_opt_hdr *ndisc_next_useropt(struct nd_opt_hdr *cur,
					     struct nd_opt_hdr *end)
{
	if (!cur || !end || cur >= end)
		return NULL;
	do {
		cur = ((void *)cur) + (cur->nd_opt_len << 3);
	} while (cur < end && !ndisc_is_useropt(cur));
	return cur <= end && ndisc_is_useropt(cur) ? cur : NULL;
}
