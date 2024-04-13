static int ppp_mp_explode(struct ppp *ppp, struct sk_buff *skb)
{
	int len, totlen;
	int i, bits, hdrlen, mtu;
	int flen;
	int navail, nfree, nzero;
	int nbigger;
	int totspeed;
	int totfree;
	unsigned char *p, *q;
	struct list_head *list;
	struct channel *pch;
	struct sk_buff *frag;
	struct ppp_channel *chan;

	totspeed = 0; /*total bitrate of the bundle*/
	nfree = 0; /* # channels which have no packet already queued */
	navail = 0; /* total # of usable channels (not deregistered) */
	nzero = 0; /* number of channels with zero speed associated*/
	totfree = 0; /*total # of channels available and
				  *having no queued packets before
				  *starting the fragmentation*/

	hdrlen = (ppp->flags & SC_MP_XSHORTSEQ)? MPHDRLEN_SSN: MPHDRLEN;
	i = 0;
	list_for_each_entry(pch, &ppp->channels, clist) {
		if (pch->chan) {
			pch->avail = 1;
			navail++;
			pch->speed = pch->chan->speed;
		} else {
			pch->avail = 0;
		}
		if (pch->avail) {
			if (skb_queue_empty(&pch->file.xq) ||
				!pch->had_frag) {
					if (pch->speed == 0)
						nzero++;
					else
						totspeed += pch->speed;

					pch->avail = 2;
					++nfree;
					++totfree;
				}
			if (!pch->had_frag && i < ppp->nxchan)
				ppp->nxchan = i;
		}
		++i;
	}
	/*
	 * Don't start sending this packet unless at least half of
	 * the channels are free.  This gives much better TCP
	 * performance if we have a lot of channels.
	 */
	if (nfree == 0 || nfree < navail / 2)
		return 0; /* can't take now, leave it in xmit_pending */

	/* Do protocol field compression */
	p = skb->data;
	len = skb->len;
	if (*p == 0 && mp_protocol_compress) {
		++p;
		--len;
	}

	totlen = len;
	nbigger = len % nfree;

	/* skip to the channel after the one we last used
	   and start at that one */
	list = &ppp->channels;
	for (i = 0; i < ppp->nxchan; ++i) {
		list = list->next;
		if (list == &ppp->channels) {
			i = 0;
			break;
		}
	}

	/* create a fragment for each channel */
	bits = B;
	while (len > 0) {
		list = list->next;
		if (list == &ppp->channels) {
			i = 0;
			continue;
		}
		pch = list_entry(list, struct channel, clist);
		++i;
		if (!pch->avail)
			continue;

		/*
		 * Skip this channel if it has a fragment pending already and
		 * we haven't given a fragment to all of the free channels.
		 */
		if (pch->avail == 1) {
			if (nfree > 0)
				continue;
		} else {
			pch->avail = 1;
		}

		/* check the channel's mtu and whether it is still attached. */
		spin_lock_bh(&pch->downl);
		if (pch->chan == NULL) {
			/* can't use this channel, it's being deregistered */
			if (pch->speed == 0)
				nzero--;
			else
				totspeed -= pch->speed;

			spin_unlock_bh(&pch->downl);
			pch->avail = 0;
			totlen = len;
			totfree--;
			nfree--;
			if (--navail == 0)
				break;
			continue;
		}

		/*
		*if the channel speed is not set divide
		*the packet evenly among the free channels;
		*otherwise divide it according to the speed
		*of the channel we are going to transmit on
		*/
		flen = len;
		if (nfree > 0) {
			if (pch->speed == 0) {
				flen = len/nfree;
				if (nbigger > 0) {
					flen++;
					nbigger--;
				}
			} else {
				flen = (((totfree - nzero)*(totlen + hdrlen*totfree)) /
					((totspeed*totfree)/pch->speed)) - hdrlen;
				if (nbigger > 0) {
					flen += ((totfree - nzero)*pch->speed)/totspeed;
					nbigger -= ((totfree - nzero)*pch->speed)/
							totspeed;
				}
			}
			nfree--;
		}

		/*
		 *check if we are on the last channel or
		 *we exceded the length of the data to
		 *fragment
		 */
		if ((nfree <= 0) || (flen > len))
			flen = len;
		/*
		 *it is not worth to tx on slow channels:
		 *in that case from the resulting flen according to the
		 *above formula will be equal or less than zero.
		 *Skip the channel in this case
		 */
		if (flen <= 0) {
			pch->avail = 2;
			spin_unlock_bh(&pch->downl);
			continue;
		}

		/*
		 * hdrlen includes the 2-byte PPP protocol field, but the
		 * MTU counts only the payload excluding the protocol field.
		 * (RFC1661 Section 2)
		 */
		mtu = pch->chan->mtu - (hdrlen - 2);
		if (mtu < 4)
			mtu = 4;
		if (flen > mtu)
			flen = mtu;
		if (flen == len)
			bits |= E;
		frag = alloc_skb(flen + hdrlen + (flen == 0), GFP_ATOMIC);
		if (!frag)
			goto noskb;
		q = skb_put(frag, flen + hdrlen);

		/* make the MP header */
		put_unaligned_be16(PPP_MP, q);
		if (ppp->flags & SC_MP_XSHORTSEQ) {
			q[2] = bits + ((ppp->nxseq >> 8) & 0xf);
			q[3] = ppp->nxseq;
		} else {
			q[2] = bits;
			q[3] = ppp->nxseq >> 16;
			q[4] = ppp->nxseq >> 8;
			q[5] = ppp->nxseq;
		}

		memcpy(q + hdrlen, p, flen);

		/* try to send it down the channel */
		chan = pch->chan;
		if (!skb_queue_empty(&pch->file.xq) ||
			!chan->ops->start_xmit(chan, frag))
			skb_queue_tail(&pch->file.xq, frag);
		pch->had_frag = 1;
		p += flen;
		len -= flen;
		++ppp->nxseq;
		bits = 0;
		spin_unlock_bh(&pch->downl);
	}
	ppp->nxchan = i;

	return 1;

 noskb:
	spin_unlock_bh(&pch->downl);
	if (ppp->debug & 1)
		netdev_err(ppp->dev, "PPP: no memory (fragment)\n");
	++ppp->dev->stats.tx_errors;
	++ppp->nxseq;
	return 1;	/* abandon the frame */
}
