netlink_current_frame(const struct netlink_ring *ring,
		      enum nl_mmap_status status)
{
	return netlink_lookup_frame(ring, ring->head, status);
}
