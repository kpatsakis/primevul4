static ssize_t mergeable_rx_buffer_size_show(struct netdev_rx_queue *queue,
		struct rx_queue_attribute *attribute, char *buf)
{
	struct virtnet_info *vi = netdev_priv(queue->dev);
	unsigned int queue_index = get_netdev_rx_queue_index(queue);
	struct ewma *avg;

	BUG_ON(queue_index >= vi->max_queue_pairs);
	avg = &vi->rq[queue_index].mrg_avg_pkt_len;
	return sprintf(buf, "%u\n", get_mergeable_buf_len(avg));
}
