__netlink_lookup_frame(const struct netlink_ring *ring, unsigned int pos)
{
	unsigned int pg_vec_pos, frame_off;

	pg_vec_pos = pos / ring->frames_per_block;
	frame_off  = pos % ring->frames_per_block;

	return ring->pg_vec[pg_vec_pos] + (frame_off * ring->frame_size);
}
