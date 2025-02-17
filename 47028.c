static struct sk_buff *receive_big(struct net_device *dev,
				   struct virtnet_info *vi,
				   struct receive_queue *rq,
				   void *buf,
				   unsigned int len)
{
	struct page *page = buf;
	struct sk_buff *skb = page_to_skb(vi, rq, page, 0, len, PAGE_SIZE);

	if (unlikely(!skb))
		goto err;

	return skb;

err:
	dev->stats.rx_dropped++;
	give_pages(rq, page);
	return NULL;
}
