void bat_socket_receive_packet(struct icmp_packet_rr *icmp_packet,
			       size_t icmp_len)
{
	struct socket_client *hash = socket_client_hash[icmp_packet->uid];

	if (hash)
		bat_socket_add_packet(hash, icmp_packet, icmp_len);
}
