bool ping_rcv(struct sk_buff *skb)
{
	struct sock *sk;
	struct net *net = dev_net(skb->dev);
	struct icmphdr *icmph = icmp_hdr(skb);

	/* We assume the packet has already been checked by icmp_rcv */

	pr_debug("ping_rcv(skb=%p,id=%04x,seq=%04x)\n",
		 skb, ntohs(icmph->un.echo.id), ntohs(icmph->un.echo.sequence));

	/* Push ICMP header back */
	skb_push(skb, skb->data - (u8 *)icmph);

	sk = ping_lookup(net, skb, ntohs(icmph->un.echo.id));
	if (sk) {
		struct sk_buff *skb2 = skb_clone(skb, GFP_ATOMIC);

		pr_debug("rcv on socket %p\n", sk);
		if (skb2)
			ping_queue_rcv_skb(sk, skb2);
		sock_put(sk);
		return true;
	}
	pr_debug("no socket, dropping\n");

	return false;
}
