nf_ct_frag6_reasm(struct frag_queue *fq, struct sk_buff *prev,  struct net_device *dev)
{
	struct sk_buff *fp, *head = fq->q.fragments;
	int    payload_len;
	u8 ecn;

	inet_frag_kill(&fq->q, &nf_frags);

	WARN_ON(head == NULL);
	WARN_ON(NFCT_FRAG6_CB(head)->offset != 0);

	ecn = ip_frag_ecn_table[fq->ecn];
	if (unlikely(ecn == 0xff))
		return false;

	/* Unfragmented part is taken from the first segment. */
	payload_len = ((head->data - skb_network_header(head)) -
		       sizeof(struct ipv6hdr) + fq->q.len -
		       sizeof(struct frag_hdr));
	if (payload_len > IPV6_MAXPLEN) {
		net_dbg_ratelimited("nf_ct_frag6_reasm: payload len = %d\n",
				    payload_len);
		return false;
	}

	/* Head of list must not be cloned. */
	if (skb_unclone(head, GFP_ATOMIC))
		return false;

	/* If the first fragment is fragmented itself, we split
	 * it to two chunks: the first with data and paged part
	 * and the second, holding only fragments. */
	if (skb_has_frag_list(head)) {
		struct sk_buff *clone;
		int i, plen = 0;

		clone = alloc_skb(0, GFP_ATOMIC);
		if (clone == NULL)
			return false;

		clone->next = head->next;
		head->next = clone;
		skb_shinfo(clone)->frag_list = skb_shinfo(head)->frag_list;
		skb_frag_list_init(head);
		for (i = 0; i < skb_shinfo(head)->nr_frags; i++)
			plen += skb_frag_size(&skb_shinfo(head)->frags[i]);
		clone->len = clone->data_len = head->data_len - plen;
		head->data_len -= clone->len;
		head->len -= clone->len;
		clone->csum = 0;
		clone->ip_summed = head->ip_summed;

		add_frag_mem_limit(fq->q.net, clone->truesize);
	}

	/* morph head into last received skb: prev.
	 *
	 * This allows callers of ipv6 conntrack defrag to continue
	 * to use the last skb(frag) passed into the reasm engine.
	 * The last skb frag 'silently' turns into the full reassembled skb.
	 *
	 * Since prev is also part of q->fragments we have to clone it first.
	 */
	if (head != prev) {
		struct sk_buff *iter;

		fp = skb_clone(prev, GFP_ATOMIC);
		if (!fp)
			return false;

		fp->next = prev->next;

		iter = head;
		while (iter) {
			if (iter->next == prev) {
				iter->next = fp;
				break;
			}
			iter = iter->next;
		}

		skb_morph(prev, head);
		prev->next = head->next;
		consume_skb(head);
		head = prev;
	}

	/* We have to remove fragment header from datagram and to relocate
	 * header in order to calculate ICV correctly. */
	skb_network_header(head)[fq->nhoffset] = skb_transport_header(head)[0];
	memmove(head->head + sizeof(struct frag_hdr), head->head,
		(head->data - head->head) - sizeof(struct frag_hdr));
	head->mac_header += sizeof(struct frag_hdr);
	head->network_header += sizeof(struct frag_hdr);

	skb_shinfo(head)->frag_list = head->next;
	skb_reset_transport_header(head);
	skb_push(head, head->data - skb_network_header(head));

	for (fp = head->next; fp; fp = fp->next) {
		head->data_len += fp->len;
		head->len += fp->len;
		if (head->ip_summed != fp->ip_summed)
			head->ip_summed = CHECKSUM_NONE;
		else if (head->ip_summed == CHECKSUM_COMPLETE)
			head->csum = csum_add(head->csum, fp->csum);
		head->truesize += fp->truesize;
	}
	sub_frag_mem_limit(fq->q.net, head->truesize);

	head->ignore_df = 1;
	head->next = NULL;
	head->dev = dev;
	head->tstamp = fq->q.stamp;
	ipv6_hdr(head)->payload_len = htons(payload_len);
	ipv6_change_dsfield(ipv6_hdr(head), 0xff, ecn);
	IP6CB(head)->frag_max_size = sizeof(struct ipv6hdr) + fq->q.max_size;

	/* Yes, and fold redundant checksum back. 8) */
	if (head->ip_summed == CHECKSUM_COMPLETE)
		head->csum = csum_partial(skb_network_header(head),
					  skb_network_header_len(head),
					  head->csum);

	fq->q.fragments = NULL;
	fq->q.fragments_tail = NULL;

	return true;
}
