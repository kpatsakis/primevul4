static int __set_item(struct pppoe_net *pn, struct pppox_sock *po)
{
	int hash = hash_item(po->pppoe_pa.sid, po->pppoe_pa.remote);
	struct pppox_sock *ret;

	ret = pn->hash_table[hash];
	while (ret) {
		if (cmp_2_addr(&ret->pppoe_pa, &po->pppoe_pa) &&
		    ret->pppoe_ifindex == po->pppoe_ifindex)
			return -EALREADY;

		ret = ret->next;
	}

	po->next = pn->hash_table[hash];
	pn->hash_table[hash] = po;

	return 0;
}
