static int batadv_frag_size_limit(void)
{
	int limit = BATADV_FRAG_MAX_FRAG_SIZE;

	limit -= sizeof(struct batadv_frag_packet);
	limit *= BATADV_FRAG_MAX_FRAGMENTS;

	return limit;
}
