static inline int cmp_addr(struct pppoe_addr *a, __be16 sid, char *addr)
{
	return a->sid == sid && !memcmp(a->remote, addr, ETH_ALEN);
}
