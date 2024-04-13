static void free_old_xmit_skbs(struct send_queue *sq)
{
	struct sk_buff *skb;
	unsigned int len;
	struct virtnet_info *vi = sq->vq->vdev->priv;
	struct virtnet_stats *stats = this_cpu_ptr(vi->stats);

	while ((skb = virtqueue_get_buf(sq->vq, &len)) != NULL) {
		pr_debug("Sent skb %p\n", skb);

		u64_stats_update_begin(&stats->tx_syncp);
		stats->tx_bytes += skb->len;
		stats->tx_packets++;
		u64_stats_update_end(&stats->tx_syncp);

		dev_kfree_skb_any(skb);
	}
}
