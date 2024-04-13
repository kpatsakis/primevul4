netlink_previous_frame(const struct netlink_ring *ring,
		       enum nl_mmap_status status)
{
	unsigned int prev;

	prev = ring->head ? ring->head - 1 : ring->frame_max;
	return netlink_lookup_frame(ring, prev, status);
}
