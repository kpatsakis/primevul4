static int sctp_v4_skb_iif(const struct sk_buff *skb)
{
	return inet_iif(skb);
}
