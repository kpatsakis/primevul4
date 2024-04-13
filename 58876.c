struct sk_buff *skb_segment(struct sk_buff *head_skb,
			    netdev_features_t features)
{
	struct sk_buff *segs = NULL;
	struct sk_buff *tail = NULL;
	struct sk_buff *list_skb = skb_shinfo(head_skb)->frag_list;
	skb_frag_t *frag = skb_shinfo(head_skb)->frags;
	unsigned int mss = skb_shinfo(head_skb)->gso_size;
	unsigned int doffset = head_skb->data - skb_mac_header(head_skb);
	struct sk_buff *frag_skb = head_skb;
	unsigned int offset = doffset;
	unsigned int tnl_hlen = skb_tnl_header_len(head_skb);
	unsigned int partial_segs = 0;
	unsigned int headroom;
	unsigned int len = head_skb->len;
	__be16 proto;
	bool csum, sg;
	int nfrags = skb_shinfo(head_skb)->nr_frags;
	int err = -ENOMEM;
	int i = 0;
	int pos;
	int dummy;

	__skb_push(head_skb, doffset);
	proto = skb_network_protocol(head_skb, &dummy);
	if (unlikely(!proto))
		return ERR_PTR(-EINVAL);

	sg = !!(features & NETIF_F_SG);
	csum = !!can_checksum_protocol(features, proto);

	if (sg && csum && (mss != GSO_BY_FRAGS))  {
		if (!(features & NETIF_F_GSO_PARTIAL)) {
			struct sk_buff *iter;

			if (!list_skb ||
			    !net_gso_ok(features, skb_shinfo(head_skb)->gso_type))
				goto normal;

			/* Split the buffer at the frag_list pointer.
			 * This is based on the assumption that all
			 * buffers in the chain excluding the last
			 * containing the same amount of data.
			 */
			skb_walk_frags(head_skb, iter) {
				if (skb_headlen(iter))
					goto normal;

				len -= iter->len;
			}
		}

		/* GSO partial only requires that we trim off any excess that
		 * doesn't fit into an MSS sized block, so take care of that
		 * now.
		 */
		partial_segs = len / mss;
		if (partial_segs > 1)
			mss *= partial_segs;
		else
			partial_segs = 0;
	}

normal:
	headroom = skb_headroom(head_skb);
	pos = skb_headlen(head_skb);

	do {
		struct sk_buff *nskb;
		skb_frag_t *nskb_frag;
		int hsize;
		int size;

		if (unlikely(mss == GSO_BY_FRAGS)) {
			len = list_skb->len;
		} else {
			len = head_skb->len - offset;
			if (len > mss)
				len = mss;
		}

		hsize = skb_headlen(head_skb) - offset;
		if (hsize < 0)
			hsize = 0;
		if (hsize > len || !sg)
			hsize = len;

		if (!hsize && i >= nfrags && skb_headlen(list_skb) &&
		    (skb_headlen(list_skb) == len || sg)) {
			BUG_ON(skb_headlen(list_skb) > len);

			i = 0;
			nfrags = skb_shinfo(list_skb)->nr_frags;
			frag = skb_shinfo(list_skb)->frags;
			frag_skb = list_skb;
			pos += skb_headlen(list_skb);

			while (pos < offset + len) {
				BUG_ON(i >= nfrags);

				size = skb_frag_size(frag);
				if (pos + size > offset + len)
					break;

				i++;
				pos += size;
				frag++;
			}

			nskb = skb_clone(list_skb, GFP_ATOMIC);
			list_skb = list_skb->next;

			if (unlikely(!nskb))
				goto err;

			if (unlikely(pskb_trim(nskb, len))) {
				kfree_skb(nskb);
				goto err;
			}

			hsize = skb_end_offset(nskb);
			if (skb_cow_head(nskb, doffset + headroom)) {
				kfree_skb(nskb);
				goto err;
			}

			nskb->truesize += skb_end_offset(nskb) - hsize;
			skb_release_head_state(nskb);
			__skb_push(nskb, doffset);
		} else {
			nskb = __alloc_skb(hsize + doffset + headroom,
					   GFP_ATOMIC, skb_alloc_rx_flag(head_skb),
					   NUMA_NO_NODE);

			if (unlikely(!nskb))
				goto err;

			skb_reserve(nskb, headroom);
			__skb_put(nskb, doffset);
		}

		if (segs)
			tail->next = nskb;
		else
			segs = nskb;
		tail = nskb;

		__copy_skb_header(nskb, head_skb);

		skb_headers_offset_update(nskb, skb_headroom(nskb) - headroom);
		skb_reset_mac_len(nskb);

		skb_copy_from_linear_data_offset(head_skb, -tnl_hlen,
						 nskb->data - tnl_hlen,
						 doffset + tnl_hlen);

		if (nskb->len == len + doffset)
			goto perform_csum_check;

		if (!sg) {
			if (!nskb->remcsum_offload)
				nskb->ip_summed = CHECKSUM_NONE;
			SKB_GSO_CB(nskb)->csum =
				skb_copy_and_csum_bits(head_skb, offset,
						       skb_put(nskb, len),
						       len, 0);
			SKB_GSO_CB(nskb)->csum_start =
				skb_headroom(nskb) + doffset;
			continue;
		}

		nskb_frag = skb_shinfo(nskb)->frags;

		skb_copy_from_linear_data_offset(head_skb, offset,
						 skb_put(nskb, hsize), hsize);

		skb_shinfo(nskb)->tx_flags = skb_shinfo(head_skb)->tx_flags &
			SKBTX_SHARED_FRAG;

		while (pos < offset + len) {
			if (i >= nfrags) {
				BUG_ON(skb_headlen(list_skb));

				i = 0;
				nfrags = skb_shinfo(list_skb)->nr_frags;
				frag = skb_shinfo(list_skb)->frags;
				frag_skb = list_skb;

				BUG_ON(!nfrags);

				list_skb = list_skb->next;
			}

			if (unlikely(skb_shinfo(nskb)->nr_frags >=
				     MAX_SKB_FRAGS)) {
				net_warn_ratelimited(
					"skb_segment: too many frags: %u %u\n",
					pos, mss);
				goto err;
			}

			if (unlikely(skb_orphan_frags(frag_skb, GFP_ATOMIC)))
				goto err;

			*nskb_frag = *frag;
			__skb_frag_ref(nskb_frag);
			size = skb_frag_size(nskb_frag);

			if (pos < offset) {
				nskb_frag->page_offset += offset - pos;
				skb_frag_size_sub(nskb_frag, offset - pos);
			}

			skb_shinfo(nskb)->nr_frags++;

			if (pos + size <= offset + len) {
				i++;
				frag++;
				pos += size;
			} else {
				skb_frag_size_sub(nskb_frag, pos + size - (offset + len));
				goto skip_fraglist;
			}

			nskb_frag++;
		}

skip_fraglist:
		nskb->data_len = len - hsize;
		nskb->len += nskb->data_len;
		nskb->truesize += nskb->data_len;

perform_csum_check:
		if (!csum) {
			if (skb_has_shared_frag(nskb)) {
				err = __skb_linearize(nskb);
				if (err)
					goto err;
			}
			if (!nskb->remcsum_offload)
				nskb->ip_summed = CHECKSUM_NONE;
			SKB_GSO_CB(nskb)->csum =
				skb_checksum(nskb, doffset,
					     nskb->len - doffset, 0);
			SKB_GSO_CB(nskb)->csum_start =
				skb_headroom(nskb) + doffset;
		}
	} while ((offset += len) < head_skb->len);

	/* Some callers want to get the end of the list.
	 * Put it in segs->prev to avoid walking the list.
	 * (see validate_xmit_skb_list() for example)
	 */
	segs->prev = tail;

	if (partial_segs) {
		struct sk_buff *iter;
		int type = skb_shinfo(head_skb)->gso_type;
		unsigned short gso_size = skb_shinfo(head_skb)->gso_size;

		/* Update type to add partial and then remove dodgy if set */
		type |= (features & NETIF_F_GSO_PARTIAL) / NETIF_F_GSO_PARTIAL * SKB_GSO_PARTIAL;
		type &= ~SKB_GSO_DODGY;

		/* Update GSO info and prepare to start updating headers on
		 * our way back down the stack of protocols.
		 */
		for (iter = segs; iter; iter = iter->next) {
			skb_shinfo(iter)->gso_size = gso_size;
			skb_shinfo(iter)->gso_segs = partial_segs;
			skb_shinfo(iter)->gso_type = type;
			SKB_GSO_CB(iter)->data_offset = skb_headroom(iter) + doffset;
		}

		if (tail->len - doffset <= gso_size)
			skb_shinfo(tail)->gso_size = 0;
		else if (tail != segs)
			skb_shinfo(tail)->gso_segs = DIV_ROUND_UP(tail->len - doffset, gso_size);
	}

	/* Following permits correct backpressure, for protocols
	 * using skb_set_owner_w().
	 * Idea is to tranfert ownership from head_skb to last segment.
	 */
	if (head_skb->destructor == sock_wfree) {
		swap(tail->truesize, head_skb->truesize);
		swap(tail->destructor, head_skb->destructor);
		swap(tail->sk, head_skb->sk);
	}
	return segs;

err:
	kfree_skb_list(segs);
	return ERR_PTR(err);
}
