static struct nl_mmap_hdr *netlink_mmap_hdr(struct sk_buff *skb)
{
	return (struct nl_mmap_hdr *)(skb->head - NL_MMAP_HDRLEN);
}
