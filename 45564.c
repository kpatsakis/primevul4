static struct sk_buff *netlink_alloc_large_skb(unsigned int size,
					       int broadcast)
{
	struct sk_buff *skb;
	void *data;

	if (size <= NLMSG_GOODSIZE || broadcast)
		return alloc_skb(size, GFP_KERNEL);

	size = SKB_DATA_ALIGN(size) +
	       SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	data = vmalloc(size);
	if (data == NULL)
		return NULL;

	skb = build_skb(data, size);
	if (skb == NULL)
		vfree(data);
	else {
		skb->head_frag = 0;
		skb->destructor = netlink_skb_destructor;
	}

	return skb;
}
