netlink_lookup_frame(const struct netlink_ring *ring, unsigned int pos,
		     enum nl_mmap_status status)
{
	struct nl_mmap_hdr *hdr;

	hdr = __netlink_lookup_frame(ring, pos);
	if (netlink_get_status(hdr) != status)
		return NULL;

	return hdr;
}
