static void *pppoe_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct pppoe_net *pn = pppoe_pernet(seq_file_net(seq));
	struct pppox_sock *po;

	++*pos;
	if (v == SEQ_START_TOKEN) {
		po = pppoe_get_idx(pn, 0);
		goto out;
	}
	po = v;
	if (po->next)
		po = po->next;
	else {
		int hash = hash_item(po->pppoe_pa.sid, po->pppoe_pa.remote);

		po = NULL;
		while (++hash < PPPOE_HASH_SIZE) {
			po = pn->hash_table[hash];
			if (po)
				break;
		}
	}

out:
	return po;
}
