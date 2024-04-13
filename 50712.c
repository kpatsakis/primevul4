static void tpacket_set_protocol(const struct net_device *dev,
				 struct sk_buff *skb)
{
	if (dev->type == ARPHRD_ETHER) {
		skb_reset_mac_header(skb);
		skb->protocol = eth_hdr(skb)->h_proto;
	}
}
