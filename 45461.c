static int rfcomm_sock_debugfs_show(struct seq_file *f, void *p)
{
	struct sock *sk;

	read_lock(&rfcomm_sk_list.lock);

	sk_for_each(sk, &rfcomm_sk_list.head) {
		seq_printf(f, "%pMR %pMR %d %d\n",
			   &rfcomm_pi(sk)->src, &rfcomm_pi(sk)->dst,
			   sk->sk_state, rfcomm_pi(sk)->channel);
	}

	read_unlock(&rfcomm_sk_list.lock);

	return 0;
}
