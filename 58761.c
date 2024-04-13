static int ipx_sendmsg(struct socket *sock, struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct ipx_sock *ipxs = ipx_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_ipx *, usipx, msg->msg_name);
	struct sockaddr_ipx local_sipx;
	int rc = -EINVAL;
	int flags = msg->msg_flags;

	lock_sock(sk);
	/* Socket gets bound below anyway */
/*	if (sk->sk_zapped)
		return -EIO; */	/* Socket not bound */
	if (flags & ~(MSG_DONTWAIT|MSG_CMSG_COMPAT))
		goto out;

	/* Max possible packet size limited by 16 bit pktsize in header */
	if (len >= 65535 - sizeof(struct ipxhdr))
		goto out;

	if (usipx) {
		if (!ipxs->port) {
			struct sockaddr_ipx uaddr;

			uaddr.sipx_port		= 0;
			uaddr.sipx_network	= 0;
#ifdef CONFIG_IPX_INTERN
			rc = -ENETDOWN;
			if (!ipxs->intrfc)
				goto out; /* Someone zonked the iface */
			memcpy(uaddr.sipx_node, ipxs->intrfc->if_node,
				IPX_NODE_LEN);
#endif
			rc = __ipx_bind(sock, (struct sockaddr *)&uaddr,
					sizeof(struct sockaddr_ipx));
			if (rc)
				goto out;
		}

		rc = -EINVAL;
		if (msg->msg_namelen < sizeof(*usipx) ||
		    usipx->sipx_family != AF_IPX)
			goto out;
	} else {
		rc = -ENOTCONN;
		if (sk->sk_state != TCP_ESTABLISHED)
			goto out;

		usipx = &local_sipx;
		usipx->sipx_family 	= AF_IPX;
		usipx->sipx_type 	= ipxs->type;
		usipx->sipx_port 	= ipxs->dest_addr.sock;
		usipx->sipx_network 	= ipxs->dest_addr.net;
		memcpy(usipx->sipx_node, ipxs->dest_addr.node, IPX_NODE_LEN);
	}

	rc = ipxrtr_route_packet(sk, usipx, msg, len, flags & MSG_DONTWAIT);
	if (rc >= 0)
		rc = len;
out:
	release_sock(sk);
	return rc;
}
