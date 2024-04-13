static int tipc_sk_publish(struct tipc_sock *tsk, uint scope,
			   struct tipc_name_seq const *seq)
{
	struct net *net = sock_net(&tsk->sk);
	struct publication *publ;
	u32 key;

	if (tsk->connected)
		return -EINVAL;
	key = tsk->portid + tsk->pub_count + 1;
	if (key == tsk->portid)
		return -EADDRINUSE;

	publ = tipc_nametbl_publish(net, seq->type, seq->lower, seq->upper,
				    scope, tsk->portid, key);
	if (unlikely(!publ))
		return -EINVAL;

	list_add(&publ->pport_list, &tsk->publications);
	tsk->pub_count++;
	tsk->published = 1;
	return 0;
}
