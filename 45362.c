static inline int cmp_2_addr(struct pppoe_addr *a, struct pppoe_addr *b)
{
	return a->sid == b->sid && !memcmp(a->remote, b->remote, ETH_ALEN);
}
