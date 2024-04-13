static void *listening_get_next(struct seq_file *seq, void *cur)
{
	struct tcp_iter_state *st = seq->private;
	struct net *net = seq_file_net(seq);
	struct inet_listen_hashbucket *ilb;
	struct sock *sk = cur;

	if (!sk) {
get_head:
		ilb = &tcp_hashinfo.listening_hash[st->bucket];
		spin_lock_bh(&ilb->lock);
		sk = sk_head(&ilb->head);
		st->offset = 0;
		goto get_sk;
	}
	ilb = &tcp_hashinfo.listening_hash[st->bucket];
	++st->num;
	++st->offset;

	sk = sk_next(sk);
get_sk:
	sk_for_each_from(sk) {
		if (!net_eq(sock_net(sk), net))
			continue;
		if (sk->sk_family == st->family)
			return sk;
	}
	spin_unlock_bh(&ilb->lock);
	st->offset = 0;
	if (++st->bucket < INET_LHTABLE_SIZE)
		goto get_head;
	return NULL;
}
