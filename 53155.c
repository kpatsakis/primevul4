static bool tcp_syn_flood_action(const struct sock *sk,
				 const struct sk_buff *skb,
				 const char *proto)
{
	struct request_sock_queue *queue = &inet_csk(sk)->icsk_accept_queue;
	const char *msg = "Dropping request";
	bool want_cookie = false;

#ifdef CONFIG_SYN_COOKIES
	if (sysctl_tcp_syncookies) {
		msg = "Sending cookies";
		want_cookie = true;
		NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_TCPREQQFULLDOCOOKIES);
	} else
#endif
		NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_TCPREQQFULLDROP);

	if (!queue->synflood_warned &&
	    sysctl_tcp_syncookies != 2 &&
	    xchg(&queue->synflood_warned, 1) == 0)
		pr_info("%s: Possible SYN flooding on port %d. %s.  Check SNMP counters.\n",
			proto, ntohs(tcp_hdr(skb)->dest), msg);

	return want_cookie;
}
