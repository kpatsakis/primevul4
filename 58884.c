struct sk_buff *sock_dequeue_err_skb(struct sock *sk)
{
	struct sk_buff_head *q = &sk->sk_error_queue;
	struct sk_buff *skb, *skb_next = NULL;
	bool icmp_next = false;
	unsigned long flags;

	spin_lock_irqsave(&q->lock, flags);
	skb = __skb_dequeue(q);
	if (skb && (skb_next = skb_peek(q)))
		icmp_next = is_icmp_err_skb(skb_next);
	spin_unlock_irqrestore(&q->lock, flags);

	if (is_icmp_err_skb(skb) && !icmp_next)
		sk->sk_err = 0;

	if (skb_next)
		sk->sk_error_report(sk);

	return skb;
}
