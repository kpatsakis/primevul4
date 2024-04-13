static int __ipx_bind(struct socket *sock,
			struct sockaddr *uaddr, int addr_len)
{
	struct sock *sk = sock->sk;
	struct ipx_sock *ipxs = ipx_sk(sk);
	struct ipx_interface *intrfc;
	struct sockaddr_ipx *addr = (struct sockaddr_ipx *)uaddr;
	int rc = -EINVAL;

	if (!sock_flag(sk, SOCK_ZAPPED) || addr_len != sizeof(struct sockaddr_ipx))
		goto out;

	intrfc = ipxitf_find_using_net(addr->sipx_network);
	rc = -EADDRNOTAVAIL;
	if (!intrfc)
		goto out;

	if (!addr->sipx_port) {
		addr->sipx_port = ipx_first_free_socketnum(intrfc);
		rc = -EINVAL;
		if (!addr->sipx_port)
			goto out_put;
	}

	/* protect IPX system stuff like routing/sap */
	rc = -EACCES;
	if (ntohs(addr->sipx_port) < IPX_MIN_EPHEMERAL_SOCKET &&
	    !capable(CAP_NET_ADMIN))
		goto out_put;

	ipxs->port = addr->sipx_port;

#ifdef CONFIG_IPX_INTERN
	if (intrfc == ipx_internal_net) {
		/* The source address is to be set explicitly if the
		 * socket is to be bound on the internal network. If a
		 * node number 0 was specified, the default is used.
		 */

		rc = -EINVAL;
		if (!memcmp(addr->sipx_node, ipx_broadcast_node, IPX_NODE_LEN))
			goto out_put;
		if (!memcmp(addr->sipx_node, ipx_this_node, IPX_NODE_LEN))
			memcpy(ipxs->node, intrfc->if_node, IPX_NODE_LEN);
		else
			memcpy(ipxs->node, addr->sipx_node, IPX_NODE_LEN);

		rc = -EADDRINUSE;
		if (ipxitf_find_internal_socket(intrfc, ipxs->node,
						ipxs->port)) {
			SOCK_DEBUG(sk,
				"IPX: bind failed because port %X in use.\n",
				ntohs(addr->sipx_port));
			goto out_put;
		}
	} else {
		/* Source addresses are easy. It must be our
		 * network:node pair for an interface routed to IPX
		 * with the ipx routing ioctl()
		 */

		memcpy(ipxs->node, intrfc->if_node, IPX_NODE_LEN);

		rc = -EADDRINUSE;
		if (ipxitf_find_socket(intrfc, addr->sipx_port)) {
			SOCK_DEBUG(sk,
				"IPX: bind failed because port %X in use.\n",
				ntohs(addr->sipx_port));
			goto out_put;
		}
	}

#else	/* !def CONFIG_IPX_INTERN */

	/* Source addresses are easy. It must be our network:node pair for
	   an interface routed to IPX with the ipx routing ioctl() */

	rc = -EADDRINUSE;
	if (ipxitf_find_socket(intrfc, addr->sipx_port)) {
		SOCK_DEBUG(sk, "IPX: bind failed because port %X in use.\n",
				ntohs((int)addr->sipx_port));
		goto out_put;
	}

#endif	/* CONFIG_IPX_INTERN */

	ipxitf_insert_socket(intrfc, sk);
	sock_reset_flag(sk, SOCK_ZAPPED);

	rc = 0;
out_put:
	ipxitf_put(intrfc);
out:
	return rc;
}
