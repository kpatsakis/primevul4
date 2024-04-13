static int tipc_sk_withdraw(struct tipc_sock *tsk, uint scope,
			    struct tipc_name_seq const *seq)
{
	struct net *net = sock_net(&tsk->sk);
	struct publication *publ;
	struct publication *safe;
	int rc = -EINVAL;

	list_for_each_entry_safe(publ, safe, &tsk->publications, pport_list) {
		if (seq) {
			if (publ->scope != scope)
				continue;
			if (publ->type != seq->type)
				continue;
			if (publ->lower != seq->lower)
				continue;
			if (publ->upper != seq->upper)
				break;
			tipc_nametbl_withdraw(net, publ->type, publ->lower,
					      publ->ref, publ->key);
			rc = 0;
			break;
		}
		tipc_nametbl_withdraw(net, publ->type, publ->lower,
				      publ->ref, publ->key);
		rc = 0;
	}
	if (list_empty(&tsk->publications))
		tsk->published = 0;
	return rc;
}
