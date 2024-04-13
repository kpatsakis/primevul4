ppp_receive_mp_frame(struct ppp *ppp, struct sk_buff *skb, struct channel *pch)
{
	u32 mask, seq;
	struct channel *ch;
	int mphdrlen = (ppp->flags & SC_MP_SHORTSEQ)? MPHDRLEN_SSN: MPHDRLEN;

	if (!pskb_may_pull(skb, mphdrlen + 1) || ppp->mrru == 0)
		goto err;		/* no good, throw it away */

	/* Decode sequence number and begin/end bits */
	if (ppp->flags & SC_MP_SHORTSEQ) {
		seq = ((skb->data[2] & 0x0f) << 8) | skb->data[3];
		mask = 0xfff;
	} else {
		seq = (skb->data[3] << 16) | (skb->data[4] << 8)| skb->data[5];
		mask = 0xffffff;
	}
	PPP_MP_CB(skb)->BEbits = skb->data[2];
	skb_pull(skb, mphdrlen);	/* pull off PPP and MP headers */

	/*
	 * Do protocol ID decompression on the first fragment of each packet.
	 */
	if ((PPP_MP_CB(skb)->BEbits & B) && (skb->data[0] & 1))
		*skb_push(skb, 1) = 0;

	/*
	 * Expand sequence number to 32 bits, making it as close
	 * as possible to ppp->minseq.
	 */
	seq |= ppp->minseq & ~mask;
	if ((int)(ppp->minseq - seq) > (int)(mask >> 1))
		seq += mask + 1;
	else if ((int)(seq - ppp->minseq) > (int)(mask >> 1))
		seq -= mask + 1;	/* should never happen */
	PPP_MP_CB(skb)->sequence = seq;
	pch->lastseq = seq;

	/*
	 * If this packet comes before the next one we were expecting,
	 * drop it.
	 */
	if (seq_before(seq, ppp->nextseq)) {
		kfree_skb(skb);
		++ppp->dev->stats.rx_dropped;
		ppp_receive_error(ppp);
		return;
	}

	/*
	 * Reevaluate minseq, the minimum over all channels of the
	 * last sequence number received on each channel.  Because of
	 * the increasing sequence number rule, we know that any fragment
	 * before `minseq' which hasn't arrived is never going to arrive.
	 * The list of channels can't change because we have the receive
	 * side of the ppp unit locked.
	 */
	list_for_each_entry(ch, &ppp->channels, clist) {
		if (seq_before(ch->lastseq, seq))
			seq = ch->lastseq;
	}
	if (seq_before(ppp->minseq, seq))
		ppp->minseq = seq;

	/* Put the fragment on the reconstruction queue */
	ppp_mp_insert(ppp, skb);

	/* If the queue is getting long, don't wait any longer for packets
	   before the start of the queue. */
	if (skb_queue_len(&ppp->mrq) >= PPP_MP_MAX_QLEN) {
		struct sk_buff *mskb = skb_peek(&ppp->mrq);
		if (seq_before(ppp->minseq, PPP_MP_CB(mskb)->sequence))
			ppp->minseq = PPP_MP_CB(mskb)->sequence;
	}

	/* Pull completed packets off the queue and receive them. */
	while ((skb = ppp_mp_reconstruct(ppp))) {
		if (pskb_may_pull(skb, 2))
			ppp_receive_nonmp_frame(ppp, skb);
		else {
			++ppp->dev->stats.rx_length_errors;
			kfree_skb(skb);
			ppp_receive_error(ppp);
		}
	}

	return;

 err:
	kfree_skb(skb);
	ppp_receive_error(ppp);
}
