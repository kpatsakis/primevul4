static void *established_get_next(struct seq_file *seq, void *cur)
{
	struct sock *sk = cur;
	struct hlist_nulls_node *node;
	struct tcp_iter_state *st = seq->private;
	struct net *net = seq_file_net(seq);

	++st->num;
	++st->offset;

	sk = sk_nulls_next(sk);

	sk_nulls_for_each_from(sk, node) {
		if (sk->sk_family == st->family && net_eq(sock_net(sk), net))
			return sk;
	}

	spin_unlock_bh(inet_ehash_lockp(&tcp_hashinfo, st->bucket));
	++st->bucket;
	return established_get_first(seq);
}
