static struct packet_fanout *fanout_release(struct sock *sk)
{
	struct packet_sock *po = pkt_sk(sk);
	struct packet_fanout *f;

	mutex_lock(&fanout_mutex);
	f = po->fanout;
	if (f) {
		po->fanout = NULL;

		if (refcount_dec_and_test(&f->sk_ref))
			list_del(&f->list);
		else
			f = NULL;

		if (po->rollover)
			kfree_rcu(po->rollover, rcu);
	}
	mutex_unlock(&fanout_mutex);

	return f;
}
