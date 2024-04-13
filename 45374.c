static inline struct pppox_sock *pppoe_get_idx(struct pppoe_net *pn, loff_t pos)
{
	struct pppox_sock *po;
	int i;

	for (i = 0; i < PPPOE_HASH_SIZE; i++) {
		po = pn->hash_table[i];
		while (po) {
			if (!pos--)
				goto out;
			po = po->next;
		}
	}

out:
	return po;
}
