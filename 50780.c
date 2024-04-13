static void tcp_v6_reqsk_destructor(struct request_sock *req)
{
	kfree(inet_rsk(req)->ipv6_opt);
	kfree_skb(inet_rsk(req)->pktopts);
}
