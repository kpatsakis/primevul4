static bool packet_use_direct_xmit(const struct packet_sock *po)
{
	return po->xmit == packet_direct_xmit;
}
