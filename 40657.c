sctp_disposition_t sctp_sf_cookie_echoed_err(struct net *net,
					const struct sctp_endpoint *ep,
					const struct sctp_association *asoc,
					const sctp_subtype_t type,
					void *arg,
					sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *chunk = arg;
	sctp_errhdr_t *err;

	if (!sctp_vtag_verify(chunk, asoc))
		return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

	/* Make sure that the ERROR chunk has a valid length.
	 * The parameter walking depends on this as well.
	 */
	if (!sctp_chunk_length_valid(chunk, sizeof(sctp_operr_chunk_t)))
		return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
						  commands);

	/* Process the error here */
	/* FUTURE FIXME:  When PR-SCTP related and other optional
	 * parms are emitted, this will have to change to handle multiple
	 * errors.
	 */
	sctp_walk_errors(err, chunk->chunk_hdr) {
		if (SCTP_ERROR_STALE_COOKIE == err->cause)
			return sctp_sf_do_5_2_6_stale(net, ep, asoc, type,
							arg, commands);
	}

	/* It is possible to have malformed error causes, and that
	 * will cause us to end the walk early.  However, since
	 * we are discarding the packet, there should be no adverse
	 * affects.
	 */
	return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
}
