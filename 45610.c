static void netlink_sock_destruct(struct sock *sk)
{
	struct netlink_sock *nlk = nlk_sk(sk);

	if (nlk->cb_running) {
		if (nlk->cb.done)
			nlk->cb.done(&nlk->cb);

		module_put(nlk->cb.module);
		kfree_skb(nlk->cb.skb);
	}

	skb_queue_purge(&sk->sk_receive_queue);
#ifdef CONFIG_NETLINK_MMAP
	if (1) {
		struct nl_mmap_req req;

		memset(&req, 0, sizeof(req));
		if (nlk->rx_ring.pg_vec)
			netlink_set_ring(sk, &req, true, false);
		memset(&req, 0, sizeof(req));
		if (nlk->tx_ring.pg_vec)
			netlink_set_ring(sk, &req, true, true);
	}
#endif /* CONFIG_NETLINK_MMAP */

	if (!sock_flag(sk, SOCK_DEAD)) {
		printk(KERN_ERR "Freeing alive netlink socket %p\n", sk);
		return;
	}

	WARN_ON(atomic_read(&sk->sk_rmem_alloc));
	WARN_ON(atomic_read(&sk->sk_wmem_alloc));
	WARN_ON(nlk_sk(sk)->groups);
}
