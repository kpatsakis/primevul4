static bool __tpacket_has_room(struct packet_sock *po, int pow_off)
{
	int idx, len;

	len = po->rx_ring.frame_max + 1;
	idx = po->rx_ring.head;
	if (pow_off)
		idx += len >> pow_off;
	if (idx >= len)
		idx -= len;
	return packet_lookup_frame(po, &po->rx_ring, idx, TP_STATUS_KERNEL);
}
