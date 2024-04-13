static int sctp_v6_skb_iif(const struct sk_buff *skb)
{
	return IP6CB(skb)->iif;
}
