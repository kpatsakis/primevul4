static void netlink_increment_head(struct netlink_ring *ring)
{
	ring->head = ring->head != ring->frame_max ? ring->head + 1 : 0;
}
