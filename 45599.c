static void netlink_ring_setup_skb(struct sk_buff *skb, struct sock *sk,
				   struct netlink_ring *ring,
				   struct nl_mmap_hdr *hdr)
{
	unsigned int size;
	void *data;

	size = ring->frame_size - NL_MMAP_HDRLEN;
	data = (void *)hdr + NL_MMAP_HDRLEN;

	skb->head	= data;
	skb->data	= data;
	skb_reset_tail_pointer(skb);
	skb->end	= skb->tail + size;
	skb->len	= 0;

	skb->destructor	= netlink_skb_destructor;
	NETLINK_CB(skb).flags |= NETLINK_SKB_MMAPED;
	NETLINK_CB(skb).sk = sk;
}
