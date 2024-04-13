static int pppoe_xmit(struct ppp_channel *chan, struct sk_buff *skb)
{
	struct sock *sk = (struct sock *)chan->private;
	return __pppoe_xmit(sk, skb);
}
