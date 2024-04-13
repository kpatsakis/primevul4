static int netlink_autobind(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct netlink_table *table = &nl_table[sk->sk_protocol];
	struct nl_portid_hash *hash = &table->hash;
	struct hlist_head *head;
	struct sock *osk;
	s32 portid = task_tgid_vnr(current);
	int err;
	static s32 rover = -4097;

retry:
	cond_resched();
	netlink_table_grab();
	head = nl_portid_hashfn(hash, portid);
	sk_for_each(osk, head) {
		if (!table->compare(net, osk))
			continue;
		if (nlk_sk(osk)->portid == portid) {
			/* Bind collision, search negative portid values. */
			portid = rover--;
			if (rover > -4097)
				rover = -4097;
			netlink_table_ungrab();
			goto retry;
		}
	}
	netlink_table_ungrab();

	err = netlink_insert(sk, net, portid);
	if (err == -EADDRINUSE)
		goto retry;

	/* If 2 threads race to autobind, that is fine.  */
	if (err == -EBUSY)
		err = 0;

	return err;
}
