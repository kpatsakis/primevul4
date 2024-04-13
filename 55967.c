static void __build_flow_key(const struct net *net, struct flowi4 *fl4,
			     const struct sock *sk,
			     const struct iphdr *iph,
			     int oif, u8 tos,
			     u8 prot, u32 mark, int flow_flags)
{
	if (sk) {
		const struct inet_sock *inet = inet_sk(sk);

		oif = sk->sk_bound_dev_if;
		mark = sk->sk_mark;
		tos = RT_CONN_FLAGS(sk);
		prot = inet->hdrincl ? IPPROTO_RAW : sk->sk_protocol;
	}
	flowi4_init_output(fl4, oif, mark, tos,
			   RT_SCOPE_UNIVERSE, prot,
			   flow_flags,
			   iph->daddr, iph->saddr, 0, 0,
			   sock_net_uid(net, sk));
}
