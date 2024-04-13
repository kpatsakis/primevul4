sctp_disposition_t sctp_sf_do_5_2_3_initack(struct net *net,
					    const struct sctp_endpoint *ep,
					    const struct sctp_association *asoc,
					    const sctp_subtype_t type,
					    void *arg, sctp_cmd_seq_t *commands)
{
	/* Per the above section, we'll discard the chunk if we have an
	 * endpoint.  If this is an OOTB INIT-ACK, treat it as such.
	 */
	if (ep == sctp_sk(net->sctp.ctl_sock)->ep)
		return sctp_sf_ootb(net, ep, asoc, type, arg, commands);
	else
		return sctp_sf_discard_chunk(net, ep, asoc, type, arg, commands);
}
