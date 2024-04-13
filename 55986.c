static int ip_rt_bug(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	pr_debug("%s: %pI4 -> %pI4, %s\n",
		 __func__, &ip_hdr(skb)->saddr, &ip_hdr(skb)->daddr,
		 skb->dev ? skb->dev->name : "?");
	kfree_skb(skb);
	WARN_ON(1);
	return 0;
}
