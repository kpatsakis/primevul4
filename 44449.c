static struct sk_buff *ath_tid_dequeue(struct ath_atx_tid *tid)
{
	struct sk_buff *skb;

	skb = __skb_dequeue(&tid->retry_q);
	if (!skb)
		skb = __skb_dequeue(&tid->buf_q);

	return skb;
}
