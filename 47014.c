static int add_recvbuf_small(struct virtnet_info *vi, struct receive_queue *rq,
			     gfp_t gfp)
{
	struct sk_buff *skb;
	struct virtio_net_hdr_mrg_rxbuf *hdr;
	int err;

	skb = __netdev_alloc_skb_ip_align(vi->dev, GOOD_PACKET_LEN, gfp);
	if (unlikely(!skb))
		return -ENOMEM;

	skb_put(skb, GOOD_PACKET_LEN);

	hdr = skb_vnet_hdr(skb);
	sg_init_table(rq->sg, MAX_SKB_FRAGS + 2);
	sg_set_buf(rq->sg, hdr, vi->hdr_len);
	skb_to_sgvec(skb, rq->sg + 1, 0, skb->len);

	err = virtqueue_add_inbuf(rq->vq, rq->sg, 2, skb, gfp);
	if (err < 0)
		dev_kfree_skb(skb);

	return err;
}
