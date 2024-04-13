ppp_receive_nonmp_frame(struct ppp *ppp, struct sk_buff *skb)
{
	struct sk_buff *ns;
	int proto, len, npi;

	/*
	 * Decompress the frame, if compressed.
	 * Note that some decompressors need to see uncompressed frames
	 * that come in as well as compressed frames.
	 */
	if (ppp->rc_state && (ppp->rstate & SC_DECOMP_RUN) &&
	    (ppp->rstate & (SC_DC_FERROR | SC_DC_ERROR)) == 0)
		skb = ppp_decompress_frame(ppp, skb);

	if (ppp->flags & SC_MUST_COMP && ppp->rstate & SC_DC_FERROR)
		goto err;

	proto = PPP_PROTO(skb);
	switch (proto) {
	case PPP_VJC_COMP:
		/* decompress VJ compressed packets */
		if (!ppp->vj || (ppp->flags & SC_REJ_COMP_TCP))
			goto err;

		if (skb_tailroom(skb) < 124 || skb_cloned(skb)) {
			/* copy to a new sk_buff with more tailroom */
			ns = dev_alloc_skb(skb->len + 128);
			if (!ns) {
				netdev_err(ppp->dev, "PPP: no memory "
					   "(VJ decomp)\n");
				goto err;
			}
			skb_reserve(ns, 2);
			skb_copy_bits(skb, 0, skb_put(ns, skb->len), skb->len);
			consume_skb(skb);
			skb = ns;
		}
		else
			skb->ip_summed = CHECKSUM_NONE;

		len = slhc_uncompress(ppp->vj, skb->data + 2, skb->len - 2);
		if (len <= 0) {
			netdev_printk(KERN_DEBUG, ppp->dev,
				      "PPP: VJ decompression error\n");
			goto err;
		}
		len += 2;
		if (len > skb->len)
			skb_put(skb, len - skb->len);
		else if (len < skb->len)
			skb_trim(skb, len);
		proto = PPP_IP;
		break;

	case PPP_VJC_UNCOMP:
		if (!ppp->vj || (ppp->flags & SC_REJ_COMP_TCP))
			goto err;

		/* Until we fix the decompressor need to make sure
		 * data portion is linear.
		 */
		if (!pskb_may_pull(skb, skb->len))
			goto err;

		if (slhc_remember(ppp->vj, skb->data + 2, skb->len - 2) <= 0) {
			netdev_err(ppp->dev, "PPP: VJ uncompressed error\n");
			goto err;
		}
		proto = PPP_IP;
		break;

	case PPP_CCP:
		ppp_ccp_peek(ppp, skb, 1);
		break;
	}

	++ppp->stats64.rx_packets;
	ppp->stats64.rx_bytes += skb->len - 2;

	npi = proto_to_npindex(proto);
	if (npi < 0) {
		/* control or unknown frame - pass it to pppd */
		skb_queue_tail(&ppp->file.rq, skb);
		/* limit queue length by dropping old frames */
		while (ppp->file.rq.qlen > PPP_MAX_RQLEN &&
		       (skb = skb_dequeue(&ppp->file.rq)))
			kfree_skb(skb);
		/* wake up any process polling or blocking on read */
		wake_up_interruptible(&ppp->file.rwait);

	} else {
		/* network protocol frame - give it to the kernel */

#ifdef CONFIG_PPP_FILTER
		/* check if the packet passes the pass and active filters */
		/* the filter instructions are constructed assuming
		   a four-byte PPP header on each packet */
		if (ppp->pass_filter || ppp->active_filter) {
			if (skb_unclone(skb, GFP_ATOMIC))
				goto err;

			*skb_push(skb, 2) = 0;
			if (ppp->pass_filter &&
			    BPF_PROG_RUN(ppp->pass_filter, skb) == 0) {
				if (ppp->debug & 1)
					netdev_printk(KERN_DEBUG, ppp->dev,
						      "PPP: inbound frame "
						      "not passed\n");
				kfree_skb(skb);
				return;
			}
			if (!(ppp->active_filter &&
			      BPF_PROG_RUN(ppp->active_filter, skb) == 0))
				ppp->last_recv = jiffies;
			__skb_pull(skb, 2);
		} else
#endif /* CONFIG_PPP_FILTER */
			ppp->last_recv = jiffies;

		if ((ppp->dev->flags & IFF_UP) == 0 ||
		    ppp->npmode[npi] != NPMODE_PASS) {
			kfree_skb(skb);
		} else {
			/* chop off protocol */
			skb_pull_rcsum(skb, 2);
			skb->dev = ppp->dev;
			skb->protocol = htons(npindex_to_ethertype[npi]);
			skb_reset_mac_header(skb);
			skb_scrub_packet(skb, !net_eq(ppp->ppp_net,
						      dev_net(ppp->dev)));
			netif_rx(skb);
		}
	}
	return;

 err:
	kfree_skb(skb);
	ppp_receive_error(ppp);
}
