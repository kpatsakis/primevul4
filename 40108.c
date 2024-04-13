void vcc_process_recv_queue(struct atm_vcc *vcc)
{
	struct sk_buff_head queue, *rq;
	struct sk_buff *skb, *tmp;
	unsigned long flags;

	__skb_queue_head_init(&queue);
	rq = &sk_atm(vcc)->sk_receive_queue;

	spin_lock_irqsave(&rq->lock, flags);
	skb_queue_splice_init(rq, &queue);
	spin_unlock_irqrestore(&rq->lock, flags);

	skb_queue_walk_safe(&queue, skb, tmp) {
		__skb_unlink(skb, &queue);
		vcc->push(vcc, skb);
	}
}
