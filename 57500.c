struct dst_entry *ip6_sk_dst_lookup_flow(struct sock *sk, struct flowi6 *fl6,
					 const struct in6_addr *final_dst)
{
	struct dst_entry *dst = sk_dst_check(sk, inet6_sk(sk)->dst_cookie);

	dst = ip6_sk_dst_check(sk, dst, fl6);
	if (!dst)
		dst = ip6_dst_lookup_flow(sk, fl6, final_dst);

	return dst;
}
