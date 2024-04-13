static struct pppox_sock *__get_item(struct pppoe_net *pn, __be16 sid,
				unsigned char *addr, int ifindex)
{
	int hash = hash_item(sid, addr);
	struct pppox_sock *ret;

	ret = pn->hash_table[hash];
	while (ret) {
		if (cmp_addr(&ret->pppoe_pa, sid, addr) &&
		    ret->pppoe_ifindex == ifindex)
			return ret;

		ret = ret->next;
	}

	return NULL;
}
