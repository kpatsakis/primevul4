static void __delete_item(struct pppoe_net *pn, __be16 sid,
					char *addr, int ifindex)
{
	int hash = hash_item(sid, addr);
	struct pppox_sock *ret, **src;

	ret = pn->hash_table[hash];
	src = &pn->hash_table[hash];

	while (ret) {
		if (cmp_addr(&ret->pppoe_pa, sid, addr) &&
		    ret->pppoe_ifindex == ifindex) {
			*src = ret->next;
			break;
		}

		src = &ret->next;
		ret = ret->next;
	}
}
