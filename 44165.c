static u8 cm_ack_timeout(u8 ca_ack_delay, u8 packet_life_time)
{
	int ack_timeout = packet_life_time + 1;

	if (ack_timeout >= ca_ack_delay)
		ack_timeout += (ca_ack_delay >= (ack_timeout - 1));
	else
		ack_timeout = ca_ack_delay +
			      (ack_timeout >= (ca_ack_delay - 1));

	return min(31, ack_timeout);
}
