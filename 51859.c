void tipc_sk_reinit(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	const struct bucket_table *tbl;
	struct rhash_head *pos;
	struct tipc_sock *tsk;
	struct tipc_msg *msg;
	int i;

	rcu_read_lock();
	tbl = rht_dereference_rcu((&tn->sk_rht)->tbl, &tn->sk_rht);
	for (i = 0; i < tbl->size; i++) {
		rht_for_each_entry_rcu(tsk, pos, tbl, i, node) {
			spin_lock_bh(&tsk->sk.sk_lock.slock);
			msg = &tsk->phdr;
			msg_set_prevnode(msg, tn->own_addr);
			msg_set_orignode(msg, tn->own_addr);
			spin_unlock_bh(&tsk->sk.sk_lock.slock);
		}
	}
	rcu_read_unlock();
}
