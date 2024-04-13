ppp_xmit_process(struct ppp *ppp)
{
	struct sk_buff *skb;

	ppp_xmit_lock(ppp);
	if (!ppp->closing) {
		ppp_push(ppp);
		while (!ppp->xmit_pending &&
		       (skb = skb_dequeue(&ppp->file.xq)))
			ppp_send_frame(ppp, skb);
		/* If there's no work left to do, tell the core net
		   code that we can accept some more. */
		if (!ppp->xmit_pending && !skb_peek(&ppp->file.xq))
			netif_wake_queue(ppp->dev);
		else
			netif_stop_queue(ppp->dev);
	}
	ppp_xmit_unlock(ppp);
}
