static void ipxitf_def_skb_handler(struct sock *sock, struct sk_buff *skb)
{
	if (sock_queue_rcv_skb(sock, skb) < 0)
		kfree_skb(skb);
}
