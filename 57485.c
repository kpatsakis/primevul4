struct dst_entry *ip6_dst_lookup_flow(const struct sock *sk, struct flowi6 *fl6,
				      const struct in6_addr *final_dst)
{
	struct dst_entry *dst = NULL;
	int err;

	err = ip6_dst_lookup_tail(sock_net(sk), sk, &dst, fl6);
	if (err)
		return ERR_PTR(err);
	if (final_dst)
		fl6->daddr = *final_dst;

	return xfrm_lookup_route(sock_net(sk), dst, flowi6_to_flowi(fl6), sk, 0);
}
