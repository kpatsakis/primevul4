static int ipx_connect(struct socket *sock, struct sockaddr *uaddr,
	int addr_len, int flags)
{
	struct sock *sk = sock->sk;
	struct ipx_sock *ipxs = ipx_sk(sk);
	struct sockaddr_ipx *addr;
	int rc = -EINVAL;
	struct ipx_route *rt;

	sk->sk_state	= TCP_CLOSE;
	sock->state 	= SS_UNCONNECTED;

	lock_sock(sk);
	if (addr_len != sizeof(*addr))
		goto out;
	addr = (struct sockaddr_ipx *)uaddr;

	/* put the autobinding in */
	if (!ipxs->port) {
		struct sockaddr_ipx uaddr;

		uaddr.sipx_port		= 0;
		uaddr.sipx_network 	= 0;

#ifdef CONFIG_IPX_INTERN
		rc = -ENETDOWN;
		if (!ipxs->intrfc)
			goto out; /* Someone zonked the iface */
		memcpy(uaddr.sipx_node, ipxs->intrfc->if_node,
			IPX_NODE_LEN);
#endif	/* CONFIG_IPX_INTERN */

		rc = __ipx_bind(sock, (struct sockaddr *)&uaddr,
			      sizeof(struct sockaddr_ipx));
		if (rc)
			goto out;
	}

	/* We can either connect to primary network or somewhere
	 * we can route to */
	rt = ipxrtr_lookup(addr->sipx_network);
	rc = -ENETUNREACH;
	if (!rt && !(!addr->sipx_network && ipx_primary_net))
		goto out;

	ipxs->dest_addr.net  = addr->sipx_network;
	ipxs->dest_addr.sock = addr->sipx_port;
	memcpy(ipxs->dest_addr.node, addr->sipx_node, IPX_NODE_LEN);
	ipxs->type = addr->sipx_type;

	if (sock->type == SOCK_DGRAM) {
		sock->state 	= SS_CONNECTED;
		sk->sk_state 	= TCP_ESTABLISHED;
	}

	if (rt)
		ipxrtr_put(rt);
	rc = 0;
out:
	release_sock(sk);
	return rc;
}
