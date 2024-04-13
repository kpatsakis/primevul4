int tipc_nl_sk_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	int err;
	struct tipc_sock *tsk;
	const struct bucket_table *tbl;
	struct rhash_head *pos;
	struct net *net = sock_net(skb->sk);
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	u32 tbl_id = cb->args[0];
	u32 prev_portid = cb->args[1];

	rcu_read_lock();
	tbl = rht_dereference_rcu((&tn->sk_rht)->tbl, &tn->sk_rht);
	for (; tbl_id < tbl->size; tbl_id++) {
		rht_for_each_entry_rcu(tsk, pos, tbl, tbl_id, node) {
			spin_lock_bh(&tsk->sk.sk_lock.slock);
			if (prev_portid && prev_portid != tsk->portid) {
				spin_unlock_bh(&tsk->sk.sk_lock.slock);
				continue;
			}

			err = __tipc_nl_add_sk(skb, cb, tsk);
			if (err) {
				prev_portid = tsk->portid;
				spin_unlock_bh(&tsk->sk.sk_lock.slock);
				goto out;
			}
			prev_portid = 0;
			spin_unlock_bh(&tsk->sk.sk_lock.slock);
		}
	}
out:
	rcu_read_unlock();
	cb->args[0] = tbl_id;
	cb->args[1] = prev_portid;

	return skb->len;
}
