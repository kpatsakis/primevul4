static int raw_v4_input(struct sk_buff *skb, const struct iphdr *iph, int hash)
{
	int sdif = inet_sdif(skb);
	struct sock *sk;
	struct hlist_head *head;
	int delivered = 0;
	struct net *net;

	read_lock(&raw_v4_hashinfo.lock);
	head = &raw_v4_hashinfo.ht[hash];
	if (hlist_empty(head))
		goto out;

	net = dev_net(skb->dev);
	sk = __raw_v4_lookup(net, __sk_head(head), iph->protocol,
			     iph->saddr, iph->daddr,
			     skb->dev->ifindex, sdif);

	while (sk) {
		delivered = 1;
		if ((iph->protocol != IPPROTO_ICMP || !icmp_filter(sk, skb)) &&
		    ip_mc_sf_allow(sk, iph->daddr, iph->saddr,
				   skb->dev->ifindex, sdif)) {
			struct sk_buff *clone = skb_clone(skb, GFP_ATOMIC);

			/* Not releasing hash table! */
			if (clone)
				raw_rcv(sk, clone);
		}
		sk = __raw_v4_lookup(net, sk_next(sk), iph->protocol,
				     iph->saddr, iph->daddr,
				     skb->dev->ifindex, sdif);
	}
out:
	read_unlock(&raw_v4_hashinfo.lock);
	return delivered;
}
