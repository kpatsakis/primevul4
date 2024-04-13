static void free_unused_bufs(struct virtnet_info *vi)
{
	void *buf;
	int i;

	for (i = 0; i < vi->max_queue_pairs; i++) {
		struct virtqueue *vq = vi->sq[i].vq;
		while ((buf = virtqueue_detach_unused_buf(vq)) != NULL)
			dev_kfree_skb(buf);
	}

	for (i = 0; i < vi->max_queue_pairs; i++) {
		struct virtqueue *vq = vi->rq[i].vq;

		while ((buf = virtqueue_detach_unused_buf(vq)) != NULL) {
			if (vi->mergeable_rx_bufs) {
				unsigned long ctx = (unsigned long)buf;
				void *base = mergeable_ctx_to_buf_address(ctx);
				put_page(virt_to_head_page(base));
			} else if (vi->big_packets) {
				give_pages(&vi->rq[i], buf);
			} else {
				dev_kfree_skb(buf);
			}
		}
	}
}
