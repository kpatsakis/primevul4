struct sctp_chunk *sctp_assoc_lookup_asconf_ack(
					const struct sctp_association *asoc,
					__be32 serial)
{
	struct sctp_chunk *ack;

	/* Walk through the list of cached ASCONF-ACKs and find the
	 * ack chunk whose serial number matches that of the request.
	 */
	list_for_each_entry(ack, &asoc->asconf_ack_list, transmitted_list) {
		if (sctp_chunk_pending(ack))
			continue;
		if (ack->subh.addip_hdr->serial == serial) {
			sctp_chunk_hold(ack);
			return ack;
		}
	}

	return NULL;
}
