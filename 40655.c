static int sctp_sf_check_restart_addrs(const struct sctp_association *new_asoc,
				       const struct sctp_association *asoc,
				       struct sctp_chunk *init,
				       sctp_cmd_seq_t *commands)
{
	struct net *net = sock_net(new_asoc->base.sk);
	struct sctp_transport *new_addr;
	int ret = 1;

	/* Implementor's Guide - Section 5.2.2
	 * ...
	 * Before responding the endpoint MUST check to see if the
	 * unexpected INIT adds new addresses to the association. If new
	 * addresses are added to the association, the endpoint MUST respond
	 * with an ABORT..
	 */

	/* Search through all current addresses and make sure
	 * we aren't adding any new ones.
	 */
	list_for_each_entry(new_addr, &new_asoc->peer.transport_addr_list,
			    transports) {
		if (!list_has_sctp_addr(&asoc->peer.transport_addr_list,
					&new_addr->ipaddr)) {
			sctp_sf_send_restart_abort(net, &new_addr->ipaddr, init,
						   commands);
			ret = 0;
			break;
		}
	}

	/* Return success if all addresses were found. */
	return ret;
}
