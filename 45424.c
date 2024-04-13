static __inline__ int is_ip_over_ddp(struct sk_buff *skb)
{
	return skb->data[12] == 22;
}
