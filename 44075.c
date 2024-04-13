static u64 __skb_get_pay_offset(u64 ctx, u64 A, u64 X, u64 r4, u64 r5)
{
	struct sk_buff *skb = (struct sk_buff *)(long) ctx;

	return __skb_get_poff(skb);
}
