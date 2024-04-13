static struct sock *ipxitf_find_internal_socket(struct ipx_interface *intrfc,
						unsigned char *ipx_node,
						__be16 port)
{
	struct sock *s;

	ipxitf_hold(intrfc);
	spin_lock_bh(&intrfc->if_sklist_lock);

	sk_for_each(s, &intrfc->if_sklist) {
		struct ipx_sock *ipxs = ipx_sk(s);

		if (ipxs->port == port &&
		    !memcmp(ipx_node, ipxs->node, IPX_NODE_LEN))
			goto found;
	}
	s = NULL;
found:
	spin_unlock_bh(&intrfc->if_sklist_lock);
	ipxitf_put(intrfc);
	return s;
}
