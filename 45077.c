void skb_insert(struct sk_buff *old, struct sk_buff *newsk, struct sk_buff_head *list)
{
	unsigned long flags;

	spin_lock_irqsave(&list->lock, flags);
	__skb_insert(newsk, old->prev, old, list);
	spin_unlock_irqrestore(&list->lock, flags);
}
