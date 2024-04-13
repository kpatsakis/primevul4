struct dst_entry *inet_csk_update_pmtu(struct sock *sk, u32 mtu)
{
	struct dst_entry *dst = __sk_dst_check(sk, 0);
	struct inet_sock *inet = inet_sk(sk);

	if (!dst) {
		dst = inet_csk_rebuild_route(sk, &inet->cork.fl);
		if (!dst)
			goto out;
	}
	dst->ops->update_pmtu(dst, sk, NULL, mtu);

	dst = __sk_dst_check(sk, 0);
	if (!dst)
		dst = inet_csk_rebuild_route(sk, &inet->cork.fl);
out:
	return dst;
}
