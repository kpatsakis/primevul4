static int ipx_getname(struct socket *sock, struct sockaddr *uaddr,
			int *uaddr_len, int peer)
{
	struct ipx_address *addr;
	struct sockaddr_ipx sipx;
	struct sock *sk = sock->sk;
	struct ipx_sock *ipxs = ipx_sk(sk);
	int rc;

	*uaddr_len = sizeof(struct sockaddr_ipx);

	lock_sock(sk);
	if (peer) {
		rc = -ENOTCONN;
		if (sk->sk_state != TCP_ESTABLISHED)
			goto out;

		addr = &ipxs->dest_addr;
		sipx.sipx_network	= addr->net;
		sipx.sipx_port		= addr->sock;
		memcpy(sipx.sipx_node, addr->node, IPX_NODE_LEN);
	} else {
		if (ipxs->intrfc) {
			sipx.sipx_network = ipxs->intrfc->if_netnum;
#ifdef CONFIG_IPX_INTERN
			memcpy(sipx.sipx_node, ipxs->node, IPX_NODE_LEN);
#else
			memcpy(sipx.sipx_node, ipxs->intrfc->if_node,
				IPX_NODE_LEN);
#endif	/* CONFIG_IPX_INTERN */

		} else {
			sipx.sipx_network = 0;
			memset(sipx.sipx_node, '\0', IPX_NODE_LEN);
		}

		sipx.sipx_port = ipxs->port;
	}

	sipx.sipx_family = AF_IPX;
	sipx.sipx_type	 = ipxs->type;
	sipx.sipx_zero	 = 0;
	memcpy(uaddr, &sipx, sizeof(sipx));

	rc = 0;
out:
	release_sock(sk);
	return rc;
}
