ppp_push(struct ppp *ppp)
{
	struct list_head *list;
	struct channel *pch;
	struct sk_buff *skb = ppp->xmit_pending;

	if (!skb)
		return;

	list = &ppp->channels;
	if (list_empty(list)) {
		/* nowhere to send the packet, just drop it */
		ppp->xmit_pending = NULL;
		kfree_skb(skb);
		return;
	}

	if ((ppp->flags & SC_MULTILINK) == 0) {
		/* not doing multilink: send it down the first channel */
		list = list->next;
		pch = list_entry(list, struct channel, clist);

		spin_lock_bh(&pch->downl);
		if (pch->chan) {
			if (pch->chan->ops->start_xmit(pch->chan, skb))
				ppp->xmit_pending = NULL;
		} else {
			/* channel got unregistered */
			kfree_skb(skb);
			ppp->xmit_pending = NULL;
		}
		spin_unlock_bh(&pch->downl);
		return;
	}

#ifdef CONFIG_PPP_MULTILINK
	/* Multilink: fragment the packet over as many links
	   as can take the packet at the moment. */
	if (!ppp_mp_explode(ppp, skb))
		return;
#endif /* CONFIG_PPP_MULTILINK */

	ppp->xmit_pending = NULL;
	kfree_skb(skb);
}
