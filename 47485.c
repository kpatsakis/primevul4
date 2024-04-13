static void pndisc_redo(struct sk_buff *skb)
{
	ndisc_recv_ns(skb);
	kfree_skb(skb);
}
