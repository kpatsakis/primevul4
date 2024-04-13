sctp_disposition_t sctp_sf_cookie_echoed_prm_shutdown(
	struct net *net,
	const struct sctp_endpoint *ep,
	const struct sctp_association *asoc,
	const sctp_subtype_t type,
	void *arg, sctp_cmd_seq_t *commands)
{
	/* There is a single T1 timer, so we should be able to use
	 * common function with the COOKIE-WAIT state.
	 */
	return sctp_sf_cookie_wait_prm_shutdown(net, ep, asoc, type, arg, commands);
}
