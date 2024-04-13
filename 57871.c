inet_csk_find_open_port(struct sock *sk, struct inet_bind_bucket **tb_ret, int *port_ret)
{
	struct inet_hashinfo *hinfo = sk->sk_prot->h.hashinfo;
	int port = 0;
	struct inet_bind_hashbucket *head;
	struct net *net = sock_net(sk);
	int i, low, high, attempt_half;
	struct inet_bind_bucket *tb;
	u32 remaining, offset;

	attempt_half = (sk->sk_reuse == SK_CAN_REUSE) ? 1 : 0;
other_half_scan:
	inet_get_local_port_range(net, &low, &high);
	high++; /* [32768, 60999] -> [32768, 61000[ */
	if (high - low < 4)
		attempt_half = 0;
	if (attempt_half) {
		int half = low + (((high - low) >> 2) << 1);

		if (attempt_half == 1)
			high = half;
		else
			low = half;
	}
	remaining = high - low;
	if (likely(remaining > 1))
		remaining &= ~1U;

	offset = prandom_u32() % remaining;
	/* __inet_hash_connect() favors ports having @low parity
	 * We do the opposite to not pollute connect() users.
	 */
	offset |= 1U;

other_parity_scan:
	port = low + offset;
	for (i = 0; i < remaining; i += 2, port += 2) {
		if (unlikely(port >= high))
			port -= remaining;
		if (inet_is_local_reserved_port(net, port))
			continue;
		head = &hinfo->bhash[inet_bhashfn(net, port,
						  hinfo->bhash_size)];
		spin_lock_bh(&head->lock);
		inet_bind_bucket_for_each(tb, &head->chain)
			if (net_eq(ib_net(tb), net) && tb->port == port) {
				if (!inet_csk_bind_conflict(sk, tb, false, false))
					goto success;
				goto next_port;
			}
		tb = NULL;
		goto success;
next_port:
		spin_unlock_bh(&head->lock);
		cond_resched();
	}

	offset--;
	if (!(offset & 1))
		goto other_parity_scan;

	if (attempt_half == 1) {
		/* OK we now try the upper half of the range */
		attempt_half = 2;
		goto other_half_scan;
	}
	return NULL;
success:
	*port_ret = port;
	*tb_ret = tb;
	return head;
}
