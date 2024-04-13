static void netlink_forward_ring(struct netlink_ring *ring)
{
	unsigned int head = ring->head, pos = head;
	const struct nl_mmap_hdr *hdr;

	do {
		hdr = __netlink_lookup_frame(ring, pos);
		if (hdr->nm_status == NL_MMAP_STATUS_UNUSED)
			break;
		if (hdr->nm_status != NL_MMAP_STATUS_SKIP)
			break;
		netlink_increment_head(ring);
	} while (ring->head != head);
}
