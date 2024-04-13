void sctp_ootb_pkt_free(struct sctp_packet *packet)
{
	sctp_transport_free(packet->transport);
}
