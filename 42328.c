static int rfcomm_sock_debugfs_show(struct seq_file *f, void *p)
{
	struct sock *sk;
	struct hlist_node *node;

	read_lock(&rfcomm_sk_list.lock);

	sk_for_each(sk, node, &rfcomm_sk_list.head) {
		seq_printf(f, "%s %s %d %d\n",
				batostr(&bt_sk(sk)->src),
				batostr(&bt_sk(sk)->dst),
				sk->sk_state, rfcomm_pi(sk)->channel);
	}

	read_unlock(&rfcomm_sk_list.lock);

	return 0;
}
