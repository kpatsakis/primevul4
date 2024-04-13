static bool match_fanout_group(struct packet_type *ptype, struct sock * sk)
{
	if (ptype->af_packet_priv == (void*)((struct packet_sock *)sk)->fanout)
		return true;

	return false;
}
