static inline int sctp_v4_xmit(struct sk_buff *skb,
			       struct sctp_transport *transport)
{
	struct inet_sock *inet = inet_sk(skb->sk);

	pr_debug("%s: skb:%p, len:%d, src:%pI4, dst:%pI4\n", __func__, skb,
		 skb->len, &transport->fl.u.ip4.saddr, &transport->fl.u.ip4.daddr);

	inet->pmtudisc = transport->param_flags & SPP_PMTUD_ENABLE ?
			 IP_PMTUDISC_DO : IP_PMTUDISC_DONT;

	SCTP_INC_STATS(sock_net(&inet->sk), SCTP_MIB_OUTSCTPPACKS);

	return ip_queue_xmit(&inet->sk, skb, &transport->fl);
}
