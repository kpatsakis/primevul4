static struct sk_buff *receive_mergeable(struct net_device *dev,
					 struct virtnet_info *vi,
					 struct receive_queue *rq,
					 unsigned long ctx,
					 unsigned int len)
{
	void *buf = mergeable_ctx_to_buf_address(ctx);
	struct virtio_net_hdr_mrg_rxbuf *hdr = buf;
	u16 num_buf = virtio16_to_cpu(vi->vdev, hdr->num_buffers);
	struct page *page = virt_to_head_page(buf);
	int offset = buf - page_address(page);
	unsigned int truesize = max(len, mergeable_ctx_to_buf_truesize(ctx));

	struct sk_buff *head_skb = page_to_skb(vi, rq, page, offset, len,
					       truesize);
	struct sk_buff *curr_skb = head_skb;

	if (unlikely(!curr_skb))
		goto err_skb;
	while (--num_buf) {
		int num_skb_frags;

		ctx = (unsigned long)virtqueue_get_buf(rq->vq, &len);
		if (unlikely(!ctx)) {
			pr_debug("%s: rx error: %d buffers out of %d missing\n",
				 dev->name, num_buf,
				 virtio16_to_cpu(vi->vdev,
						 hdr->num_buffers));
			dev->stats.rx_length_errors++;
			goto err_buf;
		}

		buf = mergeable_ctx_to_buf_address(ctx);
		page = virt_to_head_page(buf);

		num_skb_frags = skb_shinfo(curr_skb)->nr_frags;
		if (unlikely(num_skb_frags == MAX_SKB_FRAGS)) {
			struct sk_buff *nskb = alloc_skb(0, GFP_ATOMIC);

			if (unlikely(!nskb))
				goto err_skb;
			if (curr_skb == head_skb)
				skb_shinfo(curr_skb)->frag_list = nskb;
			else
				curr_skb->next = nskb;
			curr_skb = nskb;
			head_skb->truesize += nskb->truesize;
			num_skb_frags = 0;
		}
		truesize = max(len, mergeable_ctx_to_buf_truesize(ctx));
		if (curr_skb != head_skb) {
			head_skb->data_len += len;
			head_skb->len += len;
			head_skb->truesize += truesize;
		}
		offset = buf - page_address(page);
		if (skb_can_coalesce(curr_skb, num_skb_frags, page, offset)) {
			put_page(page);
			skb_coalesce_rx_frag(curr_skb, num_skb_frags - 1,
					     len, truesize);
		} else {
			skb_add_rx_frag(curr_skb, num_skb_frags, page,
					offset, len, truesize);
		}
	}

	ewma_add(&rq->mrg_avg_pkt_len, head_skb->len);
	return head_skb;

err_skb:
	put_page(page);
	while (--num_buf) {
		ctx = (unsigned long)virtqueue_get_buf(rq->vq, &len);
		if (unlikely(!ctx)) {
			pr_debug("%s: rx error: %d buffers missing\n",
				 dev->name, num_buf);
			dev->stats.rx_length_errors++;
			break;
		}
		page = virt_to_head_page(mergeable_ctx_to_buf_address(ctx));
		put_page(page);
	}
err_buf:
	dev->stats.rx_dropped++;
	dev_kfree_skb(head_skb);
	return NULL;
}
