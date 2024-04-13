ppp_mp_reconstruct(struct ppp *ppp)
{
	u32 seq = ppp->nextseq;
	u32 minseq = ppp->minseq;
	struct sk_buff_head *list = &ppp->mrq;
	struct sk_buff *p, *tmp;
	struct sk_buff *head, *tail;
	struct sk_buff *skb = NULL;
	int lost = 0, len = 0;

	if (ppp->mrru == 0)	/* do nothing until mrru is set */
		return NULL;
	head = list->next;
	tail = NULL;
	skb_queue_walk_safe(list, p, tmp) {
	again:
		if (seq_before(PPP_MP_CB(p)->sequence, seq)) {
			/* this can't happen, anyway ignore the skb */
			netdev_err(ppp->dev, "ppp_mp_reconstruct bad "
				   "seq %u < %u\n",
				   PPP_MP_CB(p)->sequence, seq);
			__skb_unlink(p, list);
			kfree_skb(p);
			continue;
		}
		if (PPP_MP_CB(p)->sequence != seq) {
			u32 oldseq;
			/* Fragment `seq' is missing.  If it is after
			   minseq, it might arrive later, so stop here. */
			if (seq_after(seq, minseq))
				break;
			/* Fragment `seq' is lost, keep going. */
			lost = 1;
			oldseq = seq;
			seq = seq_before(minseq, PPP_MP_CB(p)->sequence)?
				minseq + 1: PPP_MP_CB(p)->sequence;

			if (ppp->debug & 1)
				netdev_printk(KERN_DEBUG, ppp->dev,
					      "lost frag %u..%u\n",
					      oldseq, seq-1);

			goto again;
		}

		/*
		 * At this point we know that all the fragments from
		 * ppp->nextseq to seq are either present or lost.
		 * Also, there are no complete packets in the queue
		 * that have no missing fragments and end before this
		 * fragment.
		 */

		/* B bit set indicates this fragment starts a packet */
		if (PPP_MP_CB(p)->BEbits & B) {
			head = p;
			lost = 0;
			len = 0;
		}

		len += p->len;

		/* Got a complete packet yet? */
		if (lost == 0 && (PPP_MP_CB(p)->BEbits & E) &&
		    (PPP_MP_CB(head)->BEbits & B)) {
			if (len > ppp->mrru + 2) {
				++ppp->dev->stats.rx_length_errors;
				netdev_printk(KERN_DEBUG, ppp->dev,
					      "PPP: reconstructed packet"
					      " is too long (%d)\n", len);
			} else {
				tail = p;
				break;
			}
			ppp->nextseq = seq + 1;
		}

		/*
		 * If this is the ending fragment of a packet,
		 * and we haven't found a complete valid packet yet,
		 * we can discard up to and including this fragment.
		 */
		if (PPP_MP_CB(p)->BEbits & E) {
			struct sk_buff *tmp2;

			skb_queue_reverse_walk_from_safe(list, p, tmp2) {
				if (ppp->debug & 1)
					netdev_printk(KERN_DEBUG, ppp->dev,
						      "discarding frag %u\n",
						      PPP_MP_CB(p)->sequence);
				__skb_unlink(p, list);
				kfree_skb(p);
			}
			head = skb_peek(list);
			if (!head)
				break;
		}
		++seq;
	}

	/* If we have a complete packet, copy it all into one skb. */
	if (tail != NULL) {
		/* If we have discarded any fragments,
		   signal a receive error. */
		if (PPP_MP_CB(head)->sequence != ppp->nextseq) {
			skb_queue_walk_safe(list, p, tmp) {
				if (p == head)
					break;
				if (ppp->debug & 1)
					netdev_printk(KERN_DEBUG, ppp->dev,
						      "discarding frag %u\n",
						      PPP_MP_CB(p)->sequence);
				__skb_unlink(p, list);
				kfree_skb(p);
			}

			if (ppp->debug & 1)
				netdev_printk(KERN_DEBUG, ppp->dev,
					      "  missed pkts %u..%u\n",
					      ppp->nextseq,
					      PPP_MP_CB(head)->sequence-1);
			++ppp->dev->stats.rx_dropped;
			ppp_receive_error(ppp);
		}

		skb = head;
		if (head != tail) {
			struct sk_buff **fragpp = &skb_shinfo(skb)->frag_list;
			p = skb_queue_next(list, head);
			__skb_unlink(skb, list);
			skb_queue_walk_from_safe(list, p, tmp) {
				__skb_unlink(p, list);
				*fragpp = p;
				p->next = NULL;
				fragpp = &p->next;

				skb->len += p->len;
				skb->data_len += p->len;
				skb->truesize += p->truesize;

				if (p == tail)
					break;
			}
		} else {
			__skb_unlink(skb, list);
		}

		ppp->nextseq = PPP_MP_CB(tail)->sequence + 1;
	}

	return skb;
}
