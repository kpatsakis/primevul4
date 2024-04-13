static struct sock *atalk_search_socket(struct sockaddr_at *to,
					struct atalk_iface *atif)
{
	struct sock *s;

	read_lock_bh(&atalk_sockets_lock);
	sk_for_each(s, &atalk_sockets) {
		struct atalk_sock *at = at_sk(s);

		if (to->sat_port != at->src_port)
			continue;

		if (to->sat_addr.s_net == ATADDR_ANYNET &&
		    to->sat_addr.s_node == ATADDR_BCAST)
			goto found;

		if (to->sat_addr.s_net == at->src_net &&
		    (to->sat_addr.s_node == at->src_node ||
		     to->sat_addr.s_node == ATADDR_BCAST ||
		     to->sat_addr.s_node == ATADDR_ANYNODE))
			goto found;

		/* XXXX.0 -- we got a request for this router. make sure
		 * that the node is appropriately set. */
		if (to->sat_addr.s_node == ATADDR_ANYNODE &&
		    to->sat_addr.s_net != ATADDR_ANYNET &&
		    atif->address.s_node == at->src_node) {
			to->sat_addr.s_node = atif->address.s_node;
			goto found;
		}
	}
	s = NULL;
found:
	read_unlock_bh(&atalk_sockets_lock);
	return s;
}
