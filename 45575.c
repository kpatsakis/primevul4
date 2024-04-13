static bool netlink_dump_space(struct netlink_sock *nlk)
{
	struct netlink_ring *ring = &nlk->rx_ring;
	struct nl_mmap_hdr *hdr;
	unsigned int n;

	hdr = netlink_current_frame(ring, NL_MMAP_STATUS_UNUSED);
	if (hdr == NULL)
		return false;

	n = ring->head + ring->frame_max / 2;
	if (n > ring->frame_max)
		n -= ring->frame_max;

	hdr = __netlink_lookup_frame(ring, n);

	return hdr->nm_status == NL_MMAP_STATUS_UNUSED;
}
