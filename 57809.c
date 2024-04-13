static int sctp_v6_xmit(struct sk_buff *skb, struct sctp_transport *transport)
{
	struct sock *sk = skb->sk;
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct flowi6 *fl6 = &transport->fl.u.ip6;
	int res;

	pr_debug("%s: skb:%p, len:%d, src:%pI6 dst:%pI6\n", __func__, skb,
		 skb->len, &fl6->saddr, &fl6->daddr);

	IP6_ECN_flow_xmit(sk, fl6->flowlabel);

	if (!(transport->param_flags & SPP_PMTUD_ENABLE))
		skb->ignore_df = 1;

	SCTP_INC_STATS(sock_net(sk), SCTP_MIB_OUTSCTPPACKS);

	rcu_read_lock();
	res = ip6_xmit(sk, skb, fl6, sk->sk_mark, rcu_dereference(np->opt),
		       np->tclass);
	rcu_read_unlock();
	return res;
}
