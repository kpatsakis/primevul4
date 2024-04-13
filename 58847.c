static bool is_icmp_err_skb(const struct sk_buff *skb)
{
	return skb && (SKB_EXT_ERR(skb)->ee.ee_origin == SO_EE_ORIGIN_ICMP ||
		       SKB_EXT_ERR(skb)->ee.ee_origin == SO_EE_ORIGIN_ICMP6);
}
